// EA Skate BIG4 / BIGF Parser By GHFear.
#pragma once
#include <filesystem>
#include "Archives/Compression/refpack/refpackd.h"
#include "IoTools.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"

namespace big4
{
  
  struct Big4Header
  {
    char magic[4];
    uint32_t length;
    uint32_t number_files;
    uint32_t header_length;
  };

  struct Big4Fat
  {
    uint32_t offset;
    uint32_t size;
    char filename[256];
  };

  struct Big4HeaderTailSettings
  {
    char Version[4];
    uint16_t DJBHashKey;
    uint16_t bIsCompressed;
  };

  struct ParsedBig4Struct
  {
    Big4Header header;
    std::vector<Big4Fat> toc;
    Big4HeaderTailSettings tail_settings;
  };

  bool unpack_empty_file(FILE* archive, std::wstring Filedirectory, std::wstring Filepath)
  {
    FILE* file = nullptr;

    // Attempt to create the directory
    if (CreateDirectoryRecursively(Filedirectory.c_str())) {
      wprintf(L"Directory created: %s\n", Filedirectory.c_str());
    }
    else {
      wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
    }

    // Create empty file.
    if (!IoTools::create_file(file, Filepath)) {
      return false;
    }

    return true;
  }

  bool unpack_uncompressed_file(FILE* archive, DWORD size, std::wstring Filedirectory, std::wstring Filepath)
  {
    FILE* file = nullptr;

    // Allocate memory for our file buffer
    char* file_buffer = (char*)malloc(size);
    if (file_buffer == NULL) {
      perror("Error allocating memory");
      return false;
    }

    // Read file into buffer
    fread(file_buffer, size, 1, archive); 

    // Attempt to create the directory
    if (CreateDirectoryRecursively(Filedirectory.c_str())) {
      wprintf(L"Directory created: %s\n", Filedirectory.c_str());
    }
    else {
      wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
    }

    // Write to file.
    if (!IoTools::write_file(file, Filepath, (char*)file_buffer, size)) {
      free(file_buffer);
      return false;
    }

    free(file_buffer);
    return true;
  }

  bool unpack_refpack_file(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
  {
    FILE* file = nullptr;

    // Allocate memory for our file buffer
    char* file_buffer = (char*)malloc(SIZE);
    if (file_buffer == NULL) {
      perror("Error allocating memory");
      return false;
    }

    // Read file into buffer
    fread(file_buffer, SIZE, 1, archive); 

    // Decompress file
    std::vector<uint8_t> decompression_in_buffer_vector(file_buffer, file_buffer + SIZE);
    std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
    size_t decompressed_size = decompression_out_buffer_vector.size();

    // Attempt to create the directory
    if (CreateDirectoryRecursively(Filedirectory.c_str())) {
      wprintf(L"Directory created: %s\n", Filedirectory.c_str());
    }
    else {
      wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
    }

    // Write to file.
    if (!IoTools::write_file(file, Filepath, (char*)decompression_out_buffer_vector.data(), decompressed_size)) {
      free(file_buffer);
      return false;
    }

    free(file_buffer);
    return true;
  }

  // Function to unpack files from a big4 archive.
  auto parse_big4_archive(const wchar_t* archiveName, bool unpack, int64_t selected_file_index) 
  {
    struct RESULT { std::vector<Archive_Parse_Struct>  parsed_info; bool success; }; // Return type

    // Declare local variables.
    FILE* archive = nullptr;
    std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
    ParsedBig4Struct parsed_big4_struct = {};
    Big4Header big4_header = {};

    // Open archive.
    _wfopen_s(&archive, archiveName, L"rb");
    if (archive == NULL) {
      perror("Error opening archive");
      return RESULT{ Archive_Parse_Struct_vector , false };
    }

    // Save start position.
    uint64_t start_of_archive = _ftelli64(archive);

    // Save archive size and go back to start position.
    fseek(archive, 0, SEEK_END);
    uint64_t archive_size = _ftelli64(archive);
    fseek(archive, start_of_archive, SEEK_SET);

    // Read header into struct.
    fread(&big4_header, sizeof(big4_header), 1, archive);
    big4_header.header_length = BigToLittleUINT(big4_header.header_length);
    //big4_header.length = BigToLittleUINT(big4_header.length); // This value is already little endian for some reason.
    big4_header.number_files = BigToLittleUINT(big4_header.number_files);

    if (big4_header.header_length > archive_size || 
        big4_header.length > archive_size || big4_header.number_files > archive_size) {
      fclose(archive);
      return RESULT{ Archive_Parse_Struct_vector , false };
    }

    parsed_big4_struct.header = big4_header;

    for (size_t i = 0; i < big4_header.number_files; i++)
    {
      std::wstring wide_archiv_path = archiveName;
      std::wstring directory = ParseFilePath(wide_archiv_path).first;
      std::wstring full_out_filepath = directory;
      std::wstring full_out_file_directory = directory;
      Archive_Parse_Struct Parsed_Archive_Struct = {};

      // Save location.
      uint32_t next_toc_offset = 0;
      next_toc_offset = _ftelli64(archive);
      Parsed_Archive_Struct.toc_offset = next_toc_offset; // Set current toc offset.

      // Get file information and convert to little endian.
      Big4Fat big4_fat = {};
      fread(&big4_fat, sizeof(big4_fat), 1, archive);
      big4_fat.offset = BigToLittleUINT(big4_fat.offset);
      big4_fat.size = BigToLittleUINT(big4_fat.size);
      parsed_big4_struct.toc.push_back(big4_fat);

      // Check value bounds.
      if (big4_fat.size > archive_size || big4_fat.offset > archive_size) {
        fclose(archive);
        return RESULT{ Archive_Parse_Struct_vector , false };
      }

      // Get filename length.
      size_t filename_length = 0;
      filename_length = std::strlen(big4_fat.filename);

      // Calculate the next toc index offset.
      next_toc_offset = next_toc_offset + sizeof(big4_fat.offset) + sizeof(big4_fat.size) + filename_length + 1;

      // Convert the string to a filesystem path.
      std::filesystem::path path(big4_fat.filename);

      // Build full path and directory.
      full_out_file_directory += path.parent_path().wstring();
      full_out_filepath += (path.parent_path().wstring() + L"/" + path.filename().wstring());
      std::replace(full_out_file_directory.begin(), full_out_file_directory.end(), L'/', L'\\');
      std::replace(full_out_filepath.begin(), full_out_filepath.end(), L'/', L'\\');

      // Seek to file offset, read first 4 bytes and go back to file offset.
      _fseeki64(archive, big4_fat.offset, SEEK_SET);
      uint16_t compression_type = 0;
      fread(&compression_type, sizeof(compression_type), 1, archive);
      compression_type = BigToLittleUINT(compression_type); // Convert ztype to little endian.
      _fseeki64(archive, big4_fat.offset, SEEK_SET); // Seek back to offset

      // Set Parsed_Archive struct members.
      Parsed_Archive_Struct.filename = path.string();
      Parsed_Archive_Struct.file_size = big4_fat.size;
      Parsed_Archive_Struct.file_offset = big4_fat.offset;

      // Parse and unpack conditions.
      bool full_archive_unpack = unpack && selected_file_index == -1; // Do a full archive unpack.
      bool single_archive_unpack = unpack && selected_file_index == i; // Do a single file archive unpack.

      // Set compression type string for UI.
      if (compression_type == 0x10FB) {
        strcpy_s(Parsed_Archive_Struct.ztype, "REFPACK");
      }
      else {
        strcpy_s(Parsed_Archive_Struct.ztype, "NONE");
      }

      // Run the unpacking functions.
      if ((unpack == true && compression_type == 0x10FB && big4_fat.size > 0) && (full_archive_unpack || single_archive_unpack)) { // Compressed file unpacking.
        if (!unpack_refpack_file(archive, big4_fat.size, full_out_file_directory, full_out_filepath)) {
          fclose(archive);
          return RESULT{ Archive_Parse_Struct_vector , false };
        }
      }
      else if ((unpack == true && compression_type != 0x10FB && big4_fat.size > 0) && (full_archive_unpack || single_archive_unpack)) { // Uncompressed file unpacking.
        if (!unpack_uncompressed_file(archive, big4_fat.size, full_out_file_directory, full_out_filepath)) {
          fclose(archive);
          return RESULT{ Archive_Parse_Struct_vector , false };
        }
      }
      else if ((unpack == true && big4_fat.size == 0) && (full_archive_unpack || single_archive_unpack)) { // Empty file unpacking.
        if (!unpack_empty_file(archive, full_out_file_directory, full_out_filepath)) {
          fclose(archive);
          return RESULT{ Archive_Parse_Struct_vector , false };
        }
      }

      Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);
      _fseeki64(archive, next_toc_offset, SEEK_SET);
    }

    // Get parse the last settings details.
    Big4HeaderTailSettings big4header_tail_settings = {};
    fread(&big4header_tail_settings, sizeof(big4header_tail_settings), 1, archive);
    big4header_tail_settings.bIsCompressed = BigToLittleUINT(big4header_tail_settings.bIsCompressed);
    big4header_tail_settings.DJBHashKey = BigToLittleUINT(big4header_tail_settings.DJBHashKey);
    parsed_big4_struct.tail_settings = big4header_tail_settings;

    fclose(archive);
    return RESULT{ Archive_Parse_Struct_vector, true };
  }
}