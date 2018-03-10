/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\XAudio2Device.h"

namespace onesnd
{
    class SoundObject;

    struct SoundObjectState : public IXAudio2VoiceCallback
    {
        SoundObject* sound;
        bool isInitial;		// is the Sound object Rewinded to its initial position?
        bool isPlaying = false;		// is the Voice digesting buffers?
        bool isLoopable;	// should this sound act as a loopable sound?
        bool isPaused;		// currently paused?
        XABuffer shallow;	// a shallow buffer reference (no actual data)

        SoundObjectState(SoundObject* so) : 
            sound(so),
            isInitial(false),
            isPlaying(false),
            isLoopable(false), isPaused(false)
        { }

        void __stdcall OnStreamEnd() override;
        void __stdcall OnBufferEnd(void* ctx) override;
        
        void __stdcall OnVoiceProcessingPassStart(UINT32 samplesRequired) override
        { }
        void __stdcall OnVoiceProcessingPassEnd() override
        { }
        void __stdcall OnBufferStart(void* ctx) override
        { }
        void __stdcall OnLoopEnd(void* ctx) override
        { }
        void __stdcall OnVoiceError(void* ctx, HRESULT error) override
        { }
    };
}