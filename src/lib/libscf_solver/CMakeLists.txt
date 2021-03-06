set(headers_list "")
# List of headers
list(APPEND headers_list hf.h pairs.h sad.h rhf.h uhf.h cuhf.h rohf.h ks.h stability.h)

# If you want to remove some headers specify them explictly here
if(DEVELOPMENT_CODE)
   list(REMOVE_ITEM headers_list "")
else()
   list(REMOVE_ITEM headers_list "")
endif()
# Sort alphabetically
list(SORT headers_list)

if(MPI_FOUND)
   include_directories(${PROJECT_SOURCE_DIR}/plugins/JKFactory/src)
endif()

set(sources_list "")
# List of sources
list(APPEND sources_list uhf.cc ks.cc cuhf.cc rhf.cc hf.cc sad.cc frac.cc mom.cc rohf.cc stability.cc)

# If you want to remove some sources specify them explictly here
if(DEVELOPMENT_CODE)
   list(REMOVE_ITEM sources_list "")
else()
   list(REMOVE_ITEM sources_list "")
endif()

# Write list of files to be passed to cloc for counting lines of code.
# Only files that are actually compiled are counted.
set(to_count "${sources_list}" "${headers_list}")
write_to_cloc_list("${to_count}")

# Build static library
add_library(scf_solver STATIC ${sources_list})
# Specify dependencies for the library (if any)
add_dependencies(scf_solver mints)
if(BUILD_JK_FACTORY)
  add_dependencies(scf_solver GTFockInt GTFockCInt)
endif()
set_property(GLOBAL APPEND PROPERTY LIBLIST scf_solver)
if(BUILD_CUSTOM_BOOST)
   add_dependencies(scf_solver custom_boost)
endif()

# Sets install directory for all the headers in the list
install_list_FILES("${headers_list}" include/libscf_solver)
