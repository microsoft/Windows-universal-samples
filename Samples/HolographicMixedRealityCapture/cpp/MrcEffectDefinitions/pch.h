#pragma once

#include <algorithm>

template<typename T, typename U>
U GetValueFromPropertySet(Windows::Foundation::Collections::IPropertySet^ propertySet, Platform::String^ key, U defaultValue)
{
    try
    {
        return static_cast<U>(safe_cast<T>(propertySet->Lookup(key)));
    }
    catch (Platform::OutOfBoundsException^)
    {
        // The key is not present in the PropertySet. Return the default value.
        return defaultValue;
    }
}