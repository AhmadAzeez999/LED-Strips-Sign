document.addEventListener('DOMContentLoaded', function() 
{
    const pixelBoard = document.getElementById('pixelBoard');
    const colorPicker = document.getElementById('colorPicker');
    const eraserBtn = document.getElementById('eraserBtn');
    const clearBtn = document.getElementById('clearBtn');
    const sendBtn = document.getElementById('sendBtn');
    
    let currentColor = '#000000';
    let isDrawing = false;
    let isErasing = false;
    let drawnPixels = new Set();

    
    // Create pixel board
    function createPixelBoard()
    {
        pixelBoard.innerHTML = '';
        
        for (let row = 0; row < 15; row++)
        {
            for (let col = 0; col < 60; col++)
            {
                const pixel = document.createElement('div');
                pixel.className = 'pixel';
                pixel.dataset.row = row;
                pixel.dataset.col = col;
                pixel.dataset.color = currentColor;
                pixel.style.backgroundColor = '#ffffff';
                
                pixel.addEventListener('mousedown', startDrawing);
                pixel.addEventListener('mouseover', draw);
                pixel.addEventListener('touchstart', handleTouch);
                pixel.addEventListener('touchmove', handleTouchMove);
                
                pixelBoard.appendChild(pixel);
            }
        }
        
        pixelBoard.addEventListener('mouseup', stopDrawing);  // This had document.listener
        pixelBoard.addEventListener('touchend', stopDrawing); // This had document.listener
    }
    
    function startDrawing(e)
    {
        isDrawing = true;
        draw(e);
    }
    
    function draw(e)
    {
        if (!isDrawing)
            return;
        let pixel = e.target;
        let row = pixel.dataset.row;
        let col = pixel.dataset.col;
        // let color = pixel.dataset.color;
        let prevColor = pixel.style.backgroundColor;
        if (isErasing)
        {
            e.target.style.backgroundColor = '#ffffff';
            drawnPixels.delete(`(${row},${col},${prevColor})`);
        }
        else
        {
            e.target.style.backgroundColor = currentColor;
            if (prevColor === "rgb(0,0,0)"){
                drawnPixels.add(`(${row},${col},#ffffff)`); // If the current color of this pixel is black, you add it as white
            }
            else{
                drawnPixels.add(`(${row},${col},${currentColor})`);
            }
     
        }
    }
    
    function stopDrawing()
    {
        isDrawing = false;
    }
    
    function handleTouch(e)
    {
        e.preventDefault();
        startDrawing(e);
    }
    
    function handleTouchMove(e)
    {
        e.preventDefault();
        const touch = e.touches[0];
        const pixel = document.elementFromPoint(touch.clientX, touch.clientY);
        
        if (pixel && pixel.classList.contains('pixel'))
        {
            let row = pixel.dataset.row;
            let col = pixel.dataset.col;
            // let color = pixel.dataset.color;
            let prevColor = pixel.style.backgroundColor;
            if (isErasing)
            {
                pixel.style.backgroundColor = '#ffffff';
                drawnPixels.delete(`(${row},${col},${prevColor})`);
            }
            else
            {
                pixel.style.backgroundColor = currentColor;
                if (color === "#000000"){
                    drawnPixels.add(`(${row},${col},#ffffff)`); // If the current color of this pixel is black, you add it as white
                }
                else{
                    drawnPixels.add(`(${row},${col},${currentColor})`);
                }
            }
        }
    }
    
    // Color picker event
    colorPicker.addEventListener('input', function(e)
    {
        currentColor = e.target.value;
        isErasing = false;
        eraserBtn.style.backgroundColor = '#333';
    });
    
    // Eraser button
    eraserBtn.addEventListener('click', function()
    {
        isErasing = !isErasing;
        if (isErasing)
        {
            eraserBtn.style.backgroundColor = '#f7c331';
        }
        else
        {
            eraserBtn.style.backgroundColor = '#e9e9e9';
        }
    });
    
    // Clear button
    clearBtn.addEventListener('click', function()
    {
        const pixels = document.querySelectorAll('.pixel');
        pixels.forEach(pixel =>
        {
            pixel.style.backgroundColor = '#ffffff';
        });
        drawnPixels.clear();
    });
    
    sendBtn.addEventListener('click', async (e) => {
        e.preventDefault();
        sendBtn.disabled = true;
        sendBtn.style.cursor = "not-allowed";
        let f_list = Array.from(drawnPixels).join(',');
        const resp =  await fetch(`${API_URL}/dashboard/post`,
            {
                method: 'POST',
                headers: {"Content-Type": "application/json"},
                body: JSON.stringify(
                {
                    "command": "custom",
                    "data": f_list
                })
        });
    
        if (resp.status != 200)
        {
            alert('Failed to send message');
        }
        setTimeout(function(){
            sendBtn.disabled = false;
            sendBtn.style.cursor = "pointer";
        }, 5000);
        
    });
    // Initialize board
    createPixelBoard();

});