set(LIBRARY tinypngoutput)

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(${LIBRARY} STATIC ${SOURCES})
target_include_directories(${LIBRARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})