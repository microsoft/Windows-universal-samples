'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System.Text
Imports Windows.Foundation
Imports Windows.Globalization
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario5_TimeZone
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Function ReportCalendarData(calendar As Calendar) As String
            Return String.Format("In {0} time zone:   {1}   {2} {3}, {4}   {5}:{6}:{7} {8}  {9}", calendar.GetTimeZone(), calendar.DayOfWeekAsSoloString(), calendar.MonthAsSoloString(), calendar.DayAsPaddedString(2), calendar.YearAsString(), calendar.HourAsPaddedString(2), calendar.MinuteAsPaddedString(2), calendar.SecondAsPaddedString(2), calendar.PeriodAsString(), calendar.TimeZoneAsString(3))
        End Function

        Private Sub ShowResults()
            ' This scenario illustrates time zone support in Windows.Globalization.Calendar class
            ' Displayed time zones in addition to the local time zone.
            Dim timeZones As String() = {"UTC", "America/New_York", "Asia/Kolkata"}
            ' Store results here.
            Dim results As StringBuilder = New StringBuilder()
            ' Create default Calendar object
            Dim calendar As Calendar = New Calendar()
            Dim localTimeZone As String = calendar.GetTimeZone()
            results.AppendLine("Current date and time:")
            results.AppendLine(ReportCalendarData(calendar))
            For Each timeZone In timeZones
                calendar.ChangeTimeZone(timeZone)
                results.AppendLine(ReportCalendarData(calendar))
            Next

            results.AppendLine()
            calendar.ChangeTimeZone(localTimeZone)
            results.AppendLine("Same time on 14th day of second month of next year:")
            calendar.AddYears(1)
            calendar.Month = 2
            calendar.Day = 14
            results.AppendLine(ReportCalendarData(calendar))
            For Each timeZone In timeZones
                calendar.ChangeTimeZone(timeZone)
                results.AppendLine(ReportCalendarData(calendar))
            Next

            results.AppendLine()
            calendar.ChangeTimeZone(localTimeZone)
            results.AppendLine("Same time on 14th day of tenth month of next year:")
            calendar.AddMonths(8)
            results.AppendLine(ReportCalendarData(calendar))
            For Each timeZone In timeZones
                calendar.ChangeTimeZone(timeZone)
                results.AppendLine(ReportCalendarData(calendar))
            Next

            results.AppendLine()
            OutputTextBlock.Text = results.ToString()
        End Sub
    End Class
End Namespace
