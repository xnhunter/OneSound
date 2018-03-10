/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\SoundType\SoundBuffer.h"
#include "OneSound\SoundType\SoundStream.h"

#include "OneSound\SoundType\Sound2D.h"

namespace onesnd
{
    class ONE_SOUND_API OneSound
    {
    public:
        OneSound(const bool& has_initialize = true); // Param has_initialize gives us the
                                                     // opportunity to initialize OneSound then.
       ~OneSound();

        OneSound(const OneSound&) = delete;
        OneSound(OneSound&&) = delete;

        OneSound& operator=(const OneSound&) = delete;
        OneSound& operator=(OneSound&&) = delete;

    public:
        void initialize() const;

        XAUDIO2_PERFORMANCE_DATA getPerfomanceData() const;

        void finalize() const;

    public:
        unsigned long long getLibraryVersion() const;
        std::string getLibraryVersionStr() const;

        std::string getLibraryStatus() const;
        std::string getLibraryName() const;

    };
}