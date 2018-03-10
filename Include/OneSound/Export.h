/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

namespace onesnd
{
    #ifdef _MSC_VER
    #   define ONE_SOUND_API __declspec(dllexport)
    #else
    #   define ONE_SOUND_API
    #   error Not a Windows platform.
    #endif
}