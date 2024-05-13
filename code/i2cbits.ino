

void setup_i2c() {

  Wire.begin(121);                  // Join I2C bus as the slave with address 1
  Wire.onReceive(ReceivedCommand);  // When the data transmition is detected call receiveEvent function
  Wire.onRequest(SendData);

  pinMode(LED_BUILTIN, OUTPUT);  // Sets the DO_Blink as output
}

String requestDataFromMicrobit;

/*Function/Event call****************************************************************************/
void ReceivedCommand(int howMany) {

  String command;

  Serial.print("ReceivedCommand:");

  while (Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
    command.concat(c);
  }

  Serial.println("");

  if (command.startsWith("TEST:1")) {
    digitalWrite(LED_BUILTIN, HIGH);  // Sets the LAD On
  } else if (command.startsWith("TEST:0")) {
    digitalWrite(LED_BUILTIN, LOW);  // Sets the LAD Off
  } else if (command.startsWith("RUMBLE")) {
    playDualRumble();  //Perform controller rumble
  } else if (command.startsWith("LED:")) {

    int Led;

    sscanf(command.c_str(), "LED:%d", &Led);

    Serial.print("setPlayerLEDs:");
    Serial.print(Led);
    Serial.println("");

    setPlayerLEDs(Led);  //Turn on the 4 LED. Each bit represents one LED.

  } else if (command.startsWith("COLOUR:")) {

    uint8_t r, g, b;

    sscanf(command.c_str(), "COLOUR:%u,%u,%u", &r, &g, &b);

    Serial.print("COLOUR:");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.println("");

    setColourLED(r, g, b);  //Turn on the colour LEDs

  } else {
    requestDataFromMicrobit = command;
    Serial.print("requestDataFromMicrobit:");
    Serial.println(command);

    // for (auto myController : myControllers) {
    //   if (myController && myController->isConnected()) {
    //     if (myController->isGamepad()) {
    //       char txt[32];

    //       snprintf(txt, sizeof txt, "%i,%i\n", myController->axisX(), myController->axisY());

    //       Serial.println(txt);
    //     }
    //   }
    // }
  }
}

void SendData() {

  char txt[32];  // respond with message of 32 bytes (the max wire.h supports)

  for (auto myController : myControllers) {
    if (myController && myController->isConnected()) {
      if (myController->isGamepad()) {

        if (requestDataFromMicrobit.startsWith("DPAD")) {

          bool DPAD_UP_PRESSED = 0;
          bool DPAD_DOWN_PRESSED = 0;
          bool DPAD_RIGHT_PRESSED = 0;
          bool DPAD_LEFT_PRESSED = 0;

          if (myController->dpad() == DPAD_UP) { DPAD_UP_PRESSED = true; }
          if (myController->dpad() == DPAD_DOWN) { DPAD_DOWN_PRESSED = true; }
          if (myController->dpad() == DPAD_RIGHT) { DPAD_RIGHT_PRESSED = true; }
          if (myController->dpad() == DPAD_LEFT) { DPAD_LEFT_PRESSED = true; }

          snprintf(txt, sizeof txt, ">>%d,%d,%d,%d\n", DPAD_UP_PRESSED, DPAD_DOWN_PRESSED, DPAD_RIGHT_PRESSED, DPAD_LEFT_PRESSED);

        } else if (requestDataFromMicrobit.startsWith("BUTTONS")) {
          snprintf(txt, sizeof txt, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", myController->a(), myController->b(), myController->x(), myController->y(),
                   myController->l1(), myController->l2(), myController->r1(), myController->r2(), myController->thumbL(), myController->thumbR(),
                   myController->miscSystem(), myController->miscSelect(), myController->miscStart(), myController->miscCapture());

        } else if (requestDataFromMicrobit.startsWith("AXISL")) {
          snprintf(txt, sizeof txt, "%i,%i\n", myController->axisX(), myController->axisY());
        } else if (requestDataFromMicrobit.startsWith("AXISR")) {
          snprintf(txt, sizeof txt, "%i,%i\n", myController->axisRX(), myController->axisRY());
        } else if (requestDataFromMicrobit.startsWith("TRIGGERS")) {
          snprintf(txt, sizeof txt, "%i,%i\n", myController->brake(), myController->throttle());
        } else if (requestDataFromMicrobit.startsWith("GYRO")) {
          snprintf(txt, sizeof txt, "%i,%i,%i\n", myController->gyroX(), myController->gyroY(), myController->gyroZ());
        } else if (requestDataFromMicrobit.startsWith("ACCEL")) {
          snprintf(txt, sizeof txt, "%i,%i,%i\n", myController->accelX(), myController->accelY(), myController->accelZ());
        }

        Serial.println(txt);
        Wire.write(txt);
      }
    }
  }
}
