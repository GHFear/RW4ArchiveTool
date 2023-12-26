#pragma once
#include <ShObjIdl.h>
#include "../../Unspecified/Global.h"
#include "../Paths/Paths.h"
#include <codecvt>
#include <locale>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <atlbase.h>
#include <stack>
#include <atlalloc.h>
#include <propsys.h>

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


// Function to retrieve the path from a selected item in the ListView
std::wstring GetPathFromListView(HWND hwndListView, int index) {

  wchar_t Filename[256] = {};
  wchar_t FileDirectory[256] = {};

  ListView_GetItemText(hwndListView, index, 0, Filename, sizeof(Filename) / sizeof(Filename[0]));
  ListView_GetItemText(hwndListView, index, 2, FileDirectory, sizeof(FileDirectory) / sizeof(FileDirectory[0]));

  // Create file path wstring.
  std::wstring FullPath = FileDirectory;
  FullPath += Filename;

  return FullPath;
}

// Function to retrieve paths from all selected items in the ListView
std::vector<std::wstring> GetSelectedPaths(HWND hwndListView) {
  std::vector<std::wstring> paths;

  int index = -1;
  while ((index = ListView_GetNextItem(hwndListView, index, LVNI_SELECTED)) != -1) {
    paths.push_back(GetPathFromListView(hwndListView, index));
  }

  return paths;
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

  // Set redraw to false to save many instuctions.
  SendMessage(hwnd_list, WM_SETREDRAW, FALSE, 0);

  // Iterate through the vector and add each struct's information to the ListView
  LVITEM lvItem;
  memset(&lvItem, 0, sizeof(LVITEM));
  lvItem.mask = LVIF_TEXT;

  // Set the total number of items beforehand
  SendMessage(hwnd_list, LVM_SETITEMCOUNT, fileVector.size(), 0);
  for (size_t i = 0; i < fileVector.size(); ++i) {
    // Add the index to the first column
    lvItem.iItem = i;
    lvItem.iSubItem = 0;
    lvItem.pszText = LPSTR_TEXTCALLBACK;
    ListView_InsertItem(hwnd_list, &lvItem);

    std::wstring WideFileName = to_wstring(fileVector[i].filename);
    std::wstring file_size = std::to_wstring(fileVector[i].file_size);
    std::wstring file_index = std::to_wstring(i);
    std::wstring file_offset = std::to_wstring(fileVector[i].file_offset);
    std::wstring toc_offset = std::to_wstring(fileVector[i].toc_offset);
    std::wstring WideZtype = to_wstring(fileVector[i].ztype);

    // Add other columns
    ListView_SetItemText(hwnd_list, i, 0, const_cast<LPWSTR>(file_index.c_str()));
    ListView_SetItemText(hwnd_list, i, 1, const_cast<LPWSTR>(WideFileName.c_str()));
    ListView_SetItemText(hwnd_list, i, 2, const_cast<LPWSTR>(file_size.c_str()));
    ListView_SetItemText(hwnd_list, i, 3, const_cast<LPWSTR>(file_offset.c_str()));
    ListView_SetItemText(hwnd_list, i, 4, const_cast<LPWSTR>(WideZtype.c_str()));
    ListView_SetItemText(hwnd_list, i, 5, const_cast<LPWSTR>(toc_offset.c_str()));
  }

  // Set redraw to true after adding items.
  SendMessage(hwnd_list, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(hwnd_list, NULL, TRUE);
}

// Function to update the ListView1 with Archive information
void UpdateArchiveView(HWND hwnd, wchar_t* file)
{
  // Update the list view
  HWND hListView = GetDlgItem(hwnd, ID_LIST_VIEW);
  ListView_DeleteAllItems(hwndListView);
  ListView_DeleteAllItems(hwndListView2);

  // Set redraw to false to save many instuctions.
  SendMessage(hwndListView, WM_SETREDRAW, FALSE, 0);

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

    // Create file path wstring.
    std::wstring FullPath = selected_archives.first.c_str();
    FullPath += selected_archives.second[i].c_str();

    // Get information
    std::wstring archive_size = std::to_wstring(GetFileSizeFromOpenFileName(hwnd, selected_archives.first, selected_archives.second[i]));

    // Fill listview with information.
    switch (magic::magic_parser(FullPath.c_str()))
    {
    case BIG_EB:
      ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
      ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
      ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(selected_archives.first.c_str()));
      break;
    case BIG4:
      ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
      ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
      ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(selected_archives.first.c_str()));
      break;
    case ARENA:
      ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
      ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
      ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(selected_archives.first.c_str()));
      break;
    case SFIL:
    {
      SFArchiveHeaderNotDynamic parsed_sf_header = sf::parse_sfa_header(FullPath.c_str());
      std::wstring archive_streamformat = std::to_wstring(parsed_sf_header.StreamFormat);
      std::wstring archive_Version = std::to_wstring(parsed_sf_header.Version);
      std::wstring archive_NumCollections = std::to_wstring(parsed_sf_header.NumCollections);
      ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(selected_archives.second[i].c_str()));
      ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
      ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(selected_archives.first.c_str()));
      ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(archive_streamformat.c_str()));
      ListView_SetItemText(hwndListView, i, 4, const_cast<LPWSTR>(archive_Version.c_str()));
      ListView_SetItemText(hwndListView, i, 5, const_cast<LPWSTR>(archive_NumCollections.c_str()));
      break;
    }
    case UNKNOWNARCHIVE:
      continue;
      break;
    default:
      break;
    }
  }
  // Set redraw to true after adding items.
  SendMessage(hwndListView, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(hwndListView, NULL, TRUE);
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
    // Delete old listview items.
    ListView_DeleteAllItems(hwndListView);
    ListView_DeleteAllItems(hwndListView2);

    // Set redraw to false to save many instuctions.
    SendMessage(hwndListView, WM_SETREDRAW, FALSE, 0);

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

        // Check if the file extension is valid
        if (
          fileExtension != L"psf"
          && fileExtension != L"wsf"
          && fileExtension != L"xsf"
          && fileExtension != L"big"
          && fileExtension != L"viv"
          && fileExtension != L"rx2"
          && fileExtension != L"psg"
          && fileExtension != L"rg2"
          && fileExtension != L"rpsgl"
          )
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

      // Create file path wstring.
      std::wstring FullPath = directories[i].c_str();
      FullPath += filenames[i].c_str();

      // Get information
      std::wstring archive_size = std::to_wstring(GetFileSizeFromOpenFileName(hwnd, directories[i].c_str(), filenames[i].c_str()));

      switch (magic::magic_parser(FullPath.c_str()))
      {
      case BIG_EB:
        ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(filenames[i].c_str()));
        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(directories[i].c_str()));
        break;
      case BIG4:
        ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(filenames[i].c_str()));
        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(directories[i].c_str()));
        break;
      case ARENA:
        ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(filenames[i].c_str()));
        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(directories[i].c_str()));
        break;
      case SFIL:
      {
        SFArchiveHeaderNotDynamic parsed_sf_header = sf::parse_sfa_header(FullPath.c_str());
        std::wstring archive_streamformat = std::to_wstring(parsed_sf_header.StreamFormat);
        std::wstring archive_Version = std::to_wstring(parsed_sf_header.Version);
        std::wstring archive_NumCollections = std::to_wstring(parsed_sf_header.NumCollections);
        ListView_SetItemText(hwndListView, i, 0, const_cast<LPWSTR>(filenames[i].c_str()));
        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(archive_size.c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<LPWSTR>(directories[i].c_str()));
        ListView_SetItemText(hwndListView, i, 3, const_cast<LPWSTR>(archive_streamformat.c_str()));
        ListView_SetItemText(hwndListView, i, 4, const_cast<LPWSTR>(archive_Version.c_str()));
        ListView_SetItemText(hwndListView, i, 5, const_cast<LPWSTR>(archive_NumCollections.c_str()));
        break;
      }
      case UNKNOWNARCHIVE:
        continue;
        break;
      default:
        break;
      }
    }

    // Set redraw to true after adding items.
    SendMessage(hwndListView, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwndListView, NULL, TRUE);
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

// Function to open a file dialog and load the file into the buffer
void OpenFileAndLoadBuffer(HWND hwnd) {
  OPENFILENAME ofn = { sizeof ofn };
  wchar_t file[10240] = {};
  file[0] = '\0';
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = file;
  ofn.nMaxFile = 10240;
  ofn.lpstrFilter = TEXT("EA Skate Archives\0*.xsf;*.psf;*.wsf;*.big;*.viv;*.rx2;*.psg;*.rg2;*.rpsgl\0\0");
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

std::vector<std::wstring> EnumerateFolder(IShellFolder* folder) {

  std::vector<std::wstring> filepath_wstring_vector = {};
  std::stack<IShellFolder*> folderStack;

  folderStack.push(folder);

  while (!folderStack.empty()) {
    IShellFolder* currentFolder = folderStack.top();
    folderStack.pop();

    IEnumIDList* enumItems;
    HRESULT hr = currentFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumItems);
    if (hr != S_OK) {
      std::wcerr << L"Failed to get enumerator: " << hr << std::endl;
      return filepath_wstring_vector;
    }

    PITEMID_CHILD pItem;
    while (enumItems->Next(1, &pItem, NULL) == S_OK && pItem) {
      STRRET strRet;
      WCHAR szDisplayName[MAX_PATH];
      hr = currentFolder->GetDisplayNameOf(pItem, SIGDN_FILESYSPATH, &strRet);
      if (hr == S_OK) {
        StrRetToBuf(&strRet, pItem, szDisplayName, MAX_PATH);
        std::wcout << L"Item: " << szDisplayName << std::endl;
      }
      else {
        std::wcerr << L"Failed to get display name: " << hr << std::endl;
      }

      // Check if the item is a folder and add it to the stack for processing
      SFGAOF attributes = SFGAO_FOLDER;
      hr = currentFolder->GetAttributesOf(1, (PCUITEMID_CHILD*)&pItem, &attributes);
      if (hr == S_OK && (attributes & SFGAO_FOLDER)) {
        IShellFolder* pSubFolder;
        hr = currentFolder->BindToObject(pItem, NULL, IID_IShellFolder, (void**)&pSubFolder);
        if (hr == S_OK) {
          folderStack.push(pSubFolder);
        }
      }
      else
      {
        if (szDisplayName != nullptr)
        {
          std::wstring filepath_wstring = szDisplayName;
          filepath_wstring_vector.push_back(filepath_wstring);
        }
      }

      CoTaskMemFree(pItem);
    }

    enumItems->Release();
  }

  return filepath_wstring_vector;
}

auto OpenFolder(HWND hwnd) {

  struct RESULT { std::vector<std::wstring> all_files_in_subfolders; std::wstring selected_path; };

  std::vector<std::wstring> filepath_wstring_vector = {};
  std::wstring selected_path = L"";
  IFileDialog* pfd;
  HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
  if (hr != S_OK) {
    std::wcerr << L"Failed to create FileOpenDialog: " << hr << std::endl;
    return RESULT{ filepath_wstring_vector, selected_path };
  }

  DWORD dwOptions;
  hr = pfd->GetOptions(&dwOptions);
  if (hr != S_OK) {
    std::wcerr << L"Failed to get options: " << hr << std::endl;
    pfd->Release();
    return RESULT{ filepath_wstring_vector, selected_path };
  }

  hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
  if (hr != S_OK) {
    std::wcerr << L"Failed to set options: " << hr << std::endl;
    pfd->Release();
    return RESULT{ filepath_wstring_vector, selected_path };
  }

  // Show the dialog
  if (pfd->Show(hwnd) == S_OK) {
    IShellItem* psi;
    if (pfd->GetResult(&psi) == S_OK) {
      IShellFolder* pFolder;
      WCHAR szPath[MAX_PATH];
      hr = psi->BindToHandler(NULL, BHID_SFObject, IID_IShellFolder, (void**)&pFolder);
      if (hr == S_OK) {
        PWSTR pszPath;
        hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
        if (SUCCEEDED(hr)) {
          // Free the allocated memory
          filepath_wstring_vector = EnumerateFolder(pFolder);
          selected_path = pszPath;
          CoTaskMemFree(pszPath);
        }

        pFolder->Release();
      }
      else {
        std::wcerr << L"Failed to get shell folder: " << hr << std::endl;
      }
      psi->Release();
    }
    else {
      std::wcerr << L"Failed to get result from FileOpenDialog." << std::endl;
    }
  }
  else {
    std::wcerr << L"User canceled or an error occurred during FileOpenDialog." << std::endl;
  }

  pfd->Release();

  return RESULT{ filepath_wstring_vector, selected_path };
}


// Function to update the ListView1 with Archive information
void UpdateBig4PackerFilepathView(HWND hwnd, std::vector<std::wstring> arg_filepath_vector, std::wstring selected_path)
{

  ListView_DeleteAllItems(hwnd);

  // Set redraw to false to save many instuctions.
  SendMessage(hwnd, WM_SETREDRAW, FALSE, 0);

  // Iterate through the vector and add each struct's information to the ListView
  LVITEM lvItem;
  memset(&lvItem, 0, sizeof(LVITEM));
  lvItem.mask = LVIF_TEXT;

  std::wstring top_level_path = removeLastFolder(selected_path);

  for (size_t i = 0; i < arg_filepath_vector.size(); ++i)
  {

    std::wstring TOC_Compatible_Path = arg_filepath_vector[i];
    size_t pos = TOC_Compatible_Path.find(top_level_path);

    if (pos != std::wstring::npos) {
      TOC_Compatible_Path = make_big_compatible_paths(pos, top_level_path, TOC_Compatible_Path);
      // Add the index to the first column
      lvItem.iItem = i;
      lvItem.iSubItem = 0;
      lvItem.pszText = LPSTR_TEXTCALLBACK;
      ListView_InsertItem(hwnd, &lvItem);
      ListView_SetItemText(hwnd, i, 0, const_cast<LPWSTR>(TOC_Compatible_Path.c_str()));
    }
    else {
      std::wcout << L"Substring not found." << std::endl;
    }
  }
  // Set redraw to true after adding items.
  SendMessage(hwnd, WM_SETREDRAW, TRUE, 0);
  InvalidateRect(hwnd, NULL, TRUE);
}