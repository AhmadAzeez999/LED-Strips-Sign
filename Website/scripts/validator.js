document.addEventListener("DOMContentLoaded", () =>
{
    const input = document.getElementById("message");
    const charCount = document.getElementById("char-count");
    let maxChars = 8;

    // This is for helping users keep the test they typed for maxChars = 20
    let prevText = "";

    document.querySelectorAll('input[name="display"]').forEach((radio) =>
    {
        radio.addEventListener("change", function ()
        {
            updateMaxCount();
        });
    });

    
    document.querySelectorAll('input[name="animation"]').forEach((radio) =>
    {
        radio.addEventListener("change", function ()
        {
            updateMaxCount();
        });
    });

    input.addEventListener("input", () =>
    {
        updateMaxCount();
    });

    function updateMaxCount()
    {
        const display = document.querySelector('input[name="display"]:checked').value;
        const animation = document.querySelector('input[name="animation"]:checked').value;

        console.log(animation);
        console.log(display);

        if (animation === "statc" ||
            animation === "pause")
        {
            if (display === "f")
                maxChars = 8;
            else if (display === "t" || display === "b")
                maxChars = 20;

            prevText = input.value;
        }
        else if (animation === "scroll")
        {           
            maxChars = 100;

            if (prevText.startsWith(input.value))
            {
                input.value = prevText;
            }
        }

        if (input.value.length > maxChars)
        {
            input.value = input.value.substring(0, maxChars); // Trims excess characters
            charCount.style.color = "red";
        }
        else
        {
            charCount.style.color = "black";
        }

        charCount.textContent = `${input.value.length}/${maxChars}`;
    }
});
