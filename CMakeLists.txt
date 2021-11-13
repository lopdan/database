# Require CMAKE 3.1 or higher
cmake_minimum_required(VERSION 3.1)

# Project name
project(database)

# Build for C++20 if we can
set(CMAKE_C_COMPILE "gcc-8.1")

# Turn on any compiler-specific flags
if (WIN32)
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
else()
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	# if ("$ENV{SANITIZE}" STREQUAL "ON")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	# endif()
	
	if(APPLE)
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Debug] "YES")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=MinSizeRel] "NO")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=RelWithDebInfo] "YES")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Release] "NO")
	endif()
endif()

# Find all headers and implementation files
include(cmake/SourcesAndHeaders.cmake)


# Name of executable
add_executable(${PROJECT_NAME} ${exe_sources})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})
