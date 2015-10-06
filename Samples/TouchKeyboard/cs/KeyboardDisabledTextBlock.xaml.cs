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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Automation.Peers;    // needed for FrameworkElementAutomationPeer class

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace SDKTemplate
{
    /// <summary>
    /// Custom Text box control derived from Textbox class.
    /// </summary>
    public class CustomTextBox : TextBox
    {
        public CustomTextBox()
        {
            this.Background = new SolidColorBrush(Windows.UI.Colors.Coral);
            this.BorderThickness = new Thickness(1);
            this.HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Center;
        }
    }

    /// <summary>
    /// A sample custom control displays the date/time when a key is pressed.  This control is for 
    /// illustrative purposes only and does not contain a full implementation of a text control, and it
    /// does not process any text that is entered.
    /// The control turns to yellow when it has input focus, otherwise it is gray.  Lost focus will also 
    /// clear the contents that is in the control.
    /// </summary>
    public sealed partial class KeyboardDisabledTextBlock : UserControl
    {
        /// <summary>
        /// Loads the XAML UI contents and set properties required for this custom control.
        /// </summary>
        public KeyboardDisabledTextBlock()
        {
            this.InitializeComponent();
            this.IsTabStop = true;
            this.IsTapEnabled = true;
        }

        /// <summary>
        /// Create the Automation peer implementations for custom control (CustomInputBox1) to provide the accessibility support.
        /// </summary>
        /// <returns>Automation Peer implementation for this control</returns>
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new KeyboardDisabledTextBlockAutomationPeer(this);
        }

        /// <summary>
        /// Override the default event handler for GotFocus.
        /// When the control got focus, indicate it has focus by highlighting the control by changing the background color to yellow.
        /// </summary>
        /// <param name="e">State information and event data associated with GotFocus event.</param>
        protected override void OnGotFocus(RoutedEventArgs e)
        {
            this.myBorder.Background = new SolidColorBrush(Windows.UI.Colors.Yellow);
        }

        /// <summary>
        /// Override the default event handler for LostFocus.
        /// When the control lost focus, indicate it does not have focus by changing the background color to gray.
        /// And the content is cleared.
        /// </summary>
        /// <param name="e">State information and event data associated with LostFocus event.</param>
        protected override void OnLostFocus(RoutedEventArgs e)
        {
            this.myBorder.Background = new SolidColorBrush(Windows.UI.Colors.Gray);
            this.myTextBlock.Text = "";
        }

        /// <summary>
        /// Override the default event handler for Tapped.
        /// Set input focus to the control when tapped on.
        /// </summary>
        /// <param name="e">State information and event data associated with Tapped event.</param>
        protected override void OnTapped(TappedRoutedEventArgs e)
        {
            this.Focus(Windows.UI.Xaml.FocusState.Pointer);
        }

        /// <summary>
        /// Override the default event handler for KeyDown.
        /// Displays the text "A key is pressed" and the approximate time when the key is pressed.
        /// </summary>
        /// <param name="e">State information and event data associated with KeyDown event.</param>
        protected override void OnKeyDown(KeyRoutedEventArgs e)
        {
            this.myTextBlock.Text = "A key was pressed @ " + DateTime.Now.ToString();
        }
    }

    /// <summary>
    /// Automation Peer class for CustomInputBox1.  
    /// 
    /// Note: The difference between this and CustomControl2AutomationPeer is that this one does not implement 
    /// Text Pattern (ITextProvider) and Value Pattern (IValuePattern) interfaces.  So Touch keyboard does not show 
    /// automatically when user taps on the control with Touch or Pen.
    /// </summary>
    public class KeyboardDisabledTextBlockAutomationPeer : FrameworkElementAutomationPeer
    {
        private KeyboardDisabledTextBlock customInputBox;
        private string accClass = "KeyboardDisabledTextBlock";

        public KeyboardDisabledTextBlockAutomationPeer(KeyboardDisabledTextBlock owner)
            : base(owner)
        {
            this.customInputBox = owner;
        }
        
        /// <summary>
        /// Override GetClassNameCore and set the name of the class that defines the type associated with this control.
        /// </summary>
        /// <returns>The name of the control class</returns>
        protected override string GetClassNameCore()
        {
            return this.accClass;
        }
    }
}
