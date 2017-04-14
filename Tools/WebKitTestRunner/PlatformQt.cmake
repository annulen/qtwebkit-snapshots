# FIXME: It should be in WebKitFS actually
set(FORWARDING_HEADERS_DIR "${DERIVED_SOURCES_DIR}/ForwardingHeaders")

add_custom_target(WebKitTestRunner-forwarding-headers
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl --include-path ${WEBKIT_TESTRUNNER_DIR} --output ${FORWARDING_HEADERS_DIR} --platform qt
)

set(ForwardingHeadersForWebKitTestRunner_NAME WebKitTestRunner-forwarding-headers)

list(APPEND WebKitTestRunner_SOURCES
    qt/EventSenderProxyQt.cpp
    qt/PlatformWebViewQt.cpp
    qt/TestControllerQt.cpp
    qt/TestInvocationQt.cpp
    qt/main.cpp
)

list(APPEND WebKitTestRunner_INCLUDE_DIRECTORIES
    ${FORWARDING_HEADERS_DIR}
    "${FORWARDING_HEADERS_DIR}/QtWebKit"
    "${WEBKIT_DIR}/qt/WebCoreSupport"
    "${WEBKIT2_DIR}/UIProcess/API/qt"
)

list(APPEND WebKitTestRunner_SYSTEM_INCLUDE_DIRECTORIES
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

list(APPEND WebKitTestRunner_LIBRARIES
    WTF
    WebKitWidgets
    ${Qt5Gui_LIBRARIES}
    ${Qt5Test_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

set(WebKitTestRunnerInjectedBundle_LIBRARIES
    WebCoreTestSupport
    WebKit2
    ${Qt5Widgets_LIBRARIES}
)

list(APPEND WebKitTestRunnerInjectedBundle_SOURCES
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/qt/ActivateFontsQt.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/qt/InjectedBundleQt.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/qt/TestRunnerQt.cpp
)

add_definitions(
    -DFONTS_CONF_DIR="${TOOLS_DIR}/WebKitTestRunner/gtk/fonts"
    -DTOP_LEVEL_DIR="${CMAKE_SOURCE_DIR}"
)
