#1. 确定CMake的最低版本需求
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

#2. 确定工程名
PROJECT(WtPorter LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE}/bin/WtPorter)

#7. 添加源码
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

INCLUDE_DIRECTORIES(${INCS})
LINK_DIRECTORIES(${LNKS})
ADD_LIBRARY(WtPorter SHARED ${SRCS} ${HDRS})

SET(LIBS
	WtCore
	WTSTools
	WTSUtils
	dl
	pthread
	Boost::filesystem
	Boost::thread
)

TARGET_LINK_LIBRARIES(WtPorter ${LIBS})
add_dependencies(WtPorter WtExeFact WtDataStorage)
SET_TARGET_PROPERTIES(WtPorter PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN 1
	LINK_FLAGS_RELEASE -s)

file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH}/parsers/)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH}/traders/)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH}/executer/)

add_custom_command(TARGET WtPorter POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E
	copy_if_different $<TARGET_FILE:WtExeFact> ${LIBRARY_OUTPUT_PATH}/executer/

	COMMAND ${CMAKE_COMMAND} -E
	copy_if_different $<TARGET_FILE:WtDataStorage> ${LIBRARY_OUTPUT_PATH}/

	COMMENT "depends modules copied")

if(TARGET TraderXTP)
	add_dependencies(WtPorter TraderXTP)
	set_property(TARGET WtPorter APPEND PROPERTY LINK_DEPENDS
		"$<TARGET_FILE:TraderXTP>" "${XTP_SDK_DIR}/libxtptraderapi.so")
	add_custom_command(TARGET WtPorter POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different $<TARGET_FILE:TraderXTP> ${LIBRARY_OUTPUT_PATH}/traders/
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different "${XTP_SDK_DIR}/libxtptraderapi.so" ${LIBRARY_OUTPUT_PATH}/traders/
		COMMENT "copy XTP trader module and SDK"
		VERBATIM)
endif()


