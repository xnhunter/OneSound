/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\SoundType\SoundStream.h"

#include "OneSound\StreamType\AudioStream.h"

namespace onesnd
{
    SoundStream::SoundStream()
    { }

    SoundStream::SoundStream(const fs::path& file)
    {
        Load(file);
    }

    SoundStream::~SoundStream()
    {
        if (xaBuffer) // active buffer?
            Unload();
    }

    int SoundStream::Size() const
    {
        return alStream ? (alStream->Size() / FullSampleSize()) : 0;
    }

    bool SoundStream::IsStream() const
    {
        return true;
    }

    bool SoundStream::Load(const fs::path& file)
    {
        if (XAudio2Device::instance().getEngine() == nullptr)
            throw std::runtime_error("Can't create sound because XAudio2 Device is not created.");

        if (xaBuffer) // is there existing data?
            return false;

        if (!(alStream = createAudioStream(file.string().c_str())))
            return false; // :(

        if (!alStream->OpenStream(file))
            return false;

        // load the first buffer in the stream:
        xaBuffer = XABuffer::create(this, alStream->BytesPerSecond(), alStream, 0);

        return xaBuffer != nullptr;
    }

    bool SoundStream::Unload()
    {
        if (!xaBuffer)
            return true; // yes, its unloaded
        if (referance_count > 0)
        {
            //indebug(printf("SoundStream::Unload() Memory Leak: failed to delete xaBuffer, because it's still in use.\n"));
            return false; // can't do anything here while still referenced
        }
        XABuffer::destroy(xaBuffer);

        if (alStream)
        {
            delete alStream; 
            alStream = nullptr;
        }
        return true;
    }

    bool SoundStream::BindSource(SoundObject* so)
    {
        if (!xaBuffer)
            return false; // no data loaded yet

        alSources.emplace_back(so);				// default streamPos
        LoadStreamData(alSources.back(), 0);	// load initial stream data (2 buffers)

        ++referance_count;
        return true;
    }

    bool SoundStream::UnbindSource(SoundObject* so)
    {
        if (!xaBuffer)
            return false; // no data loaded yet

        auto* e = GetSOEntry(so);
        if (!e)
            return false; // source doesn't exist

        ClearStreamData(*e); // unload all buffers

        alSources.erase(alSources.begin() + (e - alSources.data()));
        --referance_count;

        return true;
    }

    bool SoundStream::ResetBuffer(SoundObject* so)
    {
        return ResetStream(so);
    }

    bool SoundStream::StreamNext(SoundObject* so)
    {
        if (!xaBuffer)
            return false; // nothing to do here

        if (auto* e = GetSOEntry(so))
        {
            if (e->busy) 
                return false; // can't stream when stream is busy

            return StreamNext(*e);
        }

        return false; // nothing to stream
    }

    bool SoundStream::ResetStream(SoundObject* so)
    {
        // simply clear and load again, to avoid code maintenance hell
        if (auto* e = GetSOEntry(so))
        {
            ClearStreamData(*e);
            return LoadStreamData(*e, 0);
        }

        return false;
    }

    bool SoundStream::IsEOS(const SoundObject* so) const
    {
        auto* e = GetSOEntry(so);
        return e ? true : (alStream ? (e->next >= alStream->Size()) : true);
    }

    SoundStream::SO_ENTRY* SoundStream::GetSOEntry(const SoundObject* so) const
    {
        for (const auto & e : alSources)
            if (e.obj == so)
                return (SO_ENTRY*)&e;

        return nullptr; // not found
    }

    void SoundStream::Seek(SoundObject* so, int samplepos)
    {
        if (SO_ENTRY* e = GetSOEntry(so))
        {
            // if samplepos out of bounds THEN 0 ELSE convert samplepos to bytepos
            auto bytepos = (samplepos >= Size()) ? 0 : samplepos * xaBuffer->wf.nBlockAlign;

            ClearStreamData(*e);
            LoadStreamData(*e, bytepos);

        }

    }

    bool SoundStream::StreamNext(SO_ENTRY& e)
    {
        if (e.next >= alStream->Size()) // is EOF?
            return false;

        // front buffer was processed, swap buffers:
        std::swap(e.front, e.back);

        e.base = e.next; // shift the base pointer forward
        if (e.back == xaBuffer) // we can't refill xaBuffer
        {
            e.back = XABuffer::create(this, xaBuffer->wf.nAvgBytesPerSec, alStream, &e.next);
            if (!e.back)
                return false; // oh no...
        }
        else
        {
            if (!e.back) // no backbuffer. probably ClearStreamData() was called
                return false;

            XABuffer::stream(e.back, alStream, &e.next);
        }

        e.obj->getSource()->SubmitSourceBuffer(e.back); // submit the backbuffer to the queue
        return true;
    }

    bool SoundStream::LoadStreamData(SO_ENTRY& so, int streampos)
    {
        auto pos = streampos == -1 ? so.next : streampos; // -1: use next, else use streampos
        auto streamSize = alStream->Size() - pos; // lets calculate stream size from the SEEK position
        auto bytesPerSecond = alStream->BytesPerSecond();
        auto numBuffers = streamSize > bytesPerSecond ? 2 : 1;
        auto* source = so.obj->getSource();

        so.base = pos;
        if (pos == 0) // pos 0 means we load alBuffer
        {
            pos += xaBuffer->AudioBytes; // update pos
            source->SubmitSourceBuffer(so.front = xaBuffer);
        }
        else // load at arbitrary position
        {
            so.front = XABuffer::create(this, bytesPerSecond, alStream, &pos);
            source->SubmitSourceBuffer(so.front);
        }

        if (numBuffers == 2) // also load a backbuffer
        {
            so.back = XABuffer::create(this, bytesPerSecond, alStream, &pos);
            source->SubmitSourceBuffer(so.back);
        }
        so.next = pos;  // pos variable was updated by CreateXABuffer

        return true;
    }

    void SoundStream::ClearStreamData(SO_ENTRY& so)
    {
        so.busy = true;
        auto* source = so.obj->getSource();
        source->Stop();

        if (XABuffer::getBuffersQueued(source)) // only flush if we have something to flush
            source->FlushSourceBuffers();

        if (so.front)
        {
            if (so.front != xaBuffer)
                delete so.front,
                so.front = nullptr;
        }
        if (so.back)
        {
            delete so.back;
            so.back = nullptr;
        }

        so.busy = false;
    }
}