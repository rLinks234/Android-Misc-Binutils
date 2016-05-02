#include <boost/program_options.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace po = boost::program_options;

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

inline po::options_description getHelpDescription() {

	po::options_description help("Usage Options");
	help.add_options()
			("version", "print version string")
			("help,?", "produce help message")
			("usage", "produce help message")
	;

	return help;

}

inline bool hasHelpOptions(po::variables_map& vm) {
	return vm.count("help") || vm.count("usage");
}

inline bool hasVersion(po::variables_map& vm) {
	return vm.count("version") > 0;
}

static void printUsage(po::options_description& pGeneral, const char* pExtra) {

	cout << "\nUsage:\n" << pGeneral << endl;
	printf(pExtra);

}