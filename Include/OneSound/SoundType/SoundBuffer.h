/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\Utility.h"

#include"OneSound\SoundType\SoundObject.h"

namespace onesnd
{
    struct XABuffer;
    /**
    * A simple SoundBuffer designed for loading small sound files into a static buffer.
    * Should be used for sound files smaller than 64KB (1.5s @ 41kHz).
    */
    class ONE_SOUND_API SoundBuffer
    {
    protected:
        // number of references of this buffer held by SoundObjects; 
        // NOTE: SoundBuffer can't be unloaded until referance_count == 0.
        int referance_count;
        XABuffer* xaBuffer;

    public:

        /**
        * Creates a new SoundBuffer object
        */
        SoundBuffer();

        /**
        * Creates a new SoundBuffer and loads the specified sound file
        * @param file Path to sound file to load
        */
        SoundBuffer(const fs::path& file);

        /**
        * Destroyes and unloads this buffer
        */
        virtual ~SoundBuffer();

        XABuffer* getXABuffer() const { return xaBuffer; }

        /**
        * @return Frequency in Hz of this SoundBuffer data
        */
        int Frequency() const;

        /**
        * @return Number of bits in a sample of this SoundBuffer data (8 or 16)
        */
        int SampleBits() const;

        /**
        * @return Number of bytes in a sample of this SoundBuffer data (1 or 2)
        */
        int SampleBytes() const;

        /**
        * @return Number of sound channels in the SoundBuffer data (1 or 2)
        */
        int Channels() const;

        /**
        * @return Size of a full sample block in bytes [LL][RR] (1 to 4)
        */
        int FullSampleSize() const;

        /**
        * @return Size of this SoundBuffer or SoundStream in BYTES
        */
        int SizeBytes() const;

        /**
        * @return Number of Bytes Per Second for this audio data (Frequency * Channels * SampleBytes)
        */
        int BytesPerSecond() const;

        /**
        * @return Size of this SoundBuffer in PCM SAMPLES
        */
        virtual int Size() const;

        /**
        * @return Number of SoundObjects that still reference this SoundBuffer.
        */
        int getReferanceCount() const;

        /**
        * @return TRUE if this object is a Sound stream
        */
        virtual bool IsStream() const;

        /**
        * @return Wave format descriptor for this buffer
        */
        const WAVEFORMATEX* WaveFormat() const;

        /**
        * @return Unique hash to distinguish between different Wave formats
        */
        unsigned WaveFormatHash() const;

        /**
        * Loads this SoundBuffer with data found in the specified file.
        * Supported formats: .wav .mp3
        * @param file Sound file to load
        * @return TRUE if loading succeeded and a valid buffer was created.
        */
        virtual bool Load(const fs::path& file);

        /**
        * Tries to release the underlying sound buffer and free the memory.
        * @note This function will fail if refCount > 0. This means there are SoundObjects still using this SoundBuffer
        * @return TRUE if SoundBuffer data was freed, FALSE if SoundBuffer is still used by a SoundObject.
        */
        virtual bool Unload();

        /**
        * Binds a specific source to this SoundBuffer and increases the refCount.
        * @param so SoundObject to bind to this SoundBuffer.
        * @return FALSE if the binding failed. TODO: POSSIBLE REASONS?
        */
        virtual bool BindSource(SoundObject* so);

        /**
        * Unbinds a specific source from this SoundBuffer and decreases the refCount.
        * @param so SoundObject to unbind from this SoundBuffer.
        * @return FALSE if the unbinding failed. TODO: POSSIBLE REASONS?
        */
        virtual bool UnbindSource(SoundObject* so);

        /**
        * Resets the buffer in the context of the specified SoundObject
        * @note Calls ResetStream on AudioStreams.
        * @param so SoundObject to reset this buffer for
        * @return TRUE if reset was successful
        */
        virtual bool ResetBuffer(SoundObject* so);
    };
}