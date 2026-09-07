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

#basic libraries
ADD_SUBDIRECTORY(WTSUtils)
ADD_SUBDIRECTORY(WTSTools)

#parser for quote data from the XeleSkt channel
ADD_SUBDIRECTORY(ParserXeleSkt)
ADD_SUBDIRECTORY(WtDataStorage)
ADD_SUBDIRECTORY(WtDataStorageAD)
ADD_SUBDIRECTORY(WtDtCore)
ADD_SUBDIRECTORY(WtDtHelper)
ADD_SUBDIRECTORY(WtDtPorter)
ADD_SUBDIRECTORY(WtDtServo)
#quote date processor
ADD_SUBDIRECTORY(QuoteFactory)

#back test core library
ADD_SUBDIRECTORY(WtBtCore)
#back test c porter for outter libraries
ADD_SUBDIRECTORY(WtBtPorter)
ADD_SUBDIRECTORY(WtBtRunner)

ADD_SUBDIRECTORY(TraderATP)
ADD_SUBDIRECTORY(TraderXTP)
ADD_SUBDIRECTORY(WtExeFact)
ADD_SUBDIRECTORY(WtRiskMonFact)

ADD_SUBDIRECTORY(WtMsgQue)
ADD_SUBDIRECTORY(WtCore)
ADD_SUBDIRECTORY(WtPorter)
ADD_SUBDIRECTORY(WtExecMon)
ADD_SUBDIRECTORY(WtRunner)

ADD_SUBDIRECTORY(WtShareHelper)

#strategy factories
ADD_SUBDIRECTORY(WtCtaStraFact)
ADD_SUBDIRECTORY(WtSelStraFact)

