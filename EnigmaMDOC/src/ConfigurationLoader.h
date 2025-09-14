#pragma once
#include <vector>
#include <map>
#include <string>

namespace EMDOC {

	enum class FileType
	{
		None = 0,
		CnCPP,    // C / CPP
		BASH,     // Bash
		PYTHON    // Python
	};
	FileType GetFileLanguageType(const std::string& extension);

	// @EMDOC DEF The configurations for the MarkDown document generator
	struct Config {
		std::string outputPath; // @EMDOC DEF The folder final MarkDown documents get stored too
		std::string inputPath;  // @EMDOC DEF The folder that the generator will read files from

		std::map<std::string, FileType> extensions; // @EMDOC DEF The file extensions that the generator will read from

		std::string commandTag; // @EMDOC DEF The tag that the generator looks for in the source code comments 
		bool createJSONInt; // @EMDOC DEF Tells the generator to output the intermedeat JSON to a file
	};
	// @EMDOC NDEF

	Config LoadConfigFile();

	void PrintConfig(const Config& config);
}