// EA Skate Magic Parser By GHFear.

#pragma once
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"


namespace magic
{
    struct MagicArchiveHeader {
        uint32_t Magic;
    };

    const uint32_t BIGF_ID = 0x42494746;
    const uint32_t BIG4_ID = 0x42494734;
    const uint32_t BIGEB_V3_ID = 0x45420003;
    const uint32_t SF_ID = 0x5346494C;
    const uint32_t Arena_ID = 0x89525734;

    // Function to decide which archive we're dealing with (Doesn't decompress)
    ARCHIVETYPE magic_parser(const wchar_t* archiveName) {
        // Declare local variables.
        FILE* archive = nullptr;
        ARCHIVETYPE type = UNKNOWNARCHIVE;

        // Open archive
        _wfopen_s(&archive, archiveName, L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return type;
        }

        // Read Archive header.
        MagicArchiveHeader archive_header = {};
        fread(&archive_header, sizeof(archive_header.Magic), 1, archive);
        archive_header.Magic = BigToLittleUINT(archive_header.Magic);

        switch (archive_header.Magic)
        {
        case BIGEB_V3_ID:
            type = BIG_EB;
            break;
        case BIG4_ID:
            type = BIG4;
            break;
        case BIGF_ID:
            type = BIG4;
            break;
        case SF_ID:
            type = SFIL;
            break;
        case Arena_ID:
            type = ARENA;
            break;
        default:
            type = UNKNOWNARCHIVE;
            break;
        }

        // Close the archive.
        fclose(archive);

        return type;
    }
}