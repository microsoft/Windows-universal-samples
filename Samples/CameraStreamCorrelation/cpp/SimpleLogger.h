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

#pragma once

namespace SDKTemplate
{
    private ref class SimpleLogger sealed
    {
    public:
        SimpleLogger(Windows::UI::Xaml::Controls::TextBlock^ textBlock)
        {
            m_textBlock = textBlock;
            m_dispatcher = textBlock->Dispatcher;
        }

        void Log(Platform::String^ message)
        {
            auto calendar = ref new Windows::Globalization::Calendar();
            auto formatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("hour minute second");

            LONG messageNumber = InterlockedIncrement(&m_messageCount);
            Platform::String^ newMessage = "[" + messageNumber.ToString() + "] " +
                formatter->Format(calendar->GetDateTime()) + " : " + message;

            m_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
                ref new Windows::UI::Core::DispatchedHandler(
                    [this, newMessage]()
            {
                m_textBlock->Text = newMessage + "\r\n" + m_textBlock->Text;
            }));
        }

    private:
        LONG m_messageCount = 0;
        Windows::UI::Xaml::Controls::TextBlock^ m_textBlock;
        Windows::UI::Core::CoreDispatcher^ m_dispatcher;
    };

} // SDKTemplate