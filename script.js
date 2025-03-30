    // DOM Elements
    const connectButton = document.getElementById("connectBleButton");
    const disconnectButton = document.getElementById("disconnectBleButton");
    const upButton = document.getElementById("upButton");
    const downButton = document.getElementById("downButton");
    const retrievedValue = document.getElementById("valueContainer");
    const latestValueSent = document.getElementById("valueSent");
    const bleStateContainer = document.getElementById("bleState");
    const timestampContainer = document.getElementById("timestamp");
    const playButton = document.getElementById("playButton");

    //Define BLE Device Specs
    var deviceName = "Dino";
    var bleService = "19b10000-e8f2-537e-4f6c-d104768a1214";
    var upButtonCharacteristic = "19b10002-e8f2-537e-4f6c-d104768a1214";
    var downButtonCharacteristic = "19b10001-e8f2-537e-4f6c-d104768a1214";

    //Global Variables to Handle Bluetooth
    var bleServer;
    var bleServiceFound;
    var downButtonCharacteristicFound;

    // Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener("click", (event) => {
      if (isWebBluetoothEnabled()) {
        connectToDevice();
      }
    });

    // Disconnect Button
    disconnectButton.addEventListener("click", disconnectDevice);

    playButton.addEventListener("click", () => {
      writeDownButtonCharacteristic(200);
    });

    document.getElementById("adjustBrightnessButton").addEventListener("click", function() {
        this.style.display = "none";
        document.getElementById("overlay").style.display = "block";
        document.getElementById("brightnessPopup").style.display = "block";
      });
      
      function closePopup() {
        document.getElementById("overlay").style.display = "none";
        document.getElementById("brightnessPopup").style.display = "none";
      }
      
      function sendBrightness(value) {
        console.log("Brightness set to:", value);
        writeUpButtonCharacteristic(parseInt(value));
      }
      
      document.getElementById("brightnessSlider").addEventListener("input", function() {
        document.getElementById("brightnessValue").value = this.value;
        sendBrightness(this.value);
      });
      
      document.getElementById("brightnessValue").addEventListener("input", function() {
        document.getElementById("brightnessSlider").value = this.value;
        sendBrightness(this.value);
      });
    
    function startWritingUpButton() {
        writeUpButtonCharacteristic(1);
        writeUpButtonCharacteristic(1);
        writeUpButtonCharacteristic(1);
}

function startWritingDownButton() {
        writeDownButtonCharacteristic(1);
        writeDownButtonCharacteristic(1);
        writeDownButtonCharacteristic(1);
}

function stopWritingUpButton() {
        writeUpButtonCharacteristic(0);
        writeUpButtonCharacteristic(0);
        writeUpButtonCharacteristic(0);
}

function stopWritingDownButton() {
        writeDownButtonCharacteristic(0);
        writeDownButtonCharacteristic(0);
        writeDownButtonCharacteristic(0);
}

// ✅ Use pointer events for better cross-device support
upButton.addEventListener("pointerdown", startWritingUpButton);
upButton.addEventListener("pointerup", stopWritingUpButton);
downButton.addEventListener("pointerdown", startWritingDownButton);
downButton.addEventListener("pointerup", stopWritingDownButton);


let upPressed = false;
let downPressed = false;

document.addEventListener("keydown", (event) => {
    if (event.key === "ArrowUp" && !upPressed) {
        upPressed = true;
        startWritingUpButton();
    }
});

document.addEventListener("keyup", (event) => {
    if (event.key === "ArrowUp") {
        upPressed = false;
        stopWritingUpButton();
    }
});

document.addEventListener("keydown", (event) => {
    if (event.key === "ArrowDown" && !downPressed) {
        downPressed = true;
        startWritingDownButton();
    }
});

document.addEventListener("keyup", (event) => {
    if (event.key === "ArrowDown") {
        downPressed = false;
        stopWritingDownButton();
    }
});
    
    // Check if BLE is available in your Browser
    function isWebBluetoothEnabled() {
      if (!navigator.bluetooth) {
        console.log("Web Bluetooth API is not available in this browser!");
        bleStateContainer.innerHTML =
          "Web Bluetooth API is not available in this browser/device!";
        return false;
      }
      console.log("Web Bluetooth API supported in this browser.");
      return true;
    }

    // Connect to BLE Device and Enable Notifications
    function connectToDevice() {
      console.log("Initializing Bluetooth...");
      navigator.bluetooth
        .requestDevice({
          filters: [{ name: deviceName }],
          optionalServices: [bleService],
        })
        .then((device) => {
          console.log("Device Selected:", device.name);
          bleStateContainer.innerHTML = "Connected to device " + device.name;
          bleStateContainer.style.color = "#24af37";
          device.addEventListener("gattservicedisconnected", onDisconnected);
          return device.gatt.connect();
        })
        .then((gattServer) => {
          bleServer = gattServer;
          console.log("Connected to GATT Server");
          return bleServer.getPrimaryService(bleService);
        })
        .then((service) => {
          bleServiceFound = service;
          console.log("Service discovered:", service.uuid);
          return service.getCharacteristic(downButtonCharacteristic);
        })
        .then((characteristic) => {
          console.log("Characteristic discovered:", characteristic.uuid);
          downButtonCharacteristicFound = characteristic;
          characteristic.addEventListener(
            "characteristicvaluechanged",
            handleCharacteristicChange
          );
          characteristic.startNotifications();
          console.log("Notifications Started.");
          return characteristic.readValue();
        })
        .then((value) => {
          console.log("Read value: ", value);
          const decodedValue = new TextDecoder().decode(value);
          console.log("Decoded value: ", decodedValue);
          retrievedValue.innerHTML = decodedValue;
        })
        .catch((error) => {
          console.log("Error: ", error);
        });
    }

    function onDisconnected(event) {
      console.log("Device Disconnected:", event.target.device.name);
      bleStateContainer.innerHTML = "Device disconnected";
      bleStateContainer.style.color = "#d13a30";

      connectToDevice();
    }

    function handleCharacteristicChange(event) {
      const newValueReceived = new TextDecoder().decode(event.target.value);
      console.log("Characteristic value changed: ", newValueReceived);
      retrievedValue.innerHTML = newValueReceived;
      timestampContainer.innerHTML = getDateTime();
    }

    function writeUpButtonCharacteristic(value) {
      if (bleServer && bleServer.connected) {
        bleServiceFound
          .getCharacteristic(upButtonCharacteristic)
          .then((characteristic) => {
            console.log("Found the LED characteristic: ", characteristic.uuid);
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
          })
          .then(() => {
            latestValueSent.innerHTML = value;
            console.log("Value written to upButtonCharacteristic:", value);
          })
          .catch((error) => {
            console.error("Error writing to the LED characteristic: ", error);
          });
      } else {
        console.error(
          "Bluetooth is not connected. Cannot write to characteristic."
        );
        window.alert(
          "Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!"
        );
      }
    }

    function writeDownButtonCharacteristic(value) {
      if (bleServer && bleServer.connected) {
        bleServiceFound
          .getCharacteristic(downButtonCharacteristic)
          .then((characteristic) => {
            console.log("Found the LED characteristic: ", characteristic.uuid);
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
          })
          .then(() => {
            latestValueSent.innerHTML = value;
            console.log("Value written to upButtonCharacteristic:", value);
          })
          .catch((error) => {
            console.error("Error writing to the LED characteristic: ", error);
          });
      } else {
        console.error(
          "Bluetooth is not connected. Cannot write to characteristic."
        );
        window.alert(
          "Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!"
        );
      }
    }

    function disconnectDevice() {
      console.log("Disconnect Device.");
      if (bleServer && bleServer.connected) {
        if (downButtonCharacteristicFound) {
          downButtonCharacteristicFound
            .stopNotifications()
            .then(() => {
              console.log("Notifications Stopped");
              return bleServer.disconnect();
            })
            .then(() => {
              console.log("Device Disconnected");
              bleStateContainer.innerHTML = "Device Disconnected";
              bleStateContainer.style.color = "#d13a30";
            })
            .catch((error) => {
              console.log("An error occurred:", error);
            });
        } else {
          console.log("No characteristic found to disconnect.");
        }
      } else {
        // Throw an error if Bluetooth is not connected
        console.error("Bluetooth is not connected.");
        window.alert("Bluetooth is not connected.");
      }
    }

    function getDateTime() {
      var currentdate = new Date();
      var day = ("00" + currentdate.getDate()).slice(-2); // Convert day to string and slice
      var month = ("00" + (currentdate.getMonth() + 1)).slice(-2);
      var year = currentdate.getFullYear();
      var hours = ("00" + currentdate.getHours()).slice(-2);
      var minutes = ("00" + currentdate.getMinutes()).slice(-2);
      var seconds = ("00" + currentdate.getSeconds()).slice(-2);

      var datetime =
        day +
        "/" +
        month +
        "/" +
        year +
        " at " +
        hours +
        ":" +
        minutes +
        ":" +
        seconds;
      return datetime;
    }
