const API_URL = 'http://127.0.0.1:8080'; // Change 127.0.0.1 to localhost if you receive post errors

async function sendMessage()
{
	let message = document.getElementById('message').value;
	let message2 = document.getElementById('message2').value;
	let animation = document.querySelector('input[name="animation"]:checked').value;
	let isBig = document.querySelector('input[name="isBig"]:checked').value;
    let send = document.getElementById("btn_send");


	if (isBig === "no")
	{
			message = message + "," + message2;
	}

	if (animation == "scroll")
	{
		animation = document.querySelector('input[name="scrollType"]:checked').value;
	}

	if (!message)
	{
		alert('Please enter a message');
	}
	else
	{
		send.disabled = true;
		send.style.cursor = "not-allowed";
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
		setTimeout(function(){
			send.disabled = false;
			send.style.cursor = "pointer";
		}, 4000)
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
	let s_timer = document.querySelector('.btn-start');
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
		s_timer.disabled = true;
		s_timer.style.cursor = "not-allowed";
		const response = await fetch(`${API_URL}/dashboard/post`,
		{
			method: 'POST',
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify(
			{
				"command": "sTimer",
				"isBig": "yes",
				"data": message.toString()
			})
		});
	
		if (response.status != 200)
		{
			alert('Failed to send message');
		}
		setTimeout(function(){
			s_timer.disabled = false;
			s_timer.style.cursor = "pointer";
		}, 4000)
	}
}

async function pause_and_resume_timer()
{
	let p_timer = document.querySelector('.btn-pause-resume');
	let toggle = "";
	if(p_timer.textContent === "Pause"){
		toggle = "pTimer";
		p_timer.textContent = "Resume";
		p_timer.style.background = "green";
	}
	else{
		toggle = "resume";
		p_timer.textContent = "Pause";
		p_timer.style.background = "rgb(120, 120, 27)";
	}
	p_timer.disabled = true;
	p_timer.style.cursor = "not-allowed";
	const response = await fetch(`${API_URL}/dashboard/post`,
	{
		method: 'POST',
		headers: {"Content-Type": "application/json"},
		body: JSON.stringify(
		{
			"command": toggle
		})
	});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
	setTimeout(function(){
		p_timer.disabled = false;
		p_timer.style.cursor = "pointer";
	}, 4000)
}

async function reset_timer()
{
	let r_timer = document.querySelector('.btn-reset');
    r_timer.disabled = true;
	r_timer.style.cursor = "not-allowed";
	const response = await fetch(`${API_URL}/dashboard/post`,
	{
		method: 'POST',
		headers: {"Content-Type": "application/json"},
		body: JSON.stringify(
		{
			"command": "rTimer"
		})
	});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
	setTimeout(function(){
		r_timer.disabled = false;
		r_timer.style.cursor = "pointer";
	}, 4000)
}

async function display_time()
{
	let d_time = document.getElementById("timeOfDayBtn");
    d_time.disabled = true;
	d_time.style.cursor = "not-allowed";
	const response = await fetch(`${API_URL}/dashboard/post`,
	{
		method: 'POST',
		headers: {"Content-Type": "application/json"},
		body: JSON.stringify(
		{
			"command": "dot"
		})
	});

	if (response.status != 200)
	{
		alert('Failed to send message');
	}
	setTimeout(function(){
		d_time.disabled = false;
		d_time.style.cursor = "pointer";
	}, 4000)
	
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
	let set_btn = document.querySelector('.save-button');
	const brightness_value = document.getElementById('brightnessSlider').value;
	const top_color = document.getElementById('topTextcolour').value;
	const bottom_color = document.getElementById('bottomTextcolour').value;
    const full_text_color = document.getElementById('fullScreenTextcolour').value;
    set_btn.disabled = true;
	set_btn.style.cursor = "not-allowed";
	const response = await fetch(`${API_URL}/dashboard/post`,
	{
		method: 'POST',
		headers: {"Content-Type": "application/json"},
		body: JSON.stringify(
		{
			"command": "settns",
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
	setTimeout(function()
	{
		set_btn.disabled = false;
		set_btn.style.cursor = "pointer";
	}, 4000)
	
}

function handle_change(val)
{
	
	const slider = document.querySelector('.brightness-value');
	slider.innerHTML = val;
}