# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Medicine_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Medicine_autogen.dir/ParseCache.txt"
  "Medicine_autogen"
  )
endif()
