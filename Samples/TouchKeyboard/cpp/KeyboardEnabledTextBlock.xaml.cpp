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
// KeyboardEnabledTextBlock.xaml.cpp
// Implementation of the KeyboardEnabledTextBlock class
//

#include "pch.h"
#include "KeyboardEnabledTextBlock.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Automation;            // needed for enum SupportedTextSelection in ITextProvider
using namespace Windows::UI::Xaml::Automation::Text;      // needed for TextPatternRangeEndpoint
using namespace Windows::UI::Xaml::Automation::Peers;     // needed for FrameworkElementAutomationPeer class
using namespace Windows::UI::Xaml::Automation::Provider;  // needed for ITextProvider and IValueProvider

namespace SDKTemplate
{
    /// <summary>
    /// Automation Peer class for KeyboardEnabledTextBlock.  
    /// 
    /// Note: The difference between this and NonKeyboardEnabledTextBlockAutomationPeer is that this one implements
    /// Text Pattern (ITextProvider) and Value Pattern (IValuePattern) interfaces.  So Touch keyboard shows 
    /// automatically when user taps on the control with Touch or Pen.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class KeyboardEnabledTextBlockAutomationPeer sealed : FrameworkElementAutomationPeer, ITextProvider, IValueProvider
    {
    public:

        KeyboardEnabledTextBlockAutomationPeer(KeyboardEnabledTextBlock^ owner)
            : FrameworkElementAutomationPeer(owner)
        {
            this->textBlock = owner;
            this->accClass = "KeyboardEnabledTextBlock";
        }
    protected:
        /// <summary>
        /// Override GetPatternCore to return the object that supports the specified pattern.  In this case the Value pattern, Text
        /// patter and any base class patterns.
        /// </summary>
        /// <param name="patternInterface"></param>
        /// <returns>the object that supports the specified pattern</returns>
        virtual Object^ GetPatternCore(PatternInterface patternInterface) override
        {
            if (patternInterface == PatternInterface::Value)
            {
                return this;
            }
            else if (patternInterface == PatternInterface::Text)
            {
                return this;
            }
            return FrameworkElementAutomationPeer::GetPatternCore(patternInterface);
        }

        /// <summary>
        /// Override GetClassNameCore and set the name of the class that defines the type associated with this control.
        /// </summary>
        /// <returns>The name of the control class</returns>
        virtual String^ GetClassNameCore() override
        {
            return this->accClass;
        }
    public:
#pragma region ITextProvider_Implementation
        // Complete implementation of the ITextPattern is beyond the scope of this sample.  The implementation provided
        // is specific to this sample's custom control, so it is unlikely that they are directly transferable to other 
        // custom control.

        property ITextRangeProvider^ DocumentRange
        {
            // A real implementation of this method is beyond the scope of this sample.
            // If your custom control has complex text involving both readonly and non-readonly ranges, 
            // it will need a smarter implementation than just returning a fixed range
            virtual ITextRangeProvider^ get() { return ref new KeyboardEnabledTextBlockRangeProvider(textBlock->ContentText, this); }
        }

        virtual Array<ITextRangeProvider^>^ GetSelection()
        {
            return ref new Array<ITextRangeProvider^>(0);
        }

        virtual Array<ITextRangeProvider^>^ GetVisibleRanges()
        {
            auto ret = ref new Array<ITextRangeProvider^>(1);
            ret[0] = ref new KeyboardEnabledTextBlockRangeProvider(textBlock->ContentText, this);
            return ret;
        }

        virtual ITextRangeProvider^ RangeFromChild(IRawElementProviderSimple^ childElement)
        {
            return ref new KeyboardEnabledTextBlockRangeProvider(textBlock->ContentText, this);
        }

        virtual ITextRangeProvider^ RangeFromPoint(Point screenLocation)
        {
            return ref new KeyboardEnabledTextBlockRangeProvider(textBlock->ContentText, this);
        }

        property SupportedTextSelection SupportedTextSelection
        {
            virtual Windows::UI::Xaml::Automation::SupportedTextSelection get() { return Windows::UI::Xaml::Automation::SupportedTextSelection::Single; }
        }

#pragma endregion ITextProvider_Implementation

#pragma region IValueProvider_Implementation
        // Complete implementation of the IValueProvider is beyond the scope of this sample.  The implementation provided
        // is specific to this sample's custom control, so it is unlikely that they are directly transferable to other 
        // custom control.

        /// <summary>
        /// The value needs to be false for the Touch keyboard to be launched automatically because Touch keyboard
        /// does not appear when the input focus is in a readonly UI control.
        /// </summary>
        property bool IsReadOnly
        {
            virtual bool get() { return false; }
        }

        virtual void SetValue(String^ value)
        {
            textBlock->ContentText = value;
        }

        property String^ Value
        {
            virtual String^ get() { return textBlock->ContentText; }
        }

#pragma endregion IValueProvider_Implementation

        IRawElementProviderSimple^ GetRawElementProviderSimple()
        {
            return ProviderFromPeer(this);
        }

    private:
        KeyboardEnabledTextBlock^ textBlock;
        String^ accClass;
    };

}

KeyboardEnabledTextBlockRangeProvider::KeyboardEnabledTextBlockRangeProvider(String^ text, KeyboardEnabledTextBlockAutomationPeer^ peer) : _text(text), _peer(peer)
{

}

void KeyboardEnabledTextBlockRangeProvider::AddToSelection()
{

}

ITextRangeProvider^ KeyboardEnabledTextBlockRangeProvider::Clone()
{
    return ref new KeyboardEnabledTextBlockRangeProvider(_text, _peer);
}

bool KeyboardEnabledTextBlockRangeProvider::Compare(ITextRangeProvider^ other)
{
    return true;
}

int KeyboardEnabledTextBlockRangeProvider::CompareEndpoints(TextPatternRangeEndpoint endpoint, ITextRangeProvider^ targetRange, TextPatternRangeEndpoint targetEndpoint)
{
    return 0;
}

void KeyboardEnabledTextBlockRangeProvider::ExpandToEnclosingUnit(TextUnit unit)
{

}

ITextRangeProvider^ KeyboardEnabledTextBlockRangeProvider::FindAttribute(int attribute, Object^ value, bool backward)
{
    return this;
}

ITextRangeProvider^ KeyboardEnabledTextBlockRangeProvider::FindText(String^ text, bool backward, bool ignoreCase)
{
    return this;
}

Object^ KeyboardEnabledTextBlockRangeProvider::GetAttributeValue(int attribute)
{
    return this;
}

void KeyboardEnabledTextBlockRangeProvider::GetBoundingRectangles(Array<double>^* rectangles)
{
    *rectangles = ref new Array<double>(0);
}

Array<IRawElementProviderSimple^>^ KeyboardEnabledTextBlockRangeProvider::GetChildren()
{
    return ref new Array<IRawElementProviderSimple^>(0);
}

IRawElementProviderSimple^ KeyboardEnabledTextBlockRangeProvider::GetEnclosingElement()
{
    return _peer->GetRawElementProviderSimple();
}

String^ KeyboardEnabledTextBlockRangeProvider::GetText(int maxLength)
{
    auto textLength = _text->End() - _text->Begin();
    auto retLength = (maxLength < 0) ? textLength : min(maxLength, textLength);
    return ref new String(_text->Data(), static_cast<unsigned int>(retLength));
}

int KeyboardEnabledTextBlockRangeProvider::Move(TextUnit unit, int count)
{
    return 0;
}

void KeyboardEnabledTextBlockRangeProvider::MoveEndpointByRange(TextPatternRangeEndpoint endpoint, ITextRangeProvider^ targetRange, TextPatternRangeEndpoint targetEndpoint)
{

}

int KeyboardEnabledTextBlockRangeProvider::MoveEndpointByUnit(TextPatternRangeEndpoint endpoint, TextUnit unit, int count)
{
    return 0;
}

void KeyboardEnabledTextBlockRangeProvider::RemoveFromSelection()
{

}

void KeyboardEnabledTextBlockRangeProvider::ScrollIntoView(bool alignToTop)
{

}

void KeyboardEnabledTextBlockRangeProvider::Select()
{

}

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

/// <summary>
/// Loads the XAML UI contents and set properties required for this custom control.
/// </summary>
KeyboardEnabledTextBlock::KeyboardEnabledTextBlock()
{
    InitializeComponent();
    this->IsTabStop = true;
    this->IsTapEnabled = true;
    this->contentText = "";
}

/// <summary>
/// Create the Automation peer implementations for custom control (CustomInputBox2) to provide the accessibility support.
/// </summary>
/// <returns>Automation Peer implementation for this control</returns>
AutomationPeer^ KeyboardEnabledTextBlock::OnCreateAutomationPeer()
{
    return ref new KeyboardEnabledTextBlockAutomationPeer(this);
}

/// <summary>
/// Override the default event handler for GotFocus.
/// When the control got focus, indicate it has focus by highlighting the control by changing the background color to yellow.
/// </summary>
/// <param name="e">State information and event data associated with GotFocus event.</param>
void KeyboardEnabledTextBlock::OnGotFocus(RoutedEventArgs^ e)
{
    this->myBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Yellow);
}

/// <summary>
/// Override the default event handler for LostFocus.
/// When the control lost focus, indicate it does not have focus by changing the background color to gray.
/// And the content is cleared.
/// </summary>
/// <param name="e">State information and event data associated with LostFocus event.</param>
void KeyboardEnabledTextBlock::OnLostFocus(RoutedEventArgs^ e)
{
    this->myBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Gray);
    contentText = "";
    this->myTextBlock->Text = contentText;
}

/// <summary>
/// Override the default event handler for Tapped.
/// Set input focus to the control when tapped on.
/// </summary>
/// <param name="e">State information and event data associated with Tapped event.</param>
void KeyboardEnabledTextBlock::OnTapped(TappedRoutedEventArgs^ e)
{
    this->Focus(Windows::UI::Xaml::FocusState::Pointer);
}

/// <summary>
/// Override the default event handler for KeyDown.
/// Displays the text "A key is pressed" and the approximate time when the key is pressed.
/// </summary>
/// <param name="e">State information and event data associated with KeyDown event.</param>
void KeyboardEnabledTextBlock::OnKeyDown(KeyRoutedEventArgs^ e)
{
    auto currentCalendar = ref new Windows::Globalization::Calendar();
    String^ timeOfDay = currentCalendar->HourAsPaddedString(2) + ":" +
        currentCalendar->MinuteAsPaddedString(2) + ":" +
        currentCalendar->SecondAsPaddedString(2);
    contentText = "A key was pressed @ " + timeOfDay + " ";
    this->myTextBlock->Text = contentText;
}