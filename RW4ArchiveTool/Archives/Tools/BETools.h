#pragma once

// Convert big endian WORD to little endian WORD.
WORD word_big_to_little_endian(WORD input)
{
    return ((input & 0xFF) << 8) | ((input >> 8) & 0xFF);
}

// Convert big endian DWORD to little endian DWORD.
DWORD dword_big_to_little_endian(DWORD input)
{
    return ((input & 0xFF) << 24) | (((input >> 8) & 0xFF) << 16) |
        (((input >> 16) & 0xFF) << 8) | ((input >> 24) & 0xFF);
}

// Convert big endian QWORD to little endian QWORD.
UINT64 qword_big_to_little_endian(UINT64 input)
{
    return ((input & 0xFFULL) << 56) | (((input >> 8) & 0xFFULL) << 48) |
        (((input >> 16) & 0xFFULL) << 40) | (((input >> 24) & 0xFFULL) << 32) |
        (((input >> 32) & 0xFFULL) << 24) | (((input >> 40) & 0xFFULL) << 16) |
        (((input >> 48) & 0xFFULL) << 8) | ((input >> 56) & 0xFFULL);
}

// Make a hexadecimal string from a hexadecimal array.
void bytearray_to_hexstring(const unsigned char* byteArray, size_t byteArraySize, char* hexString) {
    for (size_t i = 0; i < byteArraySize; ++i) {
        snprintf(&hexString[i * 2], 3, "%02X", byteArray[i]);
    }
}

int round_up(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}

bool CreateDirectoryRecursively(const wchar_t* path) {
    // Attempt to create the directory
    if (CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }

    // If creation failed and the error is because the directory already exists, return true
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }

    // If the error is because of missing parent directories, create them recursively
    if (GetLastError() == ERROR_PATH_NOT_FOUND) {
        size_t pos = std::wstring(path).find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            std::wstring parentPath = std::wstring(path).substr(0, pos);
            if (!CreateDirectoryRecursively(parentPath.c_str())) {
                return false;
            }

            // Retry creating the directory after creating the parent directories
            return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
        }
    }

    return false;
}

uint64_t hash(const char* str) // Hash used for big archives.
{
    uint64_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}