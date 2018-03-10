/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\StreamType\OGGStream.h"

#include "..\ThirdParty\Include\Vorbis\vorbisfile.h"


namespace onesnd
{
#pragma data_seg("SHARED")
    static HMODULE vfDll = nullptr;
    static int(*oggv_clear)(void* vf) = 0;
    static long(*oggv_read)(void* vf, char* buffer, int length, int bigendiannp, int word, int sgned, int* bitstream) = 0;
    static long(*oggv_pcm_seek)(void* vf, INT64 pos) = 0;
    static UINT64(*oggv_pcm_tell)(void* vf) = 0;
    static UINT64(*oggv_pcm_total)(void* vf, int i) = 0;
    static vorbis_info* (*oggv_info)(void* vf, int link) = 0;
    static vorbis_comment* (*oggv_comment)(void* vf, int link) = 0;
    static int(*oggv_open_callbacks)(void* datasource, int* vf, char* initial, long ibytes, ov_callbacks cb) = 0;
#pragma data_seg()

    static size_t oggv_read_func(void* ptr, size_t size, size_t nmemb, void* handle)
    {
        return file_read(handle, ptr, size * nmemb);
    }
    static int oggv_seek_func(void* handle, INT64 offset, int whence)
    {
        return file_seek(handle, (long)offset, whence);
    }
    static int oggv_close_func(void* handle)
    {
        return file_close(handle);
    }
    static long oggv_tell_func(void* handle)
    {
        return file_tell(handle);
    }

    template<class Proc> static inline void LoadVorbisProc(Proc* outProcVar, const char* procName)
    {
        *outProcVar = (Proc)GetProcAddress(vfDll, procName);
    }
    static void finalizeOGGVorbis()
    {
        FreeLibrary(vfDll);
        vfDll = nullptr;
    }
    static void initializeOGGVorbis()
    {
        static const char* vorbislib = "vorbisfile";

        // NOTE: ogg.dll and vorbis.dll is loaded by vorbisfile.dll
        vfDll = LoadLibraryA(vorbislib);
        if (!vfDll)
            throw std::runtime_error("Can't found "s + vorbislib + ".dll"s);

        LoadVorbisProc(&oggv_clear, "ov_clear");
        LoadVorbisProc(&oggv_read, "ov_read");
        LoadVorbisProc(&oggv_pcm_seek, "ov_pcm_seek");
        LoadVorbisProc(&oggv_pcm_tell, "ov_pcm_tell");
        LoadVorbisProc(&oggv_pcm_total, "ov_pcm_total");
        LoadVorbisProc(&oggv_info, "ov_info");
        LoadVorbisProc(&oggv_comment, "ov_comment");
        LoadVorbisProc(&oggv_open_callbacks, "ov_open_callbacks");

        atexit(finalizeOGGVorbis);
    }

    OGGStream::OGGStream() : AudioStream()
    {
        if (!vfDll) 
            initializeOGGVorbis();
    }

    OGGStream::OGGStream(const fs::path& file) : AudioStream()
    {
        if (!vfDll) 
            initializeOGGVorbis();

        OpenStream(file);
    }

    OGGStream::~OGGStream()
    {
        CloseStream();
    }

    bool OGGStream::OpenStream(const fs::path& file_name)
    {
        if (!vfDll || FileHandle) 
            return false;

        auto* iohandle = file_open_ro(file_name.string().c_str());
        if (!iohandle)
            throw std::runtime_error("Can't open file: "s + file_name.string());

        ov_callbacks cb = {oggv_read_func, oggv_seek_func, oggv_close_func, oggv_tell_func};
        FileHandle = reinterpret_cast<decltype(FileHandle)>(malloc(sizeof(OggVorbis_File))); // filehandle is actually Vorbis handle

        auto err = oggv_open_callbacks(iohandle, FileHandle, nullptr, 0, cb);
        if (err)
        {
            const char* errmsg = nullptr;
            switch (err)
            {
                case OV_EREAD:		errmsg = "Error reading OGG file!";			break;
                case OV_ENOTVORBIS: errmsg = "Not an OGG vorbis file!";			break;
                case OV_EVERSION:	errmsg = "Vorbis version mismatch!";		break;
                case OV_EBADHEADER: errmsg = "Invalid vorbis bitstream header"; break;
                case OV_EFAULT:		errmsg = "Internal logic fault";			break;
            }

            CloseStream();
            throw std::runtime_error("Failed to open OGG file: "s + file_name.string());
        }
        auto* info = oggv_info(FileHandle, -1);
        if (!info)
        {
            CloseStream();
            throw std::runtime_error("Failed to acquire OGG stream format from file : "s + file_name.string());
        }

        sample_rate = static_cast<decltype(sample_rate)>(info->rate);

        // FIXME: Some breaks are occured beatween every stream's block.
        if (sample_rate > 22050)
        //  Let's warn the user.
            printf("ONESOUND: WARNING: The stream has some troubles when do its.\n\
          Unknown breaks are occured beatween every stream's block\n\
          if sample rate is more than 22050. Please convert your\n\
          OGG File to the sund with 22050 sample rate until the\n\
          author not found a solution.\n");

        NumChannels = info->channels;
        SampleSize = 2;						// OGG samples are always 16-bit
        SampleBlockSize = 2 * NumChannels;	// OGG samples are always 16-bit
        stream_size = static_cast<decltype(stream_size)>(oggv_pcm_total(FileHandle, -1) * SampleBlockSize); // streamsize in total bytes

        return true;
    }

    void OGGStream::CloseStream()
    {
        if (!vfDll) 
            return; // vorbis not present

        if (FileHandle)
        {
            oggv_clear(FileHandle);
            free(FileHandle);

            FileHandle = 0;
            stream_size = 0;
            stream_position = 0;
            sample_rate = 0;
            NumChannels = 0;
            SampleSize = 0;
            SampleBlockSize = 0;
        }
    }

    int OGGStream::ReadSome(void* dstBuffer, int dstSize)
    {
        if (!vfDll || !FileHandle)
            return 0;

        auto count = stream_size - stream_position; // calc available data from stream
        if (count == 0) // if stream available bytes 0?
            return 0; // EOS reached

        if (count > dstSize) // if stream has more data than buffer
            count = dstSize; // set bytes to read bigger
        count -= count % SampleBlockSize; // make sure count is aligned to blockSize

        auto current_section = int();
        auto bytesTotal = int(); // total bytes read
        do
        {
            // FIXME: Error when streaming one handle many times.
            //        File handles are not shared, so creating two handles of the same stream can be chashed.
            //        Find a way to share the stream or the data.
            int bytesRead = oggv_read(FileHandle, (char*)dstBuffer + bytesTotal,
                (count - bytesTotal), 0, 2, 1, &current_section);

            if (bytesRead == 0)
                break; // EOF!

            bytesTotal += bytesRead;
        } while (bytesTotal < count);

        stream_position += bytesTotal;
        return bytesTotal;
    }

    unsigned int OGGStream::Seek(unsigned int streampos)
    {
        if (!vfDll) 
            return 0; // vorbis not present

        if (static_cast<int>(streampos) >= stream_size) 
            streampos = 0; // out of bounds, set to beginning
        oggv_pcm_seek(FileHandle, streampos / SampleBlockSize); // seek PCM samples

        return stream_position = streampos; // finally, update the stream position
    }
}