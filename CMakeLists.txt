cmake_minimum_required(VERSION 3.5)
project(design_of_experiments)

set(CMAKE_CXX_STANDARD 11)

# Set the CFLAGS and CXXFLAGS depending on the options the user specified.
# Only GCC-like compilers support -Wextra, and other compilers give tons of
# output for -Wall, so only -Wall and -Wextra on GCC.
if (CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif (CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

# Detect OpenMP support in a compiler. If the compiler supports OpenMP, the
# flags to compile with OpenMP are returned and added.
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (OPENMP_FOUND)

# On Windows, things end up under Debug/ or Release/.
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else (WIN32)
    # If not on Windows, put them under more standard UNIX-like places.  This is
    # necessary, otherwise they would all end up in
    # ${CMAKE_BINARY_DIR}/src/mlpack/methods/... or somewhere else random like
    # that.
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
endif (WIN32)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

set(SOURCE_FILES
        IDF/idf_parser.hpp
        IDF/idf_parser.cpp
        util/utils.cpp
        util/utils.hpp
        generation/generation.cpp
        generation/generation.hpp
        simulation/simulate.cpp
        simulation/simulate.hpp
        )

set(SOURCE_FILES_2
        IDF/idf_parser2.hpp
        IDF/idf_parser2.cpp
        util/utils.cpp
        util/utils.hpp
        generation/generation.cpp
        generation/generation.hpp
        )

set(TEST_FILES
        tst/helpers/catch.hpp
        tst/helpers/unit_test_runner.hpp
        tst/helpers/idf_parser_fixture.hpp
        tst/idf_parser.unit.cpp
        tst/end_to_end.unit.cpp
        )

add_executable(design_of_experiments
        main.cpp
        ezOptionParser.hpp
        ${SOURCE_FILES}
        )

# set(GROUPINGS "0;1;2;3")
# foreach(GROUPING ${GROUPINGS})
#     file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generated_files/${GROUPING})
# endforeach(GROUPING)
