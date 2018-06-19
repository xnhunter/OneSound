/*
 * OneSound - Modern C++17 audio library for Windows OS with XAudio2 API
 * Copyright ⓒ 2018 Valentyn Bondarenko. All rights reserved.
 * License: https://github.com/weelhelmer/OneSound/master/LICENSE
 */

#include "OneSound\OneSound.h"

namespace onesnd
{
    OneSound::OneSound(const bool& has_initialize)
    {
        if(has_initialize)
            this->initialize();
    };

    XAUDIO2_PERFORMANCE_DATA OneSound::getPerfomanceData() const
    {
        XAUDIO2_PERFORMANCE_DATA pd;
        XAudio2Device::instance().getEngine()->GetPerformanceData(&pd);

        return pd;
    }

    OneSound::~OneSound()
    {
        this->finalize();
    }

    void OneSound::initialize() const
    {
        if (!XAudio2Device::instance().getEngine())
            XAudio2Device::instance().initialize();
    }

    void OneSound::finalize() const
    {
        if (XAudio2Device::instance().getEngine())
            XAudio2Device::instance().finalize();
    }

    unsigned long long OneSound::getLibraryVersion() const
    {
        return 1;
    }

    std::string OneSound::getLibraryVersionStr() const
    {
        return "1.0"s;
    }

    std::string OneSound::getLibraryStatus() const
    {
        return "Demo"s;
    }

    std::string OneSound::getLibraryName() const
    {
        return "OneSound"s;
    }
}
