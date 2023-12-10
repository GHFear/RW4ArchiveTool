 // EA Skate SF Parser By GHFear.

#include "rw4_archive_tool.h"
#include "Archives/Parsers/magic_parser.h"
#include "Archives/Parsers/sf_parser.h"
#include "Archives/Parsers/big_eb_parser.h"
#include "Archives/Parsers/big4_parser.h"
#include "Archives/Parsers/Arena_Parser.h"
#include "Tools/Program/ProgramHandlers.h"
#include "Windows/FindWindow.h"


// Function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point of the program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    MSG msg;
    WNDCLASS wndClass = {};

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
        INITCOMMONCONTROLSEX icex = {};
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        // Create Menu
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreateMenu();
        AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, TEXT("Open"));
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("File"));

        HMENU hSubMenu2 = CreateMenu();
        AppendMenu(hSubMenu2, MF_STRING, ID_FIND_IN_FILES, TEXT("Find In Files"));
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu2, TEXT("Find"));

        // Assign the menu to the window
        SetMenu(hwnd, hMenu);

        // Create a ListView to display file information
        hwndListView = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            0, 0, 0, 0, hwnd, (HMENU)ID_LIST_VIEW, GetModuleHandle(NULL), NULL);

        // Set the extended style
        ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

        // Set up columns in the ListView
        LVCOLUMN lvc1 = {};

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 150; // Width of the column
        lvc1.pszText = (LPWSTR)L"Archive";
        lvc1.iSubItem = 0;
        ListView_InsertColumn(hwndListView, 0, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 75; // Width of the column
        lvc1.pszText = (LPWSTR)L"Size";
        lvc1.iSubItem = 1;
        ListView_InsertColumn(hwndListView, 1, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 50; // Width of the column
        lvc1.pszText = (LPWSTR)L"Path";
        lvc1.iSubItem = 2;
        ListView_InsertColumn(hwndListView, 2, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 50; // Width of the column
        lvc1.pszText = (LPWSTR)L"Format";
        lvc1.iSubItem = 3;
        ListView_InsertColumn(hwndListView, 3, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 50; // Width of the column
        lvc1.pszText = (LPWSTR)L"Ver.";
        lvc1.iSubItem = 4;
        ListView_InsertColumn(hwndListView, 4, &lvc1);

        lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc1.cx = 50; // Width of the column
        lvc1.pszText = (LPWSTR)L"Collections";
        lvc1.iSubItem = 5;
        ListView_InsertColumn(hwndListView, 5, &lvc1);

        // Enable drag-and-drop
        DragAcceptFiles(hwndListView, TRUE);

        // Subclass the first ListView control
        SubclassListView(hwndListView);

        // Create a ListView to display file information
        hwndListView2 = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            0, 0, 0, 0, hwnd, (HMENU)ID_LIST_VIEW2, GetModuleHandle(NULL), NULL);

        // Set the extended style
        ListView_SetExtendedListViewStyle(hwndListView2, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

        // Set up columns in the ListView
        LVCOLUMN lvc2 = {};
        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 50; // Width of the column
        lvc2.pszText = (LPWSTR)L"Index";
        lvc2.iSubItem = 0;
        ListView_InsertColumn(hwndListView2, 0, &lvc2);

        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 200; 
        lvc2.pszText = (LPWSTR)L"Filename";
        lvc2.iSubItem = 1;
        ListView_InsertColumn(hwndListView2, 1, &lvc2);

        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 75; 
        lvc2.pszText = (LPWSTR)L"Size";
        lvc2.iSubItem = 2;
        ListView_InsertColumn(hwndListView2, 2, &lvc2);

        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 75; 
        lvc2.pszText = (LPWSTR)L"Offset";
        lvc2.iSubItem = 3;
        ListView_InsertColumn(hwndListView2, 3, &lvc2);

        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 100;
        lvc2.pszText = (LPWSTR)L"ZType";
        lvc2.iSubItem = 4;
        ListView_InsertColumn(hwndListView2, 4, &lvc2);

        lvc2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc2.cx = 100;
        lvc2.pszText = (LPWSTR)L"TOC Offset";
        lvc2.iSubItem = 5;
        ListView_InsertColumn(hwndListView2, 5, &lvc2);

        // Subclass the second ListView control
        SubclassListView(hwndListView2);

        // Create a ListView to display file information
        hwndListView3 = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            0, 0, 0, 0, hwnd, (HMENU)ID_LIST_VIEW, GetModuleHandle(NULL), NULL);

        // Set the extended style
        ListView_SetExtendedListViewStyle(hwndListView3, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

        // Set up columns in the ListView
        LVCOLUMN lvc3 = {};

        lvc3.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc3.cx = 125; // Width of the column
        lvc3.pszText = (LPWSTR)L"These lists";
        lvc3.iSubItem = 0;
        ListView_InsertColumn(hwndListView3, 0, &lvc3);

        lvc3.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc3.cx = 125; // Width of the column
        lvc3.pszText = (LPWSTR)L"will display";
        lvc3.iSubItem = 1;
        ListView_InsertColumn(hwndListView3, 1, &lvc3);

        lvc3.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc3.cx = 150; // Width of the column
        lvc3.pszText = (LPWSTR)L"information about";
        lvc3.iSubItem = 2;
        ListView_InsertColumn(hwndListView3, 2, &lvc3);

        lvc3.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc3.cx = 150; // Width of the column
        lvc3.pszText = (LPWSTR)L"whatever you're";
        lvc3.iSubItem = 3;
        ListView_InsertColumn(hwndListView3, 3, &lvc3);

        lvc3.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc3.cx = 150; // Width of the column
        lvc3.pszText = (LPWSTR)L"highlighting.";
        lvc3.iSubItem = 4;
        ListView_InsertColumn(hwndListView3, 4, &lvc3);

        // Create context menu and set the member menu items
        hContextMenuArchives = CreatePopupMenu();
        AppendMenu(hContextMenuArchives, MF_STRING, ID_ARCHIVEMENU_ITEM1, L"Unpack");
        AppendMenu(hContextMenuArchives, MF_STRING, ID_ARCHIVEMENU_ITEM2, L"Open Directory");

        // Create context menu and set the member menu items
        hContextMenuFiles = CreatePopupMenu();
        AppendMenu(hContextMenuFiles, MF_STRING, ID_FILEMENU_ITEM1, L"Unpack File");

        break;
    } 
    case WM_SIZE:
    {
        // Set the size for the listviews
        int windowWidth = LOWORD(lParam);
        int windowHeight = HIWORD(lParam);

        // left side
        int left_listview_width = ((windowWidth) / 2);
        int left_listview_height = std::round((2.0 * windowHeight) / 3.0);

        // Right side top
        int right_top_listview_width = ((windowWidth) / 2);
        int right_top_listview_height = std::round((2.0 * windowHeight) / 3.0);

        // Right side bottom
        int right_bottom_listview_width = ((windowWidth) / 2);
        int right_bottom_listview_height = windowHeight - right_top_listview_height;

        // Padding
        int middle_padding = 6;
        int right_divider_padding = 1;
        int outer_border_padding = 5;

        MoveWindow(hwndListView, 5, 0, left_listview_width - middle_padding, windowHeight - outer_border_padding, TRUE);
        MoveWindow(hwndListView2, right_top_listview_width, 0, right_top_listview_width - outer_border_padding, right_top_listview_height - right_divider_padding, TRUE);
        MoveWindow(hwndListView3, left_listview_width, right_top_listview_height, right_bottom_listview_width - outer_border_padding, right_bottom_listview_height - outer_border_padding, TRUE);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->idFrom == ID_LIST_VIEW) 
        {
            switch (pnmh->code) 
            {
            case NM_RCLICK:
            {
                // Handle right-click or left-click
                LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
                int selectedItem = lpnmitem->iItem;
                if (selectedItem != -1)
                {
                    // Get the cursor position
                    POINT cursor;
                    GetCursorPos(&cursor);

                    // Track the context menu
                    TrackPopupMenu(hContextMenuArchives, TPM_LEFTALIGN | TPM_TOPALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
                }
                break;
            }
            case NM_DBLCLK:
                // Handle double-click
                LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)lParam;

                // Get the selected item text
                int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
                if (selectedItemIndex != -1) {
                    wchar_t Filename[512] = {};
                    wchar_t FileDirectory[512] = {};
                    ListView_GetItemText(hwndListView, selectedItemIndex, 0, Filename, sizeof(Filename) / sizeof(Filename[0]));
                    ListView_GetItemText(hwndListView, selectedItemIndex, 2, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                    // Create file path wstring.
                    std::wstring FullPath = FileDirectory;
                    FullPath += Filename;

                    // Parse Archive                
                    switch (magic::magic_parser(FullPath.c_str()))
                    {
                    case BIG_EB:
                        parsed_archive = big_eb::parse_big_eb_archive(FullPath.c_str(), FileDirectory, false);
                        break;
                    case BIG4:
                        parsed_archive = big4::parse_big4_archive(FullPath.c_str(), FileDirectory, false);
                        break;
                    case SFIL:
                        parsed_archive = sf::parse_sf_archive(FullPath.c_str(), FileDirectory, false);
                        break;
                    case ARENA:
                        parsed_archive = arena::parse_arena_filepackage(FullPath.c_str(), FileDirectory, false);
                        break;
                    case UNKNOWNARCHIVE:
                        break;
                    default:
                        break;
                    }

                    UpdateFileView(hwndListView2, parsed_archive); // Update our listview on double click.

                    break;
                }
                break;
            }
        }
        else if (pnmh->idFrom == ID_LIST_VIEW2)
        {
            switch (pnmh->code)
            {
            case NM_RCLICK:
            {
                // Handle right-click or left-click
                LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
                int selectedItem = lpnmitem->iItem;
                if (selectedItem != -1)
                {
                    // Get the cursor position
                    POINT cursor;
                    GetCursorPos(&cursor);

                    // Track the context menu
                    TrackPopupMenu(hContextMenuFiles, TPM_LEFTALIGN | TPM_TOPALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
                }
                break;
            }
            case NM_DBLCLK:
                break; 
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FIND_IN_FILES:
            CreateFindInFilesWindow();
            //MessageBox(hwnd, L"This feature hasn't been implemented just yet ;)", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
            break;
        case ID_FILE_OPEN:
            OpenFileAndLoadBuffer(hwnd);
            break;
        case ID_ARCHIVEMENU_ITEM1:
        {
            // Get the selected item text
            int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if (selectedItemIndex != -1) {
                wchar_t Filename[256] = {};
                wchar_t FileDirectory[256] = {};
                ListView_GetItemText(hwndListView, selectedItemIndex, 0, Filename, sizeof(Filename) / sizeof(Filename[0]));
                ListView_GetItemText(hwndListView, selectedItemIndex, 2, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                // Create file path wstring.
                std::wstring FullPath = FileDirectory;
                FullPath += Filename;

                // Unpack type selector.
                switch (magic::magic_parser(FullPath.c_str()))
                {
                case BIG_EB:
                    big_eb::parse_big_eb_archive(FullPath.c_str(), FileDirectory, true);
                    MessageBox(hwnd, L"Archive was unpacked successfully!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                case BIG4:
                    big4::parse_big4_archive(FullPath.c_str(), FileDirectory, true);
                    MessageBox(hwnd, L"Archive was unpacked successfully!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                case SFIL:
                    sf::parse_sf_archive(FullPath.c_str(), FileDirectory, true);
                    MessageBox(hwnd, L"Archive was unpacked successfully!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                case ARENA:
                    arena::parse_arena_filepackage(FullPath.c_str(), FileDirectory, true);
                    MessageBox(hwnd, L"Archive was unpacked successfully!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                case UNKNOWNARCHIVE:
                    MessageBox(hwnd, L"Archive is of unknown type!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    break;
                default:
                    break;
                }
            }
            break;
        }
        case ID_ARCHIVEMENU_ITEM2:
        {
            // Get the selected item text
            int selectedItemIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if (selectedItemIndex != -1) {
                wchar_t FileDirectory[256] = {};
                ListView_GetItemText(hwndListView, selectedItemIndex, 2, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

                ShellExecute(NULL, L"open", L"explorer.exe", FileDirectory, NULL, SW_SHOWNORMAL);
            }
            break;
        }
        case ID_FILEMENU_ITEM1:
            MessageBox(hwnd, L"This feature hasn't been implemented just yet ;)", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;
    case WM_DESTROY:
        DragAcceptFiles(hwndListView, FALSE);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}