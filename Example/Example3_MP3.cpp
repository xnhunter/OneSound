/*
 * Example 3 "MP3" for OneSound.
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\OneSound.h"

#include <iostream>
#include <thread>

using namespace std;
using namespace onesnd;

int main()
{
    try
    {
        auto one_sound = make_unique<OneSound>();

        // Following two sounds will be almost playing in the time.
        auto sound_1 = make_shared<Sound2D>(make_shared<SoundBuffer>("Sound\\voice.mp3"),
                                            false,
                                            true,
                                            1.0f);

        auto sound_2 = make_shared<Sound2D>(make_shared<SoundStream>("Sound\\River Frows In You.mp3"),
                                            false,
                                            true,
                                            0.75f);

        const auto exit_key = '9';
        cout << "Press" << ends << exit_key << ends << "to stop showing the information." << endl;
        cout << "XAudio2 memory usage (KB) per second:" << ends;

        while (sound_1->isPlaying() || sound_2->isPlaying())
        {
            // Wait for 1 second.
            this_thread::sleep_for<>(1s);

            // Check the user press the target key.
            if (GetAsyncKeyState(exit_key) & 0x0001) // KEY & KEY_DOWN
                break;

            // Usefull function for debuging, for more details see XAUDIO2_PERFORMANCE_DATA structure.
            // Devide on 1024 to get kilobytes instead of bytes.
            cout << one_sound->getPerfomanceData().MemoryUsageInBytes / 1024 << ends;
        }

        cout << endl << "Press any key to quit." << endl;
        auto i = char();
        cin >> i;
    }
    catch (const std::runtime_error& e)
    {
        cerr << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception." << endl;
    }

    return 0;
}