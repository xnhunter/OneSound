/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

namespace onesnd
{
    // REDO: Listener class.
    class Listener
    {
    public:
        Listener();
       ~Listener();

        Listener(const Listener&) = default;
        Listener(Listener&&) = default;

        Listener operator=(const Listener&) = delete;
        Listener operator=(Listener&&) = delete;

    public:
        void setVolume(float volume);
        float getVolume() const;
    };
}
