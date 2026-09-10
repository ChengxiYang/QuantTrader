#include "../strategy_engine/WtCore/EventNotifier.h"
#include "../Includes/WTSVariant.hpp"

#include <iostream>
#include <string>

extern "C" unsigned long test_published_count();
extern "C" unsigned long test_destroyed_count();

int main(int argc, char* argv[])
{
	if (argc != 2)
		return 1;

	const std::string scenario = argv[1];
	if (scenario == "uninitialized")
	{
		wtp::EventNotifier notifier;
		return 0;
	}

	const unsigned long expected_count = scenario == "drain" ? 100 : 0;
	{
		wtp::EventNotifier notifier;
		auto* config = wtp::WTSVariant::createObject();
		config->append("active", true);
		config->append("url", "test://local");
		const bool initialized = notifier.init(config);
		config->release();
		if (!initialized)
		{
			std::cerr << "Notifier initialization failed\n";
			return 1;
		}

		for (unsigned long index = 0; index < expected_count; ++index)
			notifier.notify_event("test event");
	}

	if (test_published_count() != expected_count || test_destroyed_count() != 1)
	{
		std::cerr << "Notifier did not drain and close correctly\n";
		return 1;
	}
	return 0;
}