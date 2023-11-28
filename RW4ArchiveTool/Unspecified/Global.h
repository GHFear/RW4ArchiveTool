#pragma once
#include "../Resources/resource.h"
#include <Windows.h>
#include <stdio.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <string>
#include <sstream>
#include "Archives/Compression/refpack/refpackd.h"
#include "../Archives/Structs/SF_Structs.h"


// Define the ID for the list box
#define ID_LIST_VIEW 1001
#define ID_LIST_VIEW2 1002
#define ID_MENU_ITEM1 1003
#define ID_MENU_ITEM2 1004

// Global variables
TCHAR szAppName[] = TEXT("RenderWare4 Archive Tool | Version 0.0.1.5");
HWND hwndMain;
TCHAR szFileName[MAX_PATH] = TEXT("");
HWND hwndListView;
HWND hwndListView2;
HMENU hContextMenu;


