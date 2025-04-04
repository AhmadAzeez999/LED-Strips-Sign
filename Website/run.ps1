
$http = [System.Net.HttpListener]::new();

$server_port = 8080
if(Get-NetTCPConnection -LocalPort $server_port -ErrorAction SilentlyContinue){
    do{
    $server_port = Read-Host "Enter Server Port"
    if ([string]::IsNullOrWhiteSpace($server_port) -or $server_port -match '\D')
    {
        Write-Host "Enter a valid port"
    }
    elseif ([int]::Parse($server_port) -ge 1 -and [int]::Parse($server_port) -le 65535)
    {
        
        if(Get-NetTCPConnection -LocalPort $server_port -ErrorAction SilentlyContinue)
        {
            Write-Host "Port is already in use, enter a different one"
        }
        else
        {
            break
        }
    }
    else
    {
        Write-Host "Invalid Port"
    }
}
while($true)
}


$enteredPort = Read-Host "Enter Comm PORT"
$signPort = "COM$enteredPort"
$baudRate = Read-Host "Enter Baud Rate"
$url = "http://127.0.0.1:$server_port/";
Write-Host $url
$http.Prefixes.Add($url);
$http.Start();

$out_port_a = new-Object System.IO.Ports.SerialPort $signPort,$baudRate,None,8,one
$out_port_a.open()

# Log ready message to terminal
if ($http.IsListening) 
{
    Write-Host "Connected to the Scoreboard!" -ForegroundColor Black -BackgroundColor Green
    Write-Host "Please Minimize this Window!" -ForegroundColor Black -BackgroundColor Green
}

# Helper functions
function send_display($data)

{
    # Uncomment this line when ready to use actual serial port
    $out_port_a.WriteLine($data)
    write-host($data)
}

function pad-text($value)
{
    
    if($value.command -eq "pTimer")
    {
        return  "$" + $value.command + "$"
    }
    elseif($value.command -eq "rTimer")
    {
        return  "$" + $value.command  + "$"
    }
    elseif($value.command -eq "settns")
    {
        return "$" + $value.command + "$" +  "[" + $value.brightness + ", " +  $value.tcolor + ", " +  $value.bcolor + ", " + $value.fcolor + "]"
    }
    elseif($value.command -eq "custom")
    {
        return "$" + $value.command + "$" + $value.param + "[" + $value.data + "]"
    }
    else
    {
        return "$" + $value.command + "$" + $value.isBig + "[" + $value.data + "]"
    }

}

# Function to add CORS headers to response
function Add-CorsHeaders($response) 
{
    $response.AppendHeader("Access-Control-Allow-Origin", "*")
    $response.AppendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
    $response.AppendHeader("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With")
    $response.AppendHeader("Access-Control-Max-Age", "86400")
}

# Main loop
while ($http.IsListening) 
{
    $context = $http.GetContext()
    
    # Handle preflight OPTIONS requests
    if ($context.Request.HttpMethod -eq 'OPTIONS')
    {
  
        Add-CorsHeaders -response $context.Response
        $context.Response.StatusCode = 200
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("")
        $context.Response.ContentLength64 = $buffer.Length
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length)
        $context.Response.OutputStream.Close()
        continue
    }
    
    # Handle POST requests to /dashboard/post
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/dashboard/post')
    {
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd()
        $dataToSend = $FormContent | ConvertFrom-Json
        
        if ($dataToSend.command -eq 'postRaw')
        {
            send_display($dataToSend.data)
        }
        else
        {
            $formattedData = pad-text($dataToSend)
            send_display($formattedData)
        }

        Add-CorsHeaders -response $context.Response
        $context.Response.StatusCode = 200
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("Success")
        $context.Response.ContentLength64 = $buffer.Length
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length)
        $context.Response.OutputStream.Close()
        continue
    }
    
    # Handle GET requests
    if ($context.Request.HttpMethod -eq 'GET' -and $context.Request.RawUrl -eq '/kill')
    {
        Write-Host "Stopping the Server"
        Add-CorsHeaders -response $context.Response
        $context.Response.StatusCode = 200
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("Server stopped")
        $context.Response.ContentLength64 = $buffer.Length
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length)
        $context.Response.OutputStream.Close()
        $http.Stop()
        break
    }
}