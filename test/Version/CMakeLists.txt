# project name
project(canvas_Version_Test)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_GetCanvasReleaseVersion.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/test_GetCanvasReleaseVersion.cpp @ONLY
  )
link_libraries(  canvas_Version  )
cet_test(GetCanvasReleaseVersion
  SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_GetCanvasReleaseVersion.cpp
  )

