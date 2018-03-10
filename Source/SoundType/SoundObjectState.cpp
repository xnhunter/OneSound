/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\SoundType\SoundObjectState.h"

#include "OneSound\SoundType\SoundBuffer.h"
#include "OneSound\SoundType\SoundStream.h"

namespace onesnd
{
    void __stdcall SoundObjectState::OnStreamEnd()
    {
        if (isLoopable) // loopable?
            sound->rewind(); // rewind the sound and continue playing
        else
            isPlaying = false;
    }

    // a buffer object finished processing
    void __stdcall SoundObjectState::OnBufferEnd(void* ctx)
    {
        isInitial = false;
        if (((SoundBuffer*)ctx)->IsStream())
        {
            // stream fetch next buffer for this sound
            ((SoundStream*)ctx)->StreamNext(sound);
        }
    }
}