foreach (ART_IPR_BASE_NAME IN ITEMS FindOne FindMany)
  unset(ART_IPR_BY_PTR)
  set(ART_IPR_CLASS_NAME ${ART_IPR_BASE_NAME})
  configure_file(${ART_IPR_BASE_NAME}.h.in
    ${ART_IPR_CLASS_NAME}.h
    @ONLY)

  set(ART_IPR_BY_PTR true)
  set(ART_IPR_CLASS_NAME "${ART_IPR_BASE_NAME}P")
  configure_file(${ART_IPR_BASE_NAME}.h.in
    ${ART_IPR_CLASS_NAME}.h
    @ONLY)
endforeach()

cet_make_library(LIBRARY_NAME AssnsAlgorithms INTERFACE
  SOURCE AssnsAlgorithms.h
  LIBRARIES INTERFACE
    Range::Range
    canvas::canvas
)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
