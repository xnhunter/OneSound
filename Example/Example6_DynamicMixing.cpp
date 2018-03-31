/*
 * Example 6 "DynamicMixing" for OneSound.
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\OneSound.h"

#include <iostream>
#include <thread>
#include <array>

using namespace std;
using namespace onesnd;

int main()
{
    try
    {
        auto one_sound = make_shared<OneSound>();

        array<shared_ptr<SoundBuffer>, 5> buffers;

        buffers[0] = make_shared<SoundBuffer>();
        buffers[0]->Load("Sound\\shot.wav"); // Load a sound after sound stream/buffer initialization.

        buffers[1] = make_shared<SoundStream>();
        buffers[1]->Load("Sound\\Crysis 1.ogg");

        buffers[2] = make_shared<SoundStream>("Sound\\forest.ogg"); // Load a sound at once.
        buffers[3] = make_shared<SoundBuffer>("Sound\\thunder.ogg");
        buffers[4] = make_shared<SoundStream>("Sound\\voice.mp3");

        cout << "Control Keys:" << endl 
             << "1 - Create Shot          (OGG Buffer)" << endl
             << "2 - Create Crysis 1      (OGG Stream)" << endl
             << "3 - Create Forest        (OGG Stream)" << endl
             << "4 - Create Thunder       (WAV Buffer)" << endl
             << "5 - Create Voice         (MP3 Stream)" << endl
             << "S - Stop all sounds" << endl << endl;

        vector<shared_ptr<Sound2D>> active_sounds; // We'll use it to keep track of generated sounds.

        auto isKeyDown([](int vkey)
        {
            if (GetAsyncKeyState(vkey) & 0x0001)
                return true;

            return false;
        });

        const auto exit_key = '9';
        cout << "Press" << ends << exit_key << ends << "to stop getting keys down." << endl;

        while (!isKeyDown(exit_key))
        {
            if (isKeyDown('1'))
                active_sounds.push_back(make_shared<Sound2D>(buffers[0], false, true));

            if (isKeyDown('2'))
            {
                auto sound_2 = make_shared<Sound2D>(buffers[1]);
                sound_2->play(); // It can be played anywhere.

                active_sounds.push_back(sound_2);
            }

            if (isKeyDown('3'))
            {
                auto sound_3 = make_shared<Sound2D>();
                sound_3->play(buffers[2], false, true, 0.7f); // Play whatever.

                active_sounds.push_back(sound_3);
            }

            if (isKeyDown('4'))
            {
                auto sound_4 = make_shared<Sound2D>(buffers[3]);
                sound_4->play(); // Play whatever.

                active_sounds.push_back(sound_4);
            }

            if (isKeyDown('5'))
                active_sounds.push_back(make_shared<Sound2D>(buffers[4], false, true));

            if (isKeyDown('S'))
            {
                // Smooth fadeout over 300 ms.
                for (int i = 0; i < 10; ++i)
                {
                    for (const auto& sound : active_sounds) // Gradually reduce all sounds by 0.05f.
                        if (sound->getVolume() >= 0.1f)
                            sound->setVolume(sound->getVolume() - 0.1f);

                    this_thread::sleep_for<>(30ms); // 10x30ms = 300 ms
                }

                for (const auto sound : active_sounds)
                    sound->stop();

                active_sounds.clear();
            }
        }

        active_sounds.clear();

        cout << "Press any key to quit." << endl;
        cin.get();
    }
    catch (const runtime_error& e)
    {
        cout << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception." << endl;
    }

    return 0;
}