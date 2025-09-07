#include "FileParser.h"
#include "Utils.h"

#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <iostream>

namespace EMDOC {
	
	std::vector<CommentBlock> FindCommentBlocksCPP(const std::string& filename, const Config& config)
	{
		std::string fileContent = LoadFileStr(filename);

		std::vector<CommentBlock> blocks;

		size_t start = 0;
		size_t end = 0;

		bool cont = false;
		while ((start = std::min(fileContent.find("//", start), fileContent.find("/*", start))) != std::string::npos) {

			// Get end of block
			if (std::string(fileContent.begin() + start, fileContent.begin() + start + 2) == "//") {
				end = fileContent.find_first_of("\n", start);
				if (end == std::string::npos) end = fileContent.size();
			}
			else {
				end = fileContent.find("*/", start) + 2;
				if (end == std::string::npos) end = fileContent.size();
			}

			// Create new block if not continuing from the last block
			if (!cont) {
				CommentBlock block;
				block.start = start;
				blocks.push_back(block);
			}

			// Update block data
			CommentBlock& block = blocks[blocks.size() - 1];
			block.end = end;
			block.text.append(std::string(fileContent.begin() + start + 2, fileContent.begin() + block.end));
			
			start = end + 1;

			if (fileContent.find_first_not_of(" \n\r\t\0", block.end) == fileContent.find_first_of("/", block.end)) {
				cont = true;
			}
			// Split the comment block based on the command blocks, if the block is done
			else {
				cont = false;

				// Find all command tags
				CommentBlock Whole = blocks[blocks.size() - 1];
				blocks.pop_back();
				std::string text = std::string(fileContent.begin() + Whole.start, fileContent.begin() + Whole.end);
				std::vector<size_t> tags;
				end = 0;
				while ((end = text.find(config.commandTag, end)) != std::string::npos) tags.push_back(end++);

				if (tags.size() == 0) continue;

				for (size_t i = 0; i < tags.size(); ++i) {
					CommentBlock left;
					left.start = Whole.start + tags[i];
					if (i + 1 < tags.size()) left.end = Whole.start + tags[i + 1];
					else left.end = Whole.end;
					left.text = std::string(fileContent.begin() + left.start, fileContent.begin() + left.end);

					while((end = std::min(std::min(left.text.find("/*"), left.text.find("*/")), left.text.find("//"))) != std::string::npos)
					{
						std::string right = std::string(left.text.begin() + end + 2, left.text.end());
						left.text = std::string(left.text.begin(), left.text.begin() + end) + right;
					}

					blocks.push_back(left);
				}
			}
		}

		// Remove white spaces other than the space charactor from the comments
		for (CommentBlock& block : blocks) {
			start = 0;
			while ((start = block.text.find_first_of("\t\n\r\0", start)) != std::string::npos) {
				block.text.erase(block.text.begin() + start);
			}
		}

		return blocks;
	}

	std::vector<CommentBlock> FindCommentBlocks(const std::string& filename, const Config& config)
	{
		// Find the file's extension, skip it if it isn't one of the specified extensions
		std::string extension = std::filesystem::path(filename).extension().string();
		if (std::find(config.extensions.begin(), config.extensions.end(), extension) == config.extensions.end()) {
			return std::vector<CommentBlock>();
		}

		// Call the correct find comment blocks function based on the language the extension specifies
		if (extension == ".h" || extension == ".c" || extension == ".hpp" || extension == ".cpp") {
			return FindCommentBlocksCPP(filename, config);
		}

		return std::vector<CommentBlock>();
	}
	
}