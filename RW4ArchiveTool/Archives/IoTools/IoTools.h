

namespace IoTools {

    bool create_file(FILE* file, std::wstring out_path)
    {
        // Write to file.
        if (_wfopen_s(&file, out_path.c_str(), L"wb+") == 0)
        {
            if (file != nullptr) {
                fclose(file);
            }
            else {
                OutputDebugStringA("Error opening file for append.\n"); return false;
            }
        }
        else {
            OutputDebugStringA("Error opening file.\n"); return false;
        }

        return true;
    }

    bool write_file(FILE* file, std::wstring out_path, char* buffer, uint64_t size)
    {
        // Write to file.
        if (_wfopen_s(&file, out_path.c_str(), L"wb+") == 0)
        {
            if (file != nullptr) {
                size_t bytesWritten = fwrite(buffer, sizeof(char), size, file);
                if (bytesWritten != size) {
                    OutputDebugStringA("Error writing to file.\n"); return false;
                }
                fclose(file);
            }
            else {
                OutputDebugStringA("Error opening file for append.\n"); return false;
            }
        }
        else {
            OutputDebugStringA("Error opening file.\n"); return false;
        }

        return true;
    }

    bool append_file(FILE* file, std::wstring out_path, char* buffer, uint64_t size)
    {
        // Write to file.
        if (_wfopen_s(&file, out_path.c_str(), L"ab+") == 0)
        {
            if (file != nullptr) {
                size_t bytesWritten = fwrite(buffer, sizeof(char), size, file);
                if (bytesWritten != size) {
                    OutputDebugStringA("Error writing to file.\n"); return false;
                }
                fclose(file);
            }
            else {
                OutputDebugStringA("Error opening file for append.\n"); return false;
            }
        }
        else {
            OutputDebugStringA("Error opening file.\n"); return false;
        }

        return true;
    }




};