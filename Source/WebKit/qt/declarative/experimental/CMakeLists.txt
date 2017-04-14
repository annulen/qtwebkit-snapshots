add_library(qmlwebkitexperimentalplugin MODULE plugin.cpp)
target_link_libraries(qmlwebkitexperimentalplugin
    WebKit Qt5::Quick)
set_target_properties(qmlwebkitexperimentalplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/imports/QtWebKit/experimental")

add_custom_target(
    qmlwebkitexperimentalplugin-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different qmldir "${CMAKE_BINARY_DIR}/imports/QtWebKit/experimental/qmldir"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS qmlwebkitexperimentalplugin
        DESTINATION "${QML_INSTALL_DIR}/QtWebKit/experimental")
install(FILES qmldir
        DESTINATION "${QML_INSTALL_DIR}/QtWebKit/experimental")
