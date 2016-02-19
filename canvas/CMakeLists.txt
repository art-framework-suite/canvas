# project name
include(ArtMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory (DictionaryManager)
add_subdirectory (Version)
add_subdirectory (Utilities)
add_subdirectory (Persistency)
