#include "MarkDownGenerator.h"
#include "Utils.h"


#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <iostream>

namespace EMDOC {

	std::map<std::string, size_t> BranchPriorityMap = {
		{ "class", 0 },
		{ "struct", 1 },
		{ "variable", 2 },
		{ "function", 3 },
	};

	std::string GenerateMDTable(const CLT::Table& table)
	{
		if (table.entries.empty()) return "";

		CLT::Table condencedTable;
		condencedTable.name = "";
		condencedTable.fields = table.fields;
		condencedTable.format = table.format;

		std::vector<bool> used;
		used.resize(table.fields.size(), false);
		for (size_t i = 0; i < table.entries.size(); ++i) {
			if (!table.entries[i].text.empty()) used[i % table.fields.size()] = true;
			else used[i % table.fields.size()] = false;
		}
		size_t unusedCount = 0;
		for (size_t i = 0; i < used.size(); ++i) {
			if (!used[i]) {
				condencedTable.fields.erase(condencedTable.fields.begin() + i - unusedCount);
				unusedCount += 1;
			}
		}
		for (size_t i = 0; i < table.entries.size(); ++i) {
			if (!used[i % table.fields.size()]) continue;
			condencedTable.entries.push_back(table.entries[i]);
		}

		std::string str = CLT::GenerateTableString(condencedTable);
		size_t width = CLT::CalculateTableWidth(condencedTable);
		size_t start = (width + 1) * 3;
		size_t end = str.size() - (width + 1);
		str = std::string(str.begin() + start, str.begin() + end);
		return str;
	}

	std::string GenerateClassMD(const CLT::DataBranch& docTree, size_t depth)
	{
		std::stringstream outputBuffer;
		std::string header;
		header.resize(depth, '#');
		outputBuffer << header << " " << (std::string)docTree.children.at("name").data << " <sub>*";
		outputBuffer << (std::string)docTree.children.at("branch_type").data << "*</sub>\n";
		outputBuffer << (std::string)docTree.children.at("description").data << "\n";

		outputBuffer << GenerateBranchMD(docTree.children.at("children"), depth);
		outputBuffer << "\n---\n\n";
		return outputBuffer.str();
	}

	void GenerateVariableMD(CLT::Table& variableTable, const CLT::DataBranch& docTree, size_t depth)
	{
		variableTable << docTree.children.at("name").data;
		variableTable << "`" + (std::string)docTree.children.at("type").data + "`";
		variableTable << docTree.children.at("description").data;
		variableTable << docTree.children.at("default_value").data;
		return;
	}

	std::string GenerateFunctionMD(const CLT::DataBranch& docTree, size_t depth)
	{
		std::stringstream outputBuffer;
		std::string header;
		header.resize(depth, '#');
		outputBuffer << header << " " << (std::string)docTree.children.at("name").data << " <sub>*function*</sub>\n";
		outputBuffer << (std::string)docTree.children.at("description").data << "\n";
		outputBuffer << "```cpp\n" << (std::string)docTree.children.at("function_definition").data << "\n```\n";
		
		CLT::Table parameterTable = CLT::CreateTable("", { "Name", "Type", "Description", "Default Value" }, { '|', '-', '|' });
		for (const auto& [name, branch] : docTree.children.at("parameters").children) {
			GenerateVariableMD(parameterTable, branch, depth + 1);
		}
		if (!parameterTable.entries.empty()) {
			outputBuffer << header << "# Parameters\n";
			outputBuffer << GenerateMDTable(parameterTable) << "\n";
		}
		if (!docTree.children.at("return_description").data.IsNull()) {
			outputBuffer << "Return\n: " << (std::string)docTree.children.at("return_description").data << "\n";
		}

		return outputBuffer.str();
	}

	std::string GenerateBranchMD(const CLT::DataBranch& docTree, size_t depth)
	{
		std::vector<CLT::DataBranch> Ordered;
		std::vector<size_t> positions;
		positions.resize(BranchPriorityMap.size() + 1, 0);

		for (const auto& [name, branch] : docTree.children) {
			std::string type = branch.children.at("branch_type").data;
			Ordered.insert(Ordered.begin() + positions[BranchPriorityMap[type]], branch);
			for (size_t i = BranchPriorityMap[type] + 1; i < positions.size(); ++i) positions[i] += 1;
		}

		std::string outputBuffer;
		CLT::Table variableTable = CLT::CreateTable("", { "Name", "Type", "Description", "Default Value" }, { '|', '-', '|' });
		for (const auto& branch : Ordered) {
			std::string type = branch.children.at("branch_type").data;
			if (type == "class" || type == "struct") {
				outputBuffer.append(GenerateClassMD(branch, depth + 1));
			}
			else if (type == "variable") {
				GenerateVariableMD(variableTable, branch, depth + 1);
			}
			else if (type == "function") {
				outputBuffer.append(GenerateFunctionMD(branch, depth + 1));
			}
		}
		if (variableTable.entries.empty()) return outputBuffer;

		std::string tableStr;
		tableStr.resize(depth + 1, '#');
		tableStr += " Variables\n";
		tableStr += GenerateMDTable(variableTable);
		outputBuffer = tableStr + outputBuffer;

		return outputBuffer;
	}

	void GenerateMarkdown(const CLT::DataBranch& docTree, const Config& config)
	{
		for (const auto& [filename, branch] : docTree.children) {
			// Get a new path for the file
			std::string name = std::filesystem::path(filename).filename().string();
			size_t start = name.find_last_of(".");
			name = std::string(name.begin(), name.begin() + start);
			std::string relativePath = std::filesystem::path(filename).parent_path().string();
			relativePath = std::string(relativePath.begin() + config.inputPath.size(), relativePath.end());
			relativePath = config.outputPath + "\\" + relativePath;
			std::string newPath = relativePath + name + ".md";
			
			// Create/Open file
			std::ofstream file;
			file.open(newPath);
			if (!file.is_open()) {
				WARNING("Failed to create/open file ( " + newPath + " )");
				continue;
			}

			file << GenerateBranchMD(branch["content"], 0);

			file.close();

			std::cout << "Wrote file ( " << filename << " ) to file ( " << newPath << " )\n";
		}
	}
}