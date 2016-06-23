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

using System;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.System;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        public Scenario2()
        {
            this.InitializeComponent();
        }

        // returns a rect for selected text
        // if no text is selected, returns caret location
        // textbox should not be empty
        private Rect GetTextboxSelectionRect(TextBox textbox)
        {
            Rect rectFirst, rectLast;
            if (textbox.SelectionStart == textbox.Text.Length)
            {
                rectFirst = textbox.GetRectFromCharacterIndex(textbox.SelectionStart - 1, true);
            }
            else
            {
                rectFirst = textbox.GetRectFromCharacterIndex(textbox.SelectionStart, false);
            }

            int lastIndex = textbox.SelectionStart + textbox.SelectionLength;
            if (lastIndex == textbox.Text.Length)
            {
                rectLast = textbox.GetRectFromCharacterIndex(lastIndex - 1, true);
            }
            else
            {
                rectLast = textbox.GetRectFromCharacterIndex(lastIndex, false);
            }

            rectFirst.Union(rectLast);

            GeneralTransform transform = textbox.TransformToVisual(null);
            return transform.TransformBounds(rectFirst);
        }

        private async void ReadOnlyTextBox_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            e.Handled = true;
            TextBox textbox = (TextBox)sender;
            if (textbox.SelectionLength > 0)
            {
                // Create a menu and add commands specifying an id value for each instead of a delegate.
                var menu = new PopupMenu();
                menu.Commands.Add(new UICommand("Copy", null, 1));
                menu.Commands.Add(new UICommandSeparator());
                menu.Commands.Add(new UICommand("Highlight", null, 2));
                menu.Commands.Add(new UICommand("Look up", null, 3));

                // We don't want to obscure content, so pass in a rectangle representing the selection area.
                // NOTE: this code only handles textboxes with a single line. If a textbox has multiple lines,
                //       then the context menu should be placed at cursor/pointer location by convention.
                OutputTextBlock.Text = "Context menu shown";
                Rect rect = GetTextboxSelectionRect(textbox);
                var chosenCommand = await menu.ShowForSelectionAsync(rect);
                if (chosenCommand != null)
                {
                    switch ((int)chosenCommand.Id)
                    {
                        case 1:
                            String selectedText = ((TextBox)sender).SelectedText;
                            var dataPackage = new DataPackage();
                            dataPackage.SetText(selectedText);
                            Clipboard.SetContent(dataPackage);
                            OutputTextBlock.Text = "'" + chosenCommand.Label + "'(" + chosenCommand.Id.ToString() + ") selected; '" + selectedText + "' copied to clipboard";
                            break;

                        case 2:
                            OutputTextBlock.Text = "'" + chosenCommand.Label + "'(" + chosenCommand.Id.ToString() + ") selected";
                            break;

                        case 3:
                            OutputTextBlock.Text = "'" + chosenCommand.Label + "'(" + chosenCommand.Id.ToString() + ") selected";
                            break;
                    }
                }
                else
                {
                    OutputTextBlock.Text = "Context menu dismissed";
                }
            }
            else
            {
                OutputTextBlock.Text = "Context menu not shown because there is no text selected";
            }
        }
    }
}
