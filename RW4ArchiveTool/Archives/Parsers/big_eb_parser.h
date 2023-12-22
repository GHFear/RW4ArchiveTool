// EA Skate EB Parser By GHFear.

#pragma once
#include "refpack/refpackd.h"
#include "Zlib/Include/zlib-1.3/zlib.h"
#include "IoTools.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"

namespace big_eb
{
    struct BigEBArchiveHeader {
        WORD Signature;
        WORD HeaderVersion;
        DWORD FileAmount;
        WORD Flags;
        BYTE Alignment;
        BYTE Reserved;
        DWORD NamesOffset;
        DWORD NamesSize;
        BYTE FilenameLength;
        BYTE FoldernameLength;
        WORD FolderAmount;
        UINT64 ArchiveSize;
        DWORD FatSize;
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
        char ID[8];
        uint32_t VersionNum;
        uint32_t uncompressedLength;
        uint32_t blockSize;
        uint32_t numSegments;
        uint32_t alignedTo;
    };

    struct ChunkBlockHeader {
        uint32_t chunkSizeCompressed;
        uint32_t compressionType;
    };

    struct TOCIndex {
        DWORD offset;
        DWORD compressed_size;
        DWORD size;
        DWORD hash1;
        DWORD hash2; // Used only in 64-bit hash mode.
    };

    bool unpack_empty_file(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
    {
        FILE* file = nullptr;

        // Attempt to create the directory
        if (CreateDirectoryRecursively(Filedirectory.c_str())) {
            wprintf(L"Directory created: %s\n", Filedirectory.c_str());
        }
        else {
            wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
        }

        // Create empty file.
        if (!IoTools::create_file(file, Filepath))
        {
            return false;
        }

        return true;
    }

    bool decompress_deflate(const unsigned char* compressedData, size_t compressedSize, unsigned char* decompressedData, size_t& decompressedSize) {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        stream.avail_in = static_cast<uInt>(compressedSize);
        stream.next_in = const_cast<Bytef*>(compressedData);

        if (inflateInit2(&stream, -MAX_WBITS) != Z_OK) {
            std::cerr << "Error initializing zlib inflate for deflate format." << std::endl;
            return false;
        }

        stream.avail_out = static_cast<uInt>(decompressedSize);
        stream.next_out = decompressedData;

        if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
            std::cerr << "Error in zlib decompression for deflate format." << std::endl;
            inflateEnd(&stream);
            return false;
        }

        decompressedSize = stream.total_out;

        if (inflateEnd(&stream) != Z_OK) {
            std::cerr << "Error ending zlib inflate for deflate format." << std::endl;
            return false;
        }

        return true;
    }

    bool chunk_decompress(FILE* archive, std::wstring Filedirectory, std::wstring Filepath)
    {
        // Local Variables.
        std::vector<uint8_t> decompression_out_buffer_vector = {};
        size_t decompressed_size = 0;
        
        ChunkPackHeader chunk_pack_header = {};
        fread(&chunk_pack_header, sizeof(chunk_pack_header), 1, archive);
        chunk_pack_header.alignedTo = BigToLittleUINT(chunk_pack_header.alignedTo);
        chunk_pack_header.blockSize = BigToLittleUINT(chunk_pack_header.blockSize);
        chunk_pack_header.numSegments = BigToLittleUINT(chunk_pack_header.numSegments);
        chunk_pack_header.uncompressedLength = BigToLittleUINT(chunk_pack_header.uncompressedLength);
        chunk_pack_header.VersionNum = BigToLittleUINT(chunk_pack_header.VersionNum);

        for (size_t i = 0; i < chunk_pack_header.numSegments; i++)
        {
            // Get the current location.
            size_t current_location = _ftelli64(archive);
            std::cout << current_location << "\n";

            // Extract the last hexadecimal digit
            size_t lastDigit = current_location % 0x10;

            // Check if the last digit is greater than 0x08 or 0.
            if (lastDigit > 0x08 || lastDigit == 0) {
                std::cout << "The offset ends with a value greater than or equal to 0x08." << std::endl;
                current_location = roundUpToMultiple(current_location, chunk_pack_header.alignedTo);
                current_location += 8;
            }
            else {
                std::cout << "The offset does not end with a value greater than or equal to 0x08." << std::endl;
                current_location = roundUpToMultiple(current_location, 8);
            }

            _fseeki64(archive, current_location, SEEK_SET); // Seek to closest aligned location recognized by the BIG EB v3 format.

            // Read RW chunk block header.
            ChunkBlockHeader block_header = {};
            fread(&block_header, sizeof(block_header), 1, archive);
            block_header.chunkSizeCompressed = BigToLittleUINT(block_header.chunkSizeCompressed);
            block_header.compressionType = BigToLittleUINT(block_header.compressionType);

            // Allocate memory for our file buffer
            char* file_buffer = (char*)malloc(block_header.chunkSizeCompressed);
            if (file_buffer == NULL) {
                perror("Error allocating memory");
                return false;
            }

            // Read chunk into file buffer.
            fread(file_buffer, block_header.chunkSizeCompressed, 1, archive); // Read file into buffer
            std::vector<uint8_t> decompression_in_buffer_vector(file_buffer, file_buffer + block_header.chunkSizeCompressed);

            // Check if the chunk is compressed.
            if (block_header.compressionType == 2) // Chunk is compressed with refpack.
            {
                std::vector<uint8_t> chunk_decompression_out_buffer_vector = refpack::decompress(decompression_in_buffer_vector);
                decompressed_size += chunk_decompression_out_buffer_vector.size();
                decompression_out_buffer_vector.insert(decompression_out_buffer_vector.end(), chunk_decompression_out_buffer_vector.begin(), chunk_decompression_out_buffer_vector.end());
            }
            else if (block_header.compressionType == 4) // Chunk is not compressed
            {
                decompression_out_buffer_vector.insert(decompression_out_buffer_vector.end(), decompression_in_buffer_vector.begin(), decompression_in_buffer_vector.end());
            }
            else if (block_header.compressionType == 1) // Chunk is compressed with zlib.
            {
                // Allocate a buffer for the decompressed data
                size_t chunk_decompressed_size = chunk_pack_header.uncompressedLength;
                unsigned char* decompressedData = new unsigned char[chunk_decompressed_size];

                // Decompress the data
                if (decompress_deflate(decompression_in_buffer_vector.data(), block_header.chunkSizeCompressed, decompressedData, chunk_decompressed_size)) {
                    decompressed_size += chunk_decompressed_size;
                    decompression_out_buffer_vector.insert(decompression_out_buffer_vector.end(), decompressedData, decompressedData + chunk_decompressed_size);
                    delete[] decompressedData;
                }
                else {
                    std::cerr << "Decompression failed." << std::endl;
                    MessageBoxA(0, "Decompression failed! Reach out to GHFear for support.", "Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    free(file_buffer);
                    return false;
                }
            }
            else
            {
                MessageBoxA(0, "Unknown compression type! Reach out to GHFear for support.", "Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                free(file_buffer);
                return false;
            }

            free(file_buffer);
        }

        // Attempt to create the directory
        if (CreateDirectoryRecursively(Filedirectory.c_str())) 
        {
            wprintf(L"Directory created: %s\n", Filedirectory.c_str());
        }
        else 
        {
            wprintf(L"Failed to create directory or directory already exists: %s\n", Filedirectory.c_str());
        }

        // Write to file.
        FILE* file = nullptr;
        if (!IoTools::write_file(file, Filepath, (char*)decompression_out_buffer_vector.data(), chunk_pack_header.uncompressedLength))
        {
            return false;
        }

        return true;
    }

    bool unpack_uncompressed_file(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
    {
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
        FILE* file = nullptr;
        if (!IoTools::write_file(file, Filepath, file_buffer, SIZE))
        {
            free(file_buffer);
            return false;
        }

        free(file_buffer);
        return true;
    }

    void failed_to_unpack_messagebox(std::string input_msg)
    {
        std::string output_string = " Failed out unpack.";
        output_string += input_msg;
        MessageBoxA(0, output_string.c_str(), "Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
    }

    bool unpack_refpack_file(FILE* archive, DWORD SIZE, std::wstring Filedirectory, std::wstring Filepath)
    {
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
        FILE* file = nullptr;
        if (!IoTools::write_file(file, Filepath, (char*)decompression_out_buffer_vector.data(), decompressed_size))
        {
            free(file_buffer);
            return false;
        }

        free(file_buffer);
        return true;
    }

    // Function to parse files from a big eb archive. (Doesn't decompress yet)
    auto parse_big_eb_archive(const wchar_t* archiveName, bool unpack, int64_t selected_file_index)
    {
        struct RESULT { std::vector<Archive_Parse_Struct>  parsed_info; bool success; };

        // Declare local variables.
        FILE* archive = nullptr;
        BigEBArchiveHeader archive_header = {};
        std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
        std::vector<UINT64> toc_offset_Vector;
        std::vector<UINT64> offset_Vector;
        std::vector<DWORD> size_Vector;
        std::vector<BYTE> compression_type_Vector;

        DWORD folders_offset = 0;

        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return RESULT{ Archive_Parse_Struct_vector , false };
        }

        // Save start position.
        uint64_t start_of_archive = _ftelli64(archive);

        // Save archive size and go back to start position.
        fseek(archive, 0, SEEK_END);
        uint64_t archive_size = _ftelli64(archive);
        fseek(archive, start_of_archive, SEEK_SET);
        
        // Read archive header and convert to little endian.
        fread(&archive_header, sizeof(archive_header), 1, archive);
        archive_header.Signature = BigToLittleUINT(archive_header.Signature);
        archive_header.HeaderVersion = BigToLittleUINT(archive_header.HeaderVersion);
        archive_header.FileAmount = BigToLittleUINT(archive_header.FileAmount);
        archive_header.Flags = BigToLittleUINT(archive_header.Flags);
        archive_header.NamesOffset = BigToLittleUINT(archive_header.NamesOffset);
        archive_header.NamesSize = BigToLittleUINT(archive_header.NamesSize);
        archive_header.FolderAmount = BigToLittleUINT(archive_header.FolderAmount);
        archive_header.ArchiveSize = BigToLittleUINT(archive_header.ArchiveSize);
        archive_header.FatSize = BigToLittleUINT(archive_header.FatSize);

        if (archive_header.FileAmount > archive_size || archive_header.ArchiveSize > archive_size || archive_header.FatSize > archive_size)
        {
            fclose(archive);
            return RESULT{ Archive_Parse_Struct_vector , false };
        }

        // Set folder offset.
        folders_offset = archive_header.FileAmount;
        folders_offset *= archive_header.FilenameLength;
        folders_offset += archive_header.NamesOffset;
        folders_offset = roundUpToMultiple(folders_offset, 0x10);
        archive_header.FilenameLength -= 2;

        _fseeki64(archive, 0x30, SEEK_SET);
        for (size_t i = 0; i < archive_header.FileAmount; i++)
        {
            // Build toc_index (or whatever we would call this)
            TOCIndex toc_index = {};
            toc_offset_Vector.push_back(_ftelli64(archive));
            fread(&toc_index, sizeof(toc_index), 1, archive);
            toc_index.offset = BigToLittleUINT(toc_index.offset);
            toc_index.compressed_size = BigToLittleUINT(toc_index.compressed_size);
            toc_index.size = BigToLittleUINT(toc_index.size);
            toc_index.hash1 = BigToLittleUINT(toc_index.hash1);
            toc_index.hash2 = BigToLittleUINT(toc_index.hash2);

            if (toc_index.offset > archive_size || toc_index.compressed_size > archive_size || toc_index.size > archive_size)
            {
                fclose(archive);
                return RESULT{ Archive_Parse_Struct_vector , false };
            }

            if (!(archive_header.Flags & FLAG_64BITHASH))
            {
                _fseeki64(archive, -4, SEEK_CUR);
            }

            // Push values into vector.
            UINT64 OFFSET64 = toc_index.offset;
            OFFSET64 = OFFSET64 << archive_header.Alignment;
            offset_Vector.push_back(OFFSET64);
            size_Vector.push_back(toc_index.size);
        }

        for (size_t i = 0; i < archive_header.FileAmount; i++)
        {
            // Read compression type. (This doesn't exist on all versions of EB V3 and you may need to figure this out some other way.)
            BYTE compression_type = 0;
            fread(&compression_type, sizeof(compression_type), 1, archive);
            compression_type_Vector.push_back(compression_type);
        }

        _fseeki64(archive, archive_header.NamesOffset, SEEK_SET);
        for (size_t i = 0; i < archive_header.FileAmount; i++)
        {
            // Declare local variables.
            Archive_Parse_Struct Parsed_Archive_Struct = {};

            std::wstring wide_archiv_path = archiveName;
            std::wstring directory = ParseFilePath(wide_archiv_path).first;
            std::wstring out_filepath = directory;
            std::wstring out_filedirectory = directory;
            
            // Read the folder number and convert to little endian.
            WORD folder_number = 0;
            fread(&folder_number, sizeof(folder_number), 1, archive);
            folder_number = BigToLittleUINT(folder_number);

            // Read and build directory. (Stage 1: Filename) 
            std::vector<char> name_buffer(archive_header.FilenameLength);
            fread(name_buffer.data(), archive_header.FilenameLength, 1, archive);
            std::string name(name_buffer.begin(), std::find(name_buffer.begin(), name_buffer.end(), '\0'));
            std::string filename = name;

            DWORD next_name_offset = _ftelli64(archive); // Save the current location as next name offset location.
            DWORD foldername_offset = folder_number;
            foldername_offset *= archive_header.FoldernameLength;
            foldername_offset += folders_offset;

            _fseeki64(archive, foldername_offset, SEEK_SET); // Seek to foldername location in archive.

            // Read and build directory. (Stage 2: directory)
            std::vector<char> folder_buffer(archive_header.FoldernameLength);
            fread(folder_buffer.data(), archive_header.FoldernameLength, 1, archive);
            std::string folder(folder_buffer.begin(), std::find(folder_buffer.begin(), folder_buffer.end(), '\0'));
            std::string final_extracted_filepath = folder;
            final_extracted_filepath += "/";
            final_extracted_filepath += name;

            _fseeki64(archive, offset_Vector[i], SEEK_SET); // Seek to file offset location.

            // Prepare final wide character strings. (Stage 3: Build proper path)
            out_filedirectory += to_wstring(folder);
            std::replace(out_filepath.begin(), out_filepath.end(), L'/', L'\\');
            std::replace(out_filedirectory.begin(), out_filedirectory.end(), L'/', L'\\');
            out_filepath += to_wstring(final_extracted_filepath);

            if (!unpack)
            {
                // Set Parsed_Archive struct members.
                Parsed_Archive_Struct.filename = folder + name;
                Parsed_Archive_Struct.file_size = size_Vector[i];
                Parsed_Archive_Struct.file_offset = offset_Vector[i];
                Parsed_Archive_Struct.toc_offset = toc_offset_Vector[i];
            }

            // Read RW chunk pack header.
            ChunkPackHeader chunk_pack_header = {};
            fread(&chunk_pack_header, sizeof(chunk_pack_header), 1, archive);
            std::string chunkpack_id = chunk_pack_header.ID;

            _fseeki64(archive, offset_Vector[i], SEEK_SET); // Seek to file offset location.

            bool full_archive_unpack = unpack && selected_file_index == -1; // Do a full archive unpack.
            bool single_archive_unpack = unpack && selected_file_index == i; // Do a single file archive unpack.

            if (size_Vector[i] != 0)
            {                
                if (chunkpack_id == "chunkref") // Chunkpacked file compressed with refpack.
                {
                    strcpy_s(Parsed_Archive_Struct.ztype, "CHUNKREF");
                    if (!unpack) 
                    { 
                        goto dont_unpack_loc; 
                    }
                    else if (full_archive_unpack || single_archive_unpack)
                    {
                        if (chunk_decompress(archive, out_filedirectory, out_filepath) != true)
                        {
                            fclose(archive);
                            return RESULT{ Archive_Parse_Struct_vector , false };
                        }
                    }
                }
                else if (chunkpack_id == "chunkzip") // Chunkpacked file compressed with zlib.
                {
                    strcpy_s(Parsed_Archive_Struct.ztype, "CHUNKZIP");
                    if (!unpack)
                    {
                        goto dont_unpack_loc;
                    }
                    else if (full_archive_unpack || single_archive_unpack)
                    {
                        if (chunk_decompress(archive, out_filedirectory, out_filepath) != true)
                        {
                            fclose(archive);
                            return RESULT{ Archive_Parse_Struct_vector , false };
                        }
                    }
                }
                else if (chunkpack_id == "chunklzx") // Chunkpacked file compressed with lzx.
                {
                    strcpy_s(Parsed_Archive_Struct.ztype, "CHUNKLZX");
                    if (!unpack) { goto dont_unpack_loc; }
                    fclose(archive);
                    return RESULT{ Archive_Parse_Struct_vector , false };
                }
                else //Single file (can be uncompressed or compressed with refpack)
                {
                    WORD refpack_magic = 0;
                    fread(&refpack_magic, sizeof(WORD), 1, archive);
                    refpack_magic = BigToLittleUINT(refpack_magic);
                    _fseeki64(archive, offset_Vector[i], SEEK_SET); // Seek to file offset location.
                    if (refpack_magic == 0x10FB) // Single file compressed with refpack
                    {
                        strcpy_s(Parsed_Archive_Struct.ztype, "REFPACK");
                        if (!unpack)
                        {
                            goto dont_unpack_loc;
                        }
                        else if (full_archive_unpack || single_archive_unpack)
                        {
                            if (unpack_refpack_file(archive, size_Vector[i], out_filedirectory, out_filepath) != true)
                            {
                                fclose(archive);
                                return RESULT{ Archive_Parse_Struct_vector , false };
                            }
                        }
                    }
                    else // Single file uncompressed
                    {
                        strcpy_s(Parsed_Archive_Struct.ztype, "NONE");
                        if (!unpack)
                        {
                            goto dont_unpack_loc;
                        }
                        else if (full_archive_unpack || single_archive_unpack)
                        {
                            if (unpack_uncompressed_file(archive, size_Vector[i], out_filedirectory, out_filepath) != true)
                            {
                                fclose(archive);
                                return RESULT{ Archive_Parse_Struct_vector , false };
                            }
                        }
                    }
                }
            }
            else
            {
                strcpy_s(Parsed_Archive_Struct.ztype, "EMPTY FILE");
                if (!unpack)
                {
                    goto dont_unpack_loc;
                }
                else if (full_archive_unpack || single_archive_unpack)
                {
                    if (unpack_empty_file(archive, size_Vector[i], out_filedirectory, out_filepath) != true)
                    {
                        fclose(archive);
                        return RESULT{ Archive_Parse_Struct_vector , false };
                    }
                }
            }

            dont_unpack_loc:
            name_buffer.clear();
            name.clear();
            folder_buffer.clear();
            folder.clear();
            Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);
            _fseeki64(archive, next_name_offset, SEEK_SET); // Seek to the saved next name location.
        }

        toc_offset_Vector.clear();
        offset_Vector.clear();
        size_Vector.clear();
        compression_type_Vector.clear();

        // Close the archive.
        fclose(archive);
        return RESULT{ Archive_Parse_Struct_vector , true };
    }
}
