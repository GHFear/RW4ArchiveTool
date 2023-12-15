// EA Skate BIG4 / BIGF Parser By GHFear.
#pragma once
#include <filesystem>
#include "Archives/Compression/refpack/refpackd.h"
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
        uint32_t offset, size;
        char filename[256];
    };


    bool unpack_uncompressed_file(FILE* archive, DWORD size, std::wstring Filedirectory, std::wstring Filepath)
    {
        FILE* file = nullptr;

        // Allocate memory for our file buffer
        char* file_buffer = (char*)malloc(size);
        if (file_buffer == NULL) {
            perror("Error allocating memory");
            return false;
        }

        fread(file_buffer, size, 1, archive); // Read file into buffer

        // Attempt to create the directory
        if (CreateDirectoryRecursively(Filedirectory.c_str())) {
            wprintf(L"Directory created: %s\n", Filedirectory.c_str());
        }
        else {
            wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
        }

        // Write to file.
        if (!IoTools::write_file(file, Filepath, (char*)file_buffer, size))
        {
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

        fread(file_buffer, SIZE, 1, archive); // Read file into buffer

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
        if (!IoTools::write_file(file, Filepath, (char*)decompression_out_buffer_vector.data(), decompressed_size))
        {
            free(file_buffer);
            return false;
        }

        free(file_buffer);
        return true;
    }

    // Function to unpack files from a big4 archive.
    auto parse_big4_archive(const wchar_t* archiveName, bool unpack, int64_t selected_file_index) {

        struct RESULT { std::vector<Archive_Parse_Struct>  parsed_info; bool success; };

        // Declare local variables.
        FILE* archive = nullptr;
        std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
        Big4Header big4_header = {};

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

        if (big4_header.header_length > archive_size || big4_header.length > archive_size || big4_header.number_files > archive_size)
        {
            fclose(archive);
            return RESULT{ Archive_Parse_Struct_vector , false };
        }

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

            if (big4_fat.size > archive_size || big4_fat.offset > archive_size)
            {
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
            Parsed_Archive_Struct.filename = path.filename().string();
            Parsed_Archive_Struct.file_size = big4_fat.size;
            Parsed_Archive_Struct.file_offset = big4_fat.offset;

            bool full_archive_unpack = unpack && selected_file_index == -1; // Do a full archive unpack.
            bool single_archive_unpack = unpack && selected_file_index == i; // Do a single file archive unpack.
            
            switch (compression_type)
            {
            case 0x10FB:
                strcpy_s(Parsed_Archive_Struct.ztype, "REFPACK");
                if (!unpack) 
                { 
                    goto dont_unpack_loc; 
                }
                else if (full_archive_unpack || single_archive_unpack)
                {
                    if (unpack_refpack_file(archive, big4_fat.size, full_out_file_directory, full_out_filepath) != true)
                    {
                        fclose(archive);
                        return RESULT{ Archive_Parse_Struct_vector , false };
                    }
                }
                break;
            default:
                strcpy_s(Parsed_Archive_Struct.ztype, "NONE");
                if (!unpack) 
                { 
                    goto dont_unpack_loc; 
                }
                else if (full_archive_unpack || single_archive_unpack)
                {
                    if (unpack_uncompressed_file(archive, big4_fat.size, full_out_file_directory, full_out_filepath) != true)
                    {
                        fclose(archive);
                        return RESULT{ Archive_Parse_Struct_vector , false };
                    }
                }
                
                break;
            }

            dont_unpack_loc:
            Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);

            _fseeki64(archive, next_toc_offset, SEEK_SET);
        }

        fclose(archive);
        return RESULT{ Archive_Parse_Struct_vector , true };
    }
}