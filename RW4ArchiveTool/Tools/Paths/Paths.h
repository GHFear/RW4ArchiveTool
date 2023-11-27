#pragma once

wchar_t* ConvertCharToWchar(const char* charString) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, charString, -1, NULL, 0);
    wchar_t* wideCharString = new wchar_t[size_needed];
    MultiByteToWideChar(CP_UTF8, 0, charString, -1, wideCharString, size_needed);
    return wideCharString;
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