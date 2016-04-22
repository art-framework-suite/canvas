# project name
project(canvas_Persistency_Common)

set(canvas_Persistency_Common_sources
  CacheStreamers.cc
  EDProduct.cc
  PtrVectorBase.cc
  RNGsnapshot.cc
  RefCore.cc
  RefCoreStreamer.cc
  detail/aggregate.cc
  detail/maybeCastObj.cc
  detail/setPtrVectorBaseStreamer.cc
  traits.cc
)

set(canvas_Persistency_Common_libraries
  canvas_Persistency_Provenance
  canvas_Utilities
  ${ROOT_CORE}
  ${Boost_THREAD_LIBRARY}
  )

art_make_library(
  LIBRARY_NAME canvas_Persistency_Common
  SOURCE ${canvas_Persistency_Common_sources}
  LIBRARIES ${canvas_Persistency_Common_libraries} ${CLHEP}
  )

art_dictionary(DICTIONARY_LIBRARIES canvas_Persistency_Common)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
