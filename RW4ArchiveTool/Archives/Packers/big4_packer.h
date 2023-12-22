// EA Skate BIG4 / BIGF Packer By GHFear.
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>  // Include this for fs::file_size
#include "refpack/refpackd.h"
#include "IoTools.h"
#include "EAHashes.h"
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

	struct BH_Header
	{
		char id[4];
		uint32_t size; // Same as headersize for BH (Viv4)
		uint32_t file_count;
		uint32_t header_size;
	};

	struct BH_TOC_Hash32
	{
		uint32_t offset;
		uint32_t size;
		uint32_t decompressed_size;
		uint32_t hash;
	};

	struct BH_TOC_Hash64
	{
		uint32_t offset;
		uint32_t size;
		uint32_t decompressed_size;
		uint64_t hash;
	};

	struct BH_Footer
	{
		uint32_t archive_size;
		uint32_t unk1;
		uint32_t unk2;
		uint32_t unk3;
	};

	struct Big4Packer_Settings
	{
		bool Big4CompressionCheckState = false;
		bool Big4IsBigFCheckState = false;
		bool IsHash64CheckState = false;
	};

	const char* Big4_Magic_Setting[2] = { "BIG4", "BIGF" };

	uint64_t get_big4_toc_index_size(const std::wstring& input_wstring)
	{
		return 8 + input_wstring.length() + 1;
	}

	bool addPadding(std::ofstream& outfile)
	{
		static const uint8_t padding = 0;
		uint8_t alignment = 64;
		size_t current_offset = outfile.tellp();

		if (current_offset % alignment == 0) {
			// No påadding needed.
			return false;
		}
		else {
			size_t padding_size = alignment - (current_offset % alignment);
			for (size_t i = 0; i < padding_size; i++)
			{
				outfile.write(reinterpret_cast<const char*>(&padding), 1);
			}
			return true;
		}
	}

	bool BuildBHVIV4(const std::vector<Big4Toc>& Big4Toc_vector, std::string BigHeaderPath, uint32_t archive_end_position, bool IsHash64)
	{
		std::ofstream outfile(BigHeaderPath, std::ios::binary);

		if (!outfile.is_open()) {
			std::cerr << "Error: Unable to open the archive file." << std::endl;
			return false;
		}

		// Local Vars
		uint32_t header_size = 16;
		uint32_t toc_size = 16 * Big4Toc_vector.size();
		uint32_t footer_size = 16;
		uint32_t file_count = Big4Toc_vector.size();

		if (IsHash64 == true)
		{
			toc_size = 20 * Big4Toc_vector.size();
		}

		// BH Header
		BH_Header BHHeader = {};
		std::memcpy(BHHeader.id, "Viv4", 4); // Set ID
		BHHeader.size = header_size + toc_size + footer_size; // Set size (Little endian)
		BHHeader.file_count = BigToLittleUINT(file_count); // Set file count (Big endian)
		BHHeader.header_size = BigToLittleUINT(BHHeader.size); // Set header size. (Big endian)
		outfile.write(reinterpret_cast<const char*>(&BHHeader), sizeof(BHHeader)); // Write header

		if (IsHash64 == true)
		{
			// BH TOC 64
			BH_TOC_Hash64 BH_TOC = {};
			for (size_t i = 0; i < Big4Toc_vector.size(); i++)
			{
				uint64_t namehash64 = EAHashes::djb2_hash_64(Big4Toc_vector[i].filename.c_str());
				BH_TOC.offset = Big4Toc_vector[i].offset;
				BH_TOC.size = Big4Toc_vector[i].size;
				BH_TOC.decompressed_size = 0;
				BH_TOC.hash = BigToLittleUINT(namehash64);

				outfile.write(reinterpret_cast<const char*>(&BH_TOC.offset), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.size), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.decompressed_size), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.hash), 8);
			}
		}
		else
		{
			// BH TOC 32
			BH_TOC_Hash32 BH_TOC = {};
			for (size_t i = 0; i < Big4Toc_vector.size(); i++)
			{
				uint32_t namehash32 = EAHashes::djb2_hash_32(Big4Toc_vector[i].filename.c_str());
				BH_TOC.offset = Big4Toc_vector[i].offset;
				BH_TOC.size = Big4Toc_vector[i].size;
				BH_TOC.decompressed_size = 0;
				BH_TOC.hash = BigToLittleUINT(namehash32);

				outfile.write(reinterpret_cast<const char*>(&BH_TOC.offset), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.size), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.decompressed_size), 4);
				outfile.write(reinterpret_cast<const char*>(&BH_TOC.hash), 4);
			}
		}

		// BH Footer
		BH_Footer footer = {};
		footer.archive_size = BigToLittleUINT(archive_end_position);
		footer.unk1 = 0;
		footer.unk2 = 0;
		footer.unk3 = 0;

		// Write BH footer
		outfile.write(reinterpret_cast<const char*>(&footer), sizeof(footer));

		// Close file
		outfile.close();
		return true;
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

	void update_header_section(const uint32_t& header_end_position, const uint32_t& archive_end_position, std::ofstream& outfile)
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
		return RESULTS{ Big4Toc_vector, toc_offset_offset_vector };
	}

	void update_toc_section(std::vector<Big4Toc>& Big4Toc_vector, std::vector<uint32_t> toc_offset_offset_vector, std::vector<uint32_t> offset_vector, std::ofstream& outfile)
	{

		for (size_t i = 0; i < toc_offset_offset_vector.size(); i++)
		{
			outfile.seekp(toc_offset_offset_vector[i], std::ios::beg);
			uint32_t offset = BigToLittleUINT(offset_vector[i]);
			Big4Toc_vector[i].offset = offset;
			outfile.write(reinterpret_cast<const char*>(&offset), sizeof(4));
		}

		return;
	}

	Big4HeaderTailSettings write_settings_section(std::ofstream& outfile, bool Big4CompressionCheckState)
	{
		uint16_t bIsCompressed = 0;
		if (Big4CompressionCheckState == true) {
			bIsCompressed = 1;
		}
		else {
			bIsCompressed = 0;
		}

		Big4HeaderTailSettings settings = {};
		std::memcpy(settings.Version, "L282", 4);
		settings.DJBHashKey = BigToLittleUINT((uint16_t)5381);  // Replace with your desired value
		settings.bIsCompressed = BigToLittleUINT(bIsCompressed); // Replace with your desired value
		outfile.write(reinterpret_cast<const char*>(&settings), sizeof(settings));

		return settings;
	}

	bool bundlebig4(std::vector<std::wstring> filepaths_wstring, std::wstring selected_path, std::wstring save_bigfile_path, Big4Packer_Settings big4_packer_settings)
	{
		std::vector<Big4Toc> Big4Toc_vector = {};
		std::wstring top_level_path = removeLastFolder(selected_path);
		std::vector<std::string> filepaths = {};
		std::vector<std::string> toc_compatible_filepaths = {};
		std::vector<uint32_t> toc_size_offset_vector = {};
		std::vector<uint32_t> toc_offset_offset_vector = {};
		std::vector<uint32_t> offset_vector = {};
		std::string OutArchivePath = WideStringToString(save_bigfile_path);
		std::string BigHeaderPath = replaceFileExtension(WideStringToString(save_bigfile_path), ".bh");


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

		std::ofstream outfile(OutArchivePath, std::ios::binary);

		if (!outfile.is_open()) {
			std::cerr << "Error: Unable to open the archive file." << std::endl;
			return false;
		}

		// Write Header.
		Big4Header header = write_init_header_section(toc_compatible_filepaths, big4_packer_settings.Big4IsBigFCheckState, outfile);

		// Write TOC entries.
		auto Big4Toc_RESULT = write_toc_section(toc_compatible_filepaths, outfile, filepaths);
		Big4Toc_vector = Big4Toc_RESULT.Big4Toc_vector;

		// Write Settings.
		Big4HeaderTailSettings settings = write_settings_section(outfile, big4_packer_settings.Big4CompressionCheckState);

		// Save header size.
		uint32_t header_end_position = static_cast<uint32_t>(outfile.tellp());

		// Add padding to the next index evenly divisible by 64 (if we need to)
		addPadding(outfile);

		// Write files to archive.
		for (size_t i = 0; i < filepaths.size(); i++) {
			uint32_t file_offset = static_cast<uint32_t>(outfile.tellp());
			offset_vector.push_back(file_offset);

			if (!isFileEmpty(filepaths[i]))
			{
				std::ifstream infile(filepaths[i], std::ios::binary);
				if (infile.is_open()) {
					outfile << infile.rdbuf();
					infile.close();
				}
				else {
					std::cerr << "Error: Unable to open file: " << filepaths[i] << std::endl;
					return false;
				}

				// Add padding to the next index evenly divisible by 64 (if we need to)
				if (i < filepaths.size() - 1)
				{
					addPadding(outfile);
				}
			}
		}

		// Save archive size.
		uint32_t archive_end_position = static_cast<uint32_t>(outfile.tellp());

		// Update header and toc with saved information.
		update_header_section(header_end_position, archive_end_position, outfile);
		update_toc_section(Big4Toc_vector, Big4Toc_RESULT.toc_offset_offset_vector, offset_vector, outfile);

		// Close filestream.
		outfile.close();

		// Write header file.
		BuildBHVIV4(Big4Toc_vector, BigHeaderPath, archive_end_position, big4_packer_settings.IsHash64CheckState);

		std::cout << "Archive created successfully: " << OutArchivePath << std::endl;

		return true;
	}

}