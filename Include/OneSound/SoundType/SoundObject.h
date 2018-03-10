/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#pragma once

#include "OneSound\Export.h"

#include "OneSound\SoundType\SoundObjectState.h"

#include "OneSound\SoundType\SoundBuffer.h"

namespace onesnd
{
    /** 
    * Base for all the 3D and Ambient sound objects
    */
    class ONE_SOUND_API SoundObject
    {
    protected:
        std::shared_ptr<SoundBuffer> sound;					// sound buffer/stream to use

        IXAudio2SourceVoice* source;		// the sound source generator (interfaces XAudio2 to generate waveforms)
        SoundObjectState* state;			// Holds and manages the current state of a SoundObject

        X3DAUDIO_EMITTER Emitter;			// 3D sound emitter data (this object)

        /**
        * Creates an uninitialzed empty SoundObject
        */
        SoundObject();
        /**
        * Creates a soundobject with an attached buffer
        * @param sound SoundBuffer this object uses for playing sounds
        * @param loop True if sound looping is wished
        * @param play True if sound should start playing immediatelly
        */
        SoundObject(const std::shared_ptr<SoundBuffer>& sound, const bool& loop = false, const bool& play = false, const float& volume = 1.f);
        ~SoundObject(); // unhooks any sounds and frees resources

    public:
        /**
        * Sets the SoundBuffer or SoundStream for this SoundObject. Set NULL to remove and unbind the SoundBuffer.
        * @param sound Sound to bind to this object. Can be NULL to unbind sounds from this object.
        * @param loop [optional] Sets the sound looping or non-looping. Streams cannot be looped.
        */
        void setSound(const std::shared_ptr<SoundBuffer>& sound, const bool& loop = false, const bool& play = false, const float& volume = 1.f);

        /**
        * @return Current soundbuffer set to this soundobject
        */
        inline std::shared_ptr<SoundBuffer> getSound() const { return sound; }

        inline IXAudio2SourceVoice* getSource() const { return source; }

        /**
        * @return TRUE if this SoundObject has an attached SoundStream that can be streamed.
        */
        bool isStreamable() const;

        /**
        * @return TRUE if this SoundObject is streamable and the End Of Stream was reached.
        */
        bool isEOS() const;

        /**
        * Starts playing the sound. If the sound is already playing, it is rewinded and played again from the start.
        */
        void play();

        /**
        * Starts playing a new sound. Any older playing sounds will be stopped and replaced with this sound.
        * @param sound SoundBuffer or SoundStream to start playing
        * @param loop [false] Sets if the sound is looping or not. Streams are never loopable.
        */
        void play(const std::shared_ptr<SoundBuffer>& sound, const bool& loop = false, const bool& play = false, const float& volume = 1.f);

        /**
        * Stops playing the sound and unloads streaming buffers.
        */
        void stop();

        /**
        * Temporarily pauses sound playback and doesn't unload any streaming buffers.
        */
        void pause();

        /**
        * If current status is Playing, then this rewinds to start of the soundbuffer/stream and starts playing again.
        * If current status is NOT playing, then any stream resources are freed and the object is reset.
        */
        void rewind();

        /**
        * @return TRUE if the sound source is PLAYING.
        */
        bool isPlaying() const;

        /**
        * @return TRUE if the sound source is STOPPED.
        */
        bool isStopped() const;

        /**
        * @return TRUE if the sound source is PAUSED.
        */
        bool isPaused() const;

        /**
        * @return TRUE if the sound source is at the beginning of the sound buffer.
        */
        bool isInitial() const;

        /**
        * @return TRUE if the sound source is in LOOPING mode.
        */
        bool isLooping() const;

        /**
        * Sets the looping mode of the sound source.
        */
        void setLooping(const bool& looping);

        /**
        * Indicates the gain (volume amplification) applied. Range [0.0f .. 1.0f]
        * Each division by 2 equals an attenuation of -6dB. Each multiplicaton with 2 equals an amplification of +6dB.
        * A value of 0.0 is meaningless with respect to a logarithmic scale; it is interpreted as zero volume - the channel is effectively disabled.
        * @param gain Gain value between 0.0..1.0
        */
        void setVolume(const float& gain);

        /**
        * @return Current gain value of this source
        */
        float getVolume() const;

        /**
        * @return Gets the current playback position in the SoundBuffer or SoundStream in SAMPLES
        */
        int getPlaybackPosition() const;

        /**
        * Sets the playback position of the underlying SoundBuffer or SoundStream in SAMPLES
        * @param seekpos Position in SAMPLES where to seek in the SoundBuffer or SoundStream [0..PlaybackSize]
        */
        void setPlaybackPosition(int seekpos);

        /**
        * @return Playback size of the underlying SoundBuffer or SoundStream in SAMPLES
        */
        int getPlaybackSize() const;

        /**
        * @return Number of samples processed every second (aka SampleRate or Frequency)
        */
        int getSamplesPerSecond() const;
    };
}