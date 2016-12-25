/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef webpage_h
#define webpage_h

#include <qwebframe.h>
#include <qwebpage.h>

class MainWindow;

class WebPage : public QWebPage {
    Q_OBJECT

public:
    WebPage(MainWindow* parent);

    QWebPage* createWindow(QWebPage::WebWindowType) override;
    QObject* createPlugin(const QString&, const QUrl&, const QStringList&, const QStringList&) override;
    bool supportsExtension(QWebPage::Extension) const override;
    bool extension(Extension, const ExtensionOption*, ExtensionReturn*) override;

    bool acceptNavigationRequest(QWebFrame*, const QNetworkRequest&, NavigationType) override;

    QString userAgentForUrl(const QUrl&) const override;
    void setInterruptingJavaScriptEnabled(bool enabled) { m_interruptingJavaScriptEnabled = enabled; }

    bool shouldInterruptJavaScript() override;

    void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID) override;

public Q_SLOTS:
    void openUrlInDefaultBrowser(const QUrl& = QUrl());
    void setUserAgent(const QString& ua) { m_userAgent = ua; }
    void authenticationRequired(QNetworkReply*, QAuthenticator*);
    void requestPermission(QWebFrame*, QWebPage::Feature);
    void featurePermissionRequestCanceled(QWebFrame*, QWebPage::Feature);
    void requestFullScreen(QWebFullScreenRequest);

private:
    void applyProxy();
    MainWindow *m_mainWindow;
    QString m_userAgent;
    bool m_interruptingJavaScriptEnabled;
};

#endif
