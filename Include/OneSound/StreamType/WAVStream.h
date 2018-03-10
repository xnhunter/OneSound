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
    class ONE_SOUND_API WAVStream : public AudioStream
    {
    public:
        /**
        * Creates a new unitialized WAV AudioStreamer.
        * You should call OpenStream(file) to initialize the stream.
        */
        inline WAVStream() : AudioStream()
        { }

        /**
        * Creates and Initializes a new WAV AudioStreamer.
        */
        inline WAVStream(const fs::path& file) : AudioStream(file)
        { }
    };
}