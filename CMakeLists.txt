# === CMake lists for the OneSound - (08/02/2018) ===

# Not sure it is correct written

cmake_minimum_required (VERSION 3.0)
project (OneSound)

# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# === Preprocessor definitions ===

macro(ADD_DEFINE IDENT)
	if(MSVC)
		add_definitions("/D${IDENT}")
	else()
		add_definitions("-D${IDENT}")
	endif()
endmacro()

macro(ADD_TEST_PROJECT TEST_NAME TEST_FILES)
	add_executable(${TEST_NAME} ${TEST_FILES})
	target_link_libraries(${TEST_NAME} OneSound)
	set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_compile_features(${TEST_NAME} PRIVATE cxx_range_for)
endmacro()

file(GLOB FilesInclude		${PROJECT_INCLUDE_DIR}/OneSound/*.*)
file(GLOB FilesSoundTypeI	${PROJECT_INCLUDE_DIR}/OneSound/SoundType/*.*)
file(GLOB FilesStreamTypeI	${PROJECT_INCLUDE_DIR}/OneSound/StreamType/*.*)
file(GLOB FilesSource		${PROJECT_SOURCE_DIR}/Source/*.*)
file(GLOB FilesSoundType	${PROJECT_SOURCE_DIR}/Source/SoundType/*.*)
file(GLOB FilesStreamType	${PROJECT_SOURCE_DIR}/Source/StreamType/*.*)

set(FilesTest1 ${PROJECT_SOURCE_DIR}/example/Example1_SimpleSound.cpp)
set(FilesTest2 ${PROJECT_SOURCE_DIR}/example/Example2_WAV.cpp)
set(FilesTest3 ${PROJECT_SOURCE_DIR}/example/Example3_MP3.cpp)
set(FilesTest4 ${PROJECT_SOURCE_DIR}/example/Example4_OGG.cpp)
set(FilesTest5 ${PROJECT_SOURCE_DIR}/example/Example5_Mixing.cpp)
set(FilesTest6 ${PROJECT_SOURCE_DIR}/example/Example6_DynamicMixing.cpp)

source_group("Include" FILES ${FilesInclude})
source_group("Include\\SoundType" FILES ${FilesSoundTypeI})
source_group("Include\\StreamType" FILES ${FilesStreamTypeI})
source_group("Source" FILES ${FilesSource})
source_group("Source\\SoundType" FILES ${FilesSoundType})
source_group("Source\\StreamType" FILES ${FilesStreamType})

# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")

set(
	FilesAll
	${FilesInclude}
	${FilesSoundTypeI}
	${FilesStreamTypeI}
	${FilesSource}
	${FilesSoundType}
	${FilesStreamType}
)

# Main Project
add_library(OneSound SHARED ${FilesAll})
set_target_properties(OneSound PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
target_compile_features(OneSound PRIVATE cxx_range_for)

if(WIN32)
	target_link_libraries(OneSound Winmm)
endif()

ADD_TEST_PROJECT(Example1_SimpleSound ${FilesTest1})
ADD_TEST_PROJECT(Example2_WAV ${FilesTest2})
ADD_TEST_PROJECT(Example3_MP3 ${FilesTest3})
ADD_TEST_PROJECT(Example4_OGG ${FilesTest4})
ADD_TEST_PROJECT(Example5_Mixing ${FilesTest5})
ADD_TEST_PROJECT(Example6_DynamicMixing ${FilesTest6})