const status = document.getElementById("status");

const notes =
{
    1: 261.63, // C
    2: 277.18, // C#
    3: 293.66, // D
    4: 311.13, // D#
    5: 329.63, // E
    6: 349.23, // F
    7: 369.99, // F#
    8: 392.00, // G
    9: 415.30, // G#
    10: 440.00, // A
    11: 466.16, // A#
    12: 493.88 // B
};

const AC = window.AudioContext || window.webkitAudioContext;
const audio = new AudioContext();

const playing = {};

document.body.addEventListener("click", () =>
{
    if(audio.state === "suspended")
    {
        audio.resume();
    }
});

function play(key)
{
    if(playing[key])
    {
        return;
    }

    const osc = audio.createOscillator();
    const gain = audio.createGain();

    osc.type = "sine";
    osc.frequency.value = notes[key];

    gain.gain.value = 0.15;

    osc.connect(gain);
    gain.connect(audio.destination);

    osc.start();

    playing[key] = {osc, gain };

}

function stop(key)
{
    if(!playing[key])
    {
        return;
    }

    playing[key].osc.stop();
    playing[key].osc.disconnect();

    delete playing[key];
}

function updateKeys(data)
{
    for(let i = 1; i <= 12; i++)
    {
        const key = document.getElementById("key" + i);

        if (data[i] == 1)
        {
            key.classList.add("active");
            play(i);
        }
        else
        {
            key.classList.remove("active");
            stop(i);
        }
    }

}

document.querySelectorAll(".key").forEach(key =>
{
    const id = parseInt(key.id.replace("key", ""));

    key.addEventListener("mousedown", () =>
    {
        if (audio.state === "suspended")
        {
            audio.resume();
        }

        key.classList.add("active");
        play(id);
    });

    key.addEventListener("mouseup", () =>
    {
        key.classList.remove("active");
        stop(id);
    });

    key.addEventListener("mouseleave", () =>
    {
        key.classList.remove("active");
        stop(id);
    });

    key.addEventListener("touchstart", (e) =>
    {
        if (audio.state === "suspended")
        {
            audio.resume();
        }
        
        e.preventDefault();
        key.classList.add("active");
        play(id);
    });

    key.addEventListener("touchend", () =>
    {
        key.classList.remove("active");
        stop(id);
    });
});

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

    setTimeout(getKeys, 20);
}

getKeys();