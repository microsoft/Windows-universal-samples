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
Imports Windows.UI.Core.AnimationMetrics

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario1_Metrics
        Inherits Page

        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        ''' <summary>
        ''' Retrieves the specified metrics and displays them in textual form.
        ''' </summary>
        ''' <param name="effect">The AnimationEffect whose metrics are to be displayed.</param>
        ''' <param name="target">The AnimationEffecTarget whose metrics are to be displayed.</param>
        Private Sub DisplayMetrics(effect As AnimationEffect, target As AnimationEffectTarget)
            Dim s = New System.Text.StringBuilder()
            Dim animationDescription As AnimationDescription = New AnimationDescription(effect, target)
            s.AppendFormat("Stagger delay = {0}ms", animationDescription.StaggerDelay.TotalMilliseconds)
            s.AppendLine()
            s.AppendFormat("Stagger delay factor = {0}", animationDescription.StaggerDelayFactor)
            s.AppendLine()
            s.AppendFormat("Delay limit = {0}ms", animationDescription.DelayLimit.TotalMilliseconds)
            s.AppendLine()
            s.AppendFormat("ZOrder = {0}", animationDescription.ZOrder)
            s.AppendLine()
            s.AppendLine()
            Dim animationIndex As Integer = 0
            For Each animation As IPropertyAnimation In animationDescription.Animations
                animationIndex += 1
                s.AppendFormat("Animation #{0}:", animationIndex)
                s.AppendLine()
                Select Case animation.Type
                    Case PropertyAnimationType.Scale
                        Dim scale As ScaleAnimation = TryCast(animation, ScaleAnimation)
                        s.AppendLine("Type = Scale")
                        If scale.InitialScaleX.HasValue Then
                            s.AppendFormat("InitialScaleX = {0}", scale.InitialScaleX.Value)
                            s.AppendLine()
                        End If

                        If scale.InitialScaleY.HasValue Then
                            s.AppendFormat("InitialScaleY = {0}", scale.InitialScaleY.Value)
                            s.AppendLine()
                        End If

                        s.AppendFormat("FinalScaleX = {0}", scale.FinalScaleX)
                        s.AppendLine()
                        s.AppendFormat("FinalScaleY = {0}", scale.FinalScaleY)
                        s.AppendLine()
                        s.AppendFormat("Origin = {0}, {1}", scale.NormalizedOrigin.X, scale.NormalizedOrigin.Y)
                        s.AppendLine()
                    Case PropertyAnimationType.Translation
                        s.AppendLine("Type = Translation")
                    Case PropertyAnimationType.Opacity
                        Dim opacity As OpacityAnimation = TryCast(animation, OpacityAnimation)
                        s.AppendLine("Type = Opacity")
                        If opacity.InitialOpacity.HasValue Then
                            s.AppendFormat("InitialOpacity = {0}", opacity.InitialOpacity.Value)
                            s.AppendLine()
                        End If

                        s.AppendFormat("FinalOpacity = {0}", opacity.FinalOpacity)
                        s.AppendLine()
                End Select

                s.AppendFormat("Delay = {0}ms", animation.Delay.TotalMilliseconds)
                s.AppendLine()
                s.AppendFormat("Duration = {0}ms", animation.Duration.TotalMilliseconds)
                s.AppendLine()
                s.AppendFormat("Cubic Bezier control points")
                s.AppendLine()
                s.AppendFormat("    X1 = {0}, Y1 = {1}", animation.Control1.X, animation.Control1.Y)
                s.AppendLine()
                s.AppendFormat("    X2 = {0}, Y2 = {1}", animation.Control2.X, animation.Control2.Y)
                s.AppendLine()
                s.AppendLine()
            Next

            Metrics.Text = s.ToString()
        End Sub

        ''' <summary>
        ''' When the selection changes, update the output window.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Animations_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            Dim lb As ListBox = TryCast(sender, ListBox)
            If lb IsNot Nothing Then
                Dim selectedListBoxItem As Object = lb.SelectedItem
                If selectedListBoxItem Is AddToListAdded Then
                    DisplayMetrics(AnimationEffect.AddToList, AnimationEffectTarget.Added)
                ElseIf selectedListBoxItem Is AddToListAffected Then
                    DisplayMetrics(AnimationEffect.AddToList, AnimationEffectTarget.Affected)
                ElseIf selectedListBoxItem Is EnterPage Then
                    DisplayMetrics(AnimationEffect.EnterPage, AnimationEffectTarget.Primary)
                End If
            End If
        End Sub
    End Class
End Namespace
