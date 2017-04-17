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
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1
        Inherits Page

        Private rootPage As MainPage

        Private Shared optedIn As Boolean = False

        Public Sub New()
            Me.InitializeComponent()
            Me.NavigationCacheMode = NavigationCacheMode.Required
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Private Sub Button_Click(sender As Object, e As RoutedEventArgs)
            Dim rootFrame As Frame = TryCast(Window.Current.Content, Frame)
            rootFrame.Navigate(GetType(SecondaryPage), optedIn)
        End Sub

        Private Sub CheckBox_Toggle(sender As Object, e As RoutedEventArgs)
            optedIn = Not optedIn
        End Sub
    End Class
End Namespace
