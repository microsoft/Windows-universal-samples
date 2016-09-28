//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

namespace SDKTemplate
{
    template<typename T, UINT32 LookupTableSize>
    class LookupTable
    {
    public:

        // Function type for lookup table generation.
        typedef std::function<T(UINT32, UINT32)> LookupTableGenerator;

        /// <summary>
        /// The values of the lookup table are generated using a function passed into the constructor.
        /// </summary>
        LookupTable(LookupTableGenerator Generator)
        {
            for (int i = 0; i < LookupTableSize; i++)
            {
                // Generate values for lookup table
                m_lookuptable[i] = Generator(i, LookupTableSize);
            }
        }

        T GetValue(float value)
        {
            int index = static_cast<int>(value * LookupTableSize);
            index = min(max(0, index), LookupTableSize - 1);
            return m_lookuptable[index];
        }

    private:
        T m_lookuptable[LookupTableSize];
    };
} // SDKTemplate