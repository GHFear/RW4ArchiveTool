#pragma once

#include <filesystem>  // Include this for fs::file_size

namespace fs = std::filesystem;

wchar_t* ConvertCharToWchar(const char* charString) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, charString, -1, NULL, 0);
    wchar_t* wideCharString = new wchar_t[size_needed];
    MultiByteToWideChar(CP_UTF8, 0, charString, -1, wideCharString, size_needed);
    return wideCharString;
}

std::wstring to_wstring(const std::string StringToConvert)
{
    int wideStringLength = MultiByteToWideChar(CP_UTF8, 0, StringToConvert.c_str(), -1, nullptr, 0);
    wchar_t* wideStringBuffer = new wchar_t[wideStringLength];
    MultiByteToWideChar(CP_UTF8, 0, StringToConvert.c_str(), -1, wideStringBuffer, wideStringLength);
    std::wstring wideString(wideStringBuffer);
    delete[] wideStringBuffer;

    return wideString;
}

std::wstring removeLastFolder(const std::wstring& path) {
    // Find the last occurrence of the path separator
    size_t pos = path.find_last_of(L'\\');

    // Check if the path separator is found
    if (pos != std::wstring::npos) {
        // Extract the substring excluding the last folder
        return path.substr(0, pos);
    }
    else {
        // Return an empty string or the original string if there is no path separator
        return path;
    }
}

std::wstring replaceBackslashesWithForward(const std::wstring& path) {
    std::wstring modifiedPath = path;

    // Find and replace all occurrences of double backslashes with forward slashes
    size_t pos = modifiedPath.find(L"\\");
    while (pos != std::wstring::npos) {
        modifiedPath.replace(pos, 1, L"/");  // Replace 2 characters with a forward slash
        pos = modifiedPath.find(L"\\", pos + 1);  // Find the next occurrence
    }

    return modifiedPath;
}

std::wstring removeFirstForwardSlash(const std::wstring& path) {
    std::wstring modifiedPath = path;

    // Check if the path starts with a forward slash
    if (!modifiedPath.empty() && modifiedPath[0] == L'/') {
        // Erase the first character (forward slash)
        modifiedPath.erase(0, 1);
    }

    return modifiedPath;
}

std::string replaceFileExtension(const std::string& filePath, const std::string& newExtension) {
    fs::path path(filePath);
    path.replace_extension(newExtension);
    return path.string();
}

std::pair<std::wstring, std::vector<std::wstring>> ParseMultiFilePath(const _TCHAR* multiFilePath) {
    std::vector<std::wstring> fileNames;
    std::wstring directory;

    if (multiFilePath) {
        // The directory is the first part of the buffer
        directory = multiFilePath;

        // Check if there is a second null character indicating multiple files
        if (multiFilePath[wcslen(directory.c_str()) + 1] != '\0') {
            // Multiple files are selected
            // The buffer contains the directory followed by null-terminated file names
            const _TCHAR* currentFile = multiFilePath + wcslen(directory.c_str()) + 1;
            while (*currentFile) {
                // 'currentFile' points to each null-terminated file name
                // Add the file name to the vector
                fileNames.push_back(currentFile);

                // Move to the next file name
                currentFile += wcslen(currentFile) + 1;
            }
        }
        else {
            // Only one file is selected
            // Add the single file name to the vector
            fileNames.push_back(multiFilePath + wcslen(directory.c_str()) + 1);
        }
    }

    return std::make_pair(directory + L"\\", fileNames);
}

// Function to get the file extension from a wchar_t filepath
const wchar_t* GetFileExtension(const wchar_t* filepath) {
    // Find the last dot ('.') character in the filepath
    const wchar_t* dot = wcsrchr(filepath, L'.');

    // Check if a dot was found and it is not the last character
    if (dot != nullptr && dot[1] != L'\0') {
        // Return the characters following the dot as the file extension
        return dot + 1;
    }

    // No file extension found
    return nullptr;
}

std::pair<std::wstring, std::vector<std::wstring>> ParseFilePath(const std::wstring& filePath) {
    // Find the last backslash to separate the directory and filename
    size_t lastBackslash = filePath.find_last_of(L'\\');

    // Check if a backslash was found
    if (lastBackslash != std::wstring::npos) {
        // Extract the directory and filename
        std::wstring directory = filePath.substr(0, lastBackslash + 1);  // Include the trailing backslash
        std::wstring filename = filePath.substr(lastBackslash + 1);

        // Return the pair
        return std::make_pair(directory, std::vector<std::wstring>{filename});
    }
    else {
        // No backslash found, treat the entire string as the filename
        return std::make_pair(L"", std::vector<std::wstring>{filePath});
    }
}

// Function to get the filename without extension from a wchar_t filepath
const wchar_t* GetFilenameWithoutExtension(const wchar_t* filepath) {
    // Find the last path separator (backslash) in the filepath
    const wchar_t* lastBackslash = wcsrchr(filepath, L'\\');

    // Find the last dot ('.') character after the last path separator
    const wchar_t* lastDot = wcsrchr(lastBackslash != nullptr ? lastBackslash : filepath, L'.');

    // Check if a dot was found and it is not the last character
    if (lastDot != nullptr && lastDot[1] != L'\0') {
        // Calculate the length of the filename without extension
        size_t length = lastDot - (lastBackslash != nullptr ? lastBackslash + 1 : filepath);

        // Allocate memory for the result and copy the filename without extension
        wchar_t* result = new wchar_t[length + 1];
        wmemcpy(result, lastBackslash != nullptr ? lastBackslash + 1 : filepath, length);
        result[length] = L'\0';

        return result;
    }

    // No extension found, return the whole filename
    return lastBackslash != nullptr ? lastBackslash + 1 : filepath;
}


std::string WideStringToString(std::wstring wstr)
{

    // Calculate the required size for the buffer
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

    // Allocate a buffer
    char* buffer = new char[size];

    // Convert wstring to string
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, size, nullptr, nullptr);
    std::string str(buffer);

    // Clean up the buffer
    delete[] buffer;

    return str;
}

std::wstring make_big_compatible_paths(const size_t& pos, const std::wstring& top_level_path, const std::wstring& input_wstring)
{
    std::wstring temp_wstring = input_wstring;
    temp_wstring.erase(pos, top_level_path.length());
    temp_wstring = replaceBackslashesWithForward(temp_wstring);
    temp_wstring = removeFirstForwardSlash(temp_wstring);
    return temp_wstring;
}