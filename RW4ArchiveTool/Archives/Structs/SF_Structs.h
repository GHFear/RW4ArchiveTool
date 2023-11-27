#pragma once

struct SFArchiveHeader {
    DWORD Magic;
    DWORD SFVersion;
    DWORD Unknown1;
    DWORD MapID;
    DWORD HeaderSize;
    DWORD Unknown2;
    DWORD CompressionType;
    DWORD Unknown4;
    BYTE Unknown5[];
};

struct StreamFileHeader {
    unsigned char FileID[8];
    DWORD FileSize;
    DWORD StreamFileHeaderSize;
    DWORD StreamFileSize;
    BYTE Padding[];
};

enum SFTYPE {
    SF1,
    SF2,
    SF3,
    UNKNOWN
};

// Add the information you want to parse from the SF into this file. (it's up to you what that is.)
struct SF_Parse_Struct {
    char filename[17];
    int file_size;
    long file_offset;
};