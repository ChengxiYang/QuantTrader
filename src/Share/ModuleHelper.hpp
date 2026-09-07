#pragma once
#include "../Share/StrUtil.hpp"

#include <dlfcn.h>
void inst_hlp() {}
static const std::string& getInstPath()
{
	static std::string moduleName;
	if (moduleName.empty())
	{
		Dl_info dl_info;
		dladdr((void *)inst_hlp, &dl_info);
		moduleName = dl_info.dli_fname;
	}

	return moduleName;
}

static const char* getBinDir()
{
	static std::string g_bin_dir;

	if (g_bin_dir.empty())
	{
		g_bin_dir = getInstPath();
		std::size_t nPos = g_bin_dir.find_last_of('/');
		g_bin_dir = g_bin_dir.substr(0, nPos + 1);
	}

	return g_bin_dir.c_str();
}