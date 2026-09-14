#include <dlfcn.h>
#include <initializer_list>
#include <iostream>
#include <string>

static bool checkLibrary(const std::string& path, std::initializer_list<const char*> symbols)
{
	void* library = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (library == nullptr)
	{
		std::cerr << path << ": " << dlerror() << '\n';
		return false;
	}

	for (const char* symbolName : symbols)
	{
		dlerror();
		dlsym(library, symbolName);
		if (const char* error = dlerror())
		{
			std::cerr << path << ": " << error << '\n';
			dlclose(library);
			return false;
		}
	}

	if (dlclose(library) != 0)
	{
		std::cerr << path << ": " << dlerror() << '\n';
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Expected the XTP module directory\n";
		return 1;
	}

	const std::string directory = argv[1];
	if (!checkLibrary(directory + "/libTraderXTP.so", {"createTrader", "deleteTrader"}) ||
		!checkLibrary(directory + "/libxtptraderapi.so",
			{"_ZN3XTP3API9TraderApi15CreateTraderApiEhPKc13XTP_LOG_LEVEL"}))
		return 1;

	return 0;
}