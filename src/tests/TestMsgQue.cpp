#include <atomic>

namespace
{
	std::atomic<unsigned long> published_count{0};
	std::atomic<unsigned long> destroyed_count{0};
}

extern "C"
{
	unsigned long create_server(const char*)
	{
		return 1;
	}

	void destroy_server(unsigned long)
	{
		++destroyed_count;
	}

	void publish_message(unsigned long, const char*, const char*, unsigned long)
	{
		++published_count;
	}

	void regiter_callbacks(void (*)(unsigned long, const char*, bool))
	{
	}

	unsigned long test_published_count()
	{
		return published_count.load();
	}

	unsigned long test_destroyed_count()
	{
		return destroyed_count.load();
	}
}