/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\StreamType\AudioStream.h"

#include "OneSound\StreamType\WAVStream.h"
#include "OneSound\StreamType\MP3Stream.h"
#include "OneSound\StreamType\OGGStream.h"

namespace onesnd
{
    enum AudioFileFormat
    {
        INVALID, WAV, MP3, OGG,
    };

    static AudioFileFormat getAudioFileFormatByExtension(const fs::path& file) // pretty cheap actually - we just check the file extension!
    {
        if (!fs::exists(file))
            throw std::runtime_error("Can't find file: "s + file.string());

        if(!file.has_extension())
            return AudioFileFormat::INVALID;

        if(file.extension() == ".wav")
            return AudioFileFormat::WAV;
        else if (file.extension() == ".mp3")
            return AudioFileFormat::MP3;
        else if (file.extension() == ".ogg")
            return AudioFileFormat::OGG;
        else
        {
            printf("ONESOUND: WARNING: Unsupported file format.");
            return AudioFileFormat::INVALID;;
        }
    }

    static bool checkMP3Tag(void* buffer)
    {
    #pragma pack(push)
    #pragma pack(1)
        struct MP3TAGV2
        {
            char id3[3];
            unsigned char vermajor;
            unsigned char verminor;
            unsigned char flags;
            unsigned int size;
        };
    #pragma pack(pop)

        auto tag = *(MP3TAGV2*)buffer;
        if (tag.id3[0] == 'I' && tag.id3[1] == 'D' && tag.id3[2] == '3')
            return true;

        return false; // not an mp3 header
    }

    static AudioFileFormat getAudioFileFormatByHeader(const char* file_name) // a bit heavier - we actually check the file header
    {
        FILE** f = nullptr;
        fopen_s(f, file_name, "rb");
        if (!f) // open file 'read-binary')
            throw std::runtime_error("Can't open file: "s + file_name);

        // MP3 has a header tag, needs 10 bytes
        // WAV has a large header with byte fields [file + 0]='RIFF' and [file + 8]='WAVE', needs 12bytes
        // OGG has a 32-bit "capture pattern" sync field 'OggS', needs 4 bytes
        int buffer[3]; // WAV requires most, so 12 bytes
        fread(buffer, sizeof(buffer), 1, *f);
        fclose(*f); 
        f = nullptr;

        if (buffer[0] == 'FFIR' && buffer[2] == 'EVAW')
            return AudioFileFormat::WAV;
        else if (buffer[0] == 'SggO')
            return AudioFileFormat::OGG;
        else if (checkMP3Tag(buffer))
            return AudioFileFormat::MP3;

        return AudioFileFormat::INVALID;
    }

    AudioStream* createAudioStream(const char* file)
    {
        auto fmt = getAudioFileFormatByExtension(file);
        if (fmt == AudioFileFormat::INVALID) 
            fmt = getAudioFileFormatByHeader(file);

        if (fmt == AudioFileFormat::INVALID)
            throw std::runtime_error("Invalid file header.");

        switch (fmt)
        {
            case AudioFileFormat::WAV: return new WAVStream();
            case AudioFileFormat::MP3: return new MP3Stream();
            case AudioFileFormat::OGG: return new OGGStream();

            default:
                return nullptr;
        }
    }

    bool createAudioStream(AudioStream* as, const char* file)
    {
        if (!as) 
            return false;
        as->CloseStream(); // just in case...

        auto fmt = getAudioFileFormatByExtension(file);
        if (!fmt) 
            fmt = getAudioFileFormatByHeader(file);
        if (!fmt) 
            return false;

        switch (fmt)
        {
            case AudioFileFormat::WAV: new (as) WAVStream(); break;
            case AudioFileFormat::MP3: new (as) MP3Stream(); break;
            case AudioFileFormat::OGG: new (as) OGGStream(); break;
            default: 
                return false;
        }

        return true;
    }

    struct RIFFCHUNK
    {
        union
        {
            int ID;			// chunk ID
            char IDStr[4];	// chunk ID as string
        };

        int Size;			// chunk SIZE
    };

    struct WAVHEADER
    {
        RIFFCHUNK Header;	// Contains the letters "RIFF" && (SizeOfFile - 8) in bytes
        union
        {
            int Format;				// Contains the letters "WAVE"
            char FormatAsStr[4];
        };

        // The FMT sub-chunk
        RIFFCHUNK Subchunk1;		// Contains the letters "fmt " && 16 bytes in size

        struct
        {
            short AudioFormat;		// Should be 1, otherwise this file is compressed!
            short NumChannels;		// Mono = 1, Stereo = 2
            int SampleRate;			// 8000, 22050, 44100, etc
            int ByteRate;			// == SampleRate * NumChannels * BitsPerSample/8
            short BlockAlign;		// == NumChannels * BitsPerSample/8
            short BitsPerSample;	// 8 bits == 8, 16 bits == 16, etc.
        };

        RIFFCHUNK NextChunk1;		// Contains the letters "info" or "data"
        RIFFCHUNK NextChunk2;		// maybe this one is data?
        int someData[4];

        RIFFCHUNK* getDataChunk()
        {
            if (NextChunk1.ID == (int)'atad') 
                return &NextChunk1;
            if (NextChunk2.ID == (int)'atad') 
                return &NextChunk2;

            return nullptr;
        }
    };


    AudioStream::AudioStream() : 
        FileHandle(nullptr),
        stream_size(0),
        stream_position(0),
        sample_rate(0),
        NumChannels(0),
        SampleSize(0),
        SampleBlockSize(0)
    { }

    AudioStream::AudioStream(const fs::path& file) : 
        FileHandle(nullptr),
        stream_size(0),
        stream_position(0),
        sample_rate(0),
        NumChannels(0), 
        SampleSize(0), 
        SampleBlockSize(0)
    {
        OpenStream(file);
    }

    AudioStream::~AudioStream()
    {
        CloseStream();
    }

    bool AudioStream::OpenStream(const fs::path& file_name)
    {
        if (FileHandle) // dont allow reopen an existing stream
            return false;

        FileHandle = reinterpret_cast<decltype(FileHandle)>(file_open_ro(file_name.string().c_str()));
        if (!FileHandle)
            throw std::runtime_error("Can't open file: "s + file_name.string());

        WAVHEADER wav;
        if (file_read(FileHandle, &wav, sizeof(wav)) <= 0)
            throw std::runtime_error("Failed to read WAV header from file: "s + file_name.string());

        // != "RIFF" || != "WAVE"
        if (wav.Header.ID != (int)'FFIR' || wav.Format != (int)'EVAW')
            throw std::runtime_error("Invalid WAV header in file: "s + file_name.string());

        auto* dataChunk = wav.getDataChunk();
        if (!dataChunk)
            throw std::runtime_error("Failed to find WAV <data> chunk in file: "s + file_name.string());

        // initialize essential variables
        stream_size = dataChunk->Size;
        sample_rate = static_cast<decltype(sample_rate)>(wav.SampleRate);
        NumChannels = static_cast<decltype(NumChannels)>(wav.NumChannels);
        SampleSize = static_cast<decltype(SampleSize)>(wav.BitsPerSample >> 3);	// BPS/8 => SampleSize
        SampleBlockSize = SampleSize * NumChannels;	 // [LL][RR] (1 to 4 bytes)

        return true; // everything went ok
    }

    void AudioStream::CloseStream()
    {
        if (FileHandle)
        {
            file_close(reinterpret_cast<void*>(FileHandle));

            FileHandle = nullptr;
            stream_size = 0;
            stream_position = 0;
            sample_rate = 0;
            NumChannels = 0;
            SampleSize = 0;
            SampleBlockSize = 0;
        }
    }

    int AudioStream::ReadSome(void* dstBuffer, int dstSize)
    {
        if (!FileHandle)
            return 0; // nothing to do here

        auto count = stream_size - stream_position; // calc available data from stream
        if (count == 0) // if are stream available bytes 0
            return 0; // EOS is reached

        if (count > dstSize) // if stream has more data than buffer
            count = dstSize; // set bytes to read bigger
        count -= count % SampleBlockSize; // make sure count is aligned to blockSize

        if (file_read(FileHandle, dstBuffer, count) <= 0)
        {
            stream_position = stream_size; // set EOS
            return 0; // no bytes read
        }
        stream_position += count;

        return count;

    }

    unsigned int AudioStream::Seek(unsigned int streampos)
    {
        if (int(streampos) >= stream_size)
            streampos = 0;

        streampos -= streampos % SampleBlockSize; // align to PCM blocksize
        file_seek(FileHandle, streampos + sizeof(WAVHEADER), SEEK_SET);
        stream_position = streampos;

        return streampos;
    }
}