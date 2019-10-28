/*
(C) 2015 dingus.dk J.�.N.

This file is part of DFPlayer.

DFPlayer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DFPlayer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with DFPlayer.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------

This is an example connecting to the DFPlayer module with the software serial.
The example will play the first 10 files.
*/
#include <SoftwareSerial.h>
#include <DFPlayer.h>

SoftwareSerial mySerial(10, 11); // RX, TX

DFPlayer player(mySerial);

void setup () {
	pinMode(8, INPUT);
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	Serial.begin (115200);
	Serial.println("Start");

	mySerial.begin (9600);

	delay(1);  //wait 1ms for mp3 module to set volume
	Serial.println("Set Initial Volume");
	player.SetVolume(15);
	delay(1000);
}
//
void loop() {

	for (int i = 1; i < 10; i++) {
		digitalWrite(7, LOW);
		player.Play(i);
		while (player.IsPlaying()) delay(10);
		digitalWrite(7, HIGH);
		Serial.println("");
		delay(2000);
	}
}
