// EA Skate SF Parser By GHFear.
#pragma once
#include "Archives/Unpackers/sf1_decompressor.h"
#include "Archives/Unpackers/sf2_decompressor.h"
#include "Archives/Unpackers/sf3_decompressor.h"


namespace sf
{

    // Function to unpack files from a sf archive. (Doesn't decompress)
    std::vector<Archive_Parse_Struct> parse_sf_archive(const wchar_t* archiveName, const wchar_t* directory, bool unpack) {
        // Declare local variables.
        FILE* archive = nullptr;
        long sfa_size = 0;
        long start_of_sfa = 0;
        uint32_t size_of_sfa_header = 0;
        std::vector<Archive_Parse_Struct> parse_struct_vector = {};

        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return parse_struct_vector;
        }

        // Save start position.
        start_of_sfa = ftell(archive);

        // Save archive size and go back to start position.
        fseek(archive, 0, SEEK_END);
        sfa_size = ftell(archive);
        fseek(archive, start_of_sfa, SEEK_SET);

        // Save archive headersize and go back to start position.
        fseek(archive, 16, SEEK_CUR);
        fread(&size_of_sfa_header, sizeof(DWORD), 1, archive);
        fseek(archive, start_of_sfa, SEEK_SET);

        // Read SF Archive header.
        size_of_sfa_header = dword_big_to_little_endian(size_of_sfa_header);
        SFArchiveHeader* sfa_header = (SFArchiveHeader*)malloc(sizeof(*sfa_header) + (size_of_sfa_header - 32));
        fread(sfa_header, size_of_sfa_header, 1, archive);

        // Main unpacking loop.
        while (ftell(archive) < sfa_size)
        {
            // Declare local variables.
            char char_filename[17];
            std::string filename = "";
            uint32_t streamfile_header_size = 0;
            long streamfile_startposition = 0;
            long stream_file_size = 0;
            Archive_Parse_Struct parse_struct = {};

            // Create our stream_file_header struct and read the header into it from our file.
            streamfile_startposition = ftell(archive);

            // Save headersize and go back to start position.
            fseek(archive, 12, SEEK_CUR);
            fread(&streamfile_header_size, sizeof(uint32_t), 1, archive);
            fseek(archive, streamfile_startposition, SEEK_SET);

            // Read SF Header.
            streamfile_header_size = dword_big_to_little_endian(streamfile_header_size);
            CollectionAsset* stream_file_header = (CollectionAsset*)malloc(sizeof(*stream_file_header) + (streamfile_header_size - 20));
            fread(stream_file_header, streamfile_header_size, 1, archive);

            // Set stream file block size
            stream_file_size = dword_big_to_little_endian(stream_file_header->Stride);

            // Set the filename
            bytearray_to_hexstring(stream_file_header->ID, sizeof(stream_file_header->ID), char_filename);
            filename = char_filename;
            const wchar_t* file_extension = GetFileExtension(archiveName);

            // Create byte array with size of file_size
            char* stream_file_bytearray = (char*)malloc(stream_file_size);
            if (stream_file_bytearray == NULL) {
                perror("Error allocating memory");
                fclose(archive);
                free(sfa_header);
                free(stream_file_header);
                free(stream_file_bytearray);
                return parse_struct_vector;
            }

            // Read current streamfile into a bytearray.
            fseek(archive, streamfile_startposition, SEEK_SET);
            fread(stream_file_bytearray, sizeof(char), stream_file_size, archive);

            // Add to parse struct vector
            parse_struct.filename = filename;
            parse_struct.file_size = stream_file_size;
            parse_struct.file_offset = streamfile_startposition;
            parse_struct.toc_offset = streamfile_startposition;
            parse_struct_vector.push_back(parse_struct);

            // Run this code if we set unpack to true when calling this function.
            if (!unpack) { goto dont_unpack_loc; }

            if (StrCmpCW(file_extension, L"psf") == 0)
            {
                filename += ".psg";
            }
            else if (StrCmpCW(file_extension, L"xsf") == 0)
            {
                filename += ".rx2";
            }
            else if (StrCmpCW(file_extension, L"wsf") == 0)
            {
                filename += ".rg2";
            }

            switch (dword_big_to_little_endian(sfa_header->StreamFormat))
            {
            case SF1:
                sf_decompress_type1(stream_file_bytearray, filename.c_str(), *stream_file_header, directory, dword_big_to_little_endian(stream_file_header->Offset));
                break;
            case SF2:
                sf_decompress_type2(stream_file_bytearray, filename.c_str(), directory, dword_big_to_little_endian(stream_file_header->Offset));
                break;
            case SF3:
                sf_decompress_type3(stream_file_bytearray, filename.c_str(), directory, dword_big_to_little_endian(stream_file_header->Offset));
                break;
            case UNKNOWN:
                break;
            default:
                break;
            }
            // End of unpacking-only code.

            // close file and free byte array memory.
            dont_unpack_loc:        
            free(stream_file_header);
            free(stream_file_bytearray);
        }

        // Close the archive.
        fclose(archive);
        free(sfa_header);

        return parse_struct_vector;
    }

    // Function to parse the SFA Header (Doesn't decompress)
    SFArchiveHeaderNotDynamic parse_sfa_header(const wchar_t* archiveName) {
        // Declare local variables.
        FILE* archive = nullptr;
        long sfa_size = 0;
        long start_of_sfa = 0;
        SFArchiveHeaderNotDynamic sfa_header = {};
        uint32_t size_of_sfa_header = 0;

        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return sfa_header;
        }

        // Save start position.
        start_of_sfa = ftell(archive);

        // Save filesize and go back to start position.
        fseek(archive, 0, SEEK_END);
        sfa_size = ftell(archive);
        fseek(archive, start_of_sfa, SEEK_SET);

        // Save headersize and go back to start position.
        fseek(archive, 16, SEEK_CUR);
        fread(&size_of_sfa_header, sizeof(DWORD), 1, archive);
        fseek(archive, start_of_sfa, SEEK_SET);

        // Read SFA header.
        size_of_sfa_header = dword_big_to_little_endian(size_of_sfa_header);
        fread(&sfa_header, sizeof(sfa_header), 1, archive);

        // Close the archive and free memory.
        fclose(archive);

        return sfa_header;
    }

}
