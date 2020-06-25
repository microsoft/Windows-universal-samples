//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario1_FindId.h"
#include "Scenario1_FindId.g.cpp"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Text;
using namespace Windows::UI::Xaml;

namespace
{
    // This is a helper method that an app could create to find one or all available 
    // ids within a string.  An id begins with a character for which IsIdStart,
    // and continues with characters that are IsIdContinue. Invalid sequences are ignored.
    std::vector<std::wstring> FindIdsInString(std::wstring_view const& inputString)
    {
        // Vector where we maintain the ids found in the input string
        std::vector<std::wstring> idList;

        // Maintains the beginning index of the id found in the input string
        size_t indexIdStart = std::wstring_view::npos;

        // Iterate through each of the characters in the string
        size_t i = 0;
        while (i < inputString.size())
        {
            size_t nextIndex;
            uint32_t codepoint = inputString[i];

            if (UnicodeCharacters::IsHighSurrogate(codepoint))
            {
                // If the character is a high surrogate, then the next characters must be a low surrogate.
                if ((i < inputString.size()) && (UnicodeCharacters::IsLowSurrogate(inputString[i + 1])))
                {
                    // Update the code point with the surrogate pair.
                    codepoint = UnicodeCharacters::GetCodepointFromSurrogatePair(codepoint, inputString[i + 1]);
                    nextIndex = i + 2;
                }
                else
                {
                    // Warning: High surrogate not followed by low surrogate.
                    codepoint = 0;
                    nextIndex = i + 1;
                }
            }
            else
            {
                // Not a surrogate pair.
                nextIndex = i + 1;
            }

            if (indexIdStart == std::wstring_view::npos)
            {
                // Not in an id. Have we found an id start?
                if (UnicodeCharacters::IsIdStart(codepoint))
                {
                    indexIdStart = i;
                }
            }
            else if (!UnicodeCharacters::IsIdContinue(codepoint))
            {
                // We have not found an id continue, so the id is complete.  We need to 
                // create the identifier string
                idList.emplace_back(inputString.substr(indexIdStart, i - indexIdStart));

                // Reset back the index start and re-examine the current code point 
                // in next iteration
                indexIdStart = std::wstring::npos;
                nextIndex = i;
            }
            i = nextIndex;
        }

        // Do we have a pending id at the end of the string?
        if (indexIdStart != std::wstring_view::npos)
        {
            //  We need to create the identifier string
            idList.emplace_back(inputString.substr(indexIdStart, i - indexIdStart));
        }

        // Return the list of identifiers found in the string
        return idList;
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario1_FindId::Scenario1_FindId()
    {
        InitializeComponent();
    }


    void Scenario1_FindId::Default_Click(IInspectable const&, RoutedEventArgs const&)
    {
        std::wstringstream result;
        bool first = true;
        for (auto id : FindIdsInString(TextInput().Text()))
        {
            if (!first)
            {
                result << L", ";
            }
            first = false;
            result << id;
        }

        TextOutput().Text(result.str());
    }
}
