cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES PRIVATE canvas::canvas)

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES PRIVATE canvas::canvas CLHEP::CLHEP)

cet_test(const_assns_iter_t LIBRARIES PRIVATE canvas::canvas)
cet_test(for_each_group_t LIBRARIES PRIVATE canvas::AssnsAlgorithms)
cet_test(for_each_group_with_left_t LIBRARIES PRIVATE canvas::AssnsAlgorithms)
cet_test(ptr_hash_t LIBRARIES PRIVATE canvas::canvas)
cet_test(maybeCastObj_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(sampled_t LIBRARIES PRIVATE canvas::canvas)
cet_test(set_ptr_customization_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(wrapper_t LIBRARIES PRIVATE canvas::canvas)
