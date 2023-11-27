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