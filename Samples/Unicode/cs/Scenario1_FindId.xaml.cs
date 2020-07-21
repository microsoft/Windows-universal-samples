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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation;
using Windows.Data.Text;
using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace SDKTemplate
{
    public sealed partial class Scenario1_FindId : Page
    {
        public Scenario1_FindId()
        {
            this.InitializeComponent();
        }

        /// <summary>
        // This is a helper method that an app could create to find one or all available 
        // ids within a string.  An id begins with a character for which IsIdStart,
        // and continues with characters that are IsIdContinue. Invalid sequences are ignored.
        /// </summary>
        /// <param name="inputString">String from which to extract ids</param>
        /// <returns>List of individual ids found in the input string</returns>
        private List<String> FindIdsInString(String inputString)
        {
            // List where we maintain the ids found in the input string
            List<String> idList = new List<String>();

            // Maintains the beginning index of the id found in the input string
            int indexIdStart = -1;

            // Iterate through each of the characters in the string
            int i = 0;
            while (i < inputString.Length)
            {
                int nextIndex;
                uint codepoint = inputString[i];         
                
                if (UnicodeCharacters.IsHighSurrogate(codepoint))         
                {
                    // If the character is a high surrogate, then the next characters must be a low surrogate.
                    if ((i < inputString.Length) && (UnicodeCharacters.IsLowSurrogate(inputString[i+1])))
                    {
                        // Update the code point with the surrogate pair.
                        codepoint = UnicodeCharacters.GetCodepointFromSurrogatePair(codepoint, inputString[i+1]);
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
                
                if (indexIdStart == -1)         
                {
                    // Not in an id. Have we found an id start?
                    if (UnicodeCharacters.IsIdStart(codepoint))
                    {
                        indexIdStart = i;
                    }
                }
                else if (!UnicodeCharacters.IsIdContinue(codepoint))         
                {             
                    // We have not found an id continue, so the id is complete.  We need to 
                    // create the identifier string
                    idList.Add(inputString.Substring(indexIdStart, i - indexIdStart));
                    
                    // Reset back the index start and re-examine the current code point 
                    // in next iteration
                    indexIdStart = -1;
                    nextIndex = i;
                }
                i = nextIndex;
            }

            // Do we have a pending id at the end of the string?
            if (indexIdStart != -1)
            {
                //  We need to create the identifier string
                idList.Add(inputString.Substring(indexIdStart, i - indexIdStart));
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
        private void Default_Click(object sender, RoutedEventArgs e)
        {
            var results = FindIdsInString(TextInput.Text);
            TextOutput.Text = String.Join(", ", results);
        }

    }
}
