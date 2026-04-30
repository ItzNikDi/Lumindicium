'use strict';

const distanceElement = document.getElementById('distance');
const distanceBox = document.getElementById('distance-box');

const yellowInput = document.getElementById('yellow-input');
const redInput = document.getElementById('red-input');

const yellowVolume = document.getElementById('yellow-volume-input');
const redVolume = document.getElementById('red-volume-input');

let yellowThreshold = 200;
let redThreshold = 30;

function getZone(dist) {
    if (dist < 0) return 'unknown';
    if (dist <= redThreshold) return 'red';
    if (dist <= yellowThreshold) return 'yellow';
    return 'green';
}

const ZONE_CLASSES = ['bg-green-500', 'bg-yellow-500', 'bg-red-500', 'bg-gray-500'];

function render(dist) {
    distanceElement.textContent = dist >= 0 ? dist.toFixed(1) : '--';

    distanceBox.classList.remove(...ZONE_CLASSES);
    switch (getZone(dist)) {
        case 'red':
            distanceBox.classList.add('bg-red-500');
            break;
        case 'yellow':
            distanceBox.classList.add('bg-yellow-500');
            break;
        case 'green':
            distanceBox.classList.add('bg-green-500');
            break;
        default:
            distanceBox.classList.add('bg-gray-500');
            break;
    }
}

function syncInputs(cfg) {
    if (cfg.yellowThreshold != null) {
        yellowThreshold = cfg.yellowThreshold;
        yellowInput.value = cfg.yellowThreshold;
    }
    if (cfg.redThreshold != null) {
        redThreshold = cfg.redThreshold;
        redInput.value = cfg.redThreshold;
    }
    if (cfg.yellowVolume != null) yellowVolume.value = cfg.yellowVolume;
    if (cfg.redVolume != null) redVolume.value = cfg.redVolume;
}

async function postConfig() {
    const y = parseFloat(yellowInput.value);
    const r = parseFloat(redInput.value);

    if (isNaN(y) || isNaN(r) || r <= 0 || y <= 0 || r >= y) {
        console.warn('Invalid thresholds — red must be < yellow and both > 0');
        return;
    }

    const body = new URLSearchParams({
        yellow: y,
        red: r,
        yellowVolume: yellowVolume.value,
        redVolume: redVolume.value,
    });

    try {
        const res = await fetch('/config', {method: 'POST', body});
        const json = await res.json();
        if (!json.ok) console.warn('Config rejected by device:', json.error);
    } catch (err) {
        console.error('Config POST failed:', err);
    }
}

let configTimer = null;

function scheduleConfig(delayMs = 400) {
    clearTimeout(configTimer);
    configTimer = setTimeout(postConfig, delayMs);
}

yellowInput.addEventListener('input', () => scheduleConfig(90));
redInput.addEventListener('input', () => scheduleConfig(90));
yellowVolume.addEventListener('input', () => scheduleConfig(200));
redVolume.addEventListener('input', () => scheduleConfig(200));

function connect() {
    const source = new EventSource("/events");

    source.addEventListener('state_update', (e) => {
        let data;
        try {
            data = JSON.parse(e.data);
        } catch {
            return;
        }

        if (data.config) syncInputs(data.config);
        render(typeof data.distance === 'number' ? data.distance : -1);
    });
}

connect();