// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "InkEventLogger.h"


InkEventLogger::InkEventLogger()
{
    _currentConfig = LogEventTypes::EventNone;
    _countOfCoalescedIndependentInput = 0;
    _countOfCoalescedStrokeMessages = 0;
    _countOfCoalescedUnprocessedInput = 0;
}

void InkEventLogger::SetConfiguration(LogEventTypes config)
{
    _currentConfig = config;
}

void InkEventLogger::EnableLoggingConfiguration(LogEventTypes type, bool addConfig)
{
    if (addConfig)
    {
        _currentConfig = (LogEventTypes)(_currentConfig | type);
    }
    else
    {
        _currentConfig = (LogEventTypes)(_currentConfig & ~(type));
    }
}

void InkEventLogger::Log(LogEventAction eventAction)
{
    std::wstring newLogEntry;
    std::wstring messageCount;

    // Filter events
    if ((eventAction >= StrokeCollected && eventAction <= StrokeErased))
    {
        if ((_currentConfig & LogEventTypes::InkingEventType) != LogEventTypes::InkingEventType)
        {
            return;
        }
    }
    else if ((eventAction >= IndependentInputEntered && eventAction <= IndependentInputExited))
    {
        if ((_currentConfig & LogEventTypes::IndependentInputEventType) != LogEventTypes::IndependentInputEventType)
        {
            return;
        }
    }
    else if ((eventAction >= UnprocessedInputEntered && eventAction <= UnprocessedInputExited))
    {
        if ((_currentConfig & LogEventTypes::UnprocessedInputEventType) != LogEventTypes::UnprocessedInputEventType)
        {
            return;
        }
    }
    else if ((eventAction >= StrokeStarted && eventAction <= StrokeCanceled))
    {
        if ((_currentConfig & LogEventTypes::StrokeEventType) != LogEventTypes::StrokeEventType)
        {
            return;
        }
    }
    else
    {
        return;
    }


    switch (eventAction)
    {
    case LogEventAction::IndependentInputEntered:
        newLogEntry = L"Independent Input - Entered";
        _countOfCoalescedIndependentInput = 0;
        break;
    case LogEventAction::IndependentInputExited:
        newLogEntry = L"Independent Input - Exited";
        if (_countOfCoalescedIndependentInput > 0)
        {
            // Coalescing hover and mouse events to avoid excessive logging
            // For hover, count the number of hover messages and display them
            // only when the stylus exits or the sytlus is in contact with the
            // screen.
            messageCount = _countOfCoalescedIndependentInput.ToString()->Data();
            newLogEntry += L"\nIndependent Input - Hover (" + messageCount + L")";
            _countOfCoalescedIndependentInput = 0;
        }
        break;
    case LogEventAction::IndependentInputHover:
        _countOfCoalescedIndependentInput++;
        break;
    case LogEventAction::IndependentInputLost:
        newLogEntry = L"Independent Input - Lost";
        _countOfCoalescedIndependentInput = 0;
        break;
    case LogEventAction::IndependentInputMoved:
        _countOfCoalescedIndependentInput++;
        break;
    case LogEventAction::IndependentInputPressed:
        newLogEntry = L"Independent Input - Pressed";
        if (_countOfCoalescedIndependentInput > 0)
        {
            // This means pen came in range and now it 
            // starts inking
            messageCount = _countOfCoalescedIndependentInput.ToString()->Data();
            newLogEntry += L"\nIndependent Input - Hover (" + messageCount + L")";
            _countOfCoalescedUnprocessedInput = 0;
        }
        break;
    case LogEventAction::IndependentInputReleased:
        newLogEntry = L"Independent Input - Released";
        if (_countOfCoalescedIndependentInput > 0)
        {
            messageCount = _countOfCoalescedIndependentInput.ToString()->Data();
            newLogEntry += L"\nIndependent Input - Moved (" + messageCount + L")";
            _countOfCoalescedIndependentInput = 0;
        }
        break;
    case LogEventAction::UnprocessedInputEntered:
        newLogEntry = L"Unprocessed Input - Entered";
        _countOfCoalescedUnprocessedInput = 0;
        break;
    case LogEventAction::UnprocessedInputExited:
        newLogEntry = L"Unprocessed Input - Exited";
        if (_countOfCoalescedUnprocessedInput > 0)
        {
            messageCount = _countOfCoalescedUnprocessedInput.ToString()->Data();
            newLogEntry += L"\nUnprocessed Input - Hover (" + messageCount + L")";
            _countOfCoalescedUnprocessedInput = 0;
        }
        break;
    case LogEventAction::UnprocessedInputHover:
        _countOfCoalescedUnprocessedInput++;
        break;
    case LogEventAction::UnprocessedInputLost:
        newLogEntry = L"Unprocessed Input - Lost";
        _countOfCoalescedUnprocessedInput = 0;
        break;
    case LogEventAction::UnprocessedInputMoved:
        _countOfCoalescedUnprocessedInput++;
        break;
    case LogEventAction::UnprocessedInputPressed:
        newLogEntry = L"Unprocessed Input - Pressed";
        newLogEntry = L"Unprocessed Input - Exited";
        if (_countOfCoalescedUnprocessedInput > 0)
        {
            messageCount = _countOfCoalescedUnprocessedInput.ToString()->Data();
            newLogEntry += L"\nUnprocessed Input - Hover (" + messageCount + L")";
            _countOfCoalescedUnprocessedInput = 0;
        }
        break;
    case LogEventAction::UnprocessedInputReleased:
        newLogEntry = L"Unprocessed Input - Released";
        if (_countOfCoalescedUnprocessedInput > 0)
        {
            messageCount = _countOfCoalescedUnprocessedInput.ToString()->Data();
            newLogEntry += L"\nUnprocessed Input - Moved (" + messageCount + L")";
            _countOfCoalescedUnprocessedInput = 0;
        }
        break;
    case LogEventAction::StrokeCanceled:
        newLogEntry = L"Stroke Canceled";
        _countOfCoalescedStrokeMessages = 0;
        break;
    case LogEventAction::StrokeCollected:
        newLogEntry = L"Stroke Collected";
        break;
    case LogEventAction::StrokeContinued:
        _countOfCoalescedStrokeMessages++;
        break;
    case LogEventAction::StrokeEnded:
        newLogEntry = L"Stroke Ended";
        if (_countOfCoalescedStrokeMessages > 0)
        {
            messageCount = _countOfCoalescedStrokeMessages.ToString()->Data();
            newLogEntry += L"\nStroke Continued (" + messageCount + L")";
            _countOfCoalescedStrokeMessages = 0;
        }
        break;
    case LogEventAction::StrokeErased:
        newLogEntry = L"Stroke Erased";
        break;
    case LogEventAction::StrokeStarted:
        newLogEntry = L"Stroke Started";
        _countOfCoalescedStrokeMessages = 0;
        break;
    default:
        newLogEntry = L"";
        break;
    }

    if (newLogEntry != L"")
    {
        _log = newLogEntry + L"\n" + _log;
    }
}