#pragma once

enum LogEventTypes
{
    EventNone                   = 0x0,
    InkingEventType             = 0x1,
    IndependentInputEventType   = 0x2,
    UnprocessedInputEventType   = 0x4,
    StrokeEventType             = 0x8
};

enum LogEventAction
{
    StrokeCollected,
    StrokeErased,
    IndependentInputEntered,
    IndependentInputHover,
    IndependentInputPressed,
    IndependentInputMoved,
    IndependentInputReleased,
    IndependentInputLost,
    IndependentInputExited,
    UnprocessedInputEntered,
    UnprocessedInputHover,
    UnprocessedInputPressed,
    UnprocessedInputMoved,
    UnprocessedInputReleased,
    UnprocessedInputLost,
    UnprocessedInputExited,
    StrokeStarted,
    StrokeContinued,
    StrokeEnded,
    StrokeCanceled
};


const std::wstring LogEventStrings[]
{
    L"Stroke Collected",
    L"Stroke Erased",
    L"An inking input is entering into the boundary of the inking area, before InkPresenter processes event",
    L"Independent input pointer hovering",
    L"Independent input pointer pressing",
    L"Independent input pointer moved",
    L"Independent input pointer released",
    L"An inking input has lost contact with the inking area without being lifted, before InkPresenter processes event",
    L"An inking input is leaving the boundary of the inking area, before InkPresenter processes event",
    L"An input has entered into the boundary of the inking area",
    L"An input has moved while hovering within the boundary of the inking area",
    L"Unprocessed input pressed",
    L"Unprocessed input moved",
    L"Unprocessed input released",
    L"An input was in contact with the inking area and is now lost without being lifted",
    L"An input has left the boundary of the inking area",
    L"An inking input has come into contact with the inking area and inking started",
    L"An inking input has come into contact with the inking area and continued to ink",
    L"An inking input has lifted from the inking area and inking ended",
    L"An inking input was in contact with the inking area and is now lost without being lifted, existing stroke cancelled"
};
