'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System
Imports System.Collections.Generic
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports Windows.ApplicationModel.Resources.Core
Imports System.Threading.Tasks
Imports Windows.Storage

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario13
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            DXFLOptionCombo.SelectedIndex = 0
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
        End Sub

        Private Async Sub DXFLOptionCombo_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            Dim combo As ComboBox = TryCast(sender, ComboBox)
            ResourceContext.SetGlobalQualifierValue("dxfeaturelevel", combo.SelectedValue.ToString())
            ' setting up the resource URI -- an ms-appx URI will be used to access the resource using
            ' a StorageFile method
            ' In a different scenario, rather than obtaining a StorageFile object, the 
            ' ResourceMap.GetValue method could be used to get the resolved candidate, from which an 
            ' absolute file path could be obtained. However, resource references used by the APIs in 
            ' Windows.ApplicationModel.Resources and Windows.ApplicationModel.Resources.Core use 
            ' ms-resource URIs, not ms-appx URIs. Within a resource map, files are always organized 
            ' under a "Files" top-level subtree. The ms-resource URI string for the file being accessed 
            ' here would be "ms-resource:///Files/appdata/appdata.dat".
            ' The URI string "ms-appx:///..." is rooted at the app package root.
            Dim uri As Uri = New Uri("ms-appx:///appdata/appdata.dat")
            ' The resource candidate will be resolved during the call to GetFileFromApplicationUriAsync 
            ' using the default context from a background thread.
            ' File IO uses asynchronous techniques. For more info, see
            ' http://msdn.microsoft.com/en-us/library/windows/apps/hh464924.aspx
            Dim file As StorageFile = Await StorageFile.GetFileFromApplicationUriAsync(uri)
            Dim content As String = Await FileIO.ReadTextAsync(file)
            ResultText.Text = content
        End Sub
    End Class
End Namespace
