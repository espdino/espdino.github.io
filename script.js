// DOM Elements
const elements = {
    connectButton: document.getElementById("connectBleButton"),
    disconnectButton: document.getElementById("disconnectBleButton"),
    upButton: document.getElementById("upButton"),
    downButton: document.getElementById("downButton"),
    bleStateContainer: document.getElementById("bleState"),
    timestampContainer: document.getElementById("timestamp"),
    playButton: document.getElementById("playButton"),
    overlay: document.getElementById("overlay"),
    brightnessPopup: document.getElementById("brightnessPopup"),
    brightnessSlider: document.getElementById("brightnessSlider"),
    brightnessValue: document.getElementById("brightnessValue")
};

// BLE Device Specs
const BLE_DEVICE = {
    name: "Dino",
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    upButtonChar: "19b10002-e8f2-537e-4f6c-d104768a1214",
    downButtonChar: "19b10001-e8f2-537e-4f6c-d104768a1214"
};

// Global BLE Variables
let bleServer, bleServiceFound, downButtonCharacteristicFound;

// Event Listeners
elements.connectButton.addEventListener("click", () => isWebBluetoothEnabled() && connectToDevice());
elements.disconnectButton.addEventListener("click", disconnectDevice);
elements.playButton.addEventListener("click", function handler() { togglePopup(true); elements.playButton.removeEventListener("click", handler); });
elements.brightnessSlider.addEventListener("input", updateBrightness);
elements.brightnessValue.addEventListener("input", updateBrightness);

elements.upButton.addEventListener("pointerdown", () => writeButtonCharacteristic(BLE_DEVICE.upButtonChar, 1));
elements.upButton.addEventListener("pointerup", () => writeButtonCharacteristic(BLE_DEVICE.upButtonChar, 0));
elements.downButton.addEventListener("pointerdown", () => writeButtonCharacteristic(BLE_DEVICE.downButtonChar, 1));
elements.downButton.addEventListener("pointerup", () => writeButtonCharacteristic(BLE_DEVICE.downButtonChar, 0));

document.addEventListener("keydown", handleKeyDown);
document.addEventListener("keyup", handleKeyUp);

// Popup Functions
function togglePopup(show) {
    elements.overlay.style.display = show ? "block" : "none";
    elements.brightnessPopup.style.display = show ? "block" : "none";
    if (!show) writeButtonCharacteristic(BLE_DEVICE.downButtonChar, 200);
}

function updateBrightness() {
    elements.brightnessValue.value = elements.brightnessSlider.value;
    writeButtonCharacteristic(BLE_DEVICE.upButtonChar, parseInt(elements.brightnessSlider.value));
}

// Keyboard Controls
let keysPressed = { ArrowUp: false, ArrowDown: false };

function handleKeyDown(event) {
    if (event.key !== "ArrowUp" && event.key !== "ArrowDown") return; // Ignore other keys

    if (!keysPressed[event.key]) { // Prevent redundant writes
        keysPressed[event.key] = true;
        const characteristic1 = event.key === "ArrowUp" ? BLE_DEVICE.upButtonChar : BLE_DEVICE.downButtonChar;
        if (characteristic1) {
            writeButtonCharacteristic(characteristic1, 1);
        }
    }
}

function handleKeyUp(event) {
    if (event.key !== "ArrowUp" && event.key !== "ArrowDown") return; // Ignore other keys

    keysPressed[event.key] = false;
    const characteristic2 = event.key === "ArrowUp" ? BLE_DEVICE.upButtonChar : BLE_DEVICE.downButtonChar;
    if (characteristic2) {
        writeButtonCharacteristic(characteristic2, 0);
        writeButtonCharacteristic(characteristic2, 0);
        writeButtonCharacteristic(characteristic2, 0);
    }
}

// Bluetooth Functions
function isWebBluetoothEnabled() {
    if (!navigator.bluetooth) {
        console.log("Web Bluetooth API is not available in this browser!");
        elements.bleStateContainer.innerHTML = "Web Bluetooth API not available!";
        return false;
    }
    return true;
}

async function connectToDevice() {
    try {
        console.log("Initializing Bluetooth...");
        const device = await navigator.bluetooth.requestDevice({
            filters: [{ name: BLE_DEVICE.name }],
            optionalServices: [BLE_DEVICE.serviceUUID]
        });
        elements.bleStateContainer.innerHTML = `Connected to ${device.name}`;
        elements.bleStateContainer.style.color = "#24af37";
        device.addEventListener("gattservicedisconnected", onDisconnected);
        
        bleServer = await device.gatt.connect();
        bleServiceFound = await bleServer.getPrimaryService(BLE_DEVICE.serviceUUID);
        
        downButtonCharacteristicFound = await bleServiceFound.getCharacteristic(BLE_DEVICE.downButtonChar);
        downButtonCharacteristicFound.addEventListener("characteristicvaluechanged", handleCharacteristicChange);
        await downButtonCharacteristicFound.startNotifications();
        console.log("Notifications Started.");
    } catch (error) {
        console.error("Connection failed: ", error);
    }
}

function onDisconnected(event) {
    console.log("Device Disconnected:", event.target.device.name);
    elements.bleStateContainer.innerHTML = "Device disconnected";
    elements.bleStateContainer.style.color = "#d13a30";
    connectToDevice();
}

function handleCharacteristicChange(event) {
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Characteristic value changed: ", newValueReceived);
    elements.timestampContainer.innerHTML = new Date().toLocaleString();
}

async function writeButtonCharacteristic(characteristicUUID, value) {
    if (!bleServer || !bleServer.connected) {
        alert("Bluetooth not connected. Connect first!");
        return;
    }
    try {
        const characteristic = await bleServiceFound.getCharacteristic(characteristicUUID);
        await characteristic.writeValue(new Uint8Array([value]));
        console.log(`Value written to ${characteristicUUID}:`, value);
    } catch (error) {
        console.error("Error writing characteristic: ", error);
    }
}

async function disconnectDevice() {
    if (bleServer && bleServer.connected) {
        try {
            if (downButtonCharacteristicFound) await downButtonCharacteristicFound.stopNotifications();
            await bleServer.disconnect();
            elements.bleStateContainer.innerHTML = "Device Disconnected";
            elements.bleStateContainer.style.color = "#d13a30";
        } catch (error) {
            console.error("Error during disconnect: ", error);
        }
    } else {
        alert("Bluetooth is not connected.");
    }
}
