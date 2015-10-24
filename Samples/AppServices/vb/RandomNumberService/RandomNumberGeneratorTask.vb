Imports System
Imports Windows.ApplicationModel.AppService
Imports Windows.ApplicationModel.Background
Imports Windows.Foundation.Collections

Namespace Global.RandomNumberService

    Public NotInheritable Class RandomNumberGeneratorTask
        Implements IBackgroundTask

        Dim serviceDeferral As BackgroundTaskDeferral

        Dim connection As AppServiceConnection

        Dim randomNumberGenerator As Random

        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            serviceDeferral = taskInstance.GetDeferral()
            AddHandler taskInstance.Canceled, AddressOf OnTaskCanceled
            randomNumberGenerator = New Random(CType(DateTime.Now.Ticks, Integer))
            Dim details = TryCast(taskInstance.TriggerDetails, AppServiceTriggerDetails)
            connection = details.AppServiceConnection
            AddHandler connection.RequestReceived, AddressOf OnRequestReceived
        End Sub

        Private Sub OnTaskCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            If serviceDeferral IsNot Nothing Then
                serviceDeferral.Complete()
                serviceDeferral = Nothing
            End If
        End Sub

        Async Sub OnRequestReceived(sender As AppServiceConnection, args As AppServiceRequestReceivedEventArgs)
            'Get a deferral so we can use an awaitable API to respond to the message
            Dim messageDeferral = args.GetDeferral()
            Try
                Dim input = args.Request.Message
                Dim minValue As Integer = CType(input("minvalue"), Integer)
                Dim maxValue As Integer = CType(input("maxvalue"), Integer)
                'Create the response
                Dim result = New ValueSet()
                result.Add("result", randomNumberGenerator.Next(minValue, maxValue))
                Await args.Request.SendResponseAsync(result)
            Finally
                messageDeferral.Complete()
            End Try
        End Sub
    End Class
End Namespace
