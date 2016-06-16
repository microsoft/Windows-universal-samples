#pragma once

namespace NavigationMenuSample {
    class SystemInformationHelpers sealed
    {
    public:
        // For now, the 10-foot experience is enabled only on Xbox.
        static bool IsTenFootExperience() { return IsXbox(); }

    private:
        static bool IsXbox()
        {
            // Calculate this once and cache the result.
            static bool _isXbox = (Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily == "Windows.Xbox");
            return _isXbox;
        }
    };
}


