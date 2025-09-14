#pragma once
#include "ConfigurationLoader.h"

#include <CommandLineTools/JSONparser.h>
#include <vector>

namespace EMDOC {

	// @EMDOC DEF Makes sure the output folders exist @EMDOC NDEF
	void VarifyDirectories(const Config& config);

	// @EMDOC DEF Searches the input folder for files with the specified extinsions
	// @EMDOC RETURN Returns a list of paths
	// @EMDOC NDEF
	std::vector<std::string> GetFilesToProcess(const std::string& currentPath, const Config& config);
	bool FileConformation(const std::vector<std::string>& files);

	// @EMDOC DEF Creates a JSON tree with the documentation info from the files @EMDOC NDEF
	void ProcessFiles(const std::vector<std::string>& files, CLT::DataBranch& docTree, const Config& config);

}