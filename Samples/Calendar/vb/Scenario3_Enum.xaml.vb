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
Imports System
Imports System.Text
Imports Windows.Foundation
Imports Windows.Globalization
Imports Windows.Globalization.DateTimeFormatting
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Partial Public NotInheritable Class Scenario3_Enum
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Sub ShowResults()
            ' This scenario uses the Windows.Globalization.Calendar class to enumerate through a calendar and
            ' perform calendar math
            Dim results As New StringBuilder()
            results.AppendLine("The number of years in each era of the Japanese era calendar is not regular. " & "It is determined by the length of the given imperial era:")
            results.AppendLine()
            ' Create Japanese calendar.
            Dim calendar As New Calendar({"en-US"}, CalendarIdentifiers.Japanese, ClockIdentifiers.TwentyFourHour)
            Do
                results.AppendLine("Era " & calendar.EraAsString() & " contains " & calendar.NumberOfYearsInThisEra & " year(s)")
                Do
                    calendar.Month = calendar.FirstMonthInThisYear
                    calendar.Day = calendar.FirstDayInThisMonth
                    calendar.Period = calendar.FirstPeriodInThisDay
                    calendar.Hour = calendar.FirstHourInThisPeriod
                    calendar.Minute = 0
                    calendar.Second = 0
                    calendar.Nanosecond = 0
                    If calendar.Year Mod 1000 = 0 Then
                        results.AppendLine()
                    ElseIf calendar.Year Mod 10 = 0 Then
                        results.Append(".")
                    End If

                    If calendar.Year = calendar.LastYearInThisEra Then
                        Exit Do
                    End If

                    calendar.AddYears(1)
                Loop

                results.AppendLine()
                If calendar.Era = calendar.LastEra Then
                    Exit Do
                End If

                calendar.AddYears(1)
            Loop

            results.AppendLine()
            results.AppendLine("The number of hours in a day is not constant. " & "The US calendar transitions from daylight saving time to standard time on 4 November 2012:" & vbLf)
            ' Create a DateTimeFormatter to display dates
            Dim displayDate As DateTimeFormatter = New DateTimeFormatter("longdate")
            ' Create a gregorian calendar for the US with 12-hour clock format
            Dim currentCal As Calendar = New Windows.Globalization.Calendar(New String() {"en-US"}, CalendarIdentifiers.Gregorian, ClockIdentifiers.TwentyFourHour, "America/Los_Angeles")
            ' Set the calendar to a the date of the Daylight Saving Time-to-Standard Time transition for the US in 2012.
            ' DST ends in the America/Los_Angeles time zone at 4 November 2012 02:00 PDT = 4 November 2012 09:00 UTC.
            Dim dstDate As DateTime = New DateTime(2012, 11, 4, 9, 0, 0, DateTimeKind.Utc)
            currentCal.SetDateTime(dstDate)
            ' Set the current calendar to one day before DST change. Create a second calendar for comparision and set it to one day after DST change.
            Dim endDate As Calendar = currentCal.Clone()
            currentCal.AddDays(-1)
            endDate.AddDays(1)
            While currentCal.Day <= endDate.Day
                ' Process current day.
                Dim [date] As DateTimeOffset = currentCal.GetDateTime()
                results.AppendFormat("{0} contains {1} hour(s)" & vbLf, displayDate.Format([date]), currentCal.NumberOfHoursInThisPeriod)
                ' Enumerate all hours in this day.
                ' Create a calendar to represent the following day.
                Dim nextDay As Calendar = currentCal.Clone()
                nextDay.AddDays(1)
                Do
                    results.AppendFormat("{0} ", currentCal.HourAsPaddedString(2))
                    If currentCal.Day = nextDay.Day AndAlso currentCal.Period = nextDay.Period Then
                        Exit Do
                    End If

                    currentCal.AddHours(1)
                Loop

                results.AppendLine()
            End While

            OutputTextBlock.Text = results.ToString()
        End Sub
    End Class
End Namespace
