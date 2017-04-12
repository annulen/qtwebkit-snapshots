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

#include "config.h"
#include "NetworkingContext.h"
#include "ResourceRequest.h"
#include "ThirdPartyCookiesQt.h"

#include "BlobData.h"
#include "BlobRegistryImpl.h"

#include <qglobal.h>

#include <QNetworkRequest>
#include <QUrl>

namespace WebCore {

// The limit can be found in qhttpnetworkconnection.cpp.
// To achieve the best result we want WebKit to schedule the jobs so we
// are using the limit as found in Qt. To allow Qt to fill its queue
// and prepare jobs we will schedule two more downloads.
// Per TCP connection there is 1 current processed, 3 possibly pipelined
// and 2 ready to re-fill the pipeline.
unsigned initializeMaximumHTTPConnectionCountPerHost()
{
    return 6 * (1 + 3 + 2);
}

static void appendBlobResolved(QByteArray& data, const QUrl& url, QString* contentType = 0)
{
    RefPtr<BlobData> blobData = static_cast<BlobRegistryImpl&>(blobRegistry()).getBlobDataFromURL(url);
    if (!blobData)
        return;

    if (contentType)
        *contentType = blobData->contentType();

    BlobDataItemList::const_iterator it = blobData->items().begin();
    const BlobDataItemList::const_iterator itend = blobData->items().end();
    for (; it != itend; ++it) {
        const BlobDataItem& blobItem = *it;
        if (blobItem.type() == BlobDataItem::Type::Data)
            data.append(reinterpret_cast<const char*>(blobItem.data().data()->data()) + static_cast<int>(blobItem.offset()), static_cast<int>(blobItem.length()));
        else if (blobItem.type() == BlobDataItem::Type::File) {
            // File types are not allowed here, so just ignore it.
            RELEASE_ASSERT_WITH_MESSAGE(false, "File types are not allowed here");
        } else
            ASSERT_NOT_REACHED();
    }
}

static void resolveBlobUrl(const QUrl& url, QUrl& resolvedUrl)
{
    RefPtr<BlobData> blobData = static_cast<BlobRegistryImpl&>(blobRegistry()).getBlobDataFromURL(url);
    if (!blobData)
        return;

    QByteArray data;
    QString contentType;
    appendBlobResolved(data, url, &contentType);

    QString dataUri(QStringLiteral("data:"));
    dataUri.append(contentType);
    dataUri.append(QStringLiteral(";base64,"));
    dataUri.append(QString::fromLatin1(data.toBase64()));
    resolvedUrl = QUrl(dataUri);
}

static inline QByteArray stringToByteArray(const String& string)
{
    if (string.is8Bit())
        return QByteArray(reinterpret_cast<const char*>(string.characters8()), string.length());
    return QString(string).toLatin1();
}

QNetworkRequest ResourceRequest::toNetworkRequest(NetworkingContext *context) const
{
    QNetworkRequest request;
    QUrl newurl = url();

    if (newurl.scheme() == QLatin1String("blob"))
        resolveBlobUrl(url(), newurl);

    request.setUrl(newurl);
    request.setOriginatingObject(context ? context->originatingObject() : 0);

    const HTTPHeaderMap &headers = httpHeaderFields();
    for (HTTPHeaderMap::const_iterator it = headers.begin(), end = headers.end();
         it != end; ++it) {
        QByteArray name = stringToByteArray(it->key);
        // QNetworkRequest::setRawHeader() would remove the header if the value is null
        // Make sure to set an empty header instead of null header.
        if (!it->value.isNull())
            request.setRawHeader(name, stringToByteArray(it->value));
        else
            request.setRawHeader(name, QByteArrayLiteral(""));
    }

    // Make sure we always have an Accept header; some sites require this to
    // serve subresources
    if (!request.hasRawHeader("Accept"))
        request.setRawHeader("Accept", "*/*");

    switch (cachePolicy()) {
    case ReloadIgnoringCacheData:
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        break;
    case ReturnCacheDataElseLoad:
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        break;
    case ReturnCacheDataDontLoad:
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);
        break;
    case UseProtocolCachePolicy:
        // QNetworkRequest::PreferNetwork
    default:
        break;
    }

    if (!allowCookies() || !thirdPartyCookiePolicyPermits(context, url(), firstPartyForCookies())) {
        request.setAttribute(QNetworkRequest::CookieSaveControlAttribute, QNetworkRequest::Manual);
        request.setAttribute(QNetworkRequest::CookieLoadControlAttribute, QNetworkRequest::Manual);
    }

    if (!allowCookies())
        request.setAttribute(QNetworkRequest::AuthenticationReuseAttribute, QNetworkRequest::Manual);

    return request;
}

}

