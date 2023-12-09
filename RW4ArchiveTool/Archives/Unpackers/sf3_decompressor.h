// EA Skate SF Parser By GHFear.
#pragma once
#include "../Compression/refpack/refpackd.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/BETools.h"

namespace sf
{
    struct SFCompressionType3 {
        DWORD CompressionHeaderSize;
        DWORD AmountCompressedChunks;
        DWORD CompressionBool;
        DWORD Unknown1;
        DWORD Unknown2;
        DWORD Unknown3;
        DWORD Unknown4;
        DWORD Unknown5;
        std::vector<DWORD> CompressedBlockSizes;
    };

    void sf_decompress_type3(char* stream_file_bytearray, const char* filename, const wchar_t* directory, uint32_t sf_header_size)
    {
        std::wstring concatenatedText = directory;
        concatenatedText += ConvertCharToWchar(filename);

        // Declare local variables.
        SFCompressionType3 compressionheader;
        size_t decompressed_size = 0;
        uint64_t stream_index = 0;
        FILE* file = nullptr;

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader, stream_file_bytearray + sf_header_size, 32);

        stream_index = sf_header_size + 32;

        // Create the CompressedBlockSizes vector.
        for (size_t i = 0; i < (dword_big_to_little_endian(compressionheader.CompressionHeaderSize) - 32) / 4; i++)
        {
            DWORD TempCompressedBlockSize = 0;
            std::memcpy(&TempCompressedBlockSize, stream_file_bytearray + stream_index, sizeof(DWORD));
            TempCompressedBlockSize = TempCompressedBlockSize;
            compressionheader.CompressedBlockSizes.push_back(TempCompressedBlockSize);
            stream_index += 4;
        }

        // Open the file in write mode to overwrite or create an empty file
        if (_wfopen_s(&file, concatenatedText.c_str(), L"wb+") == 0) {
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
        for (size_t i = 0; i < dword_big_to_little_endian(compressionheader.AmountCompressedChunks); i++)
        {
            // Create decompression in-buffer.
            uint64_t decompression_in_buffer_size = dword_big_to_little_endian(compressionheader.CompressedBlockSizes[i]);

            // Decompress inbuffer to get decompressed out buffer and size.
            std::vector<uint8_t> decompression_in_buffer_vector(stream_file_bytearray + stream_index, (stream_file_bytearray + stream_index) + decompression_in_buffer_size);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
            decompressed_size = decompression_out_buffer_vector.size();

            // Write (append) to file.
            if (_wfopen_s(&file, concatenatedText.c_str(), L"ab+") == 0)
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

            // Move one compressed chunk forward
            stream_index += decompression_in_buffer_size;
        }

        return;
    }
}
