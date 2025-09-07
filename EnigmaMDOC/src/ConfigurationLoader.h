#pragma once
#include <vector>
#include <string>

namespace EMDOC {

	// @EMDOC DEF The configurations for the MarkDown document generator
	struct Config {
		std::string outputPath; // @EMDOC ELEM The folder final MarkDown documents get stored too
		std::string inputPath;  // @EMDOC ELEM The folder that the generator will read files from

		std::vector<std::string> extensions; // @EMDOC ELEM The file extensions that the generator will read from

		std::string commandTag; // @EMDOC ELEM The tag that the generator looks for in the source code comments 
	};
	// @EMDOC NDEF

	Config LoadConfigFile();
}