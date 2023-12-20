// EA Skate BIG4 / BIGF Parser By GHFear.
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>  // Include this for fs::file_size
#include "refpack/refpackd.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"

namespace big4
{

	namespace fs = std::filesystem;

	struct Big4Toc
	{
		uint32_t offset;
		uint32_t size;
		std::string filename;
		uint8_t zero_terminator;
	};

	const char* Big4_Magic_Setting[2] = {"BIG4", "BIGF"};

	uint64_t get_big4_toc_index_size(const std::wstring& input_wstring)
	{
		return 8 + input_wstring.length() + 1;
	}

	void addPadding(std::ofstream& outfile, size_t paddingSize) 
	{
		static const uint8_t padding = 0;

		for (size_t i = 0; i < paddingSize; i++)
		{
			outfile.write(reinterpret_cast<const char*>(&padding), 1);
		}
	}

	Big4Header write_init_header_section(std::vector<std::string>& toc_compatible_filepaths, bool Big4IsBigFCheckState, std::ofstream& outfile)
	{
		Big4Header header = {};
		if (Big4IsBigFCheckState == true) {
			std::memcpy(header.magic, Big4_Magic_Setting[1], 4);
		}
		else {
			std::memcpy(header.magic, Big4_Magic_Setting[0], 4);
		}

		header.length = sizeof(Big4Header);
		header.number_files = BigToLittleUINT((uint32_t)toc_compatible_filepaths.size());
		header.header_length = sizeof(Big4Header) + toc_compatible_filepaths.size() * sizeof(Big4Toc) + sizeof(Big4HeaderTailSettings);
		outfile.write(reinterpret_cast<const char*>(&header), sizeof(Big4Header));

		return header;
	}

	void update_header_section(const uint32_t& header_end_position, const uint32_t& archive_end_position,  std::ofstream& outfile )
	{	
		uint32_t archive_length = archive_end_position;
		uint32_t header_length = BigToLittleUINT(header_end_position);
		outfile.seekp(4, std::ios::beg);
		outfile.write(reinterpret_cast<const char*>(&archive_length), sizeof(4));
		outfile.seekp(12, std::ios::beg);
		outfile.write(reinterpret_cast<const char*>(&header_length), sizeof(4));

		return;
	}

	auto write_toc_section(std::vector<std::string>& toc_compatible_filepaths, std::ofstream& outfile, std::vector<std::string> filepaths)
	{
		std::vector<uint32_t> toc_offset_offset_vector = {};
		std::vector<Big4Toc> Big4Toc_vector = {};
		struct RESULTS { std::vector<Big4Toc> Big4Toc_vector; std::vector<uint32_t> toc_offset_offset_vector; };

		for (size_t i = 0; i < filepaths.size(); i++) {
			std::streampos currentPosition = outfile.tellp();
			uint32_t currentPositionUint32 = static_cast<uint32_t>(currentPosition);
			toc_offset_offset_vector.push_back(currentPositionUint32);

			Big4Toc toc;
			toc.offset = 0;
			toc.size = BigToLittleUINT((uint32_t)fs::file_size(filepaths[i]));
			toc.filename = toc_compatible_filepaths[i];
			toc.zero_terminator = 0;
			Big4Toc_vector.push_back(toc);

			// Convert filename to a byte buffer
			const char* filenameBytes = toc.filename.c_str();
			size_t filenameSize = toc.filename.size();
			std::vector<uint8_t> filenameBuffer(filenameBytes, filenameBytes + filenameSize);

			// Write TOC entry
			outfile.write(reinterpret_cast<const char*>(&toc.offset), sizeof(toc.offset));
			outfile.write(reinterpret_cast<const char*>(&toc.size), sizeof(toc.size));
			outfile.write(reinterpret_cast<const char*>(filenameBuffer.data()), filenameBuffer.size());
			outfile.write(reinterpret_cast<const char*>(&toc.zero_terminator), sizeof(toc.zero_terminator));
		}
		return RESULTS { Big4Toc_vector, toc_offset_offset_vector };
	}

	void update_toc_section(std::vector<uint32_t> toc_offset_offset_vector, std::vector<uint32_t> offset_vector, std::ofstream& outfile)
	{

		for (size_t i = 0; i < toc_offset_offset_vector.size(); i++)
		{
			outfile.seekp(toc_offset_offset_vector[i], std::ios::beg);
			uint32_t offset = BigToLittleUINT(offset_vector[i]);
			outfile.write(reinterpret_cast<const char*>(&offset), sizeof(4));
		}

		return;
	}

	Big4HeaderTailSettings write_settings_section(std::ofstream& outfile, bool Big4CompressionCheckState)
	{
		uint16_t bIsCompressed = 0;
		if (Big4CompressionCheckState == true)
		{
			bIsCompressed = 1;
		}
		else
		{
			bIsCompressed = 0;
		}

		Big4HeaderTailSettings settings = {};
		std::memcpy(settings.Version, "L282", 4);
		settings.DJBHashKey = BigToLittleUINT((uint16_t)5381);  // Replace with your desired value
		settings.bIsCompressed = BigToLittleUINT(bIsCompressed); // Replace with your desired value
		outfile.write(reinterpret_cast<const char*>(&settings), sizeof(settings));

		return settings;
	}

	bool bundlebig4(std::vector<std::wstring> filepaths_wstring, std::wstring selected_path, bool Big4CompressionCheckState, std::wstring save_bigfile_path, bool Big4IsBigFCheckState)
	{
		
		std::wstring top_level_path = removeLastFolder(selected_path);
		std::vector<std::string> filepaths = {};
		std::vector<std::string> toc_compatible_filepaths = {};
		std::vector<uint32_t> toc_offset_offset_vector = {};
		std::vector<uint32_t> offset_vector = {};
		std::string archivePath = WideStringToString(save_bigfile_path);
		

		for (size_t i = 0; i < filepaths_wstring.size(); i++)
		{
			std::wstring TOC_Compatible_Path = filepaths_wstring[i];
			size_t pos = TOC_Compatible_Path.find(top_level_path);

			if (pos != std::wstring::npos) {
				TOC_Compatible_Path = make_big_compatible_paths(pos, top_level_path, TOC_Compatible_Path);
			}
			else {
				std::wcout << L"Substring not found." << std::endl;
			}
			toc_compatible_filepaths.push_back(WideStringToString(TOC_Compatible_Path));
			filepaths.push_back(WideStringToString(filepaths_wstring[i]));
		}

		std::ofstream outfile(archivePath, std::ios::binary);

		if (!outfile.is_open()) {
			std::cerr << "Error: Unable to open the archive file." << std::endl;
			return false;
		}

		// Write Header.
		Big4Header header = write_init_header_section(toc_compatible_filepaths, Big4IsBigFCheckState, outfile);

		// Write TOC entries.
		auto Big4Toc_vector = write_toc_section(toc_compatible_filepaths, outfile, filepaths);

		// Write Settings.
		Big4HeaderTailSettings settings = write_settings_section(outfile, Big4CompressionCheckState);

		// Save header size.
		uint32_t header_end_position = static_cast<uint32_t>(outfile.tellp());
		
		// Add padding to the next index evenly divisible by 64.
		size_t paddingSize = 64 - (outfile.tellp() % 64);
		addPadding(outfile, paddingSize);

		// Write files to archive.
		for (const auto& filepath : filepaths) {

			std::ifstream infile(filepath, std::ios::binary);
			if (infile.is_open()) {
				uint32_t file_offset = static_cast<uint32_t>(outfile.tellp());
				offset_vector.push_back(file_offset);
				outfile << infile.rdbuf();
				infile.close();
			}
			else {
				std::cerr << "Error: Unable to open file: " << filepath << std::endl;
				return false;
			}

			// Add padding to the next index evenly divisible by 64
			paddingSize = 64 - (outfile.tellp() % 64);
			addPadding(outfile, paddingSize);
		}

		// Save archive size.
		uint32_t archive_end_position = static_cast<uint32_t>(outfile.tellp());

		// Update header and toc with saved information.
		update_header_section(header_end_position, archive_end_position, outfile);
		update_toc_section(Big4Toc_vector.toc_offset_offset_vector, offset_vector, outfile);

		// Close filestream.
		outfile.close();
		std::cout << "Archive created successfully: " << archivePath << std::endl;

		return true;
	}

}