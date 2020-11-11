cet_test(EventID_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

file(GLOB cppunit_files *.cppunit.cc)
foreach(cppunit_source ${cppunit_files})
  get_filename_component(test_name ${cppunit_source} NAME_WE)
  cet_test(${test_name} SOURCE testRunner.cpp ${cppunit_source}
    LIBRARIES PRIVATE ${CPPUNIT} canvas::canvas)
endforeach()

foreach(test_source ProcessConfiguration_t.cpp ProcessHistory_t.cpp)
  get_filename_component(test_name ${test_source} NAME_WE)
  cet_test(${test_name} SOURCE ${test_source} LIBRARIES PRIVATE canvas::canvas)
endforeach()

cet_test(EventRange_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)
cet_test(RangeSet_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas)

cet_find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

cet_test(ParentageRegistry_t USE_BOOST_UNIT LIBRARIES PRIVATE canvas::canvas Threads::Threads)
