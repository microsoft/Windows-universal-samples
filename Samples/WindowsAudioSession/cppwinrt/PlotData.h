//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "PlotDataReadyEventArgs.g.h"

namespace winrt::SDKTemplate::implementation
{
    // Class for PlotDataReady events
    struct PlotDataReadyEventArgs : PlotDataReadyEventArgsT<PlotDataReadyEventArgs>
    {
        PlotDataReadyEventArgs(Windows::Storage::Streams::IBuffer const& buffer) : m_buffer(buffer) {}

        auto Points() { return m_buffer; }

    private:
        Windows::Storage::Streams::IBuffer const m_buffer;
    };

    template<typename D>
    struct PlotDataSourceT
    {
        event_token PlotDataReady(Windows::Foundation::TypedEventHandler<IPlotDataSource, SDKTemplate::PlotDataReadyEventArgs> const& handler)
        {
            return m_plotDataReady.add(handler);
        }
        void PlotDataReady(event_token token)
        {
            return m_plotDataReady.remove(token);
        }

    protected:
        event<Windows::Foundation::TypedEventHandler<IPlotDataSource, SDKTemplate::PlotDataReadyEventArgs>> m_plotDataReady;

        void ReportPlotDataReady(Windows::Storage::Streams::IBuffer const& buffer)
        {
            m_plotDataReady(*static_cast<D*>(this), make<implementation::PlotDataReadyEventArgs>(buffer));
        }
    };
}
