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
Imports Windows.Devices.Sensors
Imports Windows.UI.Core
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    Partial Public NotInheritable Class Scenario0_Choose
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()

            Dim readingTypes = New List(Of AccelerometerReadingType) From
                {AccelerometerReadingType.Standard,
                AccelerometerReadingType.Linear,
                AccelerometerReadingType.Gravity}
            ReadingTypeComboBox.ItemsSource = readingTypes
            ReadingTypeComboBox.SelectedIndex = readingTypes.IndexOf(rootPage.AccelerometerReadingType)
        End Sub

        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            rootPage.AccelerometerReadingType = CType(ReadingTypeComboBox.SelectedValue, AccelerometerReadingType)
        End Sub
    End Class
End Namespace
