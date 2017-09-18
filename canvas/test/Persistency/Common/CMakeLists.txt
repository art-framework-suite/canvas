cet_test(test_ConstAssnIter
  LIBRARIES canvas)

cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES canvas)

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES
  canvas
  ${CLHEP})
