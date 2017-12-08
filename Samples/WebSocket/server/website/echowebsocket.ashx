<%@ WebHandler Language="C#" Class="EchoWebSocket" %>

// This define is needed to enable System.Diagnostics.Trace methods. For more information go to:
// http://msdn.microsoft.com/en-us/library/system.diagnostics.trace(v=vs.110).aspx
#define TRACE

using System;
using System.Web;
using System.Net.WebSockets;
using System.Web.WebSockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

public class EchoWebSocket : IHttpHandler {
    private const int MaxBufferSize = 64 * 1024;

    public void ProcessRequest (HttpContext context)
    {
        try
        {
            context.AcceptWebSocketRequest(async wsContext =>
            {
                try
                {
                    byte[] receiveBuffer = new byte[MaxBufferSize];
                    ArraySegment<byte> buffer = new ArraySegment<byte>(receiveBuffer);
                    WebSocket socket = wsContext.WebSocket;
                    string userString;

                    if (socket.State == WebSocketState.Open)
                    {
                        // Announcement when connected
                        var announceString = "EchoWebSocket Connected at: " + DateTime.Now.ToString();
                        ArraySegment<byte> outputBuffer2 = new ArraySegment<byte>(Encoding.UTF8.GetBytes(announceString));
                        await socket.SendAsync(outputBuffer2, WebSocketMessageType.Text, true, CancellationToken.None);
                    }

                    // Stay in loop while websocket is open
                    while (socket.State == WebSocketState.Open)
                    {
                        WebSocketReceiveResult receiveResult = await socket.ReceiveAsync(buffer, CancellationToken.None);

                        if (receiveResult.MessageType == WebSocketMessageType.Close)
                        {
                            // Echo back code and reason strings 
                            await socket.CloseAsync(
                                receiveResult.CloseStatus.GetValueOrDefault(), 
                                receiveResult.CloseStatusDescription,
                                CancellationToken.None);
                            return;
                        }

                        int offset = receiveResult.Count;

                        while (receiveResult.EndOfMessage == false)
                        {
                            receiveResult = await socket.ReceiveAsync(new ArraySegment<byte>(receiveBuffer, offset, MaxBufferSize - offset), CancellationToken.None);
                            offset += receiveResult.Count;
                        }

                        if (receiveResult.MessageType == WebSocketMessageType.Text)
                        {
                            string cmdString = Encoding.UTF8.GetString(receiveBuffer, 0, offset);
                            userString = cmdString;
                            userString = "You said: \"" + userString + "\"";

                            ArraySegment<byte> outputBuffer = new ArraySegment<byte>(Encoding.UTF8.GetBytes(userString));

                            await socket.SendAsync(outputBuffer, WebSocketMessageType.Text, true, CancellationToken.None);
                        }
                        else if (receiveResult.MessageType == WebSocketMessageType.Binary)
                        {
                            userString = String.Format("binary message received, size={0} bytes", receiveResult.Count);

                            ArraySegment<byte> outputBuffer = new ArraySegment<byte>(Encoding.UTF8.GetBytes(userString));

                            await socket.SendAsync(outputBuffer, WebSocketMessageType.Text, true, CancellationToken.None);
                        }
                    }
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Trace.WriteLine(ex);
                }
            });
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex);
            context.Response.StatusCode = 500;
            context.Response.StatusDescription = ex.Message;
            context.Response.End();
        }
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }
}
