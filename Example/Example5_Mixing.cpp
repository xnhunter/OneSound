/*
 * Example 5 "Mixing" for OneSound.
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\OneSound.h"

#include <iostream>

using namespace std;
using namespace onesnd;

int main()
{
    try
    {
        auto one_sound = make_unique<OneSound>();

        unique_ptr<SoundBuffer> buffer;
        vector<unique_ptr<Sound2D>> sounds;

        auto play_sound([&](const fs::path& file_name, const bool& loop = false, const bool& play = true, const float& volume = 1.0f)
        {
            buffer = make_unique<SoundBuffer>(file_name);
            sounds.push_back(make_unique<Sound2D>(buffer, loop, play, volume));
        });

        const auto exit_key = '1';
        cout << "Press" << ends << exit_key << ends << "to stop asking the information." << endl;

        while (!GetAsyncKeyState(exit_key) & 0x0001)
        {
            auto file_name = fs::path();
            cout << "Enter the sound's file name:" << ends;
            cin >> file_name;

            if (!fs::exists(file_name))
            {
                cout << "Can't find file:" << ends << file_name << endl;
                continue; // Not to throw application.
            }

            auto loop = bool();
            cout << "Enter the sound's looping(1 or 0): " << ends;
            cin >> loop;

            auto volume = float();
            cout << "Enter the sound's volume(i.e. 0.3): " << ends;
            cin >> volume;

            play_sound(file_name, loop, true, volume);
        }

        cout << "Press any key to quit." << endl;
        cin.get();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception." << endl;
    }

    return 0;
}