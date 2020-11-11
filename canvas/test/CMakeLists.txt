# Make sure tests have correct environment settings.
include(CetTest)
cet_enable_asserts()
# If you explicitly include CetTest in a subdirectory, you will need to
# re-initialize the test environment.
cet_test_env("FHICL_FILE_PATH=." OUT_VAR CET_TEST_ENV)

# build Persistency libraries
add_subdirectory(Persistency/Common)
add_subdirectory(Persistency/Provenance)
add_subdirectory(Version)
add_subdirectory(Utilities)
