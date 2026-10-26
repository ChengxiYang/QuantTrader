
#include "trade_execution/QtRunner/QtRunner.h"

#include "WTSTools/WTSLogger.h"

#include "Share/cppcli.hpp"

//#include <vld.h>

int main(int argc, char* argv[])
{
	cppcli::Option opt(argc, argv);
	auto lParam = opt("-l", "--logcfg", "logging configuration filepath, logcfg.yaml as default", false);
	auto cParam = opt("-c", "--config", "configuration filepath, config.yaml as default", false);

	auto hParam = opt("-h", "--help", "gain help doc", false)->asHelpParam();

	opt.parse();

	if (hParam->exists())
		return 0;

	std::string filename;
	if (lParam->exists())
		filename = lParam->get<std::string>();
	else
		filename = "./logcfg.yaml";

	QtRunner runner;
	runner.init(filename);

	if (cParam->exists())
		filename = cParam->get<std::string>();
	else
		filename = "./config.yaml";

	if (!runner.config(filename))
		return 1;

	runner.run(false);
	return 0;
}