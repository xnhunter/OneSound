/*
 * Example 4 "OGG" for OneSound.
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

        shared_ptr<SoundBuffer> stream;
        vector<shared_ptr<Sound2D>> sounds;

        // Usefull lambdas for playing buffers/streams.

        // The first lambda for playing a sound.
        auto play_sound([&] (const fs::path& file_name, const bool& loop = false, const bool& play = true, const float& volume = 1.0f)
        {
            stream = make_shared<SoundBuffer>(file_name);

            // Create a sound buffer without looping.
            // It plays at once beyond loading with 100% volume.
            // The sound is playing when we push it back into the vector.
            sounds.push_back(make_shared<Sound2D>(stream, loop, play, volume));
        });

        // The second lambda for streaming a sound.
        auto stream_sound([&] (const fs::path& file_name, const bool& loop = false, const bool& play = true, const float& volume = 1.0f)
        {
        // NOTE: Only a few active streams.
            sounds.push_back(
                make_shared<Sound2D>(
                make_shared<SoundStream>(file_name),
                loop,
                play,
                volume)
            );
        });

        play_sound("Sound\\thunder.ogg", false, true, 0.79f);
        stream_sound("Sound\\Crysis 1.ogg", false, true, 1.0f);

        cout << "Press any key to quit." << endl;
        auto symbol = char();
        cin >> symbol;
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