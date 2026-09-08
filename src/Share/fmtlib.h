#pragma once

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif
// ycx 去掉原来老版spdlog的日志文件库路径，改为新版路径
// #include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/fmt.h>

namespace fmt
{
	template <>
	struct formatter<wtp::WTSKlinePeriod> : formatter<const char*>
	{
		auto format(wtp::WTSKlinePeriod period, format_context& ctx) const
		{
			const char* label = "unknown";
			static constexpr const char* names[] = {"tick", "min1", "min5", "day", "week", "month"};
			const auto idx = static_cast<int>(period);
			if (idx >= 0 && idx < static_cast<int>(sizeof(names) / sizeof(names[0])))
			{
				label = names[idx];
			}
			return formatter<const char*>::format(label, ctx);
		}
	};
}

namespace fmtutil
{
	template<typename... Args>
	inline char* format_to(char* buffer, const char* format, const Args& ...args)
	{
		char* s = fmt::format_to(buffer, format, args...);
		s[0] = '\0';
		return s;
	}

	template<int BUFSIZE=512, typename... Args>
	inline const char* format(const char* format, const Args& ...args)
	{
		thread_local static char buffer[BUFSIZE];
		char* s = fmt::format_to(buffer, format, args...);
		s[0] = '\0';
		return buffer;
	}
}
