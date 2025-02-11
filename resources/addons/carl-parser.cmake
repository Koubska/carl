
ExternalProject_Add(
    carl-parser
	GIT_REPOSITORY https://github.com/ths-rwth/carl-parser.git
	GIT_TAG deploy-latest
	CMAKE_ARGS -Dcarl_DIR=${CMAKE_BINARY_DIR}
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(carl-parser BINARY_DIR)

add_dependencies(addons carl-parser)
add_dependencies(carl-parser carl-arith-shared)

add_test(NAME carl-parser COMMAND ${BINARY_DIR}/test/carl-parser-test)
