# project name
project(DataFormatsCommonTest)

set( cc_files ptr_t.cppunit.cc
              IntValues_reflex.cc )
	      
set( test_Persistency_Common_libraries canvas_Persistency_Common
                canvas_Persistency_Provenance
                canvas_Utilities
                art_Framework_Services_Registry
		${Boost_THREAD_LIBRARY}
		${Boost_PYTHON_LIBRARY}
		${ROOT_MATRIX}
		${ROOT_REFLEX}
		${ROOT_CORE}
		${PYTHON} )	      


foreach(test_source MapOfVectors_t.cpp RefCore_t.cpp traits_t.cpp Trie_t.cpp Wrapper_t.cpp)
  get_filename_component(test_name ${test_source} NAME_WE )
  cet_test(${test_name} SOURCES ${test_source} LIBRARIES ${test_Persistency_Common_libraries})
endforeach()

cet_test(testDataFormatsCommon SOURCES testRunner.cpp ${cc_files}
                               LIBRARIES ${test_Persistency_Common_libraries})

