# project name
project(canvas_Persistency_Common)

foreach(ART_IPR_BASE_NAME FindOne FindMany)
  UNSET(ART_IPR_BY_PTR)
  SET(ART_IPR_CLASS_NAME ${ART_IPR_BASE_NAME})
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${ART_IPR_BASE_NAME}.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ART_IPR_CLASS_NAME}.h
    @ONLY)

  SET(ART_IPR_BY_PTR true)
  SET(ART_IPR_CLASS_NAME "${ART_IPR_BASE_NAME}P")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${ART_IPR_BASE_NAME}.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ART_IPR_CLASS_NAME}.h
    @ONLY)
endforeach()

set(canvas_Persistency_Common_sources
  CacheStreamers.cc
  EDProduct.cc
  PtrVectorBase.cc
  RNGsnapshot.cc
  RefCore.cc
  RefCoreStreamer.cc
  detail/maybeCastObj.cc
  detail/setPtrVectorBaseStreamer.cc
  traits.cc
)

set( canvas_Persistency_Common_libraries
        canvas_Persistency_Provenance
        canvas_Utilities
        ${ROOT_CORE}
        ${Boost_THREAD_LIBRARY}
        )

art_make_library( LIBRARY_NAME canvas_Persistency_Common
                  SOURCE ${canvas_Persistency_Common_sources}
                  LIBRARIES ${canvas_Persistency_Common_libraries}
                 )

art_dictionary(DICTIONARY_LIBRARIES canvas_Persistency_Common)

install_headers(SUBDIRS detail)
install_source(SUBDIRS detail)
