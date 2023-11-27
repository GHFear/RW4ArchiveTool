#pragma once
#include "../../Unspecified/Global.h"

void UpdateWindowTitle(const std::wstring& fileName);

void int_to_wchar(int input_int, wchar_t* wchar_reference, size_t bufferSize)
{
    swprintf_s(wchar_reference, bufferSize, L"%d", input_int);
}

void ConvertCharToWideChar(const char* charString, wchar_t* wideCharString, int wideCharBufferSize) {
    MultiByteToWideChar(CP_UTF8, 0, charString, -1, wideCharString, wideCharBufferSize);
}

// Function to update the window title with the opened file name
void UpdateWindowTitle(const std::wstring& fileName) {
    // Assuming hwndMain is the handle to your main window
    std::wstring windowTitle = szAppName + std::wstring(L" - ") + fileName;
    SetWindowText(hwndMain, windowTitle.c_str());
}

DWORD GetFileSizeFromOpenFileName(HWND& hwnd, const std::wstring& directory, const std::wstring& filename) {
    std::wstring filePath = directory + filename;

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        LPVOID errorMsg;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0, // Default language
            (LPWSTR)&errorMsg,
            0,
            NULL
        );
        MessageBox(hwnd, (LPCWSTR)errorMsg, TEXT("Error opening file"), MB_OK | MB_ICONERROR);
        LocalFree(errorMsg); // Free the buffer allocated by FormatMessage
        return 0;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        DWORD error = GetLastError();
        LPVOID errorMsg;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0, // Default language
            (LPWSTR)&errorMsg,
            0,
            NULL
        );
        MessageBox(hwnd, (LPCWSTR)errorMsg, TEXT("Error getting file size"), MB_OK | MB_ICONERROR);
        LocalFree(errorMsg); // Free the buffer allocated by FormatMessage
        CloseHandle(hFile); // Close the file handle
        return 0;
    }

    CloseHandle(hFile); // Close the file handle

    return fileSize;
}

int CountSelectedFiles(const TCHAR* buffer) {
    int count = 0;

    while (*buffer) {
        ++count;

        // Move to the next null-terminated string
        buffer += _tcslen(buffer) + 1;
    }

    return count;
}