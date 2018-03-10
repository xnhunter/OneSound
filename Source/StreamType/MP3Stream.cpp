// Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.


#include "OneSound\StreamType\MP3Stream.h"

namespace onesnd
{
#pragma data_seg("SHARED")
    static HMODULE mpgDll = nullptr;
    static void(*mpg_exit)();
    static int(*mpg_init)();
    static int* (*mpg_new)(const char* decoder, int* error);
    static int(*mpg_close)(int* mh);
    static void(*mpg_delete)(int* mh);
    static int(*mpg_open_handle)(int* mh, void* iohandle);
    static int(*mpg_getformat)(int* mh, long* rate, int* channels, int* encoding);
    static size_t(*mpg_length)(int* mh);
    static size_t(*mpg_outblock)(int* mh);
    static int(*mpg_encsize)(int encoding);
    static int(*mpg_read)(int* mh, unsigned char* outmemory, size_t outmemsize, size_t* done);
    static const char* (*mpg_strerror)(int* mh);
    static int(*mpg_errcode)(int* mh);
    static const char** (*mpg_supported_decoders)();
    static size_t(*mpg_seek)(int* mh, size_t sampleOffset, int whence);
    static const char* (*mpg_current_decoder)(int* mh);

    typedef int(*mpg_read_func)(void*, void*, size_t);
    typedef off_t(*mpg_seek_func)(void*, off_t, int);
    typedef int(*mpg_close_func)(void*);
    static int(*mpg_replace_reader_handle)(int* mh, mpg_read_func, mpg_seek_func, mpg_close_func);
#pragma data_seg()

    template<class Proc> static inline void LoadMpgProc(Proc& outProcVar, const char* procName)
    {
        outProcVar = (Proc)GetProcAddress(mpgDll, procName);
    }
    static void _UninitMPG()
    {
        mpg_exit();

        FreeLibrary(mpgDll);
        mpgDll = nullptr;
    }
    static void _InitMPG()
    {
        static const char* mpglib = "libmpg123";
        mpgDll = LoadLibraryA(mpglib);
        if (!mpgDll)
            throw std::runtime_error("Can't found "s + mpglib + ".dll"s);

        LoadMpgProc(mpg_exit, "mpg123_exit");
        LoadMpgProc(mpg_init, "mpg123_init");
        LoadMpgProc(mpg_new, "mpg123_new");
        LoadMpgProc(mpg_close, "mpg123_close");
        LoadMpgProc(mpg_delete, "mpg123_delete");
        LoadMpgProc(mpg_open_handle, "mpg123_open_handle");
        LoadMpgProc(mpg_getformat, "mpg123_getformat");
        LoadMpgProc(mpg_length, "mpg123_length");
        LoadMpgProc(mpg_outblock, "mpg123_outblock");
        LoadMpgProc(mpg_encsize, "mpg123_encsize");
        LoadMpgProc(mpg_read, "mpg123_read");
        LoadMpgProc(mpg_strerror, "mpg123_strerror");
        LoadMpgProc(mpg_errcode, "mpg123_errcode");
        LoadMpgProc(mpg_supported_decoders, "mpg123_supported_decoders");
        LoadMpgProc(mpg_seek, "mpg123_seek");
        LoadMpgProc(mpg_current_decoder, "mpg123_current_decoder");
        LoadMpgProc(mpg_replace_reader_handle, "mpg123_replace_reader_handle");

        mpg_init();

        atexit(_UninitMPG);
    }

    MP3Stream::MP3Stream() : AudioStream()
    {
        if (!mpgDll) 
            _InitMPG();
    }

    MP3Stream::MP3Stream(const fs::path& file) : AudioStream()
    {
        if (!mpgDll) 
            _InitMPG();

        OpenStream(file);
    }

    MP3Stream::~MP3Stream()
    {
        CloseStream();
    }

    bool MP3Stream::OpenStream(const fs::path& file_name)
    {
        if (!mpgDll || FileHandle) 
            return 0;

        FileHandle = mpg_new(nullptr, nullptr);
        mpg_replace_reader_handle(FileHandle, file_read, file_seek, file_close);

        auto* iohandle = file_open_ro(file_name.string().c_str());
        if (!iohandle)
            throw std::runtime_error("Can't open file: "s + file_name.string());

        mpg_open_handle(FileHandle, iohandle);

        auto rate = 0;
        auto numChannels = decltype(rate)();
        auto encoding = decltype(numChannels)();

        if (mpg_getformat(FileHandle, reinterpret_cast<long*>(&rate), &numChannels, &encoding))
            throw std::runtime_error("Failed to read MP3 header from file: "s + file_name.string());

        auto sampleSize = mpg_encsize(encoding);

        // get the actual PCM data size: (NumSamples * NumChannels * SampleSize)
        SampleSize = sampleSize;
        SampleBlockSize = numChannels * sampleSize;
        stream_size = mpg_length(FileHandle) * SampleBlockSize;
        sample_rate = rate;
        NumChannels = numChannels;

        return true;
    }

    void MP3Stream::CloseStream()
    {
        if (!mpgDll) 
            return; // mpg123 not present

        if (FileHandle)
        {
            mpg_close(FileHandle);
            mpg_delete(FileHandle);

            FileHandle = 0;
            stream_size = 0;
            stream_position = 0;
            sample_rate = 0;
            NumChannels = 0;
            SampleSize = 0;
            SampleBlockSize = 0;
        }
    }

    int MP3Stream::ReadSome(void* dstBuffer, int dstSize)
    {
        if (!mpgDll || !FileHandle)
            return 0; // mpg123 not present

        auto count = stream_size - stream_position; // calc available data from stream
        if (count == 0) // if stream available bytes 0?
            return 0; // EOS reached

        if (count > dstSize) // if stream has more data than buffer
            count = dstSize; // set bytes to read bigger
        count -= count % SampleBlockSize; // make sure count is aligned to blockSize

        auto bytesRead = size_t();
        mpg_read(FileHandle, static_cast<unsigned char*>(dstBuffer), count, &bytesRead);

        stream_position += bytesRead;

        return bytesRead;
    }

    unsigned int MP3Stream::Seek(unsigned int streampos)
    {
        if (!mpgDll) 
            return 0;

        if (int(streampos) >= stream_size)
            streampos = 0;
        int actual = streampos / SampleBlockSize; // mpg_seek works by sample blocks, so lets select the sample

        mpg_seek(FileHandle, actual, SEEK_SET);

        return streampos;
    }
}