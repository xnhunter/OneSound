/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\SoundType\SoundObjectState.h"

#include "OneSound\StreamType\AudioStream.h"
#include "OneSound\SoundType\SoundBuffer.h"
#include "OneSound\SoundType\SoundStream.h"

namespace onesnd
{
    SoundBuffer::SoundBuffer() :
        referance_count(0), 
        xaBuffer(nullptr)
    { }

    SoundBuffer::SoundBuffer(const fs::path& file) : 
        referance_count(0), 
        xaBuffer(nullptr)
    {
        Load(file);
    }

    SoundBuffer::~SoundBuffer()
    {
        if (xaBuffer)
            Unload();
    }

    int SoundBuffer::Frequency() const
    {
        return xaBuffer->wf.nSamplesPerSec;
    }

    int SoundBuffer::SampleBits() const
    {
        return xaBuffer->wf.wBitsPerSample;
    }

    int SoundBuffer::SampleBytes() const
    {
        return xaBuffer->nBytesPerSample;
    }

    int SoundBuffer::Channels() const
    {
        return xaBuffer->wf.nChannels;
    }

    int SoundBuffer::FullSampleSize() const
    {
        return xaBuffer->wf.nBlockAlign;
    }

    int SoundBuffer::SizeBytes() const
    {
        return xaBuffer->AudioBytes;
    }

    int SoundBuffer::BytesPerSecond() const
    {
        return xaBuffer->wf.nAvgBytesPerSec;
    }

    int SoundBuffer::Size() const
    {
        return xaBuffer->nPCMSamples;
    }

    int SoundBuffer::getReferanceCount() const
    {
        return referance_count;
    }

    bool SoundBuffer::IsStream() const
    {
        return false;
    }

    const WAVEFORMATEX* SoundBuffer::WaveFormat() const
    {
        return xaBuffer ? &xaBuffer->wf : nullptr;
    }

    unsigned SoundBuffer::WaveFormatHash() const
    {
        return xaBuffer ? xaBuffer->wfHash : 0;
    }

    bool SoundBuffer::Load(const fs::path& file)
    {
        if (XAudio2Device::instance().getEngine() == nullptr)
            throw std::runtime_error("Can't create sound because XAudio2 Device is not created.");

        if (xaBuffer) // is there existing data?
            return false;

        AudioStream mem; // temporary stream
        AudioStream* strm = &mem;
        if (!createAudioStream(strm, file.string().c_str()))
            return false; // invalid file format or file not found

        if (!strm->OpenStream(file))
            return false; // failed to open the stream (probably not really correct format)

        xaBuffer = XABuffer::create(this, strm->Size(), strm);
        strm->CloseStream(); // close this manually, otherwise we get a nasty error when the dtor runs...
        return xaBuffer != nullptr;
    }

    bool SoundBuffer::Unload()
    {
        if (!xaBuffer)
            return true; // yes, its unloaded
        if (referance_count > 0)
        {
            //indebug(printf("SoundBuffer::Unload() Memory Leak: failed to delete alBuffer, because it's still in use.\n"));
            return false; // can't do anything here while still referenced
        }
        XABuffer::destroy(xaBuffer);
        return true;
    }

    bool SoundBuffer::BindSource(SoundObject* so)
    {
        if (!xaBuffer)
            return false; // no data

        if (so->getSound().get() == this)
            return false; // no double-binding dude, it will mess up referance_counting.

        so->getSource()->SubmitSourceBuffer(xaBuffer); // enqueue this buffer
        ++referance_count;
        return true;
    }

    bool SoundBuffer::UnbindSource(SoundObject* so)
    {
        if (so->getSound().get() == this) // correct buffer link?
        {
            so->getSource()->Stop(); // make sure its stopped (otherwise Flush won't work)
            if (XABuffer::getBuffersQueued(so->getSource()))
                so->getSource()->FlushSourceBuffers(); // ensure not in queue anymore
            --referance_count;
        }
        return true;
    }

    bool SoundBuffer::ResetBuffer(SoundObject* so)
    {
        if (!xaBuffer || !so->getSource())
            return false; // nothing to do here

        so->getSource()->Stop();
        if (XABuffer::getBuffersQueued(so->getSource())) // only flush IF we have buffers to flush
            so->getSource()->FlushSourceBuffers();

        so->getSource()->SubmitSourceBuffer(xaBuffer);
        return true;
    }
}