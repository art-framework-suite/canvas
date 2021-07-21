include(CetTest)
cet_enable_asserts()
cet_test_env("FHICL_FILE_PATH=.")

set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads PRIVATE REQUIRED)
find_package(hep_concurrency PRIVATE REQUIRED)

# Put everything in a different export set.
cet_register_export_set(SET_NAME Testing NAMESPACE canvas_test SET_DEFAULT)

add_subdirectory(Persistency/Common)
add_subdirectory(Persistency/Provenance)
add_subdirectory(Version)
add_subdirectory(Utilities)
