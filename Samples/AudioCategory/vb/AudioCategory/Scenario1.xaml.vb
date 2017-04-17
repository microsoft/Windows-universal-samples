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
Imports Windows.UI.Xaml.Media

Namespace Global.PlaybackManager

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Sub Default_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            Playback.SetAudioCategory(AudioCategory.Movie)
            Playback.SelectFile()
        End Sub
    End Class
End Namespace
