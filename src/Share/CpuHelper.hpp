#pragma once
#include <thread>

class CpuHelper
{
public:
	static uint32_t get_cpu_cores()
	{
		static uint32_t cores = std::thread::hardware_concurrency();
		return cores;
	}

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
	static bool bind_core(uint32_t i)
	{
		int cores = get_cpu_cores();
		if (i >= cores)
			return false;

		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(i, &mask);
		return (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) >= 0);
	}
};