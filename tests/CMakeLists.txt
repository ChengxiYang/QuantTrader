add_subdirectory(backtest/WtBtCore)
add_subdirectory(backtest/WtBtPorter)
add_subdirectory(backtest/WtBtRunner)

if(BUILD_TESTING)
	add_subdirectory(unit)
endif()