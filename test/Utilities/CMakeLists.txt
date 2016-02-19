# cet_test macro

art_dictionary(NO_DEFAULT_LIBRARIES DICTIONARY_LIBRARIES cetlib NO_INSTALL)

set(default_test_libraries
  canvas_Utilities
  test_Utilities_dict
  MF_MessageLogger
  ${ROOT_CINT}
  ${ROOT_REFLEX}
  )

cet_test(FriendlyName_t USE_BOOST_UNIT
  LIBRARIES ${default_test_libraries}
  )

cet_test(TypeID_t USE_BOOST_UNIT
  LIBRARIES ${default_test_libraries}
  )

cet_test(ensurePointer_t USE_BOOST_UNIT
  LIBRARIES ${default_test_libraries}
  )
