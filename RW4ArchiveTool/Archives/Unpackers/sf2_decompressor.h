// EA Skate SF Parser By GHFear.
#pragma once
#include "../Compression/refpack/refpackd.h"
#include <iostream>
#include <fstream>
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/BETools.h"

namespace sf
{
    struct SFCompressionType2 {
        DWORD HeaderDecompressedSize;
        DWORD HeaderCompressedSize;
        DWORD HeaderCompressedBool;
        DWORD Unknown1;
        DWORD Unknown2[4];
        DWORD BodyDecompressedSize;
        DWORD BodyCompressedSize;
        DWORD BodyCompressedBool;
        DWORD Unknown3;
        DWORD Unkonwn4[4];
        DWORD Unkonwn5[4];
    };

    bool sf_decompress_type2_loop(FILE* file, DWORD HeaderCompressedBool, DWORD CompressedSize, DWORD DecompressedSize, const char* filename, char* stream_file_bytearray, uint32_t* stream_index, const wchar_t* directory)
    {
        std::wstring Filedirectory = directory;
        Filedirectory += ConvertCharToWchar(filename);

        if (dword_big_to_little_endian(HeaderCompressedBool) == 0)
        {

            uint64_t out_buffer_size = dword_big_to_little_endian(DecompressedSize);
            char* current_file_location = stream_file_bytearray + *stream_index;

            if (_wfopen_s(&file, Filedirectory.c_str(), L"ab+") != 0)
            {
                fprintf(stderr, "Error opening file.\n");
                return false;
            }

            if (file != nullptr) {
                size_t bytesWritten = fwrite(current_file_location, sizeof(char), out_buffer_size, file);
                if (bytesWritten != dword_big_to_little_endian(DecompressedSize)) {
                    fprintf(stderr, "Error writing to file.\n");
                    return false;
                }
                fclose(file);
            }
            else {
                fprintf(stderr, "Error opening file for append.\n");
                return false;
            }

            return true;
        }
        else
        {
            // Create decompression in-buffer.
            uint64_t decompression_in_buffer_size = dword_big_to_little_endian(CompressedSize);

            // Decompress inbuffer to get decompressed out buffer and size.
            std::vector<uint8_t> decompression_in_buffer_vector(stream_file_bytearray + *stream_index, stream_file_bytearray + *stream_index + decompression_in_buffer_size);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);

            // Write (append) to file.
            if (_wfopen_s(&file, Filedirectory.c_str(), L"ab+") != 0)
            {
                fprintf(stderr, "Error opening file.\n");
                return false;
            }

            if (file != nullptr) {
                size_t bytesWritten = fwrite(decompression_out_buffer_vector.data(), sizeof(char), dword_big_to_little_endian(DecompressedSize), file);
                if (bytesWritten != dword_big_to_little_endian(DecompressedSize)) {
                    fprintf(stderr, "Error writing to file.\n");
                    return false;
                }
                fclose(file);
            }
            else {
                fprintf(stderr, "Error opening file for append.\n");
                return false;
            }

            return true;
        }

        return false;
    }

    void sf_decompress_type2(char* stream_file_bytearray, const char* filename, const wchar_t* directory, uint32_t sf_header_size)
    {
        // Declare local variables.
        SFCompressionType2 compressionheader;
        size_t decompressed_size = 0;
        uint32_t decompression_header_size = 80;
        uint32_t stream_index = 0;
        uint32_t* stream_index_ptr = &stream_index;
        FILE* file = nullptr;

        std::wstring Filedirectory = directory;
        Filedirectory += ConvertCharToWchar(filename);

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader, stream_file_bytearray + sf_header_size + 0, decompression_header_size);

        *stream_index_ptr = sf_header_size + decompression_header_size;

        // Open the file in write mode to overwrite or create an empty file
        if (_wfopen_s(&file, Filedirectory.c_str(), L"wb+") != 0) {
            fprintf(stderr, "Error opening file.\n");
        }

        if (file != nullptr) {
            fclose(file);
            printf("File overwritten or created as empty: %s\n", filename);
        }
        else {
            fprintf(stderr, "Error opening file for writing.\n");
        }

        if (dword_big_to_little_endian(compressionheader.HeaderCompressedSize) != 0 || dword_big_to_little_endian(compressionheader.HeaderDecompressedSize) != 0)
        { 
            sf_decompress_type2_loop(
                file,
                compressionheader.HeaderCompressedBool,
                compressionheader.HeaderCompressedSize,
                compressionheader.HeaderDecompressedSize,
                filename,
                stream_file_bytearray,
                stream_index_ptr,
                directory);
        }

        stream_index += dword_big_to_little_endian(compressionheader.HeaderCompressedSize);

        if (dword_big_to_little_endian(compressionheader.BodyCompressedSize) != 0  || dword_big_to_little_endian(compressionheader.BodyDecompressedSize) != 0)
        {  
            sf_decompress_type2_loop(
                file,
                compressionheader.BodyCompressedBool,
                compressionheader.BodyCompressedSize,
                compressionheader.BodyDecompressedSize,
                filename,
                stream_file_bytearray,
                stream_index_ptr,
                directory);
        }

        return;
    }
}

