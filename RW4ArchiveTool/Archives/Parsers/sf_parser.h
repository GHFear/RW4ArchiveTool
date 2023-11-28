// EA Skate SF Parser By GHFear.

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "../../Unspecified/Global.h"
#include "sf_header_parser.h"

namespace sf
{

    // Function to unpack files from a sf archive. (Doesn't decompress)
    std::vector<Archive_Parse_Struct> parse_sf_archive(const wchar_t* archiveName, const wchar_t* directory, bool unpack, SFTYPE compression_type) {
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
            char filename[17];
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
            bytearray_to_hexstring(stream_file_header->ID, sizeof(stream_file_header->ID), filename);

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
            memcpy(parse_struct.filename, filename, sizeof(filename));
            parse_struct.file_size = stream_file_size;
            parse_struct.file_offset = streamfile_startposition;
            parse_struct_vector.push_back(parse_struct);

            // Unpack if we set the unpack boolean to true.
            if (unpack == true)
            {
                switch (compression_type)
                {
                case SF1:
                    sf_decompress_type1(stream_file_bytearray, filename, *stream_file_header, directory, dword_big_to_little_endian(stream_file_header->Offset));
                    break;
                case SF2:
                    sf_decompress_type2(stream_file_bytearray, filename, directory, dword_big_to_little_endian(stream_file_header->Offset));
                    break;
                case SF3:
                    sf_decompress_type3(stream_file_bytearray, filename, directory, dword_big_to_little_endian(stream_file_header->Offset));
                    break;
                case UNKNOWN:
                    break;
                default:
                    break;
                }
            }
            
            // close file and free byte array memory.
            free(stream_file_header);
            free(stream_file_bytearray);
        }

        // Close the archive.
        fclose(archive);
        free(sfa_header);

        return parse_struct_vector;
    }
}
