#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

const wchar_t* SearchMutexName = L"SearchWindowMutex";
static HANDLE hSearchMutex = nullptr;
int64_t selectedItemIndex = -1;
std::vector<Archive_Parse_Struct> parsed_archive = {};

// Identifier for the new window class
#define FIND_FILES_WINDOW_CLASS L"FindFileWinClass"

// Scroll to and focus on a specific item in the ListView.
void ScrollAndFocusListViewItem(uint64_t itemIndex) {
    ListView_EnsureVisible(hwndListView2, itemIndex, FALSE);
    ListView_SetItemState(hwndListView2, itemIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

// Convert characters to lower case.
std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        // static_cast<int(*)(int)>(std::tolower)         // wrong
        // [](int c){ return std::tolower(c); }           // wrong
        // [](char c){ return std::tolower(c); }          // wrong
        [](unsigned char c) { return std::tolower(c); } // correct
    );
    return s;
}

// Function to handle the search button click in the new window
int64_t OnSearch(HWND hwnd, int64_t current_index) {
    
    char searchText[256];
    std::size_t index = 0;

    // Retrieve the text from the edit control
    GetWindowTextA(GetDlgItem(hwnd, IDC_SEARCH_EDIT), searchText, 256);
    if (searchText[0] == '\0')
    {
        return -1;
    }

    // Use std::find_if to search the vector of structs
    auto it = std::find_if(parsed_archive.begin() + current_index, parsed_archive.end(),
        [searchText](const Archive_Parse_Struct& myStruct) {
            auto lower_filename = str_tolower(myStruct.filename);
            auto lower_searchtext = str_tolower(searchText);
            return lower_filename.find(lower_searchtext) != std::string::npos;
        });
    
    // Check if the string was found
    if (it != parsed_archive.end()) {
        index = std::distance(parsed_archive.begin(), it);
        std::cout << "String found at index: " << index << std::endl;
        return index;
    }

    return -1;
}

// Here we get the selected index from the File ListView.
bool GetSelectedIndices(int64_t* selectedItemIndex) {
    
    while ((*selectedItemIndex = ListView_GetNextItem(hwndListView2, -1, LVNI_SELECTED)) != -1) {
        return true;
    }

    return false;
}

// Check if the search window is already open by opening mutex with the same name.
bool IsSearchWindowOpen() {
    HANDLE mutexHandle = OpenMutex(SYNCHRONIZE, FALSE, SearchMutexName);

    if (mutexHandle != NULL) {
        CloseHandle(mutexHandle);
        return true; 
    }

    return false; 
}

// Window procedure for FindInFiles Window
LRESULT CALLBACK FindInFilesWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE: {
        
        // Create our search controls.
        CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            10, 10, 280, 25, hwnd, (HMENU)IDC_SEARCH_EDIT, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, L"BUTTON", L"Find", WS_VISIBLE | WS_CHILD,
            300, 10, 75, 25, hwnd, (HMENU)IDC_SEARCH_BUTTON, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_DESTROY:
        // Release the mutex when the window is destroyed.
        ReleaseMutex(hSearchMutex);
        CloseHandle(hSearchMutex);
        break;
    case WM_COMMAND: 
    {
        if (LOWORD(wParam) == IDC_SEARCH_BUTTON && HIWORD(wParam) == BN_CLICKED) {
            // Find next item that matches our search result.
            int64_t ItemToHighlight = 0;
            GetSelectedIndices(&selectedItemIndex);
            selectedItemIndex++;

            // Perform the search.
            ItemToHighlight = OnSearch(hwnd, selectedItemIndex);
            if (ItemToHighlight == -1)
            {
                selectedItemIndex = 0; ItemToHighlight = 0;
                ScrollAndFocusListViewItem(ItemToHighlight);
                break;
            }

            // Scroll and focus to the next item found.
            ScrollAndFocusListViewItem(ItemToHighlight);
            break;
        }
        break;
    }
    case WM_SIZE: {
        InvalidateRect(hwnd, NULL, TRUE);
        break;
        }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateFindInFilesWindow() {

    // Check if another instance of the search window is already open.
    if (IsSearchWindowOpen()) {
        std::cout << "Another instance of the search window is already open." << std::endl;
        return;
    }

    hSearchMutex = CreateMutex(NULL, TRUE, SearchMutexName);
    
    // Register the search window class for the new window.
    WNDCLASS newWindowClass = { 0 };
    newWindowClass.lpfnWndProc = FindInFilesWndProc;
    newWindowClass.hInstance = GetModuleHandle(NULL);
    newWindowClass.lpszClassName = FIND_FILES_WINDOW_CLASS;
    RegisterClass(&newWindowClass);

    // Create the new window.
    HWND hwndNew = CreateWindow(
        FIND_FILES_WINDOW_CLASS, L"Find In Files", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 90, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hwndNew == NULL) {
        MessageBox(NULL, L"Failed to create new window", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Set the search window as topmost.
    SetWindowPos(hwndNew, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Show the search window.
    ShowWindow(hwndNew, SW_SHOWNORMAL);
    UpdateWindow(hwndNew);

    // Set focus to the search input text box.
    SetFocus(GetDlgItem(hwndNew, IDC_SEARCH_EDIT));  
}