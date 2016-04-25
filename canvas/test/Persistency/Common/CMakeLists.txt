cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES canvas_Persistency_Common
  )

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES
  canvas_Persistency_Common
  ${CLHEP}
  )
