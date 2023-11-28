#pragma once

enum ARCHIVETYPE {
    BIG_EB,
    SFIL,
    UNKNOWNARCHIVE
};

struct SFArchiveHeaderNotDynamic {
    DWORD Magic;
    DWORD Version;
    UINT64 Stamp;
    DWORD Offset;
    DWORD NumCollections;
    DWORD StreamFormat;
    DWORD Unknown4;
};

struct SFArchiveHeader {
    DWORD Magic;
    DWORD Version;
    UINT64 Stamp;
    DWORD Offset;
    DWORD NumCollections;
    DWORD StreamFormat;
    DWORD Unknown4;
    BYTE Unknown5[];
};

struct CollectionAsset {
    unsigned char ID[8];
    DWORD Size;
    DWORD Offset;
    DWORD Stride;
    BYTE Padding[];
};

enum SFTYPE {
    SF1,
    SF2,
    SF3,
    UNKNOWN
};

// Add the information you want to parse from the SF into this file. (it's up to you what that is.)
struct Archive_Parse_Struct {
    char filename[64];
    int file_size;
    long file_offset;
};