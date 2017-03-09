<%@ Page Language="C#" %>
<script runat="server">
    protected void Page_Load(object sender, EventArgs e)
    {
        try
        {
            Response.Write("<html><body>");
            Page_LoadCore(sender, e);
            Response.Write("</body></html>");
        }
        catch (Exception ex)
        {
            Response.StatusCode = 500;
            Response.Write(Server.HtmlEncode(ex.ToString()));
        }
    }

    protected void Page_LoadCore(object sender, EventArgs e)
    {
        // Artificially inject 2 seconds of delay to enable testing
        // cancellation at the client.
        System.Threading.Thread.Sleep(2000);

        // Return 503 Service Unavailable
        if (Request.QueryString["retryAfter"] != null)
        {
            int step = 2;
            if (Request.Cookies["step"] != null)
            {
                Int32.TryParse(Request.Cookies["step"].Value, out step);
            }

            if (step > 0)
            {
                // Decrement step by 1.
                HttpCookie stepCookie = new HttpCookie("step");
                stepCookie.Value = (step - 1).ToString();
                Response.Cookies.Add(stepCookie);

                // Tell the user agent to retry in 10 seconds.
                if (Request.QueryString["retryAfter"] != "deltaSeconds")
                {
                    Response.Headers["Retry-After"] = "10";
                }
                else
                {
                    Response.Headers["Retry-After"] = DateTime.UtcNow.AddSeconds(10).ToString("r");
                }

                // Terminate response.
                Response.StatusCode = 503;
                Response.Write("Service will be available again in " + (step * 10) + " seconds.");
                return;
            }
            else
            {
                // Remove step cookie.
                HttpCookie stepCookie = new HttpCookie("step");
                stepCookie.Value = "";
                stepCookie.Expires = DateTime.Now.AddDays(-1);
                Response.Cookies.Add(stepCookie);
            }
        }

        // Write back the request headers.
        Response.Write("Request Headers:<br>");
        for (int i = 0; i < Request.Headers.Count; i++)
        {
            Response.Write(Request.Headers.Keys[i] + ": " + Server.HtmlEncode(Request.Headers[i]) + "<br>");
        }

        // Write back the request body.
        Response.Write("<br>Request Body:<br>");

        System.IO.Stream inputStream = Request.InputStream;
        Int32 inputLength, bytesRead;

        using (System.IO.StreamReader reader = new System.IO.StreamReader(Request.InputStream))
        {
            string body = reader.ReadToEnd();
            Response.Write(Server.HtmlEncode(body));
            Response.Write("<br>");
        }

        // List query string parameters.
        if (Request.QueryString.Count > 0)
        {
            Response.Write("<br>Query String Parameters:<br>");
            foreach (String key in Request.QueryString.AllKeys)
            {
                Response.Write(key + "=" + Server.HtmlEncode(Request.QueryString[key]) + "<br>");
            }
        }

        // Add cache header.
        if (Request.QueryString["cacheable"] != null)
        {
            // Add a 2 minutes expiration time.
            Response.Expires = 2;

            Response.Write("<br>This page was generated on:<br>");
            Response.Write(DateTime.Now + "<br>");
            Response.Write("<a href=\"" + Request.RawUrl + "\">Reload page.</a><br>");
        }

        // Set cookies.
        if (Request.QueryString["setCookies"] != null)
        {
            // Do not cache this response.
            Response.Cache.SetCacheability(HttpCacheability.NoCache);

            // Create a 3 minutes persistent cookie.
            HttpCookie myCookie1 = new HttpCookie("LastVisit");
            DateTime now = DateTime.Now;
            myCookie1.Value = now.ToString();
            myCookie1.Expires = now.AddMinutes(3);
            Response.Cookies.Add(myCookie1);

            // Create an HttpOnly session cookie.
            HttpCookie myCookie2 = new HttpCookie("SID");
            myCookie2.Value = "31d4d96e407aad42";
            myCookie2.HttpOnly = true;
            Response.Cookies.Add(myCookie2);
        }

        // Chunked response with delay.
        if (Request.QueryString["chunkedResponse"] != null)
        {
            Response.BufferOutput = false;
            
            Response.Write("<br>Chunks:<br>");
            for (int i = 0; i < 10; i++)
            {
                System.Threading.Thread.Sleep(500);
                Response.Write(i + "<br>");
            }
        }

        // Streaming Download, write extra data:
        if (Request.QueryString["extraData"] != null)
        {
            Response.Write("<br>Filler Data:<br>");
            int streamLength = Int32.Parse(Request.QueryString["extraData"]);
            for (int i = 0; i < streamLength; i++)
            {
                Response.Write("@");
            }
            Response.Write("<br>");
        }
    }
</script>
