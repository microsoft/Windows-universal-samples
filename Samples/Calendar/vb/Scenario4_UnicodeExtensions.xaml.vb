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

    Public NotInheritable Partial Class Scenario4_UnicodeExtensions
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Function ReportCalendarData(calendar As Calendar, calendarLabel As String) As String
            Dim results As String = calendarLabel & ": " & calendar.GetCalendarSystem() & vbLf
            results &= "Calendar system: " & calendar.GetCalendarSystem() & vbLf
            results &= "Numeral System: " & calendar.NumeralSystem & vbLf
            results &= "Resolved Language " & calendar.ResolvedLanguage & vbLf
            results &= "Name of Month: " & calendar.MonthAsSoloString() & vbLf
            results &= "Day of Month: " & calendar.DayAsPaddedString(2) & vbLf
            results &= "Day of Week: " & calendar.DayOfWeekAsSoloString() & vbLf
            results &= "Year: " & calendar.YearAsString() & vbLf
            results &= vbLf
            Return results
        End Function

        Private Sub ShowResults()
            ' This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.
            ' Create Calendar objects using different Unicode extensions for different languages.
            ' NOTE: Calendar (ca) and numeral system (nu) are the only supported extensions with any others being ignored.
            ' Note that collation (co) extension is ignored in the last example.
            Dim cal1 As Calendar = New Calendar()
            Dim cal2 As Calendar = New Calendar({"ar-SA-u-ca-gregory-nu-Latn"})
            Dim cal3 As Calendar = New Calendar({"he-IL-u-nu-arab"})
            Dim cal4 As Calendar = New Calendar({"he-IL-u-ca-hebrew-co-phonebk"})
            OutputTextBlock.Text = ReportCalendarData(cal1, "User's default Calendar object") & ReportCalendarData(cal2, "Calendar object with Arabic language, Gregorian Calendar and Latin Numeral System (ar-SA-ca-gregory-nu-Latn)") & ReportCalendarData(cal3, "Calendar object with Hebrew language, Default Calendar for that language and Arab Numeral System (he-IL-u-nu-arab)") & ReportCalendarData(cal4, "Calendar object with Hebrew language, Hebrew Calendar, Default Numeral System for that language and Phonebook collation (he-IL-u-ca-hebrew-co-phonebk)")
        End Sub
    End Class
End Namespace
