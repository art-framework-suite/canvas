# project name
project(canvas_Version_Test)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_GetReleaseVersion.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/test_GetReleaseVersion.cpp @ONLY
  )
link_libraries(  canvas_Version  )
cet_test(GetReleaseVersion
  SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_GetReleaseVersion.cpp
  )

