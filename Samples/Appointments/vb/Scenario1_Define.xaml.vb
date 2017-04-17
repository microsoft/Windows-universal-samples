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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports System

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario1_Define
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Creates an Appointment based on the input fields and validates it.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Create_Click(sender As Object, e As RoutedEventArgs)
            Dim errorMessage As String = Nothing
            Dim appointment = New Windows.ApplicationModel.Appointments.Appointment()
            ' StartTime
            Dim [date] = StartTimeDatePicker.Date
            Dim time = StartTimeTimePicker.Time
            Dim timeZoneOffset = TimeZoneInfo.Local.GetUtcOffset(DateTime.Now)
            Dim startTime = New DateTimeOffset([date].Year, [date].Month, [date].Day, time.Hours, time.Minutes, 0, timeZoneOffset)
            appointment.StartTime = startTime
            appointment.Subject = SubjectTextBox.Text
            If appointment.Subject.Length > 255 Then
                errorMessage = "The subject cannot be greater than 255 characters."
            End If

            appointment.Location = LocationTextBox.Text
            If appointment.Location.Length > 32768 Then
                errorMessage = "The location cannot be greater than 32,768 characters."
            End If

            appointment.Details = DetailsTextBox.Text
            If appointment.Details.Length > 1073741823 Then
                errorMessage = "The details cannot be greater than 1,073,741,823 characters."
            End If

            If DurationComboBox.SelectedIndex = 0 Then
                appointment.Duration = TimeSpan.FromMinutes(30)
            Else
                appointment.Duration = TimeSpan.FromHours(1)
            End If

            appointment.AllDay = AllDayCheckBox.IsChecked.Value
            If ReminderCheckBox.IsChecked.Value Then
                Select ReminderComboBox.SelectedIndex
                    Case 0
                        appointment.Reminder = TimeSpan.FromMinutes(15)
                         Case 1
                        appointment.Reminder = TimeSpan.FromHours(1)
                         Case 2
                        appointment.Reminder = TimeSpan.FromDays(1)
                         End Select
            End If

            Select BusyStatusComboBox.SelectedIndex
                Case 0
                    appointment.BusyStatus = Windows.ApplicationModel.Appointments.AppointmentBusyStatus.Busy
                     Case 1
                    appointment.BusyStatus = Windows.ApplicationModel.Appointments.AppointmentBusyStatus.Tentative
                     Case 2
                    appointment.BusyStatus = Windows.ApplicationModel.Appointments.AppointmentBusyStatus.Free
                     Case 3
                    appointment.BusyStatus = Windows.ApplicationModel.Appointments.AppointmentBusyStatus.OutOfOffice
                     Case 4
                    appointment.BusyStatus = Windows.ApplicationModel.Appointments.AppointmentBusyStatus.WorkingElsewhere
                     End Select

            Select SensitivityComboBox.SelectedIndex
                Case 0
                    appointment.Sensitivity = Windows.ApplicationModel.Appointments.AppointmentSensitivity.Public
                     Case 1
                    appointment.Sensitivity = Windows.ApplicationModel.Appointments.AppointmentSensitivity.Private
                     End Select

            If UriTextBox.Text.Length > 0 Then
                Try
                    appointment.Uri = New System.Uri(UriTextBox.Text)
                Catch
                    errorMessage = "The Uri provided is invalid."
                End Try
            End If

            If OrganizerRadioButton.IsChecked.Value Then
                Dim organizer = New Windows.ApplicationModel.Appointments.AppointmentOrganizer()
                organizer.DisplayName = OrganizerDisplayNameTextBox.Text
                If organizer.DisplayName.Length > 256 Then
                    errorMessage = "The organizer display name cannot be greater than 256 characters."
                Else
                    organizer.Address = OrganizerAddressTextBox.Text
                    If organizer.Address.Length > 321 Then
                        errorMessage = "The organizer address cannot be greater than 321 characters."
                    ElseIf organizer.Address.Length = 0 Then
                        errorMessage = "The organizer address must be greater than 0 characters."
                    Else
                        appointment.Organizer = organizer
                    End If
                End If
            End If

            If InviteeRadioButton.IsChecked.Value Then
                Dim invitee = New Windows.ApplicationModel.Appointments.AppointmentInvitee()
                invitee.DisplayName = InviteeDisplayNameTextBox.Text
                If invitee.DisplayName.Length > 256 Then
                    errorMessage = "The invitee display name cannot be greater than 256 characters."
                Else
                    invitee.Address = InviteeAddressTextBox.Text
                    If invitee.Address.Length > 321 Then
                        errorMessage = "The invitee address cannot be greater than 321 characters."
                    ElseIf invitee.Address.Length = 0 Then
                        errorMessage = "The invitee address must be greater than 0 characters."
                    Else
                        Select RoleComboBox.SelectedIndex
                            Case 0
                                invitee.Role = Windows.ApplicationModel.Appointments.AppointmentParticipantRole.RequiredAttendee
                                 Case 1
                                invitee.Role = Windows.ApplicationModel.Appointments.AppointmentParticipantRole.OptionalAttendee
                                 Case 2
                                invitee.Role = Windows.ApplicationModel.Appointments.AppointmentParticipantRole.Resource
                                 End Select

                        Select ResponseComboBox.SelectedIndex
                            Case 0
                                invitee.Response = Windows.ApplicationModel.Appointments.AppointmentParticipantResponse.None
                                 Case 1
                                invitee.Response = Windows.ApplicationModel.Appointments.AppointmentParticipantResponse.Tentative
                                 Case 2
                                invitee.Response = Windows.ApplicationModel.Appointments.AppointmentParticipantResponse.Accepted
                                 Case 3
                                invitee.Response = Windows.ApplicationModel.Appointments.AppointmentParticipantResponse.Declined
                                 Case 4
                                invitee.Response = Windows.ApplicationModel.Appointments.AppointmentParticipantResponse.Unknown
                                 End Select

                        appointment.Invitees.Add(invitee)
                    End If
                End If
            End If

            If errorMessage Is Nothing Then
                rootPage.NotifyUser("The appointment was created successfully and is valid.", NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' Organizer and Invitee properties are mutually exclusive.
        ''' This radio button enables the organizer properties while disabling the invitees.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub OrganizerRadioButton_Checked(sender As Object, e As RoutedEventArgs)
            OrganizerStackPanel.Visibility = Windows.UI.Xaml.Visibility.Visible
            InviteeStackPanel.Visibility = Windows.UI.Xaml.Visibility.Collapsed
        End Sub

        ''' <summary>
        ''' Organizer and Invitee properties are mutually exclusive.
        ''' This radio button enables the invitees properties while disabling the organizer.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub InviteeRadioButton_Checked(sender As Object, e As RoutedEventArgs)
            InviteeStackPanel.Visibility = Windows.UI.Xaml.Visibility.Visible
            OrganizerStackPanel.Visibility = Windows.UI.Xaml.Visibility.Collapsed
        End Sub

        ''' <summary>
        ''' Displays the combo box containing various reminder times.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ReminderCheckBox_Checked(sender As Object, e As RoutedEventArgs)
            ReminderComboBox.Visibility = Windows.UI.Xaml.Visibility.Visible
        End Sub

        ''' <summary>
        ''' Hides the combo box containing various reminder times.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ReminderCheckBox_UnChecked(sender As Object, e As RoutedEventArgs)
            ReminderComboBox.Visibility = Windows.UI.Xaml.Visibility.Collapsed
        End Sub
    End Class
End Namespace
