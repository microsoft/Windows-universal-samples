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

//
// FindId.xaml.cpp
// Implementation of the FindId class
//

#include "pch.h"
#include "Scenario1_FindId.xaml.h"
#include <sstream>


using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Data::Text;

Scenario1_FindId::Scenario1_FindId()
{
    InitializeComponent();
    TextInput->Text =
        L"Hello, how are you?  I hope you are ok!\n"
        L"-->id<--\n"
        L"1id 2id 3id\n"
        L"id1 id2 id3\n"
        L"\xD840\xDC00_CJK_B_1 \xD840\xDC01_CJK_B_2 \xD840\xDC02_CJK_B_3";

}

/// <summary>
// This is a helper method that an app could create to find one or all available 
// ids within a string. 
/// </summary>
/// <param name="inputString">String that contains one or more ids</param>
/// <returns>List of individual ids found in the input string</returns>
std::vector<std::wstring> Scenario1_FindId::FindIdsInString(String^ inputString)
{
    // List where we maintain the ids found in the input string
    std::vector<std::wstring> idList;

    // Maintains the beginning the id found in the input string
    const wchar_t * idStart = nullptr;

    // Iterate through each of the characters in the string
    for (const wchar_t* character = inputString->Begin(); character < inputString->End(); character++)
    {
        unsigned int codepoint = *character;         
                
        // If the character is a high surrogate, then we need to read the next character to make
        // sure it is a low surrogate.  If we are at the last character in the input string, then
        // we have an error, since a high surrogate must be matched by a low surrogate.  Update
        // the code point with the surrogate pair.
        if (UnicodeCharacters::IsHighSurrogate(codepoint)) 
        {
            if (++character >= inputString->End()) 
            {
                throw ref new Platform::Exception(E_INVALIDARG, "Bad trailing surrogate at end of string");
            }

            codepoint = UnicodeCharacters::GetCodepointFromSurrogatePair(*(character -1), *character);
        }
                
        // Have we found an id start?
        if (idStart == nullptr) 
        {
            if (UnicodeCharacters::IsIdStart(codepoint)) 
            {
                // We found a character that is an id start.  In case we had a suplemmentary
                // character (high and low surrogate), then the index needs to offset by 1.
                idStart = UnicodeCharacters::IsSupplementary(codepoint) ?  character - 1 : character;             
            }         
        }         
        else if (!UnicodeCharacters::IsIdContinue(codepoint)) 
        {             
            // We have not found an id continue, so the id is complete.  We need to 
            // create the identifier string
            idList.push_back(std::wstring(idStart, character - idStart));
                    
            // Reset back the index start and re-examine the current code point 
            // in next iteration
            idStart = nullptr;
            character--;
        }     
    }

    // Do we have a pending id at the end of the string?
    if (idStart != nullptr) {

        //  We need to create the identifier string
        idList.push_back(std::wstring(idStart));
    }

    // Return the list of identifiers found in the string
    return idList;
}

/// <summary>
/// This is the click handler for the 'Extract Identifiers' button.
/// It relies on the FindIdsInString method to implement the logic.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_FindId::Default_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    std::wstringstream result;
    for (auto id : FindIdsInString(TextInput->Text))
    {
        result << id << L" ";
    }

    TextOutput->Text = ref new String(result.str().c_str());
}
