#pragma once

#include "SizePreferenceString.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct SizePreferenceString : SizePreferenceStringT<SizePreferenceString>
    {
        SizePreferenceString(Windows::UI::ViewManagement::ViewSizePreference preference, hstring title)
            : m_preference(preference), m_title(title)
        {
        }

        hstring Title()
        {
            return m_title;
        }

        Windows::UI::ViewManagement::ViewSizePreference Preference()
        {
            return m_preference;
        }

    private:
        hstring m_title;
        Windows::UI::ViewManagement::ViewSizePreference m_preference;
    };
}
