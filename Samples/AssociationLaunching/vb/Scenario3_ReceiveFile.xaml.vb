'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
'
'*********************************************************
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Storage
Imports SDKTemplate
Imports System

Namespace Global.AssociationLaunching

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class ReceiveFile
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            If Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons") Then
                Me.WindowsScenarioDescription.Visibility = Visibility.Collapsed
            Else
                Me.PhoneScenarioDescription.Visibility = Visibility.Collapsed
            End If

            If rootPage.FileEvent IsNot Nothing Then
                Dim output As String = "File activation received. The number of files received is " & rootPage.FileEvent.Files.Count & ". The received files are:" & vbLf
                For Each file In rootPage.FileEvent.Files
                    output = output & file.Name & vbLf
                Next

                rootPage.NotifyUser(output, NotifyType.StatusMessage)
            End If
        End Sub
    End Class
End Namespace
