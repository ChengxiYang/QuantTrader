cmake_minimum_required(VERSION 3.16)

project(QuantTrader LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR
	NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
	NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message(FATAL_ERROR "QuantTrader requires Linux with GCC/G++")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MyDeps "" CACHE PATH "Optional dependency installation prefix")
if(MyDeps)
	list(PREPEND CMAKE_PREFIX_PATH "${MyDeps}")
endif()
find_package(Boost 1.90 REQUIRED CONFIG COMPONENTS filesystem thread)
find_package(Threads REQUIRED)
set(INCS "${CMAKE_SOURCE_DIR}" ${Boost_INCLUDE_DIRS})
set(LNKS "")
if(MyDeps)
	list(APPEND INCS "${MyDeps}/include")
	list(APPEND LNKS "${MyDeps}/lib")
endif()
set(PLATFORM x64)
set(PREFIX lib)
set(SUFFIX .so)

add_compile_definitions(BOOST_ASIO_NO_DEPRECATED)
option(WT_WARNINGS_AS_ERRORS "Treat enabled compiler warnings as errors" ON)
if(WT_WARNINGS_AS_ERRORS)
	add_compile_options(-Werror)
endif()

# Common libraries
add_subdirectory(WTSUtils)
add_subdirectory(WTSTools)

# 1. Market data and quote receiving
add_subdirectory(market_data/WtDataStorage)
add_subdirectory(market_data/WtDtCore)
add_subdirectory(market_data/WtDtHelper)
add_subdirectory(market_data/WtDtPorter)

# 2. Strategy engine
add_subdirectory(strategy_engine/WtCore)
add_subdirectory(strategy_engine/cta/WtCtaStraFact)
add_subdirectory(strategy_engine/sel/WtSelStraFact)

# 3. Trade execution
set(XTP_SDK_DIR "${CMAKE_SOURCE_DIR}/../deps" CACHE PATH "XTP SDK headers and libxtptraderapi.so directory")
set(WT_XTP_DEFAULT OFF)
if(EXISTS "${XTP_SDK_DIR}/xtp_trader_api.h" AND EXISTS "${XTP_SDK_DIR}/libxtptraderapi.so")
	set(WT_XTP_DEFAULT ON)
endif()
option(WT_BUILD_XTP "Build the XTP live trading adapter" ${WT_XTP_DEFAULT})
if(WT_BUILD_XTP)
	foreach(xtp_file xtp_trader_api.h libxtptraderapi.so)
		if(NOT EXISTS "${XTP_SDK_DIR}/${xtp_file}")
			message(FATAL_ERROR "WT_BUILD_XTP requires ${xtp_file} in XTP_SDK_DIR=${XTP_SDK_DIR}")
		endif()
	endforeach()
	add_subdirectory(trade_execution/TraderXTP)
else()
	message(STATUS "TraderXTP disabled; live trading requires WT_BUILD_XTP=ON and the XTP SDK")
endif()
add_subdirectory(trade_execution/WtExeFact)
add_subdirectory(trade_execution/WtPorter)
add_subdirectory(trade_execution/QtRunner)

include(CTest)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../tests" "${CMAKE_BINARY_DIR}/tests")

# Explicitly exclude non-stock / non-ETF modules from this build
# The old WonderTrader modules for futures, options, unrelated brokers and utility scaffolds
# are intentionally not included in this A-share / ETF-only project.

