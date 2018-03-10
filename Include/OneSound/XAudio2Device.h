/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "..\ThirdParty\Include\XAudio2_7\XAudio2.h"
#include "..\ThirdParty\Include\XAudio2_7\X3DAudio.h"

#if defined (_WIN32)
#   pragma comment(lib, "../ThirdParty/X3DAudio86.lib")
#elif not defined (_WIN32)
#   pragma comment(lib, "../ThirdParty/X3DAudio64.lib")
#endif

namespace onesnd
{
    class SoundBuffer;
    class AudioStream;

    class XAudio2Device
    {
    public:
        XAudio2Device() = default;
       ~XAudio2Device() = default;

    public:
        static XAudio2Device& instance()
        {
            static XAudio2Device xaudio2_device;
            return xaudio2_device;
        }

    public:
        void initialize();
        void finalize();    

        IXAudio2* getEngine() const { return xEngine; }
        IXAudio2MasteringVoice* getMaster() const { return xMaster; }

    private:
        IXAudio2* xEngine;
        IXAudio2MasteringVoice* xMaster;
        X3DAUDIO_HANDLE x3DAudioHandle;
        X3DAUDIO_LISTENER xListener;
    };
    
    struct XABuffer : XAUDIO2_BUFFER
    {
        WAVEFORMATEX wf;		// wave format descriptor
        int nBytesPerSample;	// number of bytes per single audio sample (1 or 2 bytes)
        int nPCMSamples;		// number of PCM samples in the entire buffer
        unsigned wfHash;		// waveformat pseudo-hash

        static XABuffer* create(SoundBuffer* ctx, int size, AudioStream* strm, int* pos = nullptr);
        static void destroy(XABuffer*& buffer);

        static void stream(XABuffer* buffer, AudioStream* strm, int* pos = nullptr);

        static int getBuffersQueued(IXAudio2SourceVoice* source);
    };
}