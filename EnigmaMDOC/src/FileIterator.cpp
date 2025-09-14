#include "FileIterator.h"
#include "FileParser.h"
//#include "MarkDownGenerator.h"
#include "CommentParser.h"
#include "Utils.h"

#include <CommandLineTools/IOFormat.h>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace EMDOC {
	void VarifyDirectories(const Config& config)
	{
		if (!std::filesystem::is_directory(config.inputPath)) {
			ERROR("Input path doesn't exist");
		}
		if (!std::filesystem::is_directory(config.outputPath)) {
			try { std::filesystem::create_directories(config.outputPath); }
			catch (const std::filesystem::filesystem_error& err) {
				ERROR(err.what());
			}
		}
	}

	std::vector<std::string> GetFilesToProcess(const std::string& currentPath, const Config& config)
	{
		std::vector<std::string> files;
		std::filesystem::recursive_directory_iterator dirIt(config.inputPath);
		for (const auto& entry : dirIt) {

			// If entry is a directory then iterate through the files in the directory
			if (entry.is_directory()) {
				std::vector<std::string> directoryFiles = GetFilesToProcess(entry.path().string(), config);
				files.insert(files.end(), directoryFiles.begin(), directoryFiles.end());
				continue;
			}

			// Only store the file if it has one of the specifed exensions
			std::string extension = entry.path().extension().string();
			if (config.extensions.count(extension)) {
				files.push_back(entry.path().string());
			}
		}

		return files;
	}

	bool FileConformation(const std::vector<std::string>& files)
	{
		// Prompt the user
		std::cout << "The following files will be processed:" << std::endl;
		for (const std::string& file : files) {
			std::cout << file << std::endl;
		}
		std::cout << std::endl << "Is this okay? y/n: ";
		
		// Get users response
		std::string response;
		std::getline(std::cin, response);
		for (char& c : response) c = tolower(c);

		return (response == "y" || response == "yes");
	}

	void ProcessFiles(const std::vector<std::string>& files, CLT::DataBranch& docTree, const Config& config)
	{
		for (size_t i = 0; i < files.size(); ++i) {
			std::string filename = files[i];
			std::vector<CommentBlock> commentBlocks = FindCommentBlocks(filename, config);
			if (commentBlocks.empty()) continue;

			//CLT::Table commentTable = CLT::CreateTable(filename.c_str(), { "Start", "End", { "Text", 75, CLT::FFF_FixedWidth | CLT::FFF_WrapText } });
			//for (auto& comment : commentBlocks) {
			//	commentTable << std::to_string(comment.start);
			//	commentTable << std::to_string(comment.end);
			//	commentTable << comment.text;
			//}
			//std::cout << CLT::GenerateTableString(commentTable);
			CLT::DataBranch fileBranch = CLT::DataBranchType::Object;
			fileBranch["filename"] = filename;
			ParserError error = ParseComments(fileBranch["content"], filename, commentBlocks, config);
			if (error) {
				WARNING(GetParserErrorMessage(error));
				continue;
			}
			docTree[filename] = fileBranch;
			//if (chunk.empty() || chunk.find_first_not_of(" \n\t\r\0") == std::string::npos) continue;

			// Get relative path
			//std::string parentPath = std::filesystem::path(filename).parent_path().string();
			//size_t start = config.inputPath.size();
			//size_t end = 0;
			//std::string relativePath = std::string(parentPath.begin() + start, parentPath.end()) + "\\";

			// Create new file name from relative path and the configs output path
			//start = filename.find_last_of("\\/") + 1;
			//end = filename.find_last_of(".");
			//std::string newFilename = std::string(filename.begin() + start, filename.begin() + end);
			//newFilename = config.outputPath + relativePath + newFilename + ".md";

			// Open/create the new file
			//std::ofstream file;
			//file.open(newFilename);
			//if (!file.is_open()) {
			//	WARNING("Failed to open/create file ( " + newFilename + " )");
			//	continue;
			//}

			//file << chunk;

			//std::cout << "Wrote ( " << filename << " ) to ( " << newFilename << " )" << std::endl << std::endl;

			//file.close();
		}
	}
}