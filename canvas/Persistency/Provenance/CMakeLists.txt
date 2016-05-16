cet_make(SUBDIRS detail
  LIBRARIES
  canvas_Utilities
  MF_MessageLogger
  ${Boost_THREAD_LIBRARY}
  NO_DICTIONARY
  )

art_dictionary()

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
