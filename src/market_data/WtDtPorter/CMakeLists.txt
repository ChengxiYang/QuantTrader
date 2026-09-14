#1. 确定CMake的最低版本需求
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

#2. 确定工程名
PROJECT(WtDtPorter LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE}/bin/WtDtPorter)

#7. 添加源码
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

INCLUDE_DIRECTORIES(${INCS})
LINK_DIRECTORIES(${LNKS})
ADD_LIBRARY(WtDtPorter SHARED ${SRCS} ${HDRS})

SET(LIBS
	WtDtCore
	WTSTools
	WTSUtils
	dl
	pthread
	Boost::filesystem
	Boost::thread
)

TARGET_LINK_LIBRARIES(WtDtPorter ${LIBS})
add_dependencies(WtDtPorter WtDataStorage)
SET_TARGET_PROPERTIES(WtDtPorter PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN 1
	LINK_FLAGS_RELEASE -s)

file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH}/parsers/)
add_custom_command(TARGET WtDtPorter POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different $<TARGET_FILE:WtDataStorage> ${LIBRARY_OUTPUT_PATH}/

		COMMENT "depends modules copied")
		