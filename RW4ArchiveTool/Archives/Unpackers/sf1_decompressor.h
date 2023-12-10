// EA Skate SF Parser By GHFear.
#pragma once
#include "../Compression/refpack/refpackd.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/BETools.h"

namespace sf
{
    void sf_decompress_type1(char* stream_file_bytearray, const char* filename, CollectionAsset streamfile_header, const wchar_t* directory, uint32_t sf_header_size)
    {
        std::wstring Filedirectory = directory;
        Filedirectory += ConvertCharToWchar(filename);

        // Declare local variables.
        size_t decompressed_size = 0;
        uint32_t stream_index = 0;
        FILE* file = nullptr;

        stream_index = sf_header_size;

        // Open the file in write mode to overwrite or create an empty file
        if (_wfopen_s(&file, Filedirectory.c_str(), L"wb+") == 0) {
            if (file != nullptr) {
                fclose(file);
                printf("File overwritten or created as empty: %s\n", filename);
            }
            else {
                fprintf(stderr, "Error opening file for writing.\n");
            }
        }
        else {
            fprintf(stderr, "Error opening file.\n");
        }

        // Unpack loop
        for (size_t i = 0; i < 1; i++)
        {
            // Create decompression in-buffer.
            uint64_t decompression_in_buffer_size = dword_big_to_little_endian(streamfile_header.Size);

            // Decompress inbuffer to get decompressed out buffer and size.
            std::vector<uint8_t> decompression_in_buffer_vector(stream_file_bytearray + stream_index, (stream_file_bytearray + stream_index) + decompression_in_buffer_size);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
            decompressed_size = decompression_out_buffer_vector.size();

            // Write (append) to file.
            if (_wfopen_s(&file, Filedirectory.c_str(), L"ab+") == 0)
            {
                if (file != nullptr) {
                    size_t bytesWritten = fwrite(decompression_out_buffer_vector.data(), sizeof(char), decompressed_size, file);
                    if (bytesWritten != decompressed_size) {
                        fprintf(stderr, "Error writing to file.\n");
                    }
                    fclose(file);
                }
                else {
                    fprintf(stderr, "Error opening file for append.\n");
                }
            }
            else {
                fprintf(stderr, "Error opening file.\n");
            }
        }

        return;
    }
}

