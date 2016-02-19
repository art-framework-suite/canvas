set( canvas_Persistency_Provenance_libraries
  canvas_Utilities
  ${Boost_THREAD_LIBRARY}
  ${ROOT_CINTEX}
  ${ROOT_CINT}
  ${ROOT_CORE}
  ${ROOT_THREAD}
  MF_MessageLogger
  )

art_make(SUBDIRS detail
  LIB_LIBRARIES
  canvas_Utilities
  ${Boost_THREAD_LIBRARY}
  ${ROOT_CINTEX}
  ${ROOT_RIO}
  ${ROOT_CINT}
  ${ROOT_CORE}
  ${ROOT_THREAD}
  MF_MessageLogger
  )

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
