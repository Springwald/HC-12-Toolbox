# HC-12-Toolbox

A small helper to set up a (https://amzn.to/4teBKLF)[HC-12 RC module] using a ESP32.

## Usage

1. Connect the HC-12 to the ESP32 as follows (or change the pin definitions in [HC-12-Setup-Tool.ino](HC-12-Setup-Tool/HC-12-Setup-Tool.ino)):

   | HC-12 Pin | ESP32 Pin |
   | --------- | --------- |
   | VCC       | 3.3V      |
   | GND       | GND       |
   | RX        | GPIO 19   |
   | TX        | GPIO 18   |
   | SET       | GPIO 5    |

2. Upload the [HC-12-Setup-Tool.ino](HC-12-Setup-Tool/HC-12-Setup-Tool.ino) sketch to your ESP32.

3. Open the Serial Monitor (Ctrl + Shift + M) and set the baud rate to 115200 baud.

4. Follow the prompts in the Serial Monitor to configure your HC-12 module.

## Thanks

Much of the knowledge for this project was taken from [Wolles Elektronikkiste - Die wunderbare Welt der Elektronik](https://wolles-elektronikkiste.de/hc-12-funkmodul/), which is a great resource for learning about the HC-12 module. Thank you for sharing your knowledge and code!

 