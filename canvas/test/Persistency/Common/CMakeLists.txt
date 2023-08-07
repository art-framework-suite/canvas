cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES PRIVATE canvas::canvas cetlib::cetlib)

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES PRIVATE canvas::canvas CLHEP::CLHEP)

cet_test(const_assns_iter_t LIBRARIES PRIVATE canvas::canvas)
cet_test(for_each_group_t LIBRARIES PRIVATE canvas::AssnsAlgorithms canvas::canvas)
cet_test(for_each_group_with_left_t LIBRARIES PRIVATE canvas::AssnsAlgorithms canvas::canvas)
cet_test(ptr_deduction_t LIBRARIES PRIVATE canvas::canvas)
cet_test(ptr_hash_t LIBRARIES PRIVATE canvas::canvas)
cet_test(maybeCastObj_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(sampled_t LIBRARIES PRIVATE canvas::canvas)
cet_test(set_ptr_customization_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(wrapper_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

cet_test(bad_sampled_t COMPILE_ONLY LIBRARIES PRIVATE canvas::canvas)
set_tests_properties(bad_sampled_t PROPERTIES
  PASS_REGULAR_EXPRESSION "note: the expression ‘!\\(isSampled<T>\\) \\[with T = art::Sampled<int>\\]’ evaluated to ‘false’")
