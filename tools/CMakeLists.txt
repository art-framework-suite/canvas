project_variable(TOOLS_DIR tools CONFIG
  DOCSTRING "Location of useful scripts and files for ${PROJECT_NAME}")

cet_script(DESTINATION ${canvas_TOOLS_DIR}
  find-param-req make-branch)

add_subdirectory (canvas_refactor)
add_subdirectory (migration)
