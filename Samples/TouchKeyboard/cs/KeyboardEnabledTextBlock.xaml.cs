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
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Automation;           // needed for enum SupportedTextSelection in ITextProvider
using Windows.UI.Xaml.Automation.Text;      // needed for TextPatternRangePoint
using Windows.UI.Xaml.Automation.Peers;     // needed for FrameworkElementAutomationPeer class
using Windows.UI.Xaml.Automation.Provider;  // needed for ITextProvider and IValueProvider

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace SDKTemplate
{
    /// <summary>
    /// A sample custom control displays the date/time when a key is pressed.  This control is for 
    /// illustrative purposes only and does not contain a full implementation of a text control, and it
    /// does not process any text that is entered.
    /// The control turns to yellow when it has input focus, otherwise it is gray.  Lost focus will also 
    /// clear the contents that is in the control.
    /// </summary>
    public sealed partial class KeyboardEnabledTextBlock : UserControl
    {
        internal String ContentText { get; set; }

        /// <summary>
        /// Loads the XAML UI contents and set properties required for this custom control.
        /// </summary>
        public KeyboardEnabledTextBlock()
        {
            this.InitializeComponent();
            this.IsTabStop = true;
            this.IsTapEnabled = true;
            this.ContentText = "";
        }

        /// <summary>
        /// Create the Automation peer implementations for custom control (CustomInputBox2) to provide the accessibility support.
        /// </summary>
        /// <returns>Automation Peer implementation for this control</returns>
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new KeyboardEnabledTextBlockAutomationPeer(this);
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
            ContentText = "";
            this.myTextBlock.Text = ContentText;
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
            ContentText = "A key was pressed @ " + DateTime.Now.ToString();
            this.myTextBlock.Text = ContentText;
        }
    }

    /// <summary>
    /// Automation Peer class for CustomInputBox2.  
    /// 
    /// Note: The difference between this and CustomControl1AutomationPeer is that this one implements
    /// Text Pattern (ITextProvider) and Value Pattern (IValuePattern) interfaces.  So Touch keyboard shows 
    /// automatically when user taps on the control with Touch or Pen.
    /// </summary>
    public sealed class KeyboardEnabledTextBlockAutomationPeer : FrameworkElementAutomationPeer, ITextProvider, IValueProvider
    {
        private KeyboardEnabledTextBlock customInputBox;
        private string accClass = "KeyboardEnabledTextBlock";

        /// <summary>
        /// 
        /// </summary>
        /// <param name="owner"></param>
        public KeyboardEnabledTextBlockAutomationPeer(KeyboardEnabledTextBlock owner)
            : base(owner)
        {
            this.customInputBox = owner;
        }

        /// <summary>
        /// Override GetPatternCore to return the object that supports the specified pattern.  In this case the Value pattern, Text
        /// patter and any base class patterns.
        /// </summary>
        /// <param name="patternInterface"></param>
        /// <returns>the object that supports the specified pattern</returns>
        protected override object GetPatternCore(PatternInterface patternInterface)
        {
            if (patternInterface == PatternInterface.Value)
            {
                return this;
            }
            else if (patternInterface == PatternInterface.Text)
            {
                return this;
            }
            return base.GetPatternCore(patternInterface);
        }

        /// <summary>
        /// Override GetClassNameCore and set the name of the class that defines the type associated with this control.
        /// </summary>
        /// <returns>The name of the control class</returns>
        protected override string GetClassNameCore()
        {
            return this.accClass;
        }

        #region Implementation for ITextPattern interface
        // Complete implementation of the ITextPattern is beyond the scope of this sample.  The implementation provided
        // is specific to this sample's custom control, so it is unlikely that they are directly transferable to other 
        // custom control.

        ITextRangeProvider ITextProvider.DocumentRange
        {
            // A real implementation of this method is beyond the scope of this sample.
            // If your custom control has complex text involving both readonly and non-readonly ranges, 
            // it will need a smarter implementation than just returning a fixed range
            get { return new CustomControl2RangeProvider(customInputBox.ContentText, this); ; }
        }

        ITextRangeProvider[] ITextProvider.GetSelection()
        {
            return new ITextRangeProvider[0];
        }

        ITextRangeProvider[] ITextProvider.GetVisibleRanges()
        {
            ITextRangeProvider[] ret = new ITextRangeProvider[1];
            ret[0] = new CustomControl2RangeProvider(customInputBox.ContentText, this);
            return ret;
        }

        ITextRangeProvider ITextProvider.RangeFromChild(IRawElementProviderSimple childElement)
        {
            return new CustomControl2RangeProvider(customInputBox.ContentText, this);
        }

        ITextRangeProvider ITextProvider.RangeFromPoint(Point screenLocation)
        {
            return new CustomControl2RangeProvider(customInputBox.ContentText, this);
        }

        SupportedTextSelection ITextProvider.SupportedTextSelection
        {
            get { return SupportedTextSelection.Single; }
        }

        #endregion

        #region Implementation for IValueProvider interface
        // Complete implementation of the IValueProvider is beyond the scope of this sample.  The implementation provided
        // is specific to this sample's custom control, so it is unlikely that they are directly transferable to other 
        // custom control.

        /// <summary>
        /// The value needs to be false for the Touch keyboard to be launched automatically because Touch keyboard
        /// does not appear when the input focus is in a readonly UI control.
        /// </summary>
        bool IValueProvider.IsReadOnly
        {
            get { return false; }
        }

        void IValueProvider.SetValue(string value)
        {
            customInputBox.ContentText = value;
        }

        string IValueProvider.Value
        {
            get { return customInputBox.ContentText; }
        }

        #endregion //Implementation for IValueProvider interface

        public IRawElementProviderSimple GetRawElementProviderSimple()
        {
            return ProviderFromPeer(this);
        }
    }

    /// <summary>
    /// A minimal implementation of ITextRangeProvider, used by CustomControl2AutomationPeer
    /// A real implementation is beyond the scope of this sample
    /// </summary>
    public sealed class CustomControl2RangeProvider : ITextRangeProvider
    {
        private String _text;
        private KeyboardEnabledTextBlockAutomationPeer _peer;

        public CustomControl2RangeProvider(String text, KeyboardEnabledTextBlockAutomationPeer peer)
        {
            _text = text;
            _peer = peer;
        }

        public void AddToSelection()
        {

        }

        public ITextRangeProvider Clone()
        {
            return new CustomControl2RangeProvider(_text, _peer);
        }

        public bool Compare(ITextRangeProvider other)
        {
            return true;
        }

        public int CompareEndpoints(TextPatternRangeEndpoint endpoint, ITextRangeProvider targetRange, TextPatternRangeEndpoint targetEndpoint)
        {
            return 0;
        }

        public void ExpandToEnclosingUnit(TextUnit unit)
        {

        }

        public ITextRangeProvider FindAttribute(int attribute, Object value, bool backward)
        {
            return this;
        }

        public ITextRangeProvider FindText(String text, bool backward, bool ignoreCase)
        {
            return this;
        }

        public Object GetAttributeValue(int attribute)
        {
            return this;
        }

        public void GetBoundingRectangles(out double[] rectangles)
        {
            rectangles = new double[0];
        }

        public IRawElementProviderSimple[] GetChildren()
        {
            return new IRawElementProviderSimple[0];
        }

        public IRawElementProviderSimple GetEnclosingElement()
        {
            return _peer.GetRawElementProviderSimple();
        }

        public String GetText(int maxLength)
        {
            return (maxLength < 0) ? _text : _text.Substring(0, Math.Min(_text.Length, maxLength));
        }

        public int Move(TextUnit unit, int count)
        {
            return 0;
        }

        public void MoveEndpointByRange(TextPatternRangeEndpoint endpoint, ITextRangeProvider targetRange, TextPatternRangeEndpoint targetEndpoint)
        {

        }

        public int MoveEndpointByUnit(TextPatternRangeEndpoint endpoint, TextUnit unit, int count)
        {
            return 0;
        }

        public void RemoveFromSelection()
        {

        }

        public void ScrollIntoView(bool alignToTop)
        {

        }

        public void Select()
        {

        }
    }


}