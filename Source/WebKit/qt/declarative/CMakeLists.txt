add_definitions(-DHAVE_WEBKIT2=1)
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/Source
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/QtWebKit"

    # The WebKit2 Qt APIs depend on qwebkitglobal.h, which lives in WebKit
    "${WEBKIT_DIR}/qt/Api"

    "${WEBKIT2_DIR}/Platform"
    "${WEBKIT2_DIR}/Platform/IPC"
    "${WEBKIT2_DIR}/Shared/qt"
    "${WEBKIT2_DIR}/UIProcess/API/qt"

    ${WTF_DIR}
)
include_directories(SYSTEM
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
)

add_library(qmlwebkitplugin MODULE plugin.cpp)
target_link_libraries(qmlwebkitplugin
    WebKit Qt5::Quick)
set_target_properties(qmlwebkitplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/imports/QtWebKit")

add_custom_target(
    qmlwebkitplugin-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different qmldir "${CMAKE_BINARY_DIR}/imports/QtWebKit/qmldir"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different plugins.qmltypes "${CMAKE_BINARY_DIR}/imports/QtWebKit/plugins.qmltypes"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS qmlwebkitplugin
        DESTINATION "${QML_INSTALL_DIR}/QtWebKit")
install(FILES qmldir plugins.qmltypes
        DESTINATION "${QML_INSTALL_DIR}/QtWebKit")

add_subdirectory(experimental)
