const source = new EventSource('/events');

const distanceEl = document.getElementById('distance');
const box = document.getElementById('distance-box');

const yellowInput = document.getElementById('yellow-input');
const redInput = document.getElementById('red-input');

source.onmessage = (e) => {
    const data = JSON.parse(e.data);
    const distance = data.distance;

    distanceEl.innerText = distance.toFixed(2);

    const yellow = parseInt(yellowInput.value);
    const red = parseInt(redInput.value);

    // reset color
    box.classList.remove('bg-green-500', 'bg-yellow-500', 'bg-red-500');

    if (!isNaN(yellow) && !isNaN(red)) {
        if (distance <= red) {
            box.classList.add('bg-red-500');
        } else if (distance <= yellow) {
            box.classList.add('bg-yellow-500');
        } else {
            box.classList.add('bg-green-500');
        }
    } else {
        box.classList.add('bg-green-500');
    }
};

function validateInputs() {
    const yellow = parseInt(yellowInput.value);
    const red = parseInt(redInput.value);

    if (isNaN(yellow) || isNaN(red)) return;

    if (yellow <= red) {
        yellowInput.setCustomValidity("Yellow must be greater than Red");
        redInput.setCustomValidity("Red must be less than Yellow");
    } else {
        yellowInput.setCustomValidity("");
        redInput.setCustomValidity("");
    }
}