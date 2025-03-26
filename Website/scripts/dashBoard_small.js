const API_URL = 'http://127.0.0.1:8080'; // Change 127.0.0.1 to localhost if you receive post errors

async function sendMessage()
{
	let message = document.getElementById('message').value;
	const message2 = document.getElementById('message2').value;
	const animation = document.querySelector('input[name="animation"]:checked').value;
	const isBig = document.querySelector('input[name="isBig"]:checked').value;

	if (isBig === "no")
	{
		message = message + "," + message2;
	}

	if (!message)
	{
		alert('Please enter a message');
	}
	else{
		const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify(
			{
				"command": animation,
				"isBig": isBig,
				"data": message
			})
		});
	
		if (response.status != 200)
		{
			alert("Failed to connect");
		}
	}
}

async function killServer()
{
	if (confirm('Are you sure you want to stop the server?'))
	{
		const response = await fetch(`${API_URL}/kill`)

		if (response.status === 200)
		{
			alert('Server stopped successfully');
			document.querySelector('.control-panel').innerHTML = '<h1>Server Stopped</h1><p>The server has been shutdown. Please close this window.</p>';
		}
		else
		{
			alert("Connection to server failed");
		}
	}
}


async function start_timer()
{

	const minutes = document.getElementById('minutes').value;
	const seconds = document.getElementById('seconds').value;
	let sflag = true;
	if(parseInt(minutes) > 99)
		{
		alert("Max minutes allowed are 99");
		sflag = false;
	}
	
	if(parseInt(seconds) > 59)
	{
		alert("Max seconds are 59");
		sflag = false;
	}

    if(sflag)
		{
		const message = minutes + ":" + seconds;
		const response = await fetch(`${API_URL}/dashboard/post`,
			{
				method: 'POST',
				headers: {"Content-Type": "application/json"},
				body: JSON.stringify(
				{
					"command": "startTimer",
					"isBig": "yes",
					"data": message.toString()
				})
			});
	
		if (response.status != 200)
		{
			alert('Failed to send message');
		}
	}
}

async function pause_timer()
{

	const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify(
			{
				"command": "pauseTimer"
			})
		});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
}

async function reset_timer()
{

	const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify(
			{
				"command": "resetTimer"
			})
		});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
}

async function display_time()
{
	const currentTime = new Intl.DateTimeFormat('en-US', {
		hour: 'numeric',
		minute: 'numeric',
		hour12: true
	  }).format(new Date());

	const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify(
			{
				"command": "displayTime",
				"isBig": "yes",
				"data": currentTime.toString()
			})
		});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
	
}

const all_preset_btns = document.querySelectorAll('#preset_btns');
all_preset_btns.forEach(btn => 
	{
	btn.addEventListener('click', (e) => 
		{
		e.preventDefault();
		let time_val = btn.innerHTML;
		let data = time_val.split(':');
		document.getElementById('minutes').value = data[0];
	    document.getElementById('seconds').value = data[1];
	});
});

async function send_settings()
{
	const brightness_value = document.getElementById('brightnessSlider').value;
	const top_color = document.getElementById('topTextcolour').value;
	const bottom_color = document.getElementById('bottomTextcolour').value;
    const full_text_color = document.getElementById('fullScreenTextcolour').value;

	const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify(
			{
				"command": "settings",
				"brightness": brightness_value,
				"tcolor": top_color,
				"bcolor": bottom_color,
				"fcolor": full_text_color,
			})
		});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
	
}

function handle_change(val)
{
	
	const slider = document.querySelector('.brightness-value');
	slider.innerHTML = val;
}