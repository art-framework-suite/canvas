set( canvas_Persistency_Provenance_libraries
  canvas_Utilities
  MF_MessageLogger
  ${ROOT_CORE}
  ${Boost_THREAD_LIBRARY}
  )

art_make(SUBDIRS detail
  LIB_LIBRARIES
  canvas_Utilities
  ${Boost_THREAD_LIBRARY}
  MF_MessageLogger
        )

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
