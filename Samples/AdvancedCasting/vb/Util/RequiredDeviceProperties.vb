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
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks

Namespace Global.ScreenCasting.Util

    Public Class RequiredDeviceProperties

        Public Shared ReadOnly DEVPKEY_AepContainer_FriendlyName As String = "{0bba1ede-7566-4f47-90ec-25fc567ced2a}, 5"

        Public Shared ReadOnly DEVPKEY_AepContainer_SupportsAudio As String = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 2"

        Public Shared ReadOnly DEVPKEY_AepContainer_SupportsVideo As String = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 3"

        Public Shared ReadOnly DEVPKEY_AepContainer_SupportsImages As String = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 4"

        Public Shared ReadOnly DEVPKEY_AepContainer_SupportedUriSchemes As String = "{6af55d45-38db-4495-acb0-d4728a3b8314}, 5"

        Public Shared ReadOnly DEVPKEY_AepContainer_Categories As String = "System.Devices.AepContainer.Categories"

        Public Shared ReadOnly DEVPKEY_DeviceContainer_Manufacturer As String = "System.Devices.Manufacturer"

        Public Shared ReadOnly DEVPKEY_DeviceContainer_ModelName As String = "System.Devices.ModelName"

        Public Shared ReadOnly DEVPKEY_Device_ContainerId As String = "System.Devices.ContainerId"

        Public Shared ReadOnly DEVPKEY_Device_InstanceId As String = "System.Devices.DeviceInstanceId"

        Private Sub New()
        End Sub

        Public Shared Sub AddProps(list As IList(Of String))
            For Each s In Props
                list.Add(s)
            Next
        End Sub

        Public Shared ReadOnly Property Props As List(Of String)
            Get
                Dim properties As List(Of String) = New List(Of String)()
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_FriendlyName)
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsAudio)
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsVideo)
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportsImages)
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_SupportedUriSchemes)
                properties.Add(RequiredDeviceProperties.DEVPKEY_AepContainer_Categories)
                properties.Add(RequiredDeviceProperties.DEVPKEY_DeviceContainer_Manufacturer)
                properties.Add(RequiredDeviceProperties.DEVPKEY_DeviceContainer_ModelName)
                properties.Add(RequiredDeviceProperties.DEVPKEY_Device_ContainerId)
                properties.Add(RequiredDeviceProperties.DEVPKEY_Device_InstanceId)
                Return properties
            End Get
        End Property
    End Class
End Namespace
