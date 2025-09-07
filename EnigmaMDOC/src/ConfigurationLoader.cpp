#include "ConfigurationLoader.h"
#include "Utils.h"

#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include <iostream>
#include <CommandLineTools/IOFormat.h>

namespace EMDOC {

	Config LoadConfigFile()
	{
		// Open the config file
		std::ifstream configFile;
		configFile.open("../config.EMDOC");
		ASSERT(configFile.is_open(), "Please create a \"config.EMDOC\" file");

		// Move file content to a string stream for easier parsing
		std::stringstream fileContentBuffer;
		fileContentBuffer << configFile.rdbuf();
		configFile.close();

		// Remove white spaces from the file content
		std::string fileContent = RemoveWhiteSpace(fileContentBuffer.str());

		// Find all of the config variables specified in the config file
		std::map<std::string, std::string> configVariables;
		
		size_t start = 0;
		size_t end = 0;
		while ((end = fileContent.find_first_of("=", start)) != std::string::npos) {
			// Get variable name
			std::string configVarName(fileContent.begin() + start, fileContent.begin() + end);
			start = end + 1;
			// Get variable value
			end = fileContent.find_first_of("\n\0", start);
			std::string configVarValue(fileContent.begin() + start, fileContent.begin() + end);
			ASSERT(!configVarValue.empty(), "Invalid config file format: Expected value for ( " + configVarName + " )");
			start = end + 1;

			// If variable was already defined then don't redefine it
			if (configVariables.count(configVarName)) {
				WARNING("Variable ( " + configVarName + " ) is already defined, using first definition");
				continue;
			}

			// Store the variable
			configVariables.insert({ configVarName, configVarValue });
		}
		ASSERT(fileContent.find_first_of("\n\0", start) == std::string::npos, "Invalid config file format: Expected equal sign \"=\"");

		// Make sure the crucial variables are set
		ASSERT(configVariables.count("output_path"), "No output path set, Please set the variable \"output_path\" in the config.EMDOC file");
		ASSERT(configVariables.count("input_path"),  "No input path set, Please set the variable \"input_path\" in the config.EMDOC file");

		// Create the config struct
		Config config;
		config.outputPath = configVariables["output_path"];
		config.inputPath  = configVariables["input_path"];

		// TODO: just use json bro
		//config.extensions.push_back(".c");
		config.extensions.push_back(".h");
		//config.extensions.push_back(".cpp");
		config.extensions.push_back(".hpp");

		config.commandTag = "@EMDOC";

		return config;
	}

}