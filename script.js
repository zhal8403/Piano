const status = document.getElementById("status");

const notes = {1: 261.63, 2: 293.66, 3: 329.63, 4: 349.23};

function updateKeys(data)
{
    for(let i = 1; i <= 4; i++)
    {
        const key = document.getElementById("key" + i);

        if (data[i] == 1)
        {
            key.classList.add("active");
        }
        else
        {
            key.classList.remove("active");
        }
    }
}

async function getKeys()
{
    try
    {
        const response = await fetch("/keys");
        const data = await response.json();

        status.innerHTML = "Connected";

        updateKeys(data);
    }
    catch
    {
        status.innerHTML = "Disconnected";
    }
}

setInterval(getKeys, 20);