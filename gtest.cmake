set (GOOGLETEST_ROOT external/gtest/googletest
  CACHE STRING "Google Test source root")

include_directories (SYSTEM
  ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
  ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include)

set (GOOGLETEST_SOURCES
  ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
  ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc)

foreach (src_file ${GOOGLETEST_SOURCES})
  set_source_files_properties(${src_file} PROPERTIES GENERATED 1)
endforeach ()

add_library (gtest ${GOOGLETEST_SOURCES})
