/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\StreamType\AudioStream.h"

namespace onesnd
{
    /**
    * AudioStream for streaming file in WAV format.
    * The stream is decoded into PCM format.
    */
    class OGGStream : public AudioStream
    {
    public:
        /**
        * Creates a new unitialized OGG AudioStreamer.
        * You should call OpenStream(file) to initialize the stream.
        */
        OGGStream();

        /**
        * Creates and Initializes a new OGG AudioStreamer.
        */
        OGGStream(const fs::path& file);

        /**
        * Destroys the AudioStream and frees all held resources
        */
        virtual ~OGGStream();


        /**
        * Opens a new stream for reading.
        * @param file Audio file to open
        * @return TRUE if stream is successfully opened and initialized. FALSE if the stream open failed or its already open.
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
    };
}