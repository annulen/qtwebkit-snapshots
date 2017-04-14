/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "WebURLSchemeHandlerTaskProxy.h"
#include <wtf/HashMap.h>

namespace WebCore {
class ResourceError;
class ResourceLoader;
class ResourceResponse;
}

namespace WebKit {

class WebPage;

class WebURLSchemeHandlerProxy {
    WTF_MAKE_NONCOPYABLE(WebURLSchemeHandlerProxy);
public:
    WebURLSchemeHandlerProxy(WebPage&, uint64_t identifier);
    ~WebURLSchemeHandlerProxy();

    void startNewTask(WebCore::ResourceLoader&);

    uint64_t identifier() const { return m_identifier; }
    WebPage& page() { return m_webPage; }

    void taskDidReceiveResponse(uint64_t taskIdentifier, const WebCore::ResourceResponse&);
    void taskDidReceiveData(uint64_t taskIdentifier, size_t, const uint8_t* data);
    void taskDidComplete(uint64_t taskIdentifier, const WebCore::ResourceError&);

private:
    WebPage& m_webPage;
    uint64_t m_identifier { 0 };

    HashMap<unsigned long, std::unique_ptr<WebURLSchemeHandlerTaskProxy>> m_tasks;
}; // class WebURLSchemeHandlerProxy

} // namespace WebKit
