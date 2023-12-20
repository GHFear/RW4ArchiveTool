#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace Big4PackerWindow 
{
    const wchar_t* MutexName = L"Big4PackerWindowMutex";
    static HANDLE hMutex = nullptr;
    std::vector<std::wstring> loaded_files_big4 = {};
    std::wstring selected_path;

    HWND hwnd_CreateBigPacker = {};
    HWND hwndListView_Big4_InFiles = {};
    HWND hTab = {};
    HMENU hContextMenuBig4Packer;
    bool Big4CompressionCheckState = false;

    // Identifier for the new window class
#define BIG4_PACKER_WINDOW_CLASS L"Big4PackerWinClass"


    auto SaveFileDialog(HWND hwnd)
    {
        OPENFILENAME ofn;
        WCHAR szFileName[MAX_PATH] = L"";
        std::wstring save_bigfile_path;
        struct RESULT { bool bDidWeSelect; std::wstring save_bigfile_path; };

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Big Archive (*.big)\0*.big\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"big"; 

        if (GetSaveFileName(&ofn))
        {
            // The selected file name is in szFileName
            save_bigfile_path = szFileName;
            return RESULT{ true, save_bigfile_path };
        }

        return RESULT{ false, save_bigfile_path };
    }

    bool build_big4_setup(HWND hwnd, bool Big4CompressionCheckState, std::wstring save_bigfile_path)
    {
        if (loaded_files_big4.empty())
        {
            MessageBox(hwnd, L"No files are loaded!", L"Packer Prompt", MB_OK | MB_ICONINFORMATION);
            return false;
        }
        else
        {

            if (!big4::bundlebig4(loaded_files_big4, selected_path, Big4CompressionCheckState, save_bigfile_path))
            {
                MessageBox(hwnd, L"Failed to build BIG4 archive!", L"Packer Prompt", MB_OK | MB_ICONINFORMATION);
                return false;
            }
            else
            {
                MessageBox(hwnd, L"Successfully built BIG4 archive!", L"Packer Prompt", MB_OK | MB_ICONINFORMATION);
                return true;
            }
        }
        return true;
    }

    bool load_files_into_listview(HWND hwnd)
    {
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        auto openfolder_results = OpenFolder(hwnd);
        
        if (!openfolder_results.all_files_in_subfolders.empty())
        {
            loaded_files_big4 = openfolder_results.all_files_in_subfolders;
            selected_path = openfolder_results.selected_path;
            UpdateBig4PackerFilepathView(hwndListView_Big4_InFiles, loaded_files_big4, selected_path);
            CoUninitialize();
            return true;
        }
        else
        {
            CoUninitialize();
            return false;
        }
    }

    // Check if the search window is already open by opening mutex with the same name.
    bool IsWindowOpen() {
        HANDLE mutexHandle = OpenMutex(SYNCHRONIZE, FALSE, MutexName);

        if (mutexHandle != NULL) {
            CloseHandle(mutexHandle);
            return true;
        }

        return false;
    }

    // Window procedure for CreateBigPacker Window
    LRESULT CALLBACK CreateBig4PackerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static HWND closeButton, maximizeButton, hCheckbox;

        switch (msg) {
        case WM_CREATE: 
        {

            // Create close button
            closeButton = CreateWindowEx(0, L"BUTTON", L"X", WS_CHILD | WS_VISIBLE,
                0, 0, 30, 20, hwnd, (HMENU)IDC_BIG4PACKER_CLOSE_BUTTON, GetModuleHandle(NULL), NULL);


            // Create the tab control
            hTab = CreateWindow(
                WC_TABCONTROL,              // Class name
                L"",                        // Caption
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, // Style
                0, 0, 400, 300,              // Position and size
                hwnd,                       // Parent window
                (HMENU)IDC_TAB_CONTROL,     // Control identifier
                GetModuleHandle(NULL),       // Instance handle
                NULL);                      // No window creation data

            if (hTab == NULL)
            {
                MessageBox(NULL, L"Failed to create tab control.", L"Error", MB_OK | MB_ICONERROR);
                return -1;
            }

            // Create tabs
            TCITEM tie = { 0 };
            tie.mask = TCIF_TEXT;

            tie.pszText = (LPWSTR)L"Build";
            TabCtrl_InsertItem(hTab, 0, &tie);

            tie.pszText = (LPWSTR)L"Build Settings";
            TabCtrl_InsertItem(hTab, 1, &tie);

            // Create a ListView to display file information
            hwndListView_Big4_InFiles = CreateWindow(WC_LISTVIEW, NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | WS_CLIPSIBLINGS | LVS_SINGLESEL,
                0, 0, 0, 0, hTab, (HMENU)ID_LIST_VIEW_BIG4_WINDOW, GetModuleHandle(NULL), NULL);

            // Set the extended style
            ListView_SetExtendedListViewStyle(hwndListView_Big4_InFiles, LVS_EX_FULLROWSELECT);

            // Set up columns in the ListView
            LVCOLUMN lvc1 = {};

            lvc1.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
            lvc1.cx = 1280; // Width of the column
            lvc1.pszText = (LPWSTR)L"File Path";
            lvc1.iSubItem = 0;
            ListView_InsertColumn(hwndListView_Big4_InFiles, 0, &lvc1);

            // Create a checkbox inside the first tab
            hCheckbox = CreateWindowEx(NULL, 
                L"BUTTON", L"There are compressed files in this archive. ( Check this even if there is only 1 )", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,
                20, 20, 100, 20, hwnd, (HMENU)ID_BIG4_ISCOMPRESSED_CHECKBOX, NULL, NULL);

            // Set compression check state
            if (Big4CompressionCheckState == true)
            {
                SendMessage(hCheckbox, BM_SETCHECK, BST_CHECKED, 0);
            }
            else
            {
                SendMessage(hCheckbox, BM_SETCHECK, BST_UNCHECKED, 0);
            }

            // Create context menu and set the member menu items
            hContextMenuBig4Packer = CreatePopupMenu();
            AppendMenu(hContextMenuBig4Packer, MF_STRING, ID_BIG4PACKER_OPENFOLDER_CONTEXTBTN, L"Open Folder");
            AppendMenu(hContextMenuBig4Packer, MF_STRING, ID_BIG4PACKER_BUILD_CONTEXTBTN, L"Build");
            AppendMenu(hContextMenuBig4Packer, MF_STRING, ID_BIG4PACKER_CLEARLIST_CONTEXTBTN, L"Clear List");

            if (!loaded_files_big4.empty())
            {
                UpdateBig4PackerFilepathView(hwndListView_Big4_InFiles, loaded_files_big4, selected_path);
                EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_CLEARLIST_CONTEXTBTN, MF_BYCOMMAND | (true ? MF_ENABLED : MF_GRAYED));
                EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_BUILD_CONTEXTBTN, MF_BYCOMMAND | (true ? MF_ENABLED : MF_GRAYED));
                DrawMenuBar(hwnd);
            }
            else
            {
                EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_CLEARLIST_CONTEXTBTN, MF_BYCOMMAND | (false ? MF_ENABLED : MF_GRAYED));
                EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_BUILD_CONTEXTBTN, MF_BYCOMMAND | (false ? MF_ENABLED : MF_GRAYED));
                DrawMenuBar(hwnd);
            }
            
            ShowWindow(hCheckbox, SW_HIDE);
            
            break;
        }
        case WM_DESTROY:
        {
            // Release the mutex when the window is destroyed.
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
            break;
        }
        case WM_NOTIFY:
        {
            NMHDR* pnmhdr = (NMHDR*)lParam;
            int currentTab = TabCtrl_GetCurSel(hTab);
            if (pnmhdr->idFrom == IDC_TAB_CONTROL)
            {
                
                if (pnmhdr->code == TCN_SELCHANGE)
                {
                    // Show/hide ListViews based on the selected tab
                    ShowWindow(hwndListView_Big4_InFiles, (currentTab == 0) ? SW_SHOW : SW_HIDE);
                    ShowWindow(hCheckbox, (currentTab == 1) ? SW_SHOW : SW_HIDE);
                }
                
            }

            LPNMHDR pnmh = (LPNMHDR)lParam;
            if (pnmh->idFrom == ID_LIST_VIEW_BIG4_WINDOW)
            {
                
                switch (pnmh->code)
                {
                case NM_RCLICK:
                {
                    // Get the cursor position
                    POINT cursor;
                    GetCursorPos(&cursor);

                    // Track the context menu
                    TrackPopupMenu(hContextMenuBig4Packer, TPM_LEFTALIGN | TPM_TOPALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
                    break;
                }
                }
            }

            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam)) {
                case ID_BIG4PACKER_OPENFOLDER_CONTEXTBTN:
                {
                    if (load_files_into_listview(hwnd))
                    {
                        // Enable or disable the menu item dynamically
                        EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_CLEARLIST_CONTEXTBTN, MF_BYCOMMAND | (true ? MF_ENABLED : MF_GRAYED));
                        EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_BUILD_CONTEXTBTN, MF_BYCOMMAND | (true ? MF_ENABLED : MF_GRAYED));
                        DrawMenuBar(hwnd);
                    }
                    break;
                }
                case ID_BIG4PACKER_BUILD_CONTEXTBTN:
                {
                    auto save_big4_information = SaveFileDialog(hwnd);
                    
                    if (save_big4_information.bDidWeSelect)
                    {
                        build_big4_setup(hwnd, Big4CompressionCheckState, save_big4_information.save_bigfile_path);
                    }
                    
                    break;
                }
                case ID_BIG4PACKER_CLEARLIST_CONTEXTBTN:
                {
                    if (loaded_files_big4.empty())
                    {
                        MessageBox(NULL, L"List is already empty!", L"Error", MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        loaded_files_big4.clear();
                        ListView_DeleteAllItems(hwndListView_Big4_InFiles);
                        // Enable or disable the menu item dynamically
                        EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_CLEARLIST_CONTEXTBTN, MF_BYCOMMAND | (false ? MF_ENABLED : MF_GRAYED));
                        EnableMenuItem(hContextMenuBig4Packer, ID_BIG4PACKER_BUILD_CONTEXTBTN, MF_BYCOMMAND | (false ? MF_ENABLED : MF_GRAYED));
                        DrawMenuBar(hwnd);
                    }
                    break;
                }
                case IDC_BIG4PACKER_CLOSE_BUTTON:
                {
                    DestroyWindow(hwnd);
                    break;
                }
                case ID_BIG4_ISCOMPRESSED_CHECKBOX:
                {
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        // The checkbox was clicked
                        LRESULT state = SendMessage(GetDlgItem(hwnd, ID_BIG4_ISCOMPRESSED_CHECKBOX), BM_GETCHECK, 0, 0);

                        if (state == BST_CHECKED) {
                            // The checkbox is checked
                            Big4CompressionCheckState = true;
                        }
                        else {
                            // The checkbox is unchecked
                            Big4CompressionCheckState = false;
                        }
                    }
                    break;
                }
                  
            }
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Perform your drawing operations here
            HBRUSH hBrush = CreateSolidBrush(RGB(220, 220, 220)); // Adjust the color if needed
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTLEFT || LOWORD(lParam) == HTRIGHT)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));  
                return TRUE;
            }
            else if (LOWORD(lParam) == HTTOP || LOWORD(lParam) == HTBOTTOM)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZENS));
                return TRUE;
            }
            else if (LOWORD(lParam) == HTBOTTOMLEFT || LOWORD(lParam) == HTTOPRIGHT)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZENESW));  
                return TRUE;
            }
            else if (LOWORD(lParam) == HTBOTTOMRIGHT || LOWORD(lParam) == HTTOPLEFT)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
                return TRUE;
            }
            else
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            break;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_SIZE: 
        {
            // Set the size for the listviews
            int windowWidth = LOWORD(lParam);
            int windowHeight = HIWORD(lParam) - 0;

            // left side
            int left_listview_width = ((windowWidth) - 15 );
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

            // Listview
            MoveWindow(hwndListView_Big4_InFiles, 5, 25, left_listview_width - middle_padding, windowHeight - 36, TRUE);

            // Checkboxes
            MoveWindow(hCheckbox, 15, 25, left_listview_width - middle_padding, 40, TRUE);

            // Tabs
            MoveWindow(hTab, 5, 0, windowWidth - 10, windowHeight - outer_border_padding, TRUE);

            // Window
            RECT parentClientRect;
            GetClientRect(GetParent(hwnd), &parentClientRect);
            int newWidth = parentClientRect.right - parentClientRect.left;
            int newHeight = parentClientRect.bottom - parentClientRect.top;
            SetWindowPos(hwnd, HWND_TOP, 0, 0, newWidth, newHeight, SWP_NOMOVE);
            SetWindowPos(closeButton, HWND_TOP,
                newWidth - 30, 0, 30, 20, SWP_SHOWWINDOW);

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            RedrawWindow(hTab, NULL, NULL, RDW_INVALIDATE);
            break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    void CreateBig4PackerWindow(HWND parent_hwnd) 
    {

        // Check if another instance of the search window is already open.
        if (IsWindowOpen()) {
            std::cout << "Another instance of the window is already open." << std::endl;
            return;
        }

        hMutex = CreateMutex(NULL, TRUE, MutexName);

        // Register the search window class for the new window.
        WNDCLASS newWindowClass = { 0 };
        newWindowClass.lpfnWndProc = CreateBig4PackerWndProc;
        newWindowClass.hInstance = GetModuleHandle(NULL);
        newWindowClass.lpszClassName = BIG4_PACKER_WINDOW_CLASS;
        RegisterClass(&newWindowClass);

        // Create the new window.
        hwnd_CreateBigPacker = CreateWindowEx(
            0, BIG4_PACKER_WINDOW_CLASS, L"Big4 Builder", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            50, 50, 200, 30, parent_hwnd, NULL, GetModuleHandle(NULL), NULL);

        if (hwnd_CreateBigPacker == NULL) {
            MessageBox(NULL, L"Failed to create new window", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Remove layered style
        SetWindowLong(hwnd_CreateBigPacker, GWL_EXSTYLE, GetWindowLong(hwnd_CreateBigPacker, GWL_EXSTYLE) & WS_EX_LAYERED);
        SetWindowPos(hwnd_CreateBigPacker, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE);

        // Show the search window.
        ShowWindow(hwnd_CreateBigPacker, SW_SHOWMAXIMIZED);
        UpdateWindow(hwnd_CreateBigPacker);
    }
}