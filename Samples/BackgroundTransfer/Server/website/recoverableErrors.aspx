<%@ Page Language="C#" AutoEventWireup="true" Debug="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<script runat="server">

private const uint FullContentLengthInBytes = 10000000; // 10 MB
private readonly byte[] buffer = Encoding.ASCII.GetBytes(new String('a', 1000000)); // 1 MB

private static void GetRange(string rangeHeaderValue, out uint offset, out uint lengthInBytes)
{
    offset = 0;
    lengthInBytes = FullContentLengthInBytes;

    if (String.IsNullOrEmpty(rangeHeaderValue))
    {
        return;
    }

    Regex rx = new Regex(@"(bytes)\s*=\s*(?<startIndex>\d+)\s*-\s*(?<endIndex>\d+)?", RegexOptions.Compiled | RegexOptions.IgnoreCase);
    Match match = rx.Match(rangeHeaderValue);

    uint startIndex = 0;
    Capture startIndexCapture = match.Groups["startIndex"];
    if ((startIndexCapture == null) || (!UInt32.TryParse(startIndexCapture.Value, out startIndex)))
    {
        return;
    }

    if (startIndex < 0 || startIndex >= FullContentLengthInBytes)
    {
        throw new ArgumentException(
            "Requested range starting index is negative or goes beyond the file's length");
    }

    offset = startIndex;

    uint endIndex = 0;
    Capture endIndexCapture = match.Groups["endIndex"];
    if ((endIndexCapture != null) && (UInt32.TryParse(endIndexCapture.Value, out endIndex)))
    {
        if (endIndex < startIndex)
        {
            throw new ArgumentException(
                "Requested range ending index is less than the starting index");
        }

        lengthInBytes = Math.Min(endIndex, FullContentLengthInBytes - 1) - offset + 1;
    }
    else
    {
        lengthInBytes = FullContentLengthInBytes - offset;
    }
}

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        // Determine whether this response should emulate a URL that expires.
        bool shouldExpire = (Request["shouldExpire"] != null);

        // Check if we have a request for partial content (resume scenarios).
        bool isRangeRequest = (Request.Headers["Range"] != null);

        uint offset = 0;
        uint contentLengthInBytes = FullContentLengthInBytes;

        if (isRangeRequest)
        {
            GetRange(Request.Headers["Range"], out offset, out contentLengthInBytes);
        }

        if (shouldExpire && isRangeRequest && offset > 0)
        {
            // The client side is attempting to resume an expired URL.
            // Fail the request with HTTP 403 (Forbidden) status.
            Response.StatusCode = 403;
            Response.StatusDescription = "URL has expired";
            Response.Flush();
            return;
        }

        if (contentLengthInBytes != FullContentLengthInBytes)
        {
            Response.StatusCode = 206;
            Response.Headers["Content-Range"] = String.Format(System.Globalization.CultureInfo.InvariantCulture,
                "bytes {0}-{1}/{2}", offset, offset + contentLengthInBytes - 1, FullContentLengthInBytes);
        }
        else
        {
            Response.StatusCode = 200;
        }

        Response.ContentType = "text/plain";
        Response.Headers["Content-Length"] = contentLengthInBytes.ToString(System.Globalization.CultureInfo.InvariantCulture);
        Response.Headers["Accept-Ranges"] = "bytes";
        Response.Headers["Last-Modified"] = "Thu, 21 Aug 2014 21:34:57 GMT";
        Response.Buffer = false;

        uint transferLengthInBytes = contentLengthInBytes;
        while (transferLengthInBytes > 0)
        {
            if (shouldExpire && transferLengthInBytes < contentLengthInBytes / 2)
            {
                // More than half of the response has been sent out. Let's terminate the connection abruptly to
                // simulate a URL that expires. If the client attempts to resume the same URL in the future, it
                // will receive an HTTP 403 response.
                Response.Flush();
                Response.Close();
                return;
            }

            System.Threading.Thread.Sleep(1000);

            int sendLengthInBytes = (int)Math.Min(transferLengthInBytes, this.buffer.Length);
            Response.OutputStream.Write(this.buffer, 0, sendLengthInBytes);
            transferLengthInBytes -= (uint)sendLengthInBytes;
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

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Recoverable Errors Download</title>
</head>
<body>
    Hello
</body>
</html>
