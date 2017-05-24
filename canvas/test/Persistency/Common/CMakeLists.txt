art_dictionary(NO_INSTALL)

cet_test(test_ConstAssnIter
  LIBRARIES canvas
  )

cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES canvas
  )

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES
  canvas
  ${CLHEP}
  )

cet_test(aggregate_th1_t USE_BOOST_UNIT
  LIBRARIES
  canvas
  cetlib
  ${ROOT_HIST}
  )
