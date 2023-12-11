// EA Skate SF Parser By GHFear.
#pragma once
#include "../Compression/refpack/refpackd.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"


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

    // StreamFile Decompression Type 1 (Refpack Compressed Single File)
    void sf_decompress_type1(char* stream_file_bytearray, const char* filename, CollectionAsset streamfile_header, const wchar_t* directory, uint32_t sf_header_size)
    {
        std::wstring Filedirectory = directory;
        Filedirectory += ConvertCharToWchar(filename);

        // Declare local variables.
        size_t decompressed_size = 0;
        FILE* file = nullptr;

        // Open the file in write mode to overwrite or create an empty file
        if (_wfopen_s(&file, Filedirectory.c_str(), L"wb+") == 0) {
            if (file != nullptr) {
                fclose(file);
            }
            else { OutputDebugStringA("Error opening file for writing.\n"); return; }
        }
        else { OutputDebugStringA("Error opening file.\n"); return; }

        // Decompress inbuffer to get decompressed out buffer and size.
        std::vector<uint8_t> decompression_in_buffer_vector(stream_file_bytearray + sf_header_size, (stream_file_bytearray + sf_header_size) + streamfile_header.Size);
        std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
        decompressed_size = decompression_out_buffer_vector.size();

        // Write (append) to file.
        if (_wfopen_s(&file, Filedirectory.c_str(), L"ab+") == 0)
        {
            if (file != nullptr) {
                size_t bytesWritten = fwrite(decompression_out_buffer_vector.data(), sizeof(char), decompressed_size, file);
                if (bytesWritten != decompressed_size) { OutputDebugStringA("Error writing to file.\n"); }
                fclose(file);
            }
            else { OutputDebugStringA("Error opening file for append.\n"); return; }
        }
        else { OutputDebugStringA("Error opening file.\n"); return; }

        return;
    }

    // StreamFile Decompression Type 2 (Refpack Compressed Arena Header and/or Body)
    bool sf_decompress_type2_loop(FILE* File, DWORD IsCompressed, DWORD CompressedSize, DWORD DecompressedSize, const char* Filename, char* StreamFileBuffer, uint32_t* StreamIndex, const wchar_t* Directory)
    {
        std::wstring file_directory = Directory;
        file_directory += ConvertCharToWchar(Filename);

        if (IsCompressed == 0)
        {
            char* current_file_location = StreamFileBuffer + *StreamIndex;

            if (_wfopen_s(&File, file_directory.c_str(), L"ab+") != 0)
            {
                OutputDebugStringA("Error opening/writing to file.\n"); return false;
            }

            if (File != nullptr) {
                size_t bytes_written = fwrite(current_file_location, sizeof(char), DecompressedSize, File);
                if (bytes_written != DecompressedSize) {
                    OutputDebugStringA("Error opening/writing to file.\n"); return false;
                }
                fclose(File);
            }
            else {
                OutputDebugStringA("Error opening/writing to file.\n"); return false;
            }

            return true;
        }
        else
        {
            // Decompress inbuffer to get decompressed out buffer and size.
            std::vector<uint8_t> decompression_in_buffer_vector(StreamFileBuffer + *StreamIndex, StreamFileBuffer + *StreamIndex + CompressedSize);
            std::vector<uint8_t> decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);

            // Write (append) to file.
            if (_wfopen_s(&File, file_directory.c_str(), L"ab+") != 0)
            {
                OutputDebugStringA("Error opening/writing to file.\n"); return false;
            }

            if (File != nullptr) {
                size_t bytes_written = fwrite(decompression_out_buffer_vector.data(), sizeof(char), DecompressedSize, File);
                if (bytes_written != DecompressedSize) {
                    OutputDebugStringA("Error opening/writing to file.\n"); return false;
                }
                fclose(File);
            }
            else {
                OutputDebugStringA("Error opening/writing to file.\n"); return false;
            }

            return true;
        }

        return false;
    }

    // StreamFile Decompression Type 2 (Refpack Compressed Arena Header and/or Body)
    void sf_decompress_type2(char* StreamFileBuffer, const char* Filename, const wchar_t* Directory, uint32_t StreamFileHeaderSize)
    {
        // Declare local variables.
        SFCompressionType2 compressionheader = {};
        uint32_t compression_header_size = sizeof(SFCompressionType2);
        uint32_t stream_index = 0;
        uint32_t* stream_index_ptr = &stream_index;
        std::wstring file_directory = Directory;
        file_directory += ConvertCharToWchar(Filename);

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader, StreamFileBuffer + StreamFileHeaderSize + 0, compression_header_size);
        compressionheader.BodyCompressedBool = BigToLittleUINT(compressionheader.BodyCompressedBool);
        compressionheader.BodyCompressedSize = BigToLittleUINT(compressionheader.BodyCompressedSize);
        compressionheader.BodyDecompressedSize = BigToLittleUINT(compressionheader.BodyDecompressedSize);
        compressionheader.HeaderCompressedBool = BigToLittleUINT(compressionheader.HeaderCompressedBool);
        compressionheader.HeaderCompressedSize = BigToLittleUINT(compressionheader.HeaderCompressedSize);
        compressionheader.HeaderDecompressedSize = BigToLittleUINT(compressionheader.HeaderDecompressedSize);
        compressionheader.Unknown1 = BigToLittleUINT(compressionheader.Unknown1);
        compressionheader.Unknown3 = BigToLittleUINT(compressionheader.Unknown3);

        *stream_index_ptr = StreamFileHeaderSize + compression_header_size;
        
        // Open the file in write mode to overwrite or create an empty file
        FILE* file = nullptr;
        if (_wfopen_s(&file, file_directory.c_str(), L"wb+") == 0) {
            if (file != nullptr) {
                fclose(file);
            }
            else { OutputDebugStringA("Error opening/writing to file.\n"); return; }
        } 
        else { OutputDebugStringA("Error opening/writing to file.\n"); return; }

        if (compressionheader.HeaderCompressedSize != 0 || compressionheader.HeaderDecompressedSize != 0)
        {
            sf_decompress_type2_loop(
                file,
                compressionheader.HeaderCompressedBool,
                compressionheader.HeaderCompressedSize,
                compressionheader.HeaderDecompressedSize,
                Filename,
                StreamFileBuffer,
                stream_index_ptr,
                Directory);
        }

        stream_index += compressionheader.HeaderCompressedSize;

        if (compressionheader.BodyCompressedSize != 0 || compressionheader.BodyDecompressedSize != 0)
        {
            sf_decompress_type2_loop(
                file,
                compressionheader.BodyCompressedBool,
                compressionheader.BodyCompressedSize,
                compressionheader.BodyDecompressedSize,
                Filename,
                StreamFileBuffer,
                stream_index_ptr,
                Directory);
        }

        return;
    }

    // StreamFile Decompression Type 3 (Arena Chunks Individually Compressed With Refpack)
    void sf_decompress_type3(char* stream_file_bytearray, const char* filename, const wchar_t* directory, uint32_t sf_header_size)
    {
        std::wstring Filedirectory = directory;
        Filedirectory += ConvertCharToWchar(filename);

        // Declare local variables.
        SFCompressionType3 compressionheader;
        size_t decompressed_size = 0;
        uint64_t stream_index = 0;
        FILE* file = nullptr;

        // Fill compressionheader struct with important information from the SF compression header.
        std::memcpy(&compressionheader, stream_file_bytearray + sf_header_size, 32);
        compressionheader.AmountCompressedChunks = BigToLittleUINT(compressionheader.AmountCompressedChunks);
        compressionheader.CompressionBool = BigToLittleUINT(compressionheader.CompressionBool);
        compressionheader.CompressionHeaderSize = BigToLittleUINT(compressionheader.CompressionHeaderSize);
        compressionheader.Unknown1 = BigToLittleUINT(compressionheader.Unknown1);
        compressionheader.Unknown2 = BigToLittleUINT(compressionheader.Unknown2);
        compressionheader.Unknown3 = BigToLittleUINT(compressionheader.Unknown3);
        compressionheader.Unknown4 = BigToLittleUINT(compressionheader.Unknown4);
        compressionheader.Unknown5 = BigToLittleUINT(compressionheader.Unknown5);

        stream_index = sf_header_size + 32;

        // Create the CompressedBlockSizes vector.
        for (size_t i = 0; i < (compressionheader.CompressionHeaderSize - 32) / 4; i++)
        {
            DWORD TempCompressedBlockSize = 0;
            std::memcpy(&TempCompressedBlockSize, stream_file_bytearray + stream_index, sizeof(DWORD));
            TempCompressedBlockSize = BigToLittleUINT(TempCompressedBlockSize);
            compressionheader.CompressedBlockSizes.push_back(TempCompressedBlockSize);
            stream_index += 4;
        }

        // Open the file in write mode to overwrite or create an empty file
        if (_wfopen_s(&file, Filedirectory.c_str(), L"wb+") == 0) {
            if (file != nullptr) {
                fclose(file);
            }
            else { OutputDebugStringA("Error opening/writing to file.\n"); }
        }
        else { OutputDebugStringA("Error opening/writing to file.\n"); }

        // Unpack loop
        for (size_t i = 0; i < compressionheader.AmountCompressedChunks; i++)
        {
            // Create decompression in-buffer.
            uint64_t decompression_in_buffer_size = compressionheader.CompressedBlockSizes[i];

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
                        OutputDebugStringA("Error opening/writing to file.\n");
                    }
                    fclose(file);
                }
                else { OutputDebugStringA("Error opening/writing to file.\n"); }
            }
            else { OutputDebugStringA("Error opening/writing to file.\n"); }

            // Move one compressed chunk forward
            stream_index += decompression_in_buffer_size;
        }

        return;
    }

    // Function to unpack files from a sf archive. (Doesn't decompress)
    std::vector<Archive_Parse_Struct> parse_sf_archive(const wchar_t* archiveName, const wchar_t* directory, bool unpack) {
        // Declare local variables.
        FILE* archive = nullptr;
        long sf_collection_size = 0;
        long start_of_sf_collection = 0;
        uint32_t size_of_sfa_header = 0;
        std::wstring streamfilename_directory = GetFilenameWithoutExtension(archiveName);
        std::wstring out_directory = directory;
        out_directory += streamfilename_directory + L"\\";
        std::vector<Archive_Parse_Struct> parse_struct_vector = {};

        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return parse_struct_vector;
        }

        // Save start position.
        start_of_sf_collection = ftell(archive);

        // Save archive size and go back to start position.
        fseek(archive, 0, SEEK_END);
        sf_collection_size = ftell(archive);
        fseek(archive, start_of_sf_collection, SEEK_SET);

        // Save archive headersize and go back to start position.
        fseek(archive, 16, SEEK_CUR);
        fread(&size_of_sfa_header, sizeof(DWORD), 1, archive);
        size_of_sfa_header = BigToLittleUINT(size_of_sfa_header);
        fseek(archive, start_of_sf_collection, SEEK_SET);

        // Read SF Archive header.
        SFArchiveHeader* sfa_header = (SFArchiveHeader*)malloc(sizeof(*sfa_header) + (size_of_sfa_header - 32));
        fread(sfa_header, size_of_sfa_header, 1, archive);
        sfa_header->NumCollections = BigToLittleUINT(sfa_header->NumCollections);
        sfa_header->Offset = BigToLittleUINT(sfa_header->Offset);
        sfa_header->Stamp = BigToLittleUINT(sfa_header->Stamp);
        sfa_header->StreamFormat = BigToLittleUINT(sfa_header->StreamFormat);
        sfa_header->Unknown4 = BigToLittleUINT(sfa_header->Unknown4);
        sfa_header->Version = BigToLittleUINT(sfa_header->Version);

        if (unpack) // Attempt to create the directory if we are trying to unpack.
        {
            if (CreateDirectoryRecursively(out_directory.c_str())) {
                OutputDebugStringA("Folder created successfully.\n");
            }
            else {
                OutputDebugStringA("Folder already exists.\n");
            }
        }

        // Main unpacking loop.
        while (ftell(archive) < sf_collection_size)
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
            streamfile_header_size = BigToLittleUINT(streamfile_header_size);
            CollectionAsset* stream_file_header = (CollectionAsset*)malloc(sizeof(*stream_file_header) + (streamfile_header_size - 20));
            fread(stream_file_header, streamfile_header_size, 1, archive);
            stream_file_header->Stride = BigToLittleUINT(stream_file_header->Stride);
            stream_file_header->Offset = BigToLittleUINT(stream_file_header->Offset);
            stream_file_header->Size = BigToLittleUINT(stream_file_header->Size);

            // Set stream file block size
            stream_file_size = stream_file_header->Stride;

            // Set the filename
            bytearray_to_hexstring(stream_file_header->ID, sizeof(stream_file_header->ID), char_filename);
            filename = char_filename;
            const wchar_t* file_extension = GetFileExtension(archiveName);


            // Add to parse struct vector
            parse_struct.filename = filename;
            parse_struct.file_size = stream_file_size;
            parse_struct.file_offset = streamfile_startposition;
            parse_struct.toc_offset = streamfile_startposition;
            parse_struct_vector.push_back(parse_struct);

            // Run this code if we set unpack to true when calling this function.
            if (unpack) 
            { 
                // Create byte array with size of file_size
                char* stream_file_buffer = (char*)malloc(stream_file_size);
                if (stream_file_buffer == NULL) {
                    perror("Error allocating memory");
                    fclose(archive);
                    free(sfa_header);
                    free(stream_file_header);
                    free(stream_file_buffer);
                    return parse_struct_vector;
                }

                // Read current streamfile into a bytearray.
                fseek(archive, streamfile_startposition, SEEK_SET);
                fread(stream_file_buffer, sizeof(char), stream_file_size, archive);

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

                switch (sfa_header->StreamFormat)
                {
                case SF1:
                    sf_decompress_type1(stream_file_buffer, filename.c_str(), *stream_file_header, out_directory.c_str(), stream_file_header->Offset);
                    break;
                case SF2:
                    sf_decompress_type2(stream_file_buffer, filename.c_str(), out_directory.c_str(), stream_file_header->Offset);
                    break;
                case SF3:
                    sf_decompress_type3(stream_file_buffer, filename.c_str(), out_directory.c_str(), stream_file_header->Offset);
                    break;
                case UNKNOWN:
                    break;
                default:
                    break;
                }

                free(stream_file_buffer);
            }

            // free byte array memory and seek to next streamfile start position. 
            free(stream_file_header);
            fseek(archive, streamfile_startposition + stream_file_size, SEEK_SET);
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
        uint32_t size_of_sfa_header = 0;
        fread(&size_of_sfa_header, sizeof(DWORD), 1, archive);
        size_of_sfa_header = BigToLittleUINT(size_of_sfa_header);
        fseek(archive, start_of_sfa, SEEK_SET);

        // Read SFA header.
        fread(&sfa_header, sizeof(sfa_header), 1, archive);
        sfa_header.NumCollections = BigToLittleUINT(sfa_header.NumCollections);
        sfa_header.Offset = BigToLittleUINT(sfa_header.Offset);
        sfa_header.Stamp = BigToLittleUINT(sfa_header.Stamp);
        sfa_header.StreamFormat = BigToLittleUINT(sfa_header.StreamFormat);
        sfa_header.Unknown4 = BigToLittleUINT(sfa_header.Unknown4);
        sfa_header.Version = BigToLittleUINT(sfa_header.Version);

        // Close the archive and free memory.
        fclose(archive);

        return sfa_header;
    }

}
