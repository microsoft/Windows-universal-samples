<%@ Page Language="C#" AutoEventWireup="true" Debug="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<script runat="server">

private const int iterationCount = 5;
private static readonly string sampleETag = "\"SampleETag\"";
private static readonly byte[] buffer = Encoding.ASCII.GetBytes(new String('a', 1000000));
private static readonly int fullContentLength = buffer.Length * iterationCount;

private static void GetRange(string rangeHeaderValue, out int offset, out int length)
{
    offset = 0;
    length = fullContentLength;

    if (String.IsNullOrEmpty(rangeHeaderValue))
    {
        return;
    }

    Regex rx = new Regex(@"(bytes)\s*=\s*(?<startIndex>\d+)\s*-\s*(?<endIndex>\d+)?", RegexOptions.Compiled | RegexOptions.IgnoreCase);
    Match match = rx.Match(rangeHeaderValue);

    int startIndex = 0;
    Capture startIndexCapture = match.Groups["startIndex"];
    if ((startIndexCapture == null) || (!Int32.TryParse(startIndexCapture.Value, out startIndex)))
    {
        return;
    }

    offset = startIndex;

    int endIndex = 0;
    Capture endIndexCapture = match.Groups["endIndex"];
    if ((endIndexCapture != null) && (Int32.TryParse(endIndexCapture.Value, out endIndex)))
    {
        length = endIndex - offset + 1;
    }
    else
    {
        length = fullContentLength - offset;
    }
}

protected void Page_Load(object sender, EventArgs e)
{
    try
    {
        int offset = 0;
        int contentLength = fullContentLength;
        
        // Check if we have a request for partial content (resume scenarios).
        if (String.CompareOrdinal(sampleETag, Request.Headers["If-Range"]) == 0)
        {
            GetRange(Request.Headers["Range"], out offset, out contentLength);
        }

        if (offset > 0)
        {
            Response.StatusCode = 206;
            Response.Headers["Content-Range"] = String.Format(System.Globalization.CultureInfo.InvariantCulture,
                "bytes {0}-{1}/{2}", offset, offset + contentLength - 1, fullContentLength);
        }
        else
        {
            Response.StatusCode = 200;
        }

        Response.ContentType = "text/plain";
        Response.Headers["Content-Length"] = contentLength.ToString(System.Globalization.CultureInfo.InvariantCulture);
        Response.Headers["Accept-Ranges"] = "bytes";
        Response.Headers["ETag"] = sampleETag;
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

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Download</title>
</head>
<body>
    Hello
</body>
</html>
