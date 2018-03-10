/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include <OneSound\Utility.h>

namespace onesnd
{
    /**
    * Basic AudioStreamer class for streaming audio data.
    * Data is decoded and presented in simple wave PCM format.
    *
    * The base implementation is actually the WAV streamer - other implementations
    * extend the virtual methods.
    */
    class ONE_SOUND_API AudioStream
    {
    protected:
        int* FileHandle;				// internally interpreted file handle
        int stream_size;					// size of the audiostream in PCM bytes, not File bytes
        int stream_position;					// current stream position in PCM bytes
        unsigned int sample_rate;		// frequency (or rate) of the sound data, usually 20500 or 41000 (20.5kHz / 41kHz)
        unsigned char NumChannels;		// number of channels in a sample block, usually 1 or 2 (Mono / Stereo)
        unsigned char SampleSize;		// size (in bytes) of a sample, usually 1 to 2 bytes (8bit:1 / 16bit:2)
        unsigned char SampleBlockSize;	// size (in bytes) of a sample block: SampleSize * NumChannels
    public:
        /**
        * Creates a new uninitialized AudioStreamer.
        * You should call OpenStream(file) to initialize the stream.
        */
        AudioStream();

        /**
        * Creates and Opens a new AudioStreamer.
        * @param file Full path to the audiofile to stream
        */
        AudioStream(const fs::path& file);

        /**
        * Destroys the AudioStream and frees all held resources
        */
        virtual ~AudioStream();




        /**
        * Opens a new stream for reading.
        * @param file Audio file to open
        * @return TRUE if stream is successfully opened and initialized. FALSE if the stream open failed or it's already opened.
        */
        virtual bool OpenStream(const fs::path& file);

        /**
        * Closes the stream and releases all resources held.
        */
        virtual void CloseStream();

        /**
        * Reads some Audio data from the underlying stream.
        * Audio data is decoded into PCM format, suitable for OpenAL.
        * @param dstBuffer Destination buffer that receives the data
        * @param dstSize Number of bytes to read. 64KB is good for streaming (gives ~1.5s of playback sound).
        * @return Number of bytes read. 0 if stream is uninitialized or end of stream reached.
        */
        virtual int ReadSome(void* dstBuffer, int dstSize);

        /**
        * Seeks to the appropriate byte position in the stream.
        * This value is between: [0...StreamSize]
        * @param streampos Position in the stream to seek to in BYTES
        * @return The actual position where seeked, or 0 if out of bounds (this also means the stream was reset to 0).
        */
        virtual unsigned int Seek(unsigned int streampos);

        /**
        * @return TRUE if the Stream has been opened. FALSE if it remains unopened.
        */
        inline bool IsOpen() const
        {
            return FileHandle ? true : false;
        }

        /**
        * Resets the stream position to the beginning.
        */
        inline void ResetStream()
        {
            Seek(0);
        }

        /**
        * @return Size of the stream in PCM bytes, not File bytes
        */
        inline int Size() const
        {
            return stream_size;
        }

        /**
        * @return Position of the stream in PCM bytes
        */
        inline int Position() const
        {
            return stream_position;
        }

        /**
        * @return Number of PCM bytes still available in the stream
        */
        inline int Available() const
        {
            return stream_size - stream_position;
        }

        /**
        * @return TRUE if End Of Stream was reached
        */
        inline bool IsEOS() const
        {
            return stream_position == stream_size;
        }

        /**
        * @return Playback frequency specified by the streamer. Usually 20500 or 41000 (20.5kHz / 41kHz)
        */
        inline int Frequency() const
        {
            return int(sample_rate);
        }

        /**
        * @return Number of channels in this AudioStream. Usually 1 or 2 (Mono / Stereo)
        */
        inline int Channels() const
        {
            return int(NumChannels);
        }

        /**
        * @return Size (in bytes) of a single channel sample. Usually 1 to 2 bytes (8bit:1 / 16bit:2)
        */
        inline int SingleSampleSize() const
        {
            return int(SampleSize);
        }

        /**
        * @return Size (in bytes) of a full all channels sample. Usually 1 to 4 bytes ( Channels * SingleSampleSize : [LL][RR] )
        */
        inline int FullSampleBlockSize() const
        {
            return int(SampleBlockSize);
        }

        /**
        * @return Number of Bytes Per Second for the audio data in this stream
        */
        inline int BytesPerSecond() const
        {
            return int(sample_rate) * int(SampleBlockSize);
        }
    };

    /**
    * Automatically creates a specific AudioStreamer
    * instance depending on the specified file extension
    * or by the file header format if extension not specified.
    * @note The Stream is not Opened! You must do it manually.
    * @param file Audio file string
    * @return New dynamic instance of a specific AudioStreamer. Or NULL if the file format cannot be detected.
    */
    AudioStream* createAudioStream(const char* file);

    /**
    * Automatically creates a specific AudioStreamer
    * instance depending on the specified file extension
    * or by the file header format if extension not specified.
    *
    * This initializes an already existing AudioStream instance.
    *
    * @note The Stream is not Opened! You must do it manually.
    * @param as AudioStream instance to create the streamer into. Can be any other AudioStream instance.
    * @param file Audio file string
    * @return TRUE if the instance was created.
    */
    bool createAudioStream(AudioStream* as, const char* file);
}