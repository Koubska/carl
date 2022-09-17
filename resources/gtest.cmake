ExternalProject_Add(
    GTest-EP
	URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip
	URL_HASH SHA1=${GTEST_ZIPHASH}
	DOWNLOAD_NO_PROGRESS 1
	CMAKE_ARGS
          -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	BUILD_COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest
	COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest_main
	INSTALL_COMMAND ""
	BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libgtest.a <INSTALL_DIR>/lib/libgtest_main.a
)

message(Binary Dir ${CMAKE_BINARY_DIR})

set(GTEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/resources/include")
set(GTEST_LIB "${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/lib/libgtest.a")
set(GTEST_MAIN_LIBRARIES "${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/lib/libgtest_main.a")

add_library(GTest::GTest STATIC IMPORTED)
set_target_properties(GTest::GTest PROPERTIES
	IMPORTED_LOCATION "${GTEST_LIB}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

add_library(GTest::Main STATIC IMPORTED)
set_target_properties(GTest::Main PROPERTIES
	IMPORTED_LOCATION "${GTEST_MAIN_LIBRARIES}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

find_package(Threads QUIET)
if(TARGET Threads::Threads)
    set_target_properties(GTest::GTest PROPERTIES
        INTERFACE_LINK_LIBRARIES Threads::Threads)
endif()

message(STATUS "GTest include dir: ${GTEST_INCLUDE_DIR}")
message(STATUS "GTest libraries: ${GTEST_LIB}")
message(STATUS "GTest main libraries: ${GTEST_MAIN_LIBRARIES}")

add_dependencies(GTest::GTest GTest-EP)
add_dependencies(GTest::Main GTest-EP)

