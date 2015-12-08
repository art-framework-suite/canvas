set(canvas_Persistency_RootDB_suorce
  MetaDataAccess.cc
  SQLErrMsg.cc
  SQLite3Wrapper.cc
  tkeyvfs.cc
)

art_make_library( LIBRARY_NAME canvas_Persistency_RootDB
                  SOURCE ${canvas_Persistency_RootDB_suorce}
                  LIBRARIES ${ROOT_RIO}
  ${ROOT_THREAD}
  ${ROOT_CORE}
  ${ROOT_CINT}
  canvas_Utilities
  ${SQLITE3}
  )

install_headers()
install_source()
