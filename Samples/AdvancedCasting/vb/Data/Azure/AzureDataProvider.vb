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
Imports Windows.Web.Syndication
Imports ScreenCasting.Data.Common

Namespace Global.ScreenCasting.Data.Azure

    Public Class AzureDataProvider
        Inherits ItemsDataProvider

        Private Shared s_metaDataList As Dictionary(Of String, VideoMetaData) = Nothing

        Private Const MAX_RESULTS As Integer = 10

        Public Overrides Function GetRandomVideo() As VideoMetaData
            Dim videos As List(Of VideoMetaData) = GetAll(MAX_RESULTS)
            Dim indexRandomizer As Random = New Random()
            Return videos(indexRandomizer.Next(0, videos.Count - 1))
        End Function

        Public Overrides Function GetAll(max_results As Integer) As List(Of VideoMetaData)
            If s_metaDataList Is Nothing Then
                Dim l As Dictionary(Of String, VideoMetaData) = New Dictionary(Of String, VideoMetaData)()
                Dim video1 As VideoMetaData = New VideoMetaData()
                video1.Id = "AzureMediaServicesOverview"
                video1.PubDate = DateTime.Now
                video1.VideoLink = New Uri("http://amssamples.streaming.mediaservices.windows.net/91492735-c523-432b-ba01-faba6c2206a2/AzureMediaServicesPromo.ism/manifest(format=m3u8-aapl)")
                video1.Title = "Azure Media Services Overview"
                video1.WebPageLink = New Uri("http://amsplayer.azurewebsites.net/amssamples.html")
                video1.License = "(C) Microsoft Coorporation | All Rights Reserved"
                video1.Thumbnail = New Uri("http://t3.gstatic.com/images?q=tbn:ANd9GcRoTu81vs8GuQtJBGUZ0pLEiNHVVx3dFwHsu7JCkvGop8Z-98EsPg")
                l.Add(video1.Id, video1)
                Dim video2 As VideoMetaData = New VideoMetaData()
                video2.Id = "BigBuckBunnyTrailer"
                video2.PubDate = DateTime.Now
                video2.VideoLink = New Uri("http://download.blender.org/peach/bigbuckbunny_movies/big_buck_bunny_480p_surround-fix.avi")
                video2.Title = "Big Buck Bunny"
                video2.WebPageLink = New Uri("https://peach.blender.org/")
                video2.License = "(CC) Blender Foundation | peach.blender.org"
                video2.Thumbnail = New Uri("http://t0.gstatic.com/images?q=tbn:ANd9GcQT54UKh0zX5OUDNLRyXOOwt3pqj0lpaLCJebucva2LSV49aGjRfg")
                l.Add(video2.Id, video2)
                Dim video3 As VideoMetaData = New VideoMetaData()
                video3.Id = "bc57e088-27ec-44e0-ac20-a85ccbcd50da"
                video3.PubDate = DateTime.Now
                video3.VideoLink = New Uri("http://amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TearsOfSteel.ism/manifest(format=mpd-time-csf)")
                video3.Title = "Tears of Steel"
                video3.WebPageLink = New Uri("https://mango.blender.org/")
                video3.License = "(CC) Blender Foundation | durian.blender.org/"
                video3.Thumbnail = New Uri("http://t3.gstatic.com/images?q=tbn:ANd9GcQO0MMO_vXf8Q1zjZWLHa1_566Mf_jty6vJKRi9R-C_0W-gXnNOog")
                l.Add(video3.Id, video3)
                'VideoMetaData video4 = new VideoMetaData();
                'video4.Id = "b6822ec8-5c2b-4ae0-a851-fd46a78294e9";
                'video4.PubDate = DateTime.Now;
                'video4.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/b6822ec8-5c2b-4ae0-a851-fd46a78294e9/ElephantsDream.ism/manifest(filtername=FirstFilter,format=mpd-time-csf)");
                'video4.Title = "Elephants Dream";
                'video4.WebPageLink = new Uri("https://orange.blender.org/");
                'video4.License = "(c) copyright 2006, Blender Foundation / Netherlands Media Art Institute / www.elephantsdream.org";
                'video4.Thumbnail = new Uri("https://orange.blender.org/wp-content/uploads/2006/05/edscore_cover_l.jpg");
                'l.Add(video4.Id, video4);
                Dim video5 As VideoMetaData = New VideoMetaData()
                video5.Id = "49b57c87-f5f3-48b3-ba22-c55cfdffa9cb"
                video5.PubDate = DateTime.Now
                video5.VideoLink = New Uri("http://amssamples.streaming.mediaservices.windows.net/49b57c87-f5f3-48b3-ba22-c55cfdffa9cb/Sintel.ism/manifest(format=m3u8-aapl)")
                video5.Title = "Sintel"
                video5.WebPageLink = New Uri("https://durian.blender.org/")
                video5.License = "© copyright Blender Foundation | www.sintel.org"
                video5.Thumbnail = New Uri("https://durian.blender.org/wp-content/uploads/2010/05/sintel_trailer_1080.jpg")
                l.Add(video5.Id, video5)
                s_metaDataList = l
            End If

            Return s_metaDataList.Values.ToList()
        End Function

        Public Overrides Function GetFromID(id As String) As VideoMetaData
            If s_metaDataList Is Nothing Then
                GetAll(20)
            End If

            Dim video As VideoMetaData = s_metaDataList(id)
            Return video
        End Function
    End Class
End Namespace
