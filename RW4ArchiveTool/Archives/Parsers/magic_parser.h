// EA Skate Magic Parser By GHFear.

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "../../Unspecified/Global.h"

namespace magic
{
    struct MagicArchiveHeader {
        DWORD Magic;
    };

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

        // Read EB Archive header.
        MagicArchiveHeader archive_header = {};
        fread(&archive_header, 4, 1, archive);

        switch (dword_big_to_little_endian(archive_header.Magic))
        {
        case 1161953283:
            type = BIG_EB;
            break;
        case 1397115212:
            type = SFIL;
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