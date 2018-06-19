/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#pragma warning (disable : 4251, 4172)

#include <Windows.h>

#include <vector>
#include <memory>
#include <filesystem>

namespace onesnd
{
    //// Low Latency File IO straight to Windows API, with no beating around the bush

    // opens a file with read-only rights
    inline void* file_open_ro(const char* filename)
    {
        void* fh = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fh == INVALID_HANDLE_VALUE) return NULL;
        return fh;
    }
    // close the file
    inline int file_close(void* handle)
    {
        CloseHandle(handle);
        return 0;
    }
    // reads bytes from an opened file
    inline int file_read(void* handle, void* dst, size_t size)
    {
        DWORD bytesRead;
        ReadFile(handle, dst, size, &bytesRead, NULL);
        return (int)bytesRead;
    }
    // seeks the file pointer
    inline off_t file_seek(void* handle, off_t offset, int whence)
    {
        return SetFilePointer(handle, offset, NULL, whence);
    }
    // tells the current file position
    inline off_t file_tell(void* handle)
    {
        return SetFilePointer(handle, 0, NULL, FILE_CURRENT);
    }

///------
    using namespace std::string_literals;
    namespace fs = std::experimental::filesystem;

    template<typename Process>
    inline void loadProcess(Process* var, const char* process_name, HMODULE dll_handle)
    {
        *var = reinterpret_cast<Process>(GetProcAddress(dll_handle, process_name));
    }
}
