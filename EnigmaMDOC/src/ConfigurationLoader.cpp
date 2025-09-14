#include "ConfigurationLoader.h"
#include "Utils.h"

#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include <iostream>
#include <CommandLineTools/IOFormat.h>
#include <CommandLineTools/JSONparser.h>

namespace EMDOC {
	FileType GetFileLanguageType(const std::string& extension)
	{
		std::string lower = extension;
		for (char& c : lower) c = tolower(c);

		if (lower == "c++"  || lower == "cpp" || lower == "c") return FileType::CnCPP;
		if (lower == "bash" || lower == "shell")               return FileType::BASH;
		if (lower == "py"   || lower == "python")              return FileType::PYTHON;
		
		return FileType::None;
	}

	Config LoadConfigFile()
	{
		// Load JSON config file
		CLT::DataBranch tree;
		CLT::JSONError error = CLT::JSON::LoadJSON(tree, "EMDOC.json");
		if (CLT::GetJSONErrorType(error) == CLT::JSONErrorType::FileFailedToOpen) {
			ERROR("Failed to load config file, please create a EMDOC.json file");
		}
		else if (error) {
			ERROR(CLT::JSONErrorMessage(error));
		}

		// Make sure the essential variables were set
		ASSERT(!tree["input_path"].data.IsNull(),  "Invalid configuration, please set the \"input_path\" variable");
		ASSERT(!tree["output_path"].data.IsNull(), "Invalid configuration, please set the \"output_path\" variable");
		ASSERT(!tree["extensions"].data.IsNull(),  "Invalid configuration, please set the \"extensions\" array variable");

		// Create the config struct
		Config config;
		config.inputPath  = tree["input_path"].data;
		config.outputPath = tree["output_path"].data;
		config.createJSONInt = tree["create_JSON_int"].data;

		if (!tree["command_tag"].data.IsNull()) config.commandTag = tree["command_tag"].data;
		else config.commandTag = "@EMDOC";

		// Copy the extensions to the config struct
		for (size_t i = 0; i < tree["extensions"].elements.size(); ++i) {
			if (tree["extensions"].elements[i].elements.size() != 2) continue;
			std::string extension = tree["extensions"].elements[i].elements[0].data;
			FileType type = GetFileLanguageType(tree["extensions"].elements[i].elements[1].data);
			if (type == FileType::None) continue;
			config.extensions.insert({ extension, type });
		}

		return config;
	}

	void PrintConfig(const Config& config)
	{
		// Create table with the config data
		CLT::Table configTable = CLT::CreateTable("Configuration", { "Variable", "Value" });

		configTable << "Input Path";
		configTable << config.inputPath;
		configTable << "Output Path";
		configTable << config.outputPath;

		configTable << "Command Tag";
		configTable << config.commandTag;

		configTable << "File Extensions";
		//configTable << config.extensions;

		// Print the table
		CLT::PrintTable(configTable);
		std::cout << std::endl;
	}

}