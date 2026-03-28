# HC-12-Toolbox

A very small helper to set up a [HC-12 RC module](https://amzn.to/4teBKLF) using an ESP32.

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

<!-- ## Ideas

To configure the HC-12 module in different soldering states, you can connect the board to contacts with multiple configurations.

This might look like the example board shown here:
![ESP32 HC-12 Example Board](images/HC-12-Board.jpg) -->

## To dos:

- [ ] Add more configuration options (e.g., baud rate)
- [ ] Add a web interface for easier configuration
- [ ] Add support for mini Oled displays to show the current configuration
- [ ] Add menu buttons to navigate through the configuration options via Oled display without needing a serial monitor

Feel free to fork this project and add your own configurations or features! I would love to see what you come up with.

## Thanks

Much of the knowledge for this project was taken from [Wolles Elektronikkiste - Die wunderbare Welt der Elektronik](https://wolles-elektronikkiste.de/hc-12-funkmodul/), which is a great resource for learning about the HC-12 module. Thank you Wolfgang for sharing your knowledge and code!

