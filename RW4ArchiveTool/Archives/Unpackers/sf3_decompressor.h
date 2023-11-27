// EA Skate SF Parser By GHFear.
#pragma once

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

    void sf_decompress_type3(char* stream_file_bytearray, char* filename, const wchar_t* directory, uint32_t sf_header_size)
    {
        std::wstring concatenatedText = directory;
        concatenatedText += ConvertCharToWchar(filename);

        // Declare local variables.
        SFCompressionType3 compressionheader;
        size_t decompressed_size = 0;
        uint32_t stream_index = 0;
        FILE* file = nullptr;

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader.CompressionHeaderSize, stream_file_bytearray + sf_header_size + 0, sizeof(DWORD));
        std::memcpy(&compressionheader.AmountCompressedChunks, stream_file_bytearray + sf_header_size + 4, sizeof(DWORD));
        std::memcpy(&compressionheader.CompressionBool, stream_file_bytearray + sf_header_size + 8, sizeof(DWORD));
        std::memcpy(&compressionheader.Unknown1, stream_file_bytearray + sf_header_size + 12, sizeof(DWORD));
        std::memcpy(&compressionheader.Unknown2, stream_file_bytearray + sf_header_size + 16, sizeof(DWORD));
        std::memcpy(&compressionheader.Unknown3, stream_file_bytearray + sf_header_size + 20, sizeof(DWORD));
        std::memcpy(&compressionheader.Unknown4, stream_file_bytearray + sf_header_size + 24, sizeof(DWORD));
        std::memcpy(&compressionheader.Unknown5, stream_file_bytearray + sf_header_size + 28, sizeof(DWORD));

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
            char* decompression_in_buffer = (char*)malloc(decompression_in_buffer_size);
            if (decompression_in_buffer == NULL) {
                perror("Error allocating memory");
                return;
            }

            // Decompress inbuffer to get decompressed out buffer and size.
            std::memcpy(decompression_in_buffer, stream_file_bytearray + stream_index, decompression_in_buffer_size);
            std::vector<uint8_t> decompression_in_buffer_vector(decompression_in_buffer, decompression_in_buffer + decompression_in_buffer_size);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
            decompressed_size = decompression_out_buffer_vector.size();

            // Create decompression out buffer.
            char* decompression_out_buffer = (char*)malloc(decompressed_size);
            if (decompression_out_buffer == NULL) {
                perror("Error allocating memory");
                return;
            }

            // Copy the vector into the char* buffer
            std::copy(decompression_out_buffer_vector.begin(), decompression_out_buffer_vector.end(), reinterpret_cast<uint8_t*>(decompression_out_buffer));

            // Write (append) to file.
            if (_wfopen_s(&file, concatenatedText.c_str(), L"ab+") == 0)
            {
                if (file != nullptr) {
                    size_t bytesWritten = fwrite(decompression_out_buffer, sizeof(char), decompressed_size, file);
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

            // Free memory (since we are using C to create buffers)
            free(decompression_in_buffer);
            free(decompression_out_buffer);

            // Move one compressed chunk forward
            stream_index += decompression_in_buffer_size;
        }

        return;
    }
}
