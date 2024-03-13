cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(OPENCL VERSION 0.1.0)

set(CMAKE_BUILD_FILES_DIRECTORY  "${CMAKE_HOME_DIRECTORY}/build")
set(CMAKE_BUILD_DIRECTORY  "${CMAKE_HOME_DIRECTORY}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} C:/local/boost_1_84_0/lib64-msvc-14.0/cmake/Boost-1.84.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES /bigobj")

add_subdirectory(third/tbb)
add_subdirectory(third/OpenCL)
#if( NOT OpenCL_FOUND )
#    MESSAGE( SEND_ERROR "OpenCL not found. Install SDK from NVidia (CUDA), AMD or Intell" )
#endif( NOT OpenCL_FOUND )

include_directories(src)
include_directories(src/pipeline)
include_directories(third)

file(GLOB_RECURSE  SOURCE_FILES 
	${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c
)

add_executable(MAIN ${SOURCE_FILES}  )
target_link_libraries(MAIN
	PUBLIC
        OpenCL::OpenCL
        OpenCL::Headers
        OpenCL::HeadersCpp
		tbb_static
)
