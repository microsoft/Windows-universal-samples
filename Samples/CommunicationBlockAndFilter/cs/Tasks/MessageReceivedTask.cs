//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Chat;

namespace BlockAndFilterTasks
{
    public sealed class MessageReceivedTask : IBackgroundTask
    {
        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            // A message has been received from a sender not on the block list.
            // We must decide whether to block it or to allow it.
            var deferral = taskInstance.GetDeferral();

            Dictionary<string, ChatMessageTransportKind> transportIdToTransportKind = new Dictionary<string, ChatMessageTransportKind>();

            // Prepare the mapping for TransportKind check
            var transports = await ChatMessageManager.GetTransportsAsync();
            foreach (ChatMessageTransport transport in transports)
            {
                transportIdToTransportKind[transport.TransportId] = transport.TransportKind;
            }

            ChatMessageStore msgStore = await ChatMessageManager.RequestStoreAsync();

            // Untriaged is a transient status for less than 1 second.
            // If a message is not blocked in time, the status will be changed by system.
            // The message blocked by block list will not fire this trigger.
            ChatMessageReader msgReader = msgStore.GetMessageReader(TimeSpan.FromSeconds(1));
            var msgList = await msgReader.ReadBatchAsync();

            foreach (ChatMessage msg in msgList)
            {
                if (transportIdToTransportKind[msg.TransportId] == ChatMessageTransportKind.Untriaged)
                {
                    // Check if the message should be blocked.
                    // This sample blocks messages whose text is "block me".
                    if (msg.Body == "block me")
                    {
                        try
                        {
                            await ChatMessageBlocking.MarkMessageAsBlockedAsync(msg.Id, true);
                        }
                        catch (System.UnauthorizedAccessException)
                        {
                            // Untriaged message will be recalled by system in 1 second.
                            Toast.Send("Block failed due to timeout");
                        }
                    }
                }
            }

            deferral.Complete();
        }
    }
}
