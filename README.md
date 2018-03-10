#OneSound
==========

This project provides a modern library for audio playback with 3 common formats.

Overview
--------

- **Version**: 1.0.0
- **License**: [MIT](https://github.com/weelhelmer/OneSound/master/LICENSE.txt)
- **Status**: Demo

Supported Platforms
-------------------
- **Windows** Vista
- **Windows** 7
- **Windows** 8/8.1
- **Windows** 10

Used Technology
---------------
- **XAudio2**

Supported Audio File Formats
----------------------
- **WAV** (Waveform Audio File Format) Playing buffers/Streaming
- **MP3** (MPEG-1 Audio Layer 3) Playing buffers/Streaming
- **OGG** (Ogg-Vorbis) Playing buffers/Streaming

Getting Started
---------------

Play a sound with less code as possible:

```cpp
#include <OneSound/OneSound.h>

int main()
{
    // Create OneSound that initialize XAudio2 Device.
    auto one_sound = make_unique<OneSound>();

    // Create a sound that not loops, plays at once and with 75% volume.
    auto sound_1 = make_unique<Sound2D>(make_unique<SoundBuffer>("Sound\\shot.wav"),
                                        false, // Looping
                                        true, // Playing
                                        0.75f); // Volume

    cout << "Press any key to quit: ";
    auto i = char();
    cin >> i;

	return 0;
}
```