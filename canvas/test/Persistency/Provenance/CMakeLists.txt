cet_test(EventID_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

foreach(test_source IN ITEMS ProcessConfiguration_t.cpp ProcessHistory_t.cpp)
  get_filename_component(test_name ${test_source} NAME_WE)
  cet_test(${test_name} SOURCE ${test_source} LIBRARIES PRIVATE canvas::canvas)
endforeach()

cet_test(EventRange_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(FileIndex_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(RangeSet_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(TimeStamp_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

cet_test(ParentageRegistry_t USE_BOOST_UNIT LIBRARIES PRIVATE
  canvas::canvas
  cetlib::container_algorithms
  hep_concurrency::simultaneous_function_spawner
  Threads::Threads)
