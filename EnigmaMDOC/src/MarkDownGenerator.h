#pragma once
#include "FileParser.h"
#include <CommandLineTools/IOFormat.h>
#include <CommandLineTools/JSONparser.h>
#include <string>
#include <vector>

namespace EMDOC {

	
	extern std::map<std::string, size_t> BranchPriorityMap; // @EMDOC DEF Used for organizing the differant branches based on there type

	std::string GenerateMDTable(const CLT::Table& table);
	std::string GenerateClassMD(const CLT::DataBranch& docTree, size_t depth);
	void GenerateVariableMD(CLT::Table& variableTable, const CLT::DataBranch& docTree, size_t depth);
	std::string GenerateFunctionMD(const CLT::DataBranch& docTree, size_t depth);
	std::string GenerateBranchMD(const CLT::DataBranch& docTree, size_t depth);

	/*
	@EMDOC DEF Generates MarkDown source code from a JSON tree
	@EMDOC PARAM The JSON tree
	@EMDOC PARAM The configuration that was loaded in from file
	@EMDOC NDEF
	*/
	void GenerateMarkdown(const CLT::DataBranch& docTree, const Config& config);

}