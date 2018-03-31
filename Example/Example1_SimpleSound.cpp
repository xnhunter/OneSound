/*
 * Example 1 "Simple Sound" for OneSound.
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
        // Create OneSound that initialize XAudio2 Device.
        auto one_sound = make_unique<OneSound>();

        // Create a sound that not loops, plays at once and with 75% volume.
        auto sound_1 = make_unique<Sound2D>(make_unique<SoundBuffer>("Sound\\shot.wav"),
                                            false, // Looping
                                            true, // Playing
                                            0.75f); // Volume

        cout << "Press any key to quit." << endl;
        cin.get();
    }
    catch (const std::runtime_error& e)
    {
        cerr << e.what() << endl;
    }
    catch (...)
    {
        cerr "Unknown Exception"
    }

    return 0;
}