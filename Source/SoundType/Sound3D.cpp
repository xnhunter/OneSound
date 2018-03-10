/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\SoundType\Sound3D.h"

namespace onesnd
{
    Sound3D::Sound3D() : SoundObject()
    { }

    Sound3D::Sound3D(const std::shared_ptr<SoundBuffer>& sound, const bool& loop, const bool& play, const float& volume) : SoundObject(sound, loop, play, volume)
    { }
}