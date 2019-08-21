OneSound
==========
Modern library for audio playback supporting 3 common audio formats. The software provides a **high-performance** audio sources, it is written in C++17. The library is generally suited for a game engine or media applications.

Overview
--------

- **Version**: 1.0.0
- **License**: [MIT](https://github.com/weelhelmer/OneSound/master/LICENSE)
- **Status**: Alpha

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
#include <iostream>
#include <OneSound/OneSound.h>

using namespace std;
using namespace onesnd;

int main()
{ 
    // Create OneSound that initialize XAudio2 Device.
    auto one_sound = make_unique<OneSound>();

    // Create a sound that not loops, plays at once with 75% volume.
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
