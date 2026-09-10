cmake_minimum_required(VERSION 3.16)

project(WonderTrader LANGUAGES C CXX)

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
	if(MSVC)
		add_compile_options(/WX)
	else()
		add_compile_options(-Werror)
	endif()
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
add_subdirectory(strategy_engine/WtCtaStraFact)
add_subdirectory(strategy_engine/WtSelStraFact)

# 3. Trade execution
set(XTP_SDK_DIR "${CMAKE_SOURCE_DIR}/API/XTP2.2.32.2" CACHE PATH "XTP SDK header directory")
set(WT_XTP_DEFAULT OFF)
if(EXISTS "${XTP_SDK_DIR}/xtp_trader_api.h")
	set(WT_XTP_DEFAULT ON)
endif()
option(WT_BUILD_XTP "Build the XTP live trading adapter" ${WT_XTP_DEFAULT})
if(WT_BUILD_XTP)
	if(NOT EXISTS "${XTP_SDK_DIR}/xtp_trader_api.h")
		message(FATAL_ERROR "WT_BUILD_XTP requires xtp_trader_api.h in XTP_SDK_DIR=${XTP_SDK_DIR}")
	endif()
	add_subdirectory(trade_execution/TraderXTP)
else()
	message(STATUS "TraderXTP disabled; live trading requires WT_BUILD_XTP=ON and the XTP SDK")
endif()
add_subdirectory(trade_execution/WtExeFact)
add_subdirectory(trade_execution/WtPorter)
add_subdirectory(trade_execution/WtRunner)

# 4. Strategy backtest
add_subdirectory(backtest/WtBtCore)
add_subdirectory(backtest/WtBtPorter)
add_subdirectory(backtest/WtBtRunner)

include(CTest)
if(BUILD_TESTING)
	add_subdirectory(tests)
endif()

# Explicitly exclude non-stock / non-ETF modules from this build
# The old WonderTrader modules for futures, options, unrelated brokers and utility scaffolds
# are intentionally not included in this A-share / ETF-only project.

