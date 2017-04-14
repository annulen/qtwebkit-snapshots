set(TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestWebKitAPI")
set(TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY_WTF "${TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY}/WTF")

# FIXME: It should be in WebKitFS actually
set(FORWARDING_HEADERS_DIR "${DERIVED_SOURCES_DIR}/ForwardingHeaders")

if (ENABLE_WEBKIT2)
    add_definitions(-DHAVE_WEBKIT2=1)

    add_custom_target(TestWebKitAPI-forwarding-headers
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl --include-path ${TESTWEBKITAPI_DIR} --output ${FORWARDING_HEADERS_DIR} --platform qt
        DEPENDS WebKit2-forwarding-headers
    )
    set(ForwardingHeadersForTestWebKitAPI_NAME TestWebKitAPI-forwarding-headers)
endif ()


include_directories(
    ${DERIVED_SOURCES_DIR}/ForwardingHeaders
    ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore
    ${TESTWEBKITAPI_DIR}

    # The WebKit2 Qt APIs depend on qwebkitglobal.h, which lives in WebKit
    "${WEBKIT_DIR}/qt/Api"

    "${WEBKIT2_DIR}/UIProcess/API/qt"
)

include_directories(SYSTEM
    ${ICU_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
)

add_definitions(
    -DAPITEST_SOURCE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
    -DROOT_BUILD_DIR="${CMAKE_BINARY_DIR}"
    -DQT_NO_CAST_FROM_ASCII
)

if (WIN32)
    add_definitions(-DUSE_CONSOLE_ENTRY_POINT)
    add_definitions(-DWEBCORE_EXPORT=)
    add_definitions(-DSTATICALLY_LINKED_WITH_WTF)
endif ()

set(test_main_SOURCES
    qt/main.cpp
)

set(webkit2_api_harness_SOURCES
    qt/PlatformUtilitiesQt.cpp
    qt/PlatformWebViewQt.cpp
)

set(bundle_harness_SOURCES
    qt/InjectedBundleControllerQt.cpp
    qt/PlatformUtilitiesQt.cpp
)

list(APPEND test_wtf_LIBRARIES
    ${Qt5Gui_LIBRARIES}
    ${DEPEND_STATIC_LIBS}
)

add_executable(TestWebCore
    ${test_main_SOURCES}
    ${TESTWEBKITAPI_DIR}/TestsController.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/LayoutUnit.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/URL.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/SharedBuffer.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/FileSystem.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/PublicSuffix.cpp
)

target_link_libraries(TestWebCore ${test_webcore_LIBRARIES})
if (ENABLE_WEBKIT2)
    add_dependencies(TestWebCore ${ForwardingHeadersForTestWebKitAPI_NAME})
    target_link_libraries(TestWebCore WebKit2)
    list(APPEND test_wtf_LIBRARIES
        WebKit2
    )
endif ()

add_test(TestWebCore ${TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY}/WebCore/TestWebCore)
set_tests_properties(TestWebCore PROPERTIES TIMEOUT 60)
set_target_properties(TestWebCore PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY}/WebCore)

if (ENABLE_WEBKIT2)
    add_executable(TestWebKit2
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/AboutBlankLoad.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/CanHandleRequest.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/CookieManager.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/DocumentStartUserScriptAlertCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/DOMWindowExtensionBasic.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/DOMWindowExtensionNoCache.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/DownloadDecideDestinationCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/EvaluateJavaScript.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/FailedLoad.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/Find.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/ForceRepaint.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/FrameMIMETypeHTML.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/FrameMIMETypePNG.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/Geolocation.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/GetInjectedBundleInitializationUserDataCallback.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/HitTestResultNodeHandle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/InjectedBundleBasic.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/InjectedBundleFrameHitTest.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/InjectedBundleInitializationUserDataCallbackWins.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/LoadAlternateHTMLStringWithNonDirectoryURL.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/LoadCanceledNoServerRedirectCallback.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/LoadPageOnCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/MouseMoveAfterCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/NewFirstVisuallyNonEmptyLayout.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/NewFirstVisuallyNonEmptyLayoutFails.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/NewFirstVisuallyNonEmptyLayoutForImages.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/NewFirstVisuallyNonEmptyLayoutFrames.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/PageLoadBasic.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/PageLoadDidChangeLocationWithinPageForFrame.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/ParentFrame.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/PendingAPIRequestURL.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/PreventEmptyUserAgent.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/PrivateBrowsingPushStateNoHistoryCallback.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/ReloadPageAfterCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/ResizeWindowAfterCrash.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/RestoreSessionStateContainingFormData.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/ShouldGoToBackForwardListItem.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/TextFieldDidBeginAndEndEditing.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/UserMedia.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/UserMessage.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WillSendSubmitEvent.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKPageCopySessionStateWithFiltering.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKPageGetScaleFactorNotZero.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKPreferences.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKRetainPtr.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKString.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKStringJSString.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/WKURL.cpp
    )

    target_link_libraries(TestWebKit2 ${test_webkit2_api_LIBRARIES})
    add_test(TestWebKit2 ${TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY}/WebKit2/TestWebKit2)
    set_tests_properties(TestWebKit2 PROPERTIES TIMEOUT 60)
    set_target_properties(TestWebKit2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TESTWEBKITAPI_RUNTIME_OUTPUT_DIRECTORY}/WebKit2)
endif ()
