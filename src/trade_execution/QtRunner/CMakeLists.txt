#1. 确定CMake的最低版本需求
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

#2. 确定工程名
PROJECT(QtRunner LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE}/bin/QtRunner)

#7. 添加源码
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

INCLUDE_DIRECTORIES(${INCS})
LINK_DIRECTORIES(${LNKS})

SET(LIBS
	WtCore
	WTSTools
	WTSUtils
	dl
	pthread
	Boost::filesystem
	Boost::thread
)

ADD_EXECUTABLE(QtRunner ${CMAKE_SOURCE_DIR}/main.cpp ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(QtRunner ${LIBS})
ADD_DEPENDENCIES(QtRunner WtExeFact WtDataStorage)

SET_TARGET_PROPERTIES(QtRunner PROPERTIES
	LINK_FLAGS_RELEASE -s)

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/parsers/)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/traders/)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/executer/)

add_custom_command(TARGET QtRunner POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E
	copy_if_different $<TARGET_FILE:WtExeFact> ${EXECUTABLE_OUTPUT_PATH}/executer/

	COMMAND ${CMAKE_COMMAND} -E
	copy_if_different $<TARGET_FILE:WtDataStorage> ${EXECUTABLE_OUTPUT_PATH}/

	COMMENT "depends modules copied")

if(TARGET TraderXTP)
	add_dependencies(QtRunner TraderXTP)
	set_property(TARGET QtRunner APPEND PROPERTY LINK_DEPENDS
		"$<TARGET_FILE:TraderXTP>" "${XTP_SDK_DIR}/libxtptraderapi.so")
	add_custom_command(TARGET QtRunner POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different $<TARGET_FILE:TraderXTP> ${EXECUTABLE_OUTPUT_PATH}/traders/
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different "${XTP_SDK_DIR}/libxtptraderapi.so" ${EXECUTABLE_OUTPUT_PATH}/traders/
		COMMENT "copy XTP trader module and SDK"
		VERBATIM)
endif()


