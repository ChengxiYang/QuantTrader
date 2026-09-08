/*!
 * /file main.cpp
 * /project	WonderTrader
 *
 * /author Wesley
 * /date 2020/03/30
 * 
 * /brief 
 */

#include "WtRunner.h"

#include "../WTSTools/WTSLogger.h"

#include "../Share/cppcli.hpp"

//#include <vld.h>

int main(int argc, char* argv[])
{
	cppcli::Option opt(argc, argv);
	auto lParam = opt("-l", "--logcfg", "logging configure filepath, logcfgbt.yaml as default", false);

	auto hParam = opt("-h", "--help", "gain help doc", false)->asHelpParam();

	opt.parse();

	if (hParam->exists())
		return 0;

	std::string filename;
	if (lParam->exists())
		filename = lParam->get<std::string>();
	else
		filename = "./logcfg.yaml";

	WtRunner runner;
	runner.init(filename);

	if (cParam->exists())
		filename = cParam->get<std::string>();
	else
		filename = "./config.yaml";

	runner.config(filename);

	runner.run(false);
	return 0;
}

