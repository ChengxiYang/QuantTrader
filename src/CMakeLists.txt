CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(WonderTrader)

MESSAGE(STATUS "Generator is ${CMAKE_GENERATOR}")
MESSAGE(STATUS "Operation System is Linux")

SET(MyDeps "/home/mydeps")
SET(INCS ${MyDeps}/include)
SET(LNKS ${MyDeps}/lib)
SET(PLATFORM x64)
SET(PREFIX lib)
SET(SUFFIX .so)

SET(CMAKE_CXX_STANDARD 17)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/compat)

# Common libraries
ADD_SUBDIRECTORY(WTSUtils)
ADD_SUBDIRECTORY(WTSTools)

# 1. Market data and quote receiving
ADD_SUBDIRECTORY(market_data/ParserXeleSkt)
ADD_SUBDIRECTORY(market_data/WtDataStorage)
ADD_SUBDIRECTORY(market_data/WtDtCore)
ADD_SUBDIRECTORY(market_data/WtDtHelper)
ADD_SUBDIRECTORY(market_data/WtDtPorter)

# 2. Strategy engine
ADD_SUBDIRECTORY(strategy_engine/WtCore)
ADD_SUBDIRECTORY(strategy_engine/WtCtaStraFact)
ADD_SUBDIRECTORY(strategy_engine/WtSelStraFact)

# 3. Trade execution
ADD_SUBDIRECTORY(trade_execution/TraderXTP)
ADD_SUBDIRECTORY(trade_execution/WtExeFact)
ADD_SUBDIRECTORY(trade_execution/WtPorter)
ADD_SUBDIRECTORY(trade_execution/WtRunner)

# 4. Strategy backtest
ADD_SUBDIRECTORY(backtest/WtBtCore)
ADD_SUBDIRECTORY(backtest/WtBtPorter)
ADD_SUBDIRECTORY(backtest/WtBtRunner)

# Explicitly exclude non-stock / non-ETF modules from this build
# The old WonderTrader modules for futures, options, unrelated brokers and utility scaffolds
# are intentionally not included in this A-share / ETF-only project.

