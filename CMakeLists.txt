cmake_minimum_required (VERSION 3.1)

project (lab)

file(GLOB_RECURSE Project_SOURCES "src/*.cpp")
file(GLOB_RECURSE Project_HEADERS "inc/*.h")

set (Project_INCLUDE_DIRS "")
set (Project_Test_INCLUDE_DIRS "")

foreach (_headerFile ${Project_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Project_INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${External_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Project_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES Project_INCLUDE_DIRS)

include_directories(${Project_INCLUDE_DIRS})

add_executable (lab ${Project_SOURCES} )

set_property(TARGET lab PROPERTY CXX_STANDARD 14)

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-march=native" HAS_FLAG_MARCH_NATIVE)
if (HAS_FLAG_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif()

CHECK_CXX_COMPILER_FLAG("-mtune=native" HAS_FLAG_MTUNE_NATIVE)
if (HAS_FLAG_MTUNE_NATIVE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mtune=native")
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Werror")
endif()
