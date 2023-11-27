// EA Skate SF Parser By GHFear.
#pragma once

namespace sf
{
    struct SFCompressionType2 {
        DWORD HeaderCompressedSize;
        DWORD HeaderDecompressedSize;
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

    bool sf_decompress_type2_loop(FILE* file, DWORD HeaderCompressedBool, DWORD CompressedSize, DWORD DecompressedSize, char* filename, char* stream_file_bytearray, uint32_t* stream_index, const wchar_t* directory)
    {
        std::wstring concatenatedText = directory;
        concatenatedText += ConvertCharToWchar(filename);

        if (HeaderCompressedBool == 0)
        {
            if (_wfopen_s(&file, concatenatedText.c_str(), L"ab+") != 0)
            {
                fprintf(stderr, "Error opening file.\n");
                return false;
            }

            if (file == nullptr) {
                fprintf(stderr, "Error opening file for append.\n");
                return false;
            }

            uint64_t out_buffer_size = dword_big_to_little_endian(DecompressedSize);
            char* out_buffer = (char*)malloc(out_buffer_size);
            memset(out_buffer, 0, out_buffer_size);
            if (out_buffer == NULL) {
                perror("Error allocating memory");
                return false;
            }

            char* current_file_location = stream_file_bytearray + *stream_index;
            std::memcpy(out_buffer, current_file_location, dword_big_to_little_endian(DecompressedSize));

            size_t bytesWritten = fwrite(out_buffer, sizeof(char), out_buffer_size, file);
            if (bytesWritten != dword_big_to_little_endian(DecompressedSize)) {
                fprintf(stderr, "Error writing to file.\n");
                fclose(file);
                free(out_buffer);
                return false;
            }

            fclose(file);
            free(out_buffer);
            return true;
        }
        else
        {
            // Create decompression in-buffer.
            uint64_t decompression_in_buffer_size = dword_big_to_little_endian(CompressedSize);
            char* decompression_in_buffer = (char*)malloc(decompression_in_buffer_size);
            if (decompression_in_buffer == NULL) {
                perror("Error allocating memory");
                return false;
            }

            // Decompress inbuffer to get decompressed out buffer and size.
            std::memcpy(decompression_in_buffer, stream_file_bytearray + *stream_index, decompression_in_buffer_size);
            std::vector<uint8_t> decompression_in_buffer_vector(decompression_in_buffer, decompression_in_buffer + decompression_in_buffer_size);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);

            // Create decompression out buffer.
            char* decompression_out_buffer = (char*)malloc(dword_big_to_little_endian(DecompressedSize));
            if (decompression_out_buffer == NULL) {
                perror("Error allocating memory");
                return false;
            }

            // Copy the vector into the char* buffer
            std::copy(decompression_out_buffer_vector.begin(), decompression_out_buffer_vector.end(), reinterpret_cast<uint8_t*>(decompression_out_buffer));

            // Write (append) to file.
            if (_wfopen_s(&file, concatenatedText.c_str(), L"ab+") == 0)
            {
                if (file != nullptr) {
                    size_t bytesWritten = fwrite(decompression_out_buffer, sizeof(char), dword_big_to_little_endian(DecompressedSize), file);
                    if (bytesWritten != dword_big_to_little_endian(DecompressedSize)) {
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

            return true;
        }

        return false;
    }

    void sf_decompress_type2(char* stream_file_bytearray, char* filename, const wchar_t* directory, uint32_t sf_header_size)
    {
        // Declare local variables.
        SFCompressionType2 compressionheader;
        size_t decompressed_size = 0;
        uint32_t decompression_header_size = 80;
        uint32_t stream_index = 0;
        uint32_t* stream_index_ptr = &stream_index;
        FILE* file = nullptr;

        std::wstring concatenatedText = directory;
        concatenatedText += ConvertCharToWchar(filename);

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader, stream_file_bytearray + sf_header_size + 0, sizeof(SFCompressionType2));

        *stream_index_ptr = sf_header_size + decompression_header_size;

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

        sf_decompress_type2_loop(file, compressionheader.HeaderCompressedBool, compressionheader.HeaderCompressedSize, compressionheader.HeaderDecompressedSize, filename, stream_file_bytearray, stream_index_ptr, directory);
        stream_index += dword_big_to_little_endian(compressionheader.HeaderCompressedSize);
        sf_decompress_type2_loop(file, compressionheader.BodyCompressedBool, compressionheader.BodyCompressedSize, compressionheader.BodyDecompressedSize, filename, stream_file_bytearray, stream_index_ptr, directory);
        return;
    }
}

