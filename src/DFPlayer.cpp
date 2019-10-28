/*
(C) 2015 dingus.dk J.Ø.N.

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
*/
#include <Arduino.h>
#include <debugtrace.h>
#include <DFPlayer.h>

DFPlayer::DFPlayer(Stream& playerserial) :
	playerserial(playerserial),
	isplaying( false) {

}


void DFPlayer::Send() {
	debug_print("Sending: ");
	for (int i = 0; i<10; i++) {
		playerserial.write(send_buf[i]);
		debug_print2(send_buf[i],16);
		debug_print(",");
	}
	debug_println("");
}

void DFPlayer::SendCmd(byte cmd,bool feedback, int arg) {
	send_buf[3] = cmd;
	send_buf[4] = feedback ? 1 : 0;
	send_buf[5] = arg >> 8;
	send_buf[6] = arg;
	word checksum = GetChecksum(send_buf);
	send_buf[7] = checksum >> 8;
	send_buf[8] = checksum;
	Send();
}

word DFPlayer::GetChecksum( DFPlayer::Buffer& buf) {
	word sum = 0;
	for (int i = 1; i<7; i++) {
		sum += buf[i];
	}
	return -sum;
}

void DFPlayer::Play() {

	SendCmd(0x0d,false,0);
	isplaying = true;
}

void DFPlayer::Play(int num) {

	SendCmd(0x12, true,num);
	if (!OkReturnResponse( 100)) return;
	isplaying = true;
}

void DFPlayer::Pause() {

	SendCmd(0x0e,false,0);
}

void DFPlayer::Stop() {

	SendCmd(0x16,false,0);
}

void DFPlayer::Next() {

	SendCmd(0x01,false,0);
}

void DFPlayer::Previous() {

	SendCmd(0x02,false,0);
}

/*
Volume 0-30
*/
void DFPlayer::SetVolume(int volume) {

	for (int retry = 0; retry < 3; retry++) {
		SendCmd(0x06, true, volume);
		if (OkReturnResponse(50)) {
			if (GetVolume() == volume) return;
		}
		delay(10);
		debug_println("Retrying set volume");
	}
}

int DFPlayer::GetVolume() {

	SendCmd(0x43, false, 0);
	if (!DFPlayer::ReadResponse(1000)) return -1;
	debug_print("Volume is ");
	debug_println(recv_buf[6]);
	return recv_buf[6];
}



void DFPlayer::Sleep() {

	SendCmd(0x0a,false,0);
}

void DFPlayer::Reset() {
	
	SendCmd(0x0c,true,0);
	OkReturnResponse(1000);
}


bool DFPlayer::ReadResponse( int timeout) {

	unsigned long t = millis() + timeout;
	// Start byte
	do {
		do {
			if (millis() > t) return false;
		} while (!playerserial.available());
	} while (playerserial.read() != 0x7E);

	recv_buf[0] = 0x7e;
	debug_print("Response 0x7E,");
	for (int i = 1; i<10; i++) {
		while (!playerserial.available()) {
			if (millis() > t) return false;
		}
		recv_buf[i] = playerserial.read();
		debug_print2(recv_buf[i], 16);
		debug_print(",");
	}
	debug_print( "");
	return true;
}

bool DFPlayer::CheckResponse() {

	word checksum = GetChecksum(recv_buf);
	if (recv_buf[1] != 0xFF) {
		debug_println("Bad version");
		return false;
	}
	if (recv_buf[2] != 6) {
		debug_println("Bad response length");
		return false;
	}
	if (recv_buf[7] != (checksum >> 8) || recv_buf[8] != (checksum & 0xFF)) {
		debug_println("Bad response checksum ");
		return false;
	}
	return true;
}

bool DFPlayer::OkReturnResponse( int timeout) {

	if (!ReadResponse( timeout)) return false;
	if (!CheckResponse()) return false;
	if (recv_buf[3] != 0x41) return false;
	if (recv_buf[4] != 0x00) return false;
	if (recv_buf[5] != 0x00) return false;
	if (recv_buf[6] != 0x00) return false;
	return true;
}

bool DFPlayer::IsPlaying() {

	if (!playerserial.available()) return  isplaying;
	if (ReadResponse( 100)) {
		if (CheckResponse()) {
			if (recv_buf[3] == 0x3D || recv_buf[3] == 0x40) {
				isplaying = false;
				// Eat the extra
				ReadResponse(100);
				debug_println("Stopped playing");
			}
		}
	}
	return isplaying;
}

