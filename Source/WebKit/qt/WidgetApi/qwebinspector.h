/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef QWEBINSPECTOR_H
#define QWEBINSPECTOR_H

#include <QtWebKit/qwebkitglobal.h>
#include <QtWebKitWidgets/qwebpage.h>

#include <QtWebKitWidgets/qwebview.h>

class QWebInspectorPrivate;

class QWEBKITWIDGETS_EXPORT QWebInspector : public QWidget {
    Q_OBJECT
public:
    explicit QWebInspector(QWidget* parent = Q_NULLPTR);
    ~QWebInspector();

    void setPage(QWebPage* page);
    QWebPage* page() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
    bool event(QEvent*) Q_DECL_OVERRIDE;

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent* event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private:
    QWebInspectorPrivate* d;

    friend class QWebInspectorPrivate;
    friend class QWebPage;
    friend class QWebPagePrivate;
    friend class WebCore::InspectorClientQt;
    friend class WebCore::InspectorFrontendClientQt;
};
#endif
