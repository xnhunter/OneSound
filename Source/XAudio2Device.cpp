/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\XAudio2Device.h"

#include "OneSound\StreamType\AudioStream.h"

namespace onesnd
{
    void XAudio2Device::initialize()
    {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        auto flags = long();

    #if defined (_WIN32) && defined (_DEBUG)
        flags |= XAUDIO2_DEBUG_ENGINE;
    #else
        flags |= 0;
    #endif

        XAudio2Create(&xEngine, flags);
        xEngine->CreateMasteringVoice(&xMaster, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);
        X3DAudioInitialize(SPEAKER_STEREO, X3DAUDIO_SPEED_OF_SOUND, x3DAudioHandle);
    }

    void XAudio2Device::finalize()
    {
        xMaster->DestroyVoice();
        xEngine->Release();

        if (xMaster != nullptr)
            xMaster = nullptr;

        if (xEngine != nullptr)
            xEngine = nullptr;
    }

    XABuffer* XABuffer::create(SoundBuffer* ctx, int size, AudioStream* strm, int* pos)
    {
        if (pos) 
            strm->Seek(*pos); // seek to specified pos, let the AudioStream handle error conditions
        if (strm->IsEOS()) 
            return nullptr; // EOS(), failed!

        int bytesToRead = strm->Available();
        if (size < bytesToRead) 
            bytesToRead = size;

        auto* buffer = (XABuffer*)malloc(sizeof(XABuffer) + bytesToRead);
        if (!buffer) 
            return nullptr; // out of memory

        auto* data = (BYTE*)buffer + sizeof(XABuffer); // sound data follows after the XABuffer header

        int bytesRead = strm->ReadSome(data, bytesToRead);
        if (pos) 
            *pos += bytesRead; // update position

        buffer->Flags = strm->IsEOS() ? XAUDIO2_END_OF_STREAM : 0;
        buffer->AudioBytes = bytesRead;
        buffer->pAudioData = data;
        buffer->PlayBegin = 0;		// first sample to play
        buffer->PlayLength = 0;		// number of samples to play
        buffer->LoopBegin = 0;		// first sample to loop
        buffer->LoopLength = 0;		// number of samples to loop
        buffer->LoopCount = 0;		// how many times to loop the region
        buffer->pContext = ctx;		// context of the buffer

        auto sampleSize = strm->SingleSampleSize();
        buffer->nBytesPerSample = sampleSize;

        auto& wf = buffer->wf;
        wf.wFormatTag = WAVE_FORMAT_PCM;
        wf.nChannels = strm->Channels();
        buffer->nPCMSamples = bytesRead / (sampleSize * wf.nChannels);
        wf.nSamplesPerSec = strm->Frequency();
        wf.wBitsPerSample = sampleSize * 8;
        wf.nBlockAlign = (wf.nChannels * sampleSize);
        wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;
        wf.cbSize = sizeof(WAVEFORMATEX);

        // this is enough to create an somewhat unique pseudo-hash:
        buffer->wfHash = wf.nSamplesPerSec + (wf.nChannels * 25) + (wf.wBitsPerSample * 7);
        return buffer;
    }

    void XABuffer::stream(XABuffer* buffer, AudioStream* strm, int* pos)
    {
        if (pos) 
            strm->Seek(*pos); // seek to specified pos, let the AudioStream handle error conditions

        buffer->AudioBytes = strm->ReadSome((void*)buffer->pAudioData, buffer->AudioBytes);

        if (strm->IsEOS()) // end of stream was reached
            buffer->Flags = XAUDIO2_END_OF_STREAM;

        if (pos) 
            *pos += buffer->AudioBytes; // update position
    }

    void XABuffer::destroy(XABuffer*& buffer)
    {
        free(reinterpret_cast<void*>(buffer));
        buffer = nullptr;
    }

    int XABuffer::getBuffersQueued(IXAudio2SourceVoice* source)
    {
        XAUDIO2_VOICE_STATE state;
        source->GetState(&state);

        return state.BuffersQueued;
    }
}