$url = "http://127.0.0.1:8080/";
$http = [System.Net.HttpListener]::new();
$http.Prefixes.Add($url);
$http.Start();

# Serial port setup 
$enteredPort = Read-Host "Enter Comm PORT"
$signPort = "COM $enteredPort"
$baudRate = Read-Host "Enter Baud Rate"


$out_port_a = new-Object System.IO.Ports.SerialPort $signPort,$baudRate,None,8,one
$out_port_a.open() 

# Log ready message to terminal
if ($http.IsListening) 
{
    write-host "Connected to the Scoreboard!"
    write-host "Please Minimize this Window!"
}

# Helper functions
function send_display($data)
{
    $out_port_a.WriteLine($data)
    write-host($data)
}

function pad-text($value)
{
    
    if($value.command -eq "pauseTimer")
    {
        return  "$" + $value.command
    }
    elseif($value.command -eq "resetTimer")
    {
        return  "$" + $value.command
    }
    elseif($value.command -eq "settings")
    {
        return "$" + $value.command + "[" + $value.brightness + ", " +  $value.tcolor + ", " +  $value.bcolor + ", " + $value.fcolor + "]"
    }
    elseif($value.command -eq "custom")
    {
        return "$" + $value.command + "[" + $value.data + "]"
    }
    else
    {
        return "$" + $value.command + "$" + $value.display + "[" + $value.data + "]"
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