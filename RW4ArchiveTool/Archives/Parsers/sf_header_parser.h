// EA Skate SF Parser By GHFear.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "../../Unspecified/Global.h"
#include "../Structs/SF_Structs.h"
#include "../Tools/BETools.h"
#include "../Unpackers/sf1_decompressor.h"
#include "../Unpackers/sf2_decompressor.h"
#include "../Unpackers/sf3_decompressor.h"
#include "sf_parser.h"

namespace sf
{
    SFArchiveHeaderNotDynamic parse_sfa_header(const wchar_t* archiveName);

    // Get SF compression type
    SFTYPE check_sfa_compression_type(SFArchiveHeaderNotDynamic sf_header)
    {
        if (dword_big_to_little_endian(sf_header.StreamFormat) == 1) // StreamFormat 1
        {
            return SFTYPE::SF1;
        }
        else if (dword_big_to_little_endian(sf_header.StreamFormat) == 2) // StreamFormat 2
        {
            return SFTYPE::SF2;
        }
        else if (dword_big_to_little_endian(sf_header.StreamFormat) == 3) // StreamFormat 3
        {
            return SFTYPE::SF3;
        }

        return SFTYPE::UNKNOWN;
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

    std::vector<Archive_Parse_Struct> sf_parser(const wchar_t* archive_name, const wchar_t* directory, bool unpack)
    {
        auto sf_header = parse_sfa_header(archive_name);
        auto compression_type = check_sfa_compression_type(sf_header);
        std::vector<Archive_Parse_Struct> parse_struct = {};
        parse_struct = sf::parse_sf_archive(archive_name, directory, unpack, compression_type);
        //free(sf_header); // We NEED to free this memory that was allocated in parse_sf_archive(); 
        return parse_struct;
    }
}