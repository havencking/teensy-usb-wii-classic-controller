/*   Copyright December 2015 - havencking@gmail.com
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This is for a Wii Classic Controller adapted to connect as a joystick over
 * USB using a Teensy LC.  See http://havencking.blogspot.com for more info.
 *
 * When compiling using the Arduino IDE, you must select Joystick from the
 * "Tools > USB Type" menu.
 *
 * For more info about the Wii Classic Controller, see:
 * http://wiibrew.org/wiki/Wiimote/Extension_Controllers/Classic_Controller
 */

// the improved Wire I2C library
// see:  https://www.pjrc.com/teensy/td_libs_Wire.html
#include "i2c_t3.h"

// an array to store the six bytes of controller status
int buttons[6];

// the analog controls
int LX;
int LY;
int LT;
int RX;
int RY;
int RT;

// the digital controls
bool BDR;
bool BDD;
bool BLT;
bool Bminus;
bool BH;
bool Bplus;
bool BRT;
bool BZL;
bool BB;
bool BY;
bool BA;
bool BX;
bool BZR;
bool BDL;
bool BDU;

void setup() {
  // initialize Wii I2C using "the new way"
  // see: http://wiibrew.org/wiki/Wiimote/Extension_Controllers
  Wire.begin();
  Wire.beginTransmission(0x52); // Wii Classic 7-bit I2C address is 0x52
  Wire.write(0xF0);
  Wire.write(0x55);
  Wire.endTransmission();
  Wire.beginTransmission(0x52);
  Wire.write(0xFB);
  Wire.write(0x00);
  Wire.endTransmission();

  // initialize Joystick
  // see: https://www.pjrc.com/teensy/td_joystick.html
  Joystick.useManualSend(true);
}

void loop() {
  // request the 6 bytes of controller status and store in the buttons array
  Wire.requestFrom(0x52, 6);
  while (Wire.available()) {
    for (int i = 0; i < 6; i++) {
      buttons[i] = Wire.receive();
    }

    // parse the controller data
    // analog controls
    LX = buttons[0] & B00111111;
    LY = buttons[1] & B00111111;
    LT = ((buttons[2] >> 2) & B00011000) | ((buttons[3] >> 5) & B00000111);
    RX = ((buttons[0] >> 3) & B00011000) | ((buttons[1] >> 5) & B00000110) | ((buttons[2] >> 7) & B00000001);
    RY = buttons[2] & B00011111;
    RT = buttons[3] & B00011111;

    // digital controls
    BDR = (buttons[4] >> 7) & B00000001;
    BDD = (buttons[4] >> 6) & B00000001;
    BLT = (buttons[4] >> 5) & B00000001;
    Bminus = (buttons[4] >> 4) & B00000001;
    BH = (buttons[4] >> 3) & B00000001;
    Bplus = (buttons[4] >> 2) & B00000001;
    BRT = (buttons[4] >> 1) & B00000001;
    BZL = (buttons[5] >> 7) & B00000001;
    BB = (buttons[5] >> 6) & B00000001;
    BY = (buttons[5] >> 5) & B00000001;
    BA = (buttons[5] >> 4) & B00000001;
    BX = (buttons[5] >> 3) & B00000001;
    BZR = (buttons[5] >> 2) & B00000001;
    BDL = (buttons[5] >> 1) & B00000001;
    BDU = buttons[5] & B00000001;

    // set the status of the controls mapped as a USB Joystick
    // see: https://www.pjrc.com/teensy/td_joystick.html
    // analog controls
    Joystick.X(LX);// / 64 * 1024);        // "value" is from 0 to 1023
    Joystick.Y(LY);// / 64 * 1024);        //   512 is resting position
    Joystick.Z(RX);// / 32 * 1024);
    Joystick.Zrotate(RY);// / 32 * 1024);
    Joystick.sliderLeft(LT);// / 32 * 1024);
    Joystick.sliderRight(RT);// / 32 * 1024);
    // digital controls
    // renumber these buttons as you see fit
    Joystick.button(15, !BDR);
    Joystick.button(13, !BDD);
    Joystick.button(5, !BLT);
    Joystick.button(9, !Bminus);
    Joystick.button(11, !BH);
    Joystick.button(10, !Bplus);
    Joystick.button(6, !BRT);
    Joystick.button(7, !BZL);
    Joystick.button(2, !BB);
    Joystick.button(1, !BY);
    Joystick.button(3, !BA);
    Joystick.button(4, !BX);
    Joystick.button(8, !BZR);
    Joystick.button(14, !BDL);
    Joystick.button(12, !BDU);

    // send the controls over USB as a Joystick
    Joystick.send_now();
  }
  // restart at the beginning of the address space
  Wire.beginTransmission(0x52);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(1); // don't read too fast
}
