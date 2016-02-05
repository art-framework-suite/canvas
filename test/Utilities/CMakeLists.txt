# cet_test macro

art_dictionary(NO_DEFAULT_LIBRARIES DICTIONARY_LIBRARIES cetlib NO_INSTALL)

set(default_canvas_test_libraries
  canvas_Utilities
  test_Utilities_dict
  cetlib
  MF_MessageLogger
  ${ROOT_CINT}
  ${ROOT_REFLEX}
  )

cet_test(FriendlyName_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(TypeID_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(ensurePointer_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(uniform_type_name_test USE_BOOST_UNIT
  LIBRARIES
  canvas_Utilities
  )

cet_make_exec(TypeNameBranchName_t
  NO_INSTALL
  LIBRARIES
  canvas_Utilities
  cetlib
)

set(tnum 1)
foreach(types_file
    art_test_types.txt
    microboone_MC6.txt
    mu2e.txt
    lariat_r006037_sr0080.typetester.txt
    lbne_r001161_sr01_20150526T220054.art_typetester.txt
    )
  if (tnum LESS 10)
    set(tnum_text "0${tnum}")
  else()
    set(tnum_text "${tnum}")
  endif()
  cet_test(TypeNameBranchName_test_${tnum_text} HANDBUILT
    TEST_EXEC TypeNameBranchName_t
    TEST_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TypeNameBranchName_t/${types_file}
    )
  math(EXPR tnum "${tnum} + 1")
endforeach()
