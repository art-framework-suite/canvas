cet_test(Level_t)
cet_test(InputTag_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(ParameterSet_get_artInputTag_t LIBRARIES PRIVATE canvas::canvas)
cet_test(FriendlyName_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(TypeID_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(ensurePointer_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(uniform_type_name_test USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

cet_make_exec(NAME EventIDMatcher_t NO_INSTALL
  LIBRARIES PRIVATE canvas::canvas)

cet_test(EventIDMatcher_test HANDBUILT
  TEST_EXEC EventIDMatcher_t)
