// EA Skate EB Parser By GHFear.

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "../../Unspecified/Global.h"
#include "sf_header_parser.h"

namespace big
{
    struct BigEBArchiveHeader {
        DWORD Magic;
        DWORD FILE_AMOUNT;
        DWORD UNK;
        DWORD NAMES_OFFSET; //Keep checking until not null for first string.
        DWORD NAMES_SIZE;
        BYTE FILENAME_LENGTH;
        BYTE FOLDERNAME_LENGTH;
        WORD FOLDER_AMOUNT;
        uint64_t ARCHIVE_SIZE;
    };

    int round_up(int numToRound, int multiple)
    {
        if (multiple == 0)
            return numToRound;

        int remainder = abs(numToRound) % multiple;
        if (remainder == 0)
            return numToRound;

        if (numToRound < 0)
            return -(abs(numToRound) - remainder);
        else
            return numToRound + multiple - remainder;
    }

    bool CreateDirectoryRecursively(const wchar_t* path) {
        // Attempt to create the directory
        if (CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
            return true;
        }

        // If creation failed and the error is because the directory already exists, return true
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            return true;
        }

        // If the error is because of missing parent directories, create them recursively
        if (GetLastError() == ERROR_PATH_NOT_FOUND) {
            size_t pos = std::wstring(path).find_last_of(L"\\/");
            if (pos != std::wstring::npos) {
                std::wstring parentPath = std::wstring(path).substr(0, pos);
                if (!CreateDirectoryRecursively(parentPath.c_str())) {
                    return false;
                }

                // Retry creating the directory after creating the parent directories
                return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
            }
        }

        return false;
    }

    bool unpack_big_eb_archive(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
    {
        FILE* file = nullptr;

        // Allocate memory for our file buffer
        char* file_buffer = (char*)malloc(SIZE);
        if (file_buffer == NULL) {
            perror("Error allocating memory");
            fclose(archive);
            return false;
        }

        fread(file_buffer, SIZE, 1, archive); // Read file into buffer

        // Attempt to create the directory
        if (CreateDirectoryRecursively(Filedirectory.c_str())) {
            wprintf(L"Directory created: %s\n", Filedirectory.c_str());
        }
        else {
            wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
        }

        // Write to file.
        if (_wfopen_s(&file, Filepath.c_str(), L"wb+") != 0)
        {
            fprintf(stderr, "Error opening file.\n");
            free(file_buffer);
            return false;
        }

        // Check if nullptr
        if (file == nullptr) {
            fprintf(stderr, "Error opening file for write.\n");
            fclose(file);
            free(file_buffer);
            return false;
        }

        //  Write and check if we wrote all bytes.
        size_t bytesWritten = fwrite(file_buffer, sizeof(char), SIZE, file);
        if (bytesWritten != SIZE) {
            fprintf(stderr, "Error writing to file.\n");
            fclose(file);
            free(file_buffer);
            return false;
        }

        fclose(file);
        free(file_buffer);
        return true;
    }

    // Function to parse files from a big eb archive. (Doesn't decompress)
    std::vector<Archive_Parse_Struct> parse_big_eb_archive(const wchar_t* archiveName, const wchar_t* directory, bool unpack) {
        // Declare local variables.
        FILE* archive = nullptr;
        BigEBArchiveHeader archive_header = {};
        std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
        std::vector<DWORD> offset_Vector;
        std::vector<DWORD> size_Vector;
        DWORD folders_offset = 0;

        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return Archive_Parse_Struct_vector;
        }
        
        // Read archive header.
        fread(&archive_header, sizeof(archive_header), 1, archive);

        // Set folder offset.
        folders_offset = dword_big_to_little_endian(archive_header.FILE_AMOUNT);
        folders_offset *= archive_header.FILENAME_LENGTH;
        folders_offset += dword_big_to_little_endian(archive_header.NAMES_OFFSET);
        folders_offset = round_up(folders_offset, 0x10);
        archive_header.FILENAME_LENGTH -= 2;

        fseek(archive, 0x30, SEEK_SET);
        for (size_t i = 0; i < dword_big_to_little_endian(archive_header.FILE_AMOUNT); i++)
        {
            // Declare local variables.
            DWORD offset = 0;
            DWORD zero = 0;
            DWORD size = 0;
            DWORD hash = 0;

            // Read offset.
            fread(&offset, sizeof(offset), 1, archive);
            offset = dword_big_to_little_endian(offset);
            offset *= 0x10;

            // Read an unknown value.
            fread(&zero, sizeof(zero), 1, archive);
            zero = dword_big_to_little_endian(zero);

            // Read size.
            fread(&size, sizeof(size), 1, archive);
            size = dword_big_to_little_endian(size);

            // Read hash.
            fread(&hash, sizeof(hash), 1, archive);
            hash = dword_big_to_little_endian(hash);

            // Push values into vector.
            offset_Vector.push_back(offset);
            size_Vector.push_back(size);
        }

        fseek(archive, dword_big_to_little_endian(archive_header.NAMES_OFFSET), SEEK_SET);
        for (size_t i = 0; i < dword_big_to_little_endian(archive_header.FILE_AMOUNT); i++)
        {
            // Declare local variables.
            Archive_Parse_Struct Parsed_Archive_Struct = {};
            std::wstring out_filepath = directory;
            std::wstring out_filedirectory = directory;
            std::vector<char> name_buffer(archive_header.FILENAME_LENGTH);
            std::vector<char> folder_buffer(archive_header.FOLDERNAME_LENGTH);
            DWORD offset = offset_Vector[i];
            DWORD size = size_Vector[i];
            WORD folder_number = 0;

            // Read the folder number.
            fread(&folder_number, sizeof(folder_number), 1, archive);
            folder_number = word_big_to_little_endian(folder_number);

            // Read and build directory. (Stage 1: Filename) 
            fread(name_buffer.data(), archive_header.FILENAME_LENGTH, 1, archive);
            std::string name(name_buffer.begin(), std::find(name_buffer.begin(), name_buffer.end(), '\0'));
            std::string filename = name;

            DWORD next_name_offset = ftell(archive); // Save the current location as next name offset location.

            DWORD foldername_offset = folder_number;
            foldername_offset *= archive_header.FOLDERNAME_LENGTH;
            foldername_offset += folders_offset;

            fseek(archive, foldername_offset, SEEK_SET); // Seek to foldername location in archive.

            // Read and build directory. (Stage 2: directory)
            fread(folder_buffer.data(), archive_header.FOLDERNAME_LENGTH, 1, archive);
            std::string folder(folder_buffer.begin(), std::find(folder_buffer.begin(), folder_buffer.end(), '\0'));
            std::string final_extracted_filepath = folder;
            final_extracted_filepath += "/";
            final_extracted_filepath += name;

            fseek(archive, offset, SEEK_SET); // Seek to file offset location.

            // Prepare final wide character strings. (Stage 3: Build proper path)
            out_filedirectory += ConvertCharToWchar(folder.c_str());
            std::replace(out_filepath.begin(), out_filepath.end(), L'/', L'\\');
            std::replace(out_filedirectory.begin(), out_filedirectory.end(), L'/', L'\\');
            out_filepath += ConvertCharToWchar(final_extracted_filepath.c_str());

            // Set Parsed_Archive struct members.
            memcpy(Parsed_Archive_Struct.filename, filename.c_str(), sizeof(filename));
            Parsed_Archive_Struct.file_size = size;
            Parsed_Archive_Struct.file_offset = offset;
            Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);
            
            // Check if we want to unpack.
            if (unpack)
            {
                //Check if we could unpack successfully.
                if (unpack_big_eb_archive(archive, size, out_filedirectory, out_filepath) != true)
                {
                    fclose(archive);
                    return Archive_Parse_Struct_vector;
                }
            }
            
            fseek(archive, next_name_offset, SEEK_SET); // Seek to the saved next name location.
        }

        // Close the archive.
        fclose(archive);

        return Archive_Parse_Struct_vector;
    }
}
