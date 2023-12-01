#pragma once
#include "../../Unspecified/Global.h"

void UpdateWindowTitle(const std::wstring& fileName);


void int_to_wchar(uint32_t input_int, wchar_t* wchar_reference, size_t bufferSize)
{
    swprintf_s(wchar_reference, bufferSize, L"%u", input_int);
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

// Function to update the ListView2 with file information
void UpdateFileView(HWND hwnd_list, std::vector<Archive_Parse_Struct> fileVector) {
    // Clear existing items in the ListView
    ListView_DeleteAllItems(hwnd_list);

    // Iterate through the vector and add each struct's information to the ListView
    LVITEM lvItem;
    memset(&lvItem, 0, sizeof(LVITEM));
    lvItem.mask = LVIF_TEXT;

    for (size_t i = 0; i < fileVector.size(); ++i) {
        // Add the index to the first column
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem(hwnd_list, &lvItem);

        int wideCharBufferSize = MultiByteToWideChar(CP_UTF8, 0, fileVector[i].filename, -1, NULL, 0);
        wchar_t* wideCharFilename = new wchar_t[wideCharBufferSize];
        ConvertCharToWideChar(fileVector[i].filename, wideCharFilename, wideCharBufferSize);

        // File size
        wchar_t file_size[20];
        int_to_wchar(fileVector[i].file_size, file_size, 20);

        // File index
        wchar_t file_index[20];
        int_to_wchar(i, file_index, 20);

        // File offset
        wchar_t file_offset[20];
        int_to_wchar(fileVector[i].file_offset, file_offset, 20);

        wideCharBufferSize = MultiByteToWideChar(CP_UTF8, 0, fileVector[i].ztype, -1, NULL, 0);
        wchar_t* wideCharZType = new wchar_t[wideCharBufferSize];
        ConvertCharToWideChar(fileVector[i].ztype, wideCharZType, wideCharBufferSize);

        // Add other columns
        ListView_SetItemText(hwnd_list, i, 0, const_cast<LPWSTR>(file_index));
        ListView_SetItemText(hwnd_list, i, 1, const_cast<LPWSTR>(wideCharFilename));
        ListView_SetItemText(hwnd_list, i, 2, const_cast<LPWSTR>(file_size));
        ListView_SetItemText(hwnd_list, i, 3, const_cast<LPWSTR>(file_offset));
        ListView_SetItemText(hwnd_list, i, 4, const_cast<LPWSTR>(wideCharZType));

        delete[] wideCharFilename; //Delete buffer
    }
}

// Function to update the ListView1 with Archive information
void UpdateArchiveView(HWND hwnd, wchar_t* file)
{
    // Parse the selected files and update the list view
    std::pair<std::wstring, std::vector<std::wstring>> selected_archives;
    int numSelectedFiles = CountSelectedFiles(file);
    if (numSelectedFiles == 1)
    {
        selected_archives = ParseFilePath(file); // Handle single selection archive paths.
    }
    else
    {
        selected_archives = ParseMultiFilePath(file); // Handle multiple selection archive paths.
    }

    // Update the list view
    HWND hListView = GetDlgItem(hwnd, ID_LIST_VIEW);
    ListView_DeleteAllItems(hwndListView);
    ListView_DeleteAllItems(hwndListView2);

    // Iterate through the vector and add each struct's information to the ListView
    LVITEM lvItem;
    memset(&lvItem, 0, sizeof(LVITEM));
    lvItem.mask = LVIF_TEXT;

    for (size_t i = 0; i < selected_archives.second.size(); ++i)
    {
        // Add the index to the first column
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem(hwndListView, &lvItem);

        // Archive size
        DWORD archive_size = GetFileSizeFromOpenFileName(hwnd, selected_archives.first, selected_archives.second[i]);
        wchar_t archive_size_wchar[20];
        int_to_wchar(archive_size, archive_size_wchar, 20);

        // Archive index
        wchar_t archive_index_wchar[20];
        int_to_wchar(i, archive_index_wchar, 20);

        wchar_t Filename[256];
        wchar_t FileDirectory[256];

        // Create file path wstring.
        std::wstring FullPath = selected_archives.first.c_str();
        FullPath += selected_archives.second[i].c_str();

        SFArchiveHeaderNotDynamic parsed_sf_header = {};
        // Archive streamformat
        wchar_t archive_streamformat_wchar[20];
        // Archive version
        wchar_t archive_Version_wchar[20];
        // Archive numcollections
        wchar_t archive_NumCollections_wchar[20];

        switch (magic::magic_parser(FullPath.c_str()))
        {
        case BIG_EB:
            ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
            ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
            ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
            ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(selected_archives.first.c_str()));
            break;
        case BIG4:
            ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
            ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
            ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
            ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(selected_archives.first.c_str()));
            break;
        case SFIL:
            parsed_sf_header = sf::parse_sfa_header(FullPath.c_str());
            int_to_wchar(dword_big_to_little_endian(parsed_sf_header.StreamFormat), archive_streamformat_wchar, 20);
            int_to_wchar(dword_big_to_little_endian(parsed_sf_header.Version), archive_Version_wchar, 20);
            int_to_wchar(dword_big_to_little_endian(parsed_sf_header.NumCollections), archive_NumCollections_wchar, 20);
            ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
            ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
            ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
            ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(selected_archives.first.c_str()));
            ListView_SetItemText(hwndListView, i, 4, const_cast<LPWSTR>(archive_streamformat_wchar));
            ListView_SetItemText(hwndListView, i, 5, const_cast<LPWSTR>(archive_Version_wchar));
            ListView_SetItemText(hwndListView, i, 6, const_cast<LPWSTR>(archive_NumCollections_wchar));
            break;
        case UNKNOWNARCHIVE:
            continue;
            break;
        default:
            break;
        }
    }
}

bool IsDirectoryPath(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }

    // Check if the path ends with a directory separator
    wchar_t lastChar = path.back();
    if (lastChar == L'\\' || lastChar == L'/') {
        return true;
    }

    // If the path ends with a digit, consider it as a directory path without a filename
    size_t lastSeparator = path.find_last_of(L"\\/");
    if (lastSeparator != std::wstring::npos && isdigit(path.back())) {
        return true;
    }

    return false;
}


// Subclass procedure for a specific child window
LRESULT CALLBACK SubclassListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (message)
    {
    case WM_DROPFILES:
    {
        HDROP hDrop = (HDROP)wParam;

        // Determine the number of files dropped
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

        // Vectors to store filenames and directories
        std::vector<std::wstring> filenames;
        std::vector<std::wstring> directories;

        // Process each dropped file
        for (UINT i = 0; i < fileCount; ++i)
        {
            // Get the size of the buffer needed to store the file path
            UINT bufferSize = DragQueryFile(hDrop, i, NULL, 0) + 1;

            // Allocate a buffer to store the file path
            std::vector<wchar_t> filePathBuffer(bufferSize, L'\0');

            // Get the file path
            DragQueryFile(hDrop, i, filePathBuffer.data(), bufferSize);

            // Check if the file has a .psf extension
            std::wstring filePath(filePathBuffer.data());
            size_t lastDot = filePath.find_last_of(L".");

            if (lastDot != std::wstring::npos)
            {
                std::wstring fileExtension = filePath.substr(lastDot + 1);

                // Check if the file extension is ".psf"
                if (fileExtension != L"psf" && fileExtension != L"wsf" && fileExtension != L"xsf" && fileExtension != L"big")
                {
                    // Skip files with invalid extension
                    continue;
                }
            }
            else
            {
                // If no separator found, assume the whole path is the filename
                continue;
            }

            // Split the file path into filename and directory
            size_t lastSeparator = filePath.find_last_of(L"\\");

            if (lastSeparator != std::wstring::npos)
            {
                std::wstring directory = filePath.substr(0, lastSeparator + 1);
                std::wstring filename = filePath.substr(lastSeparator + 1);

                // Add the filename and directory to the vectors
                filenames.push_back(filename);
                directories.push_back(directory);
            }
            else
            {
                // If no separator found, assume the whole path is the filename
                filenames.push_back(filePath);
                directories.push_back(L"");
            }
        }

        ListView_DeleteAllItems(hwndListView);
        ListView_DeleteAllItems(hwndListView2);

        // Display the filenames and directories
        for (size_t i = 0; i < filenames.size(); ++i)
        {
            // Iterate through the vector and add each struct's information to the ListView
            LVITEM lvItem;
            memset(&lvItem, 0, sizeof(LVITEM));
            lvItem.mask = LVIF_TEXT;

            // Add the index to the first column
            lvItem.iItem = i;
            lvItem.iSubItem = 0;
            lvItem.pszText = LPSTR_TEXTCALLBACK;
            ListView_InsertItem(hwndListView, &lvItem);

            // Archive size
            DWORD archive_size = GetFileSizeFromOpenFileName(hwnd, directories[i].c_str(), filenames[i].c_str());
            wchar_t archive_size_wchar[20];
            int_to_wchar(archive_size, archive_size_wchar, 20);

            // Archive index
            wchar_t archive_index_wchar[20];
            int_to_wchar(i, archive_index_wchar, 20);

            wchar_t Filename[256];
            wchar_t FileDirectory[256];

            // Create file path wstring.
            std::wstring FullPath = directories[i].c_str();
            FullPath += filenames[i].c_str();

            SFArchiveHeaderNotDynamic parsed_sf_header = {};
            // Archive streamformat
            wchar_t archive_streamformat_wchar[20];
            // Archive version
            wchar_t archive_Version_wchar[20];
            // Archive numcollections
            wchar_t archive_NumCollections_wchar[20];

            switch (magic::magic_parser(FullPath.c_str()))
            {
            case BIG_EB:
                ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
                ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(filenames[i].c_str()));
                ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
                ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(directories[i].c_str()));
                break;
            case BIG4:
                ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
                ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(filenames[i].c_str()));
                ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
                ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(directories[i].c_str()));
                break;
            case SFIL:
                parsed_sf_header = sf::parse_sfa_header(FullPath.c_str());
                int_to_wchar(dword_big_to_little_endian(parsed_sf_header.StreamFormat), archive_streamformat_wchar, 20);
                int_to_wchar(dword_big_to_little_endian(parsed_sf_header.Version), archive_Version_wchar, 20);
                int_to_wchar(dword_big_to_little_endian(parsed_sf_header.NumCollections), archive_NumCollections_wchar, 20);
                ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
                ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(filenames[i].c_str()));
                ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
                ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(directories[i].c_str()));
                ListView_SetItemText(hwndListView, i, 4, const_cast<LPWSTR>(archive_streamformat_wchar));
                ListView_SetItemText(hwndListView, i, 5, const_cast<LPWSTR>(archive_Version_wchar));
                ListView_SetItemText(hwndListView, i, 6, const_cast<LPWSTR>(archive_NumCollections_wchar));
                break;
            case UNKNOWNARCHIVE:
                continue;
                break;
            default:
                break;
            } 
        }
        
        DragFinish(hDrop);
        return 0;
    }
    default:
        // Call the original window procedure for unhandled messages
        return DefSubclassProc(hwnd, message, wParam, lParam);
    }
}

// Subclass a ListView control
void SubclassListView(HWND hwndListView)
{
    SetWindowSubclass(hwndListView, SubclassListViewProc, 1, 0);
}