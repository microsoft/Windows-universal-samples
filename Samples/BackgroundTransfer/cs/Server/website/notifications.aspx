<%@ Page Language="C#" AutoEventWireup="true" Debug="true" %>

<script runat="server">

private static readonly byte[] buffer = Encoding.ASCII.GetBytes(new String('b', 1000000));

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        // We expect a URI http://localhost/BackgroundTransferSample/notifications.aspx?delay={number} where {number}
        // is a positive integer indicating the number of seconds the response should take to send.
        Regex rx = new Regex(@"\?delay=(?<delayValue>\d+)", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        Match match = rx.Match(Request.Url.Query);

        ushort delay = 0;
        Capture delayCapture = match.Groups["delayValue"];
        if ((delayCapture == null) || (!UInt16.TryParse(delayCapture.Value, out delay)))
        {
            Trace.Write(String.Format(System.Globalization.CultureInfo.InvariantCulture,
                "Invalid URI: {0}", Request.Url));
            Response.StatusCode = 400;
            Response.Headers["Content-Length"] = "0";
            Response.Flush();
            Response.End();

            return;
        }

        int contentLength = buffer.Length * delay;

        Response.ContentType = "text/plain";
        Response.Headers["Content-Length"] = contentLength.ToString(System.Globalization.CultureInfo.InvariantCulture);
        Response.Buffer = false;

        int transferLength = contentLength;
        while (transferLength > 0)
        {
            System.Threading.Thread.Sleep(1000);

            int sendLength = Math.Min(transferLength, buffer.Length);
            Response.OutputStream.Write(buffer, 0, sendLength);
            transferLength -= sendLength;
        }

        Response.Flush();
    }
    catch (Exception ex)
    {
        Trace.Write(ex.Message);
        Response.StatusCode = 500;
        Response.StatusDescription = ex.Message;
    }

    Response.End();
}

</script>
