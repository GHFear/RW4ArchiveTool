// EA Skate EB Parser By GHFear.

#pragma once
#include "Archives/Compression/refpack/refpackd.h"

namespace big_eb
{
    struct BigEBArchiveHeader {
        WORD SIGNATURE;
        WORD HEADER_VERSION;
        DWORD FILE_AMOUNT;
        WORD FLAGS;
        BYTE ALIGNMENT;
        BYTE RESERVED;
        DWORD NAMES_OFFSET;
        DWORD NAMES_SIZE;
        BYTE FILENAME_LENGTH;
        BYTE FOLDERNAME_LENGTH;
        WORD FOLDER_AMOUNT;
        UINT64 ARCHIVE_SIZE;
        DWORD FAT_SIZE;
    };

    const WORD FLAG_64BITHASH = 0x0001;

    const DWORD FAT_DIRECTORY = 0;
    const DWORD FAT_FILE = 1;

    struct SortOrder {
        const WORD HASH = 0x0010;
        const WORD ALPHABETICAL = 0x0020;
        const WORD FAT = 0x0040;
        const WORD OFFSET = 0x0080;
    };

    enum CompressionType {
        NONE = 0,
        REFPACK = 1,
        REFPACK_CHUNKED = 2,
        ZLIB_CHUNKED = 3,
        LZX = 4
    };

    struct ChunkPackHeader {
        char id[8];
        uint32_t versionNum;
        uint32_t uncompressedLength;
        uint32_t blockSize;
        uint32_t numSegments;
        uint32_t alignedTo;
    };

    struct ChunkBlockHeader {
        uint32_t chunkSizeCompressed;
        uint32_t compressionType;
    };


    bool chunkref_decompress(FILE* archive, std::wstring Filedirectory, std::wstring Filepath)
    {
        // Local Variables.
        FILE* file = nullptr;
        ChunkPackHeader chunk_pack_header = {};
        std::vector<uint8_t> decompression_out_buffer_vector = {};
        size_t decompressed_size = 0;

        // Read RW chunk pack header.
        fread(&chunk_pack_header, sizeof(chunk_pack_header), 1, archive);
        
        for (size_t i = 0; i < dword_big_to_little_endian(chunk_pack_header.numSegments); i++)
        {
            // Get the current location.
            size_t current_location = ftell(archive);
            std::cout << current_location << "\n";

            // Extract the last hexadecimal digit
            int lastDigit = current_location % 0x10;

            // Check if the last digit is greater than 0x08 or 0.
            if (lastDigit > 0x08 || lastDigit == 0) {
                std::cout << "The offset ends with a value greater than or equal to 0x08." << std::endl;
                current_location = round_up(current_location, dword_big_to_little_endian(chunk_pack_header.alignedTo));
                current_location += 8;
            }
            else {
                std::cout << "The offset does not end with a value greater than or equal to 0x08." << std::endl;
                current_location = round_up(current_location, 8);
            }

             fseek(archive, current_location, SEEK_SET); // Seek to closest aligned location recognized by the BIG EB v3 format.

             // Read RW chunk block header.
             ChunkBlockHeader block_header = {};
             fread(&block_header, sizeof(block_header), 1, archive);

             // Allocate memory for our file buffer
             char* file_buffer = (char*)malloc(dword_big_to_little_endian(block_header.chunkSizeCompressed));
             if (file_buffer == NULL) {
                 perror("Error allocating memory");
                 return false;
             }

             // Read chunk into file buffer.
             fread(file_buffer, dword_big_to_little_endian(block_header.chunkSizeCompressed), 1, archive); // Read file into buffer
             std::vector<uint8_t> decompression_in_buffer_vector(file_buffer, file_buffer + dword_big_to_little_endian(block_header.chunkSizeCompressed));

             // Check if the chunk is compressed.
             if (dword_big_to_little_endian(block_header.compressionType) == 2) // Chunk is compressed
             {
                 std::vector<uint8_t> chunk_decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
                 decompressed_size += chunk_decompression_out_buffer_vector.size();
                 decompression_out_buffer_vector.insert(decompression_out_buffer_vector.end(), chunk_decompression_out_buffer_vector.begin(), chunk_decompression_out_buffer_vector.end());
             }
             else if (dword_big_to_little_endian(block_header.compressionType) == 4) // Chunk is not compressed
             {
                 decompression_out_buffer_vector.insert(decompression_out_buffer_vector.end(), decompression_in_buffer_vector.begin(), decompression_in_buffer_vector.end());
             }
             
             free(file_buffer);
        }

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
            return false;
        }

        // Check if nullptr
        if (file == nullptr) {
            fprintf(stderr, "Error opening file for write.\n");
            fclose(file);
            return false;
        }

        //  Write and check if we wrote all bytes.
        size_t bytesWritten = fwrite(decompression_out_buffer_vector.data(), sizeof(char), dword_big_to_little_endian(chunk_pack_header.uncompressedLength), file);
        if (bytesWritten != dword_big_to_little_endian(chunk_pack_header.uncompressedLength)) {
            fprintf(stderr, "Error writing to file.\n");
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }

    bool unpack_uncompressed_file(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
    {
        FILE* file = nullptr;

        // Allocate memory for our file buffer
        char* file_buffer = (char*)malloc(SIZE);
        if (file_buffer == NULL) {
            perror("Error allocating memory");
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
        size_t bytesWritten = fwrite(decompression_out_buffer_vector.data(), sizeof(char), decompressed_size, file);
        if (bytesWritten != decompressed_size) {
            fprintf(stderr, "Error writing to file.\n");
            fclose(file);
            free(file_buffer);
            return false;
        }

        fclose(file);
        free(file_buffer);
        return true;
    }

    // Function to parse files from a big eb archive. (Doesn't decompress yet)
    std::vector<Archive_Parse_Struct> parse_big_eb_archive(const wchar_t* archiveName, const wchar_t* directory, bool unpack) {
        // Declare local variables.
        FILE* archive = nullptr;
        BigEBArchiveHeader archive_header = {};
        std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
        std::vector<DWORD> offset_Vector;
        std::vector<DWORD> size_Vector;
        std::vector<BYTE> compression_type_Vector;
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
            DWORD compressed_size = 0;
            DWORD size = 0;
            DWORD hash1 = 0;
            DWORD hash2 = 0;
            
            // Read offset.
            fread(&offset, sizeof(offset), 1, archive);
            offset = dword_big_to_little_endian(offset);
            offset = offset << archive_header.ALIGNMENT;

            // Read compressed size.
            fread(&compressed_size, sizeof(compressed_size), 1, archive);
            compressed_size = dword_big_to_little_endian(compressed_size);

            // Read size.
            fread(&size, sizeof(size), 1, archive);
            size = dword_big_to_little_endian(size);

            // Read hash1.
            fread(&hash1, sizeof(hash1), 1, archive);
            hash1 = dword_big_to_little_endian(hash1);

            if (word_big_to_little_endian(archive_header.FLAGS) & FLAG_64BITHASH)
            {
                // Read hash2. (if flag is set)
                fread(&hash2, sizeof(hash2), 1, archive);
                hash2 = dword_big_to_little_endian(hash2);
            }

            // Push values into vector.
            offset_Vector.push_back(offset);
            size_Vector.push_back(size);
        }

        for (size_t i = 0; i < dword_big_to_little_endian(archive_header.FILE_AMOUNT); i++)
        {
            BYTE compression_type = 0;

            // Read compression type.
            fread(&compression_type, sizeof(compression_type), 1, archive);

            // Push type into vector.
            compression_type_Vector.push_back(compression_type);
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
            BYTE compression_type = compression_type_Vector[i];
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
            if (compression_type == 0)
            {
                char ztype[] = "NONE";
                memcpy(Parsed_Archive_Struct.ztype, ztype, sizeof(ztype));
            }
            else if (compression_type == 1)
            {
                char ztype[] = "REFPACK";
                memcpy(Parsed_Archive_Struct.ztype, ztype, sizeof(ztype));
            }
            else if (compression_type == 2)
            {
                char ztype[] = "REFPACK_CHUNKED";
                memcpy(Parsed_Archive_Struct.ztype, ztype, sizeof(ztype));
            }
            else if (compression_type == 3)
            {
                char ztype[] = "ZLIB_CHUNKED";
                memcpy(Parsed_Archive_Struct.ztype, ztype, sizeof(ztype));
            }
            else if (compression_type == 4)
            {
                char ztype[] = "LZX";
                memcpy(Parsed_Archive_Struct.ztype, ztype, sizeof(ztype));
            }
            
            Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);
            
            // Check if we want to unpack.
            if (unpack)
            {

                switch (compression_type)
                {
                case CompressionType::NONE:
                    //Check if we could unpack successfully.
                    if (unpack_uncompressed_file(archive, size, out_filedirectory, out_filepath) != true)
                    {
                        MessageBox(0, L"File couldn't be unpacked! \nClose any tool that has a handle to this archive!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                case CompressionType::REFPACK:
                    //Check if we could unpack successfully.
                    if (unpack_refpack_file(archive, size, out_filedirectory, out_filepath) != true)
                    {
                        MessageBox(0, L"File couldn't be unpacked! \nClose any tool that has a handle to this archive!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                case CompressionType::REFPACK_CHUNKED:
                    if (chunkref_decompress(archive, out_filedirectory, out_filepath) != true)
                    {
                        MessageBox(0, L"File couldn't be unpacked! \nClose any tool that has a handle to this archive!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                case CompressionType::ZLIB_CHUNKED:
                    MessageBox(0, L"File couldn't be unpacked! \nIt's of type: ZLIB_CHUNKED.", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                case CompressionType::LZX:
                    MessageBox(0, L"File couldn't be unpacked! \nIt's of type: LZX.", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                default:
                    break;
                }
            }
            
            fseek(archive, next_name_offset, SEEK_SET); // Seek to the saved next name location.
        }

        // Close the archive.
        fclose(archive);

        return Archive_Parse_Struct_vector;
    }
}
