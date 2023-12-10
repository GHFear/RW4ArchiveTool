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



// Global variables
TCHAR szAppName[] = TEXT("RenderWare4 Archive Tool | Version 0.0.6.0");
HWND hwndMain;
TCHAR szFileName[MAX_PATH] = TEXT("");
HWND hwndListView;
HWND hwndListView2;
HWND hwndListView3;
HMENU hContextMenuArchives;
HMENU hContextMenuFiles;
