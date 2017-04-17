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
Imports System.Collections.Generic
Imports System.IO
Imports System.Linq
Imports System.Runtime.InteropServices.WindowsRuntime
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class UnhandledExceptionPage
        Inherits Page

        Public Shared Sub ShowUnhandledException(ex As Exception)
            Dim rootFrame As Frame = TryCast(Window.Current.Content, Frame)
            If rootFrame Is Nothing Then
                rootFrame = New Frame()
                Window.Current.Content = rootFrame
                Window.Current.Activate()
            End If

            rootFrame.Navigate(GetType(UnhandledExceptionPage), ex.ToString())
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            MyBase.OnNavigatedTo(e)
            If TypeOf e.Parameter Is String Then
                Me.StatusMessage = CType(e.Parameter, String)
            End If
        End Sub

        Public Sub New()
            InitializeComponent()
        End Sub

        Public Property StatusMessage As String
            Get
                Return Me.Status.Text
            End Get

            Set
                Me.Status.Text = value
            End Set
        End Property
    End Class
End Namespace
