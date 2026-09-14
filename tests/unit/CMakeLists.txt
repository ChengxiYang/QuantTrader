add_library(WtTestMsgQue SHARED TestMsgQue.cpp)
set_target_properties(WtTestMsgQue PROPERTIES
	OUTPUT_NAME WtMsgQue
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(EventNotifierTest EventNotifierTest.cpp)
target_include_directories(EventNotifierTest PRIVATE ${INCS})
target_link_directories(EventNotifierTest PRIVATE ${LNKS})
target_link_libraries(EventNotifierTest PRIVATE
	WtCore WTSTools WTSUtils WtTestMsgQue Boost::filesystem Boost::thread pthread ${CMAKE_DL_LIBS})

foreach(scenario uninitialized idle drain)
	add_test(NAME EventNotifier.${scenario} COMMAND EventNotifierTest ${scenario})
	set_tests_properties(EventNotifier.${scenario} PROPERTIES
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		TIMEOUT 10)
endforeach()

if(TARGET TraderXTP)
	add_executable(XtpModuleTest XtpModuleTest.cpp)
	target_link_libraries(XtpModuleTest PRIVATE ${CMAKE_DL_LIBS})
	add_dependencies(XtpModuleTest TraderXTP WtRunner WtPorter)

	add_test(NAME XTP.module COMMAND XtpModuleTest "$<TARGET_FILE_DIR:TraderXTP>")
	set_tests_properties(XTP.module PROPERTIES TIMEOUT 10)
	foreach(runtime WtRunner WtPorter)
		add_test(NAME XTP.${runtime} COMMAND XtpModuleTest "$<TARGET_FILE_DIR:${runtime}>/traders")
		set_tests_properties(XTP.${runtime} PROPERTIES TIMEOUT 10)
	endforeach()
endif()