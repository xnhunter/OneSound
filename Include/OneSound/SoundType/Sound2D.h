/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\SoundType\SoundObject.h"

namespace onesnd
{
    class ONE_SOUND_API Sound2D : public SoundObject
    {
    public:
        Sound2D();
       ~Sound2D() = default;

        Sound2D(const std::shared_ptr<SoundBuffer>& sound, const bool& loop = false, const bool& play = false, const float& volume = 1.f);
    };
}
