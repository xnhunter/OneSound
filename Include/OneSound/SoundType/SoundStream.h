/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\Utility.h"

#include "OneSound\SoundType\SoundBuffer.h"

namespace onesnd
{
    /**
    * SoundStream stream audio data from a file source.
    * Extremely useful for large file playback. Even a 4m long mp3 can take over 40mb of ram.
    * Multiple sources can be bound to this stream.
    */
    class ONE_SOUND_API SoundStream : public SoundBuffer
    {
    protected:
        struct SO_ENTRY
        {
            SoundObject* obj;
            int base; // current PCM block offset
            int next; // the next PCM block offset to load

            XABuffer* front;	// currently playing buffer - frontbuffer
            XABuffer* back;		// enqueued backbuffer
            bool busy;			// the stream is busy on an internal operation, all other operations are ignored

            inline SO_ENTRY(SoundObject* obj) : 
                obj(obj), 
                base(0), 
                next(0), 
                front(0), 
                back(0), 
                busy(false)
            { }
        };

        std::vector<SO_ENTRY> alSources;	// bound sources
        AudioStream* alStream;			// streamer object

    public:

        /**
        * Creates a new SoundsStream object
        */
        SoundStream();

        /**
        * Creates a new SoundStream object and loads the specified sound file
        * @param file Path to the sound file to load
        */
        SoundStream(const fs::path& file);

        /**
        * Destroys and unloads any resources held
        */
        virtual ~SoundStream();

        /**
        * @return Size of this SoundStream in PCM SAMPLES
        */
        virtual int Size() const override;

        /**
        * @return TRUE if this object is a Sound stream
        */
        virtual bool IsStream() const override;

        /**
        * Initializes this SoundStream with data found in the specified file.
        * Supported formats: .wav .mp3
        * @param file Sound file to load
        * @return TRUE if loading succeeded and a stream was initialized.
        */
        virtual bool Load(const fs::path& file) override;

        /**
        * Tries to release the underlying sound buffers and free the memory.
        * @note This function will fail if refCount > 0. This means there are SoundObjects still using this SoundStream
        * @return TRUE if SoundStream data was freed, FALSE if SoundStream is still used by a SoundObject.
        */
        virtual bool Unload() override;

        /**
        * Binds a specific source to this SoundStream and increases the refCount.
        * @param so SoundObject to bind to this SoundStream.
        * @return FALSE if the binding failed. TODO: POSSIBLE REASONS?
        */
        virtual bool BindSource(SoundObject* so) override;

        /**
        * Unbinds a specific source from this SoundStream and decreases the refCount.
        * @param so SoundObject to unbind from this SoundStream.
        * @return FALSE if the unbinding failed. TODO: POSSIBLE REASONS?
        */
        virtual bool UnbindSource(SoundObject* so) override;

        /**
        * Resets the buffer in the context of the specified SoundObject
        * @note Calls ResetStream on AudioStreams.
        * @param so SoundObject to reset this buffer for
        * @return TRUE if reset was successful
        */
        virtual bool ResetBuffer(SoundObject* so) override;

        /**
        * Resets the stream by unloading previous buffers and requeuing the first two buffers.
        * @param so SoundObject to reset the stream for
        * @return TRUE if stream was successfully reloaded
        */
        bool ResetStream(SoundObject* so);

        /**
        * Streams the next Buffer block from the stream.
        * @param so Specific SoundObject to stream with.
        * @return TRUE if a buffer was streamed. FALSE if EOS() or stream is busy.
        */
        bool StreamNext(SoundObject* so);

        /**
        * @param so Specific SoundObject to check for end of stream
        * @return TRUE if End of Stream was reached or if there is no stream loaded
        */
        bool IsEOS(const SoundObject* so) const;

        /**
        * @param so SoundObject to query index of
        * @return A valid pointer if this SoundObject is bound. Otherwise NULL.
        */
        SO_ENTRY* GetSOEntry(const SoundObject* so) const;

        /**
        * Seeks to the specified sample position in the stream.
        * @note The SoundObject will stop playing and must be manually restarted!
        * @param so SoundObject to perform seek on
        * @param samplepos Position in the stream in samples [0..SoundStream::Size()]
        */
        void Seek(SoundObject* so, int samplepos);

    protected:

        /**
        * Internal stream function.
        * @param soe SoundObject Entry to stream
        * @return TRUE if a buffer was streamed
        */
        bool StreamNext(SO_ENTRY& soe);

        /**
        * [internal] Load streaming data into the specified SoundObject
        * at the optionally specified streamposition.
        * @param so SO_ENTRY handle to queue with stream data
        * @param streampos [optional] PCM byte position in stream where to seek data from.
        *                  If unspecified (default -1), stream will use the current streampos
        */
        bool LoadStreamData(SO_ENTRY& so, int streampos = -1);

        /**
        * [internal] Unloads all queued data for the specified SoundObject
        * @param so SO_ENTRY handle to unqueue and unload data for
        */
        void ClearStreamData(SO_ENTRY& so);
    };
}