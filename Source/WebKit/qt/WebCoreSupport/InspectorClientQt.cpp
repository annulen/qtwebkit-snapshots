/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InspectorClientQt.h"

#include "FrameView.h"
#include "InspectorController.h"
#include "InspectorServerQt.h"
#include "MainFrame.h"
#include "NotImplemented.h"
#include "Page.h"
#include "QWebFrameAdapter.h"
#include "QWebPageAdapter.h"
#include "ScriptDebugServer.h"
#include <QCoreApplication>
#include <QFile>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>
#include <QVariant>
#include <wtf/text/CString.h>

namespace WebCore {

static const QLatin1String settingStoragePrefix("Qt/QtWebKit/QWebInspector/");
static const QLatin1String settingStorageTypeSuffix(".type");

namespace {

class InspectorFrontendSettingsQt : public InspectorFrontendClientLocal::Settings {
public:
    virtual ~InspectorFrontendSettingsQt() { }
    String getProperty(const String& name) override
    {
#ifdef QT_NO_SETTINGS
        Q_UNUSED(name)
        Q_UNUSED(value)
        qWarning("QWebInspector: QSettings is not supported by Qt.");
        return String();
#else
        QSettings qsettings;
        if (qsettings.status() == QSettings::AccessError) {
            // QCoreApplication::setOrganizationName and QCoreApplication::setApplicationName haven't been called
            qWarning("QWebInspector: QSettings couldn't read configuration setting [%s].",
                qPrintable(static_cast<QString>(name)));
            return String();
        }

        QString settingKey(settingStoragePrefix + QString(name));
        QString storedValueType = qsettings.value(settingKey + settingStorageTypeSuffix).toString();
        QVariant storedValue = qsettings.value(settingKey);
        storedValue.convert(QVariant::nameToType(storedValueType.toLatin1().data()));
        return variantToSetting(storedValue);
#endif // QT_NO_SETTINGS
    }

    void setProperty(const String& name, const String& value) override
    {
#ifdef QT_NO_SETTINGS
        Q_UNUSED(name)
        Q_UNUSED(value)
        qWarning("QWebInspector: QSettings is not supported by Qt.");
#else
        QSettings qsettings;
        if (qsettings.status() == QSettings::AccessError) {
            qWarning("QWebInspector: QSettings couldn't persist configuration setting [%s].",
                qPrintable(static_cast<QString>(name)));
            return;
        }

        QVariant valueToStore = settingToVariant(value);
        QString settingKey(settingStoragePrefix + QString(name));
        qsettings.setValue(settingKey, valueToStore);
        qsettings.setValue(settingKey + settingStorageTypeSuffix, QLatin1String(QVariant::typeToName(valueToStore.type())));
#endif // QT_NO_SETTINGS
    }

private:
    static String variantToSetting(const QVariant& qvariant)
    {
        String retVal;

        switch (qvariant.type()) {
        case QVariant::Bool:
            retVal = qvariant.toBool() ? "true" : "false";
            break;
        case QVariant::String:
            retVal = qvariant.toString();
            break;
        default:
            break;
        }

        return retVal;
    }

    static QVariant settingToVariant(const String& setting)
    {
        QVariant retVal;
        retVal.setValue(static_cast<QString>(setting));
        return retVal;
    }
};

}

InspectorClientQt::InspectorClientQt(QWebPageAdapter* page)
    : m_inspectedWebPage(page)
    , m_frontendWebPage(nullptr)
    , m_frontendClient(nullptr)
    , m_remoteFrontEndChannel(nullptr)
{
    InspectorServerQt* webInspectorServer = InspectorServerQt::server();
    if (webInspectorServer)
        webInspectorServer->registerClient(this);
}

void InspectorClientQt::inspectedPageDestroyed()
{
    if (m_frontendClient)
        m_frontendClient->inspectorClientDestroyed();

    InspectorServerQt* webInspectorServer = InspectorServerQt::server();
    if (webInspectorServer)
        webInspectorServer->unregisterClient(this);

    delete this;
}

Inspector::FrontendChannel* InspectorClientQt::openLocalFrontend(WebCore::InspectorController* inspectorController)
{
    Inspector::FrontendChannel* frontendChannel = nullptr;
    QObject* view = nullptr;
    QWebPageAdapter* inspectorPage = nullptr;
    m_inspectedWebPage->createWebInspector(&view, &inspectorPage);
    std::unique_ptr<QObject> inspectorView(view);

    QObject* inspector = m_inspectedWebPage->inspectorHandle();
    // Remote frontend was attached.
    if (m_remoteFrontEndChannel)
        return 0;

    // This is a known hook that allows changing the default URL for the
    // Web inspector. This is used for SDK purposes. Please keep this hook
    // around and don't remove it.
    // https://bugs.webkit.org/show_bug.cgi?id=35340
    QUrl inspectorUrl;
#ifndef QT_NO_PROPERTIES
    inspectorUrl = inspector->property("_q_inspectorUrl").toUrl();
#endif
    if (!inspectorUrl.isValid())
        inspectorUrl = QUrl(QLatin1String("qrc:/webkit/inspector/UserInterface/Main.html"));

#ifndef QT_NO_PROPERTIES
    QVariant inspectorJavaScriptWindowObjects = inspector->property("_q_inspectorJavaScriptWindowObjects");
    if (inspectorJavaScriptWindowObjects.isValid())
        inspectorPage->handle()->setProperty("_q_inspectorJavaScriptWindowObjects", inspectorJavaScriptWindowObjects);
#endif

    // Is 'controller' the same object as 'inspectorController' (which appears to be unused)?
    InspectorController& controller = inspectorPage->page->inspectorController();
    m_frontendClient = std::make_unique<InspectorFrontendClientQt>(m_inspectedWebPage, inspectorController, WTFMove(inspectorView), inspectorPage->page, this);
    controller.setInspectorFrontendClient(m_frontendClient.get());
    m_frontendWebPage = inspectorPage;

    // Web Inspector should not belong to any other page groups since it is a specialized debugger window.
    m_frontendWebPage->page->setGroupName("__WebInspectorPageGroup__");
    frontendChannel = this;

    inspectorPage->mainFrameAdapter()->load(QNetworkRequest(inspectorUrl));
    m_inspectedWebPage->setInspectorFrontend(view);

    return frontendChannel;
}

void InspectorClientQt::bringFrontendToFront()
{
    m_frontendClient->bringToFront();
}

void InspectorClientQt::releaseFrontendPage()
{
    m_frontendWebPage = nullptr;
    m_frontendClient = nullptr;
}

void InspectorClientQt::attachAndReplaceRemoteFrontend(InspectorServerRequestHandlerQt* channel)
{
    m_remoteFrontEndChannel = channel;
    m_inspectedWebPage->page->inspectorController().connectFrontend(this);
}

void InspectorClientQt::detachRemoteFrontend()
{
    m_remoteFrontEndChannel = nullptr;
    m_inspectedWebPage->page->inspectorController().disconnectFrontend(this);
}

void InspectorClientQt::closeFrontendWindow()
{
    if (m_frontendClient)
        m_frontendClient->closeWindow();
}

void InspectorClientQt::highlight()
{
    hideHighlight();
}

void InspectorClientQt::hideHighlight()
{
    WebCore::Frame& frame = m_inspectedWebPage->page->mainFrame();
    QRect rect = m_inspectedWebPage->mainFrameAdapter()->frameRect();
    if (!rect.isEmpty())
        frame.view()->invalidateRect(rect);
}

InspectorClientQt::ConnectionType InspectorClientQt::connectionType() const
{
    if (m_remoteFrontEndChannel)
        return ConnectionType::Remote;

    return ConnectionType::Local;
}

bool InspectorClientQt::sendMessageToFrontend(const String& message)
{
    if (m_remoteFrontEndChannel) {
        WTF::CString msg = message.utf8();
        m_remoteFrontEndChannel->webSocketSend(msg.data(), msg.length());
        return true;
    }
    if (!m_frontendWebPage)
        return false;

    Page* frontendPage = m_frontendWebPage->page;
    return doDispatchMessageOnFrontendPage(frontendPage, message);
}

InspectorFrontendClientQt::InspectorFrontendClientQt(QWebPageAdapter* inspectedWebPage, InspectorController* inspectedPageController, std::unique_ptr<QObject> inspectorView, WebCore::Page* inspectorPage, InspectorClientQt* inspectorClient)
    : InspectorFrontendClientLocal(inspectedPageController, inspectorPage, std::make_unique<InspectorFrontendSettingsQt>())
    , m_inspectedWebPage(inspectedWebPage)
    , m_inspectorView(WTFMove(inspectorView))
    , m_destroyingInspectorView(false)
    , m_inspectorClient(inspectorClient)
{
}

InspectorFrontendClientQt::~InspectorFrontendClientQt()
{
    ASSERT(m_destroyingInspectorView);
    if (m_inspectorClient)
        m_inspectorClient->releaseFrontendPage();
}

void InspectorFrontendClientQt::frontendLoaded()
{
    InspectorFrontendClientLocal::frontendLoaded();
    setAttachedWindow(DockSide::Bottom);
}

String InspectorFrontendClientQt::localizedStringsURL()
{
    return ASCIILiteral("qrc:/webkit/inspector/Localizations/en.lproj/localizedStrings.js");
}

void InspectorFrontendClientQt::bringToFront()
{
    updateWindowTitle();
}

void InspectorFrontendClientQt::closeWindow()
{
#ifndef QT_NO_PROPERTIES
    if (QObject *inspector = m_inspectedWebPage->inspectorHandle())
        inspector->setProperty("visible", false);
#endif
    destroyInspectorView(true);
}

void InspectorFrontendClientQt::attachWindow(DockSide)
{
    notImplemented();
}

void InspectorFrontendClientQt::detachWindow()
{
    notImplemented();
}

void InspectorFrontendClientQt::setAttachedWindowHeight(unsigned)
{
    notImplemented();
}

void InspectorFrontendClientQt::setAttachedWindowWidth(unsigned)
{
    notImplemented();
}

void InspectorFrontendClientQt::inspectedURLChanged(const String& newURL)
{
    m_inspectedURL = newURL;
    updateWindowTitle();
}

void InspectorFrontendClientQt::updateWindowTitle()
{
    QString caption = QCoreApplication::translate("QWebPage", "Web Inspector - %2").arg(m_inspectedURL);
    m_inspectedWebPage->setInspectorWindowTitle(caption);
}

void InspectorFrontendClientQt::destroyInspectorView(bool notifyInspectorController)
{
    if (m_destroyingInspectorView)
        return;
    m_destroyingInspectorView = true;

    if (Page* frontendPage = this->frontendPage())
        frontendPage->inspectorController().setInspectorFrontendClient(nullptr);

    // Inspected page may have already been destroyed.
    if (m_inspectedWebPage) {
        // Clear reference from QWebInspector to the frontend view.
        m_inspectedWebPage->setInspectorFrontend(nullptr);
    }

    if (notifyInspectorController)
        m_inspectedWebPage->page->inspectorController().disconnectFrontend(m_inspectorClient);

    // If we delete view right here it will delete QWebPageAdapter in the middle
    // of mouseReleaseEvent()
    QObject* view = m_inspectorView.release();
    view->deleteLater();

    if (m_inspectorClient)
        m_inspectorClient->releaseFrontendPage();
}

void InspectorFrontendClientQt::inspectorClientDestroyed()
{
    destroyInspectorView(false);
    m_inspectorClient = 0;
    m_inspectedWebPage = 0;
}
}

