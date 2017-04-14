add_subdirectory(QtTestBrowser)

if (ENABLE_API_TESTS AND NOT ENABLE_WEBKIT2)
    add_subdirectory(TestWebKitAPI)
endif ()

if (ENABLE_TEST_SUPPORT)
    add_subdirectory(DumpRenderTree)
    add_subdirectory(ImageDiff)
endif ()

if (ENABLE_WEBKIT2)
    add_subdirectory(MiniBrowser/qt)
    add_subdirectory(WebKitTestRunner)
endif ()
