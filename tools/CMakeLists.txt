

# Building convertor data/*.ini to json format
message(STATUS "Building convertor data/*.ini to json format")
add_executable(data-json-generator config.h config.cpp ini2json.cpp)
target_link_libraries(data-json-generator GEMS3K::GEMS3K)

# Generate Qt Help Project File gems3helpconfig.qhp
message(STATUS "Generate Qt Help Project File gems3helpconfig.qhp")
add_executable(gems3helpconfig-qhp-generator help.h help.cpp qhp-generator.cpp)
target_link_libraries(gems3helpconfig-qhp-generator GEMS3K::GEMS3K)
