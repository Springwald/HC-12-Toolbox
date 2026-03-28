
#include <SoftwareSerial.h>
#include <string.h>

// ## PIN SETUP

// the VCC pin of the HC-12 should be connected to 3.3V
// the GND pin of the HC-12 should be connected to GND

#define SETPin 5 // connect to the SET pin of the HC-12 used to switch between normal mode and AT command mode
#define RxPin 18 // connect to the Tx pin of the HC-12
#define TxPin 19 // connect to the Rx pin of the HC-12

// define both serial ports, one for the PC and one for the HC-12
auto serialToPc = Serial;
SoftwareSerial serialToHc12(RxPin, TxPin);

void setup()
    serialToPc.begin(115200);
serialToPc.println("Starting HC-12 setup tool...");
serialToHc12.begin(9600);

delay(1000); // wait for the serial ports to initialize

setHc12SetMode(false); // make sure we are in normal mode before starting, otherwise the HC-12 will not respond to normal serial commands
}

void loop()
{
  // there is unexpected data coming from the HC-12, print it to the PC for debugging
  while (serialToHc12.available())
  {
    auto value = serialToHc12.read();
    serialToPc.write(value);
  }

  // read commands from the PC and execute them
  if (serialToPc.available())
  {
    String command = serialToPc.readStringUntil('\n');
    command.trim(); // remove any leading or trailing whitespace
    if (command.startsWith("channel"))
    {
      int channel = command.substring(7).toInt();
      setChannel(channel);
    }
    else if (command == "firmware")
    {
      showFirmware();
    }
    else if (command == "parameters")
    {
      showParameters();
    }
    else
    {
      serialToPc.println("Unknown command. Available commands: channel <1-4>, firmware, parameters");
    }
  }

  void showHelp()
  {
    serialToPc.println("Power Levels:");
    serialToPc.println();
    serialToPc.println("| 1      | 2      | 3      | 4      | 5       | 6      | 7      | 8      |");
    serialToPc.println("|--------+--------+--------+--------+---------+--------+--------+--------|");
    serialToPc.println("| -1 dBm | 2 dBm  | 5 dBm  | 8 dBm  | 11 dBm  | 14 dBm | 17 dBm | 20 dBm |");
    serialToPc.println("| 0.8mW  | 1.6 mW | 3.2 mW | 6.3 mW | 12.0 mW | 25 mW  | 50 mW  | 100 mW |");
    serialToPc.println();
    serialToPc.println();
    serialToPc.println("Available commands:");
    serialToPc.println();
    serialToPc.println("C<1-4> - set the channel of the HC-12");
    serialToPc.println("firmware - show the firmware version of the HC-12");
    serialToPc.println("parameters - show the current parameters of the HC-12");
  }

  void showStatus()
  {
  }

  void setHc12SetMode(bool on)
  {
    if (on == true)
    {
      pinMode(SETPin, OUTPUT);
      // digitalWrite(SETPin,LOW);
    }
    else
    {
      pinMode(SETPin, INPUT);
    }
    delay(50);
  }

  String getAtCommandResult(String command)
  {
    String result = "";
    setSetMode(true);
    rcSerial.print(command); // read the firmware version
    setSetMode(false);
    while (rcSerial.available())
    {
      char value = rcSerial.read();
      result = result + value;
    }
    return result;
  }

  void setChannel(int channel)
  {
    if (channel < 1 || channel > 4)
    {
      Serial.println("allowed channels: 1-4");
      return;
    }

    Serial.print("Set channel to" + String(channel) + ":");
    auto command = "AT+C00" + String(channel); // AT-C001 for channel 1, AT-C002 for channel 2 etc..
    auto result = getAtCommandResult(command);
    Serial.print(result);
  }

  void showFirmware()
  {
    Serial.print("Firmware: ");
    Serial.println(getAtCommandResult("AT+V"));
  }

  void showParameters()
  {
    Serial.print("Baud Rate: ");
    Serial.print(getAtCommandResult("AT+RB"));
    Serial.print("Channel: ");
    Serial.print(getAtCommandResult("AT+RC"));
    Serial.print("Mode: ");
    Serial.print(getAtCommandResult("AT+RF"));
    Serial.print("Power: ");
    Serial.print(getAtCommandResult("AT+RP"));
  }
