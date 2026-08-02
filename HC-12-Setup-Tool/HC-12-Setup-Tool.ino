
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

int baudrateToHc12 = 9600;
int baudrates[] = {9600, 1200, 2400, 4800, 19200, 38400, 57600, 115200};

const char *errorResult = "ERROR";

void setup()
{
  // start the serial ports
  serialToPc.begin(115200);
  delay(500);

  findOutBaudrateToHc12();
  serialToHc12.begin(baudrateToHc12);
  delay(500); // wait for the HC12

  serialToPc.println("Starting HC-12 setup tool...");

  setHc12SetMode(false); // make sure we are in normal mode before starting, otherwise the HC-12 will not respond to normal serial commands

  while (serialToHc12.available()) // clear any unexpected data coming from the HC-12, otherwise it may interfere with the AT command responses
    auto value = serialToHc12.read();
  while (serialToPc.available()) // clear any unexpected data coming from the PC, otherwise it may interfere with the user input
    auto value = serialToPc.read();

  delay(1000);

  clearSerialToPc();
  showStatus();
  serialToPc.println();
  showHelp();
}

void loop()
{
  // there is unexpected data coming from the HC-12, print it to the PC for debugging
  int counter = 0;
  while (serialToHc12.available() && counter++ < 100)
  {
    auto value = serialToHc12.read();
    serialToPc.write(value);
  }

  // read commands from the PC and execute them
  counter = 0;
  if (serialToPc.available() && counter++ < 100)
  {
    delay(300); // wait for the user to finish typing the command
    String command = serialToPc.readStringUntil('\n');

    processCommand(command);
  }
}

void findOutBaudrateToHc12()
{
  for (int trys = 0; trys < 2; trys++)
  {
    serialToPc.print("Finding out the baudrate of the HC-12...");
    for (int baudrate : baudrates)
    {
      serialToPc.print(".");
      serialToHc12.end();
      delay(100);
      serialToHc12.begin(baudrate);
      serialToHc12.flush();
      delay(100);
      String result = getAtCommandResult("AT+RC", true, false);
      serialToHc12.flush();
      serialToHc12.end();
      serialToPc.println();

      if (result != errorResult)
      {
        baudrateToHc12 = baudrate;
        serialToPc.println(" Found HC-12 baudrate: " + String(baudrate));
        return;
      }
    }
  }
}

void processCommand(String command)
{
  command.trim(); // remove any leading or trailing whitespace
  if (command.startsWith("c-") || command.startsWith("C-"))
  {
    int channel = command.substring(2).toInt();
    if (setChannel(channel))
      showResult("Channel set to " + String(channel));
    else
      showResult("Failed to set channel to " + String(channel));
    return;
  }

  if (command.startsWith("p-") || command.startsWith("P-"))
  {
    int power = command.substring(2).toInt();
    if (setPower(power))
      showResult("Power level set to " + String(power));
    else
      showResult("Failed to set power level to " + String(power));
    return;
  }

  if (command.startsWith("b-") || command.startsWith("B-"))
  {
    int baudrate = command.substring(2).toInt();
    if (setBaud(baudrate))
      showResult("Baud rate set to " + String(baudrate));
    else
      showResult("Failed to set baud rate to " + String(baudrate));
    return;
  }

  if (command.equalsIgnoreCase("s"))
  {
    clearSerialToPc(); // clear the terminal to make it easier to read the output
    showStatus();
    serialToPc.println();
    showHelp();
    return;
  }

  showResult("Unknown command '" + command + "'. Please enter a valid command.");
}

void showResult(String result)
{
  clearSerialToPc(); // clear the terminal to make it easier to read the output
  showHelp();
  serialToPc.println();
  serialToPc.println("###### ");
  serialToPc.println("# " + result);
  serialToPc.println("###### ");
  serialToPc.println();
}

void showHelp()
{
  serialToPc.println("Available power levels: (in germany the allowed power levels are 1-4, in other countries it may be 1-8)");
  serialToPc.println("    | 1      | 2      | 3      | 4      | 5       | 6      | 7      | 8      |");
  serialToPc.println("    |--------+--------+--------+--------+---------+--------+--------+--------|");
  serialToPc.println("    | -1 dBm | 2 dBm  | 5 dBm  | 8 dBm  | 11 dBm  | 14 dBm | 17 dBm | 20 dBm |");
  serialToPc.println("    | 0.8mW  | 1.6 mW | 3.2 mW | 6.3 mW | 12.0 mW | 25 mW  | 50 mW  | 100 mW |");
  serialToPc.println();
  serialToPc.println("Available commands:");
  serialToPc.println("    S         - show the firmware version and status of the HC-12");
  serialToPc.println("    C-<1-100> - set channel, e.g. C-1 (in germany the allowed channels are 1-4)");
  serialToPc.println("    P-<1-8>   - set power level, e.g. P-1 ");
  serialToPc.println("    B-<baudrate> - set baud rate, e.g. B-9600");
  serialToPc.println("    M-<1-4>   - set FU mode, e.g. M-1");
  delay(100);
}

void showStatus()
{
  serialToPc.println("Current HC-12 status:");
  serialToPc.println("    Firmware: " + getAtCommandResult("AT+V", false, false));
  serialToPc.print("    Channel: " + getAtCommandResult("AT+RC", true, false));
  serialToPc.print(" / Baud Rate: " + getAtCommandResult("AT+RB", true, false));
  serialToPc.print(" / Mode: " + getAtCommandResult("AT+RF", true, false));
  serialToPc.println(" / Power: " + getAtCommandResult("AT+RP", true, false));
  delay(100);
}

void clearSerialToPc()
{
  for (int i = 0; i < 40; i++)
    serialToPc.println();
  serialToPc.println("---------------------------------------------------------------"); // fallback for terminals that do not support the clear screen command
  /*
  // removed non working clear chars:
  serialToPc.write(27);                                                                  // ESC command
  serialToPc.print("[2J");                                                               // clear screen command
  serialToPc.write(27);
  serialToPc.print("[H"); // cursor to home command
  */
  serialToPc.println();
  serialToPc.println();
}

/// @brief Set the HC-12 module to AT command mode or normal mode
/// @param on true to enter AT command mode, false to enter normal mode
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

/// @brief Set the FU mode of the HC-12 module
/// @param mode The FU mode to set (1-4)
/// @return true if the FU mode was set successfully, false otherwise
bool setMode(int mode)
{
  if (mode < 1 || mode > 4)
  {
    serialToPc.println("Bad request. Allowed modes: 1-4");
    return false;
  }

  // the AT command to set the mode is "AT+Fx" where x is the mode, for example "AT+F1" for mode 1, "AT+F2" for mode 2 etc..
  auto command = "AT+F" + String(mode, DEC);
  auto result = getAtCommandResult(command, true, false);

  return result != errorResult;
}

/// @brief Set the channel of the HC-12 module
/// @param channel The channel to set (1-100)
/// @return true if the channel was set successfully, false otherwise
bool setChannel(int channel)
{
  if (channel < 1 || channel > 100)
  {
    serialToPc.println("Bad request. Allowed channels: 1-100");
    return false;
  }

  // the AT command to set the channel is "AT+Cxxx" where xxx is the channel number with leading zeros, for example "AT-C001" for channel 1, "AT-C002" for channel 2 etc..
  auto param = String(channel, DEC);
  while (param.length() < 3)
    param = "0" + param;
  auto command = "AT+C" + param;
  auto result = getAtCommandResult(command, true, false);

  return result != errorResult;
}

/// @brief Set the baud rate of the HC-12 module
/// @param baudrate The baud rate to set (1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
/// @return true if the baud rate was set successfully, false otherwise
bool setBaud(int baudrate)
{
  bool baudrateAllowed = false;
  for (int baud : baudrates)
  {
    if (baud == baudrate)
    {
      baudrateAllowed = true;
      break;
    }
  }
  if (!baudrateAllowed)
  {
    serialToPc.println("Bad request. Allowed baud rates: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200");
    return false;
  }

  // the AT command to set the baud rate is "AT+Bx" where x is the baud rate, for example "AT+B1200" for 1200 baud, "AT+B2400" for 2400 baud etc..
  auto command = "AT+B" + String(baudrate, DEC);
  auto result = getAtCommandResult(command, true, false);

  serialToHc12.end();                 // close the serial port to the HC-12, otherwise it will not be able to change the baud rate
  findOutBaudrateToHc12();            // update the baudrateToHc12 variable to the new baud rate
  serialToHc12.begin(baudrateToHc12); // reinitialize the serial port to the new baud rate

  return result != errorResult;
}

/// @brief Set the power level of the HC-12 module
/// @param power The power level to set (1-8)
/// @return true if the power level was set successfully, false otherwise
bool setPower(int power)
{
  if (power < 1 || power > 8)
  {
    serialToPc.println("Bad request. Allowed power levels: 1-8");
    return false;
  }

  // the AT command to set the power level is "AT+Px" where x is the power level, for example "AT-P1" for power level 1, "AT-P2" for power level 2 etc..
  auto command = "AT+P" + String(power, DEC);
  auto result = getAtCommandResult(command, true, false);

  return result != errorResult;
}

/// @brief Send an AT command to the HC-12 module and get the result
/// @param command The AT command to send
/// @return The result of the AT command, when the command is executed successfully, otherwise the result is "ERROR"
String getAtCommandResult(String command, bool needsOk, bool silent)
{
  String result = "";
  setHc12SetMode(true);
  serialToHc12.print(command); // read the firmware version
  delay(200);
  setHc12SetMode(false);
  delay(100);
  int counter = 0;
  while (serialToHc12.available() && counter++ < 100)
  {
    char value = serialToHc12.read();
    result = result + value;
  }

  if (needsOk)
  {
    if (result.length() == 0 || !result.startsWith("OK+"))
    {
      if (silent != true)
        serialToPc.println("(Failed to execute command: " + command + ". Result: " + result + ")");
      return errorResult;
    }

    // remove the "OK+" prefix from the result
    result = result.substring(3);
  }

  // check if the result is valid, it should start with "OK+" and have some content after that, otherwise return "ERROR"
  // trim any leading or trailing whitespace from the result
  result.trim();

  // remove linebreaks from the result
  result.replace("\n", "");
  result.replace("\r", "");
  result.replace("�", ""); // remove other non printable characters from the result

  return result;
}
