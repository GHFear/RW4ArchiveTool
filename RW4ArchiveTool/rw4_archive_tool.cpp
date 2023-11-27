// EA Skate SF Parser By GHFear.

#include "rw4_archive_tool.h"
#include "Unspecified/Global.h"
#include "Archives/Parsers/sf_header_parser.h"

// Function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Function to update the ListView2 with file information
void UpdateFileView(HWND hwnd_list, std::vector<SF_Parse_Struct> fileVector) {
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
        swprintf_s(file_size, L"%d", fileVector[i].file_size);

        // File index
        wchar_t file_index[20]; 
        swprintf_s(file_index, L"%d", i);

        // File offset
        wchar_t file_offset[20]; 
        swprintf_s(file_offset, L"%ld", fileVector[i].file_offset);

        // Add other columns
        ListView_SetItemText(hwnd_list, i, 0, const_cast<LPWSTR>(file_index));
        ListView_SetItemText(hwnd_list, i, 1, const_cast<LPWSTR>(wideCharFilename));
        ListView_SetItemText(hwnd_list, i, 2, const_cast<LPWSTR>(file_size));
        ListView_SetItemText(hwnd_list, i, 3, const_cast<LPWSTR>(file_offset));

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

        // Add other columns
        ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(archive_index_wchar));
        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(archive_size_wchar));
        ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(selected_archives.first.c_str()));
    }
}

// Function to open a file dialog and load the file into the buffer
void OpenFileAndLoadBuffer(HWND hwnd) {
    OPENFILENAME ofn = { sizeof ofn };
    wchar_t file[10240];
    file[0] = '\0';
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = file;
    ofn.nMaxFile = 10240;
    ofn.lpstrFilter = TEXT("EA Skate Archives\0*.xsf;*.psf;*.wsf;*.big\0All Files\0*.*\0\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    // Display the Open dialog box.
    if (GetOpenFileName(&ofn) == TRUE) 
    {
        UpdateArchiveView(hwnd, file);
    }
}

// Entry point of the program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    MSG msg;
    WNDCLASS wndClass;

    // Register the window class.
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = szAppName;

    if (!RegisterClass(&wndClass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    // Create the main window.
    hwndMain = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    // Display the window.
    ShowWindow(hwndMain, iCmdShow);
    UpdateWindow(hwndMain);

    // Message loop.
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Window procedure.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    switch (message) {
    case WM_CREATE:
    {
        // Initialize the common controls (If we need them at some point)
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        // Create a menu
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreateMenu();
        AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, TEXT("Open"));
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("File"));

        // Assign the menu to the window
        SetMenu(hwnd, hMenu);

        // Create a ListView to display file information
        hwndListView = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            0, 0, 0, 0, hwnd, (HMENU)ID_LIST_VIEW, GetModuleHandle(NULL), NULL);

        // Set the extended style
        ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

        // Set up columns in the ListView
        LVCOLUMN lvc1;

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 50; // Width of the column
        lvc1.pszText = (LPWSTR)L"Index";
        lvc1.iSubItem = 0;
        ListView_InsertColumn(hwndListView, 0, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 150; // Width of the column
        lvc1.pszText = (LPWSTR)L"Archive";
        lvc1.iSubItem = 1;
        ListView_InsertColumn(hwndListView, 1, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 100; // Width of the column
        lvc1.pszText = (LPWSTR)L"Size";
        lvc1.iSubItem = 2;
        ListView_InsertColumn(hwndListView, 2, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 350; // Width of the column
        lvc1.pszText = (LPWSTR)L"Path";
        lvc1.iSubItem = 3;
        ListView_InsertColumn(hwndListView, 3, &lvc1);


        hwndListView2 = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            0, 0, 0, 0, hwnd, (HMENU)ID_LIST_VIEW2, GetModuleHandle(NULL), NULL);

        // Set the extended style
        ListView_SetExtendedListViewStyle(hwndListView2, LVS_EX_DOUBLEBUFFER |LVS_EX_FULLROWSELECT);

        // Set up columns in the ListView
        LVCOLUMN lvc2;
        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 100; // Width of the column
        lvc2.pszText = (LPWSTR)L"Index";
        lvc2.iSubItem = 0;
        ListView_InsertColumn(hwndListView2, 0, &lvc2);

        lvc2.cx = 200; 
        lvc2.pszText = (LPWSTR)L"Filename";
        lvc2.iSubItem = 1;
        ListView_InsertColumn(hwndListView2, 1, &lvc2);

        lvc2.cx = 100; 
        lvc2.pszText = (LPWSTR)L"Size";
        lvc2.iSubItem = 2;
        ListView_InsertColumn(hwndListView2, 2, &lvc2);

        lvc2.cx = 100; 
        lvc2.pszText = (LPWSTR)L"Offset";
        lvc2.iSubItem = 3;
        ListView_InsertColumn(hwndListView2, 3, &lvc2);

        // Create context menu and set the member menu items
        hContextMenu = CreatePopupMenu();
        AppendMenu(hContextMenu, MF_STRING, ID_MENU_ITEM1, L"Unpack");
        AppendMenu(hContextMenu, MF_STRING, ID_MENU_ITEM2, L"Open Directory");
        HMENU hListViewMenu = GetMenu(hwnd);
        SetMenu(hwnd, NULL);
        SetMenu(hwnd, hListViewMenu);

        break;
    }
    case WM_CONTEXTMENU:
    {
        if ((HWND)wParam == hwndListView) {
            // Get the cursor position
            POINT cursor;
            GetCursorPos(&cursor);

            // Track the context menu
            TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
        }
        break;
    }
    case WM_SIZE:
    {
        // Set the size for the listviews
        int windowWidth = LOWORD(lParam);
        int windowHeight = HIWORD(lParam) - 5;
        MoveWindow(hwndListView, 5, 0, windowWidth / 2 - 10, windowHeight, TRUE);
        MoveWindow(hwndListView2, (windowWidth / 2) - 5, 0, windowWidth / 2, windowHeight, TRUE);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->idFrom == ID_LIST_VIEW) {
            switch (pnmh->code) {
            case NM_DBLCLK:
                // Handle double-click
                LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)lParam;

                // Get the selected item text
                int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
                if (selectedItemIndex != -1) {
                    wchar_t Filename[256];
                    wchar_t FileDirectory[256];
                    ListView_GetItemText(hwndListView, selectedItemIndex, 1, Filename, sizeof(Filename) / sizeof(Filename[0]));
                    ListView_GetItemText(hwndListView, selectedItemIndex, 3, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                    // Create file path wstring.
                    std::wstring FullPath = FileDirectory;
                    FullPath += Filename;

                    // Parse Archive
                    auto parsed_wsf = sf::sf_parser(FullPath.c_str(), FileDirectory, false);
                    UpdateFileView(hwndListView2, parsed_wsf); // Update our listview on double click.
                }
                break;
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_OPEN:
            OpenFileAndLoadBuffer(hwnd);
            break;
        case ID_MENU_ITEM1:
        {
            // Get the selected item text
            int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if (selectedItemIndex != -1) {
                wchar_t Filename[256];
                wchar_t FileDirectory[256];
                ListView_GetItemText(hwndListView, selectedItemIndex, 1, Filename, sizeof(Filename) / sizeof(Filename[0]));
                ListView_GetItemText(hwndListView, selectedItemIndex, 3, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                // Create file path wstring.
                std::wstring FullPath = FileDirectory;
                FullPath += Filename;

                // Unpack Archive
                auto parsed_wsf = sf::sf_parser(FullPath.c_str(), FileDirectory, true);
                MessageBox(hwnd, L"Archive was unpacked successfully!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case ID_MENU_ITEM2:
        {
            // Get the selected item text
            int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if (selectedItemIndex != -1) {
                wchar_t FileDirectory[256];
                ListView_GetItemText(hwndListView, selectedItemIndex, 3, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                ShellExecute(NULL, L"open", L"explorer.exe", FileDirectory, NULL, SW_SHOWNORMAL);
            }
            break;
        }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}