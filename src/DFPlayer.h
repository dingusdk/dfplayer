#include "Arduino.h"
#include "Stream.h"

class DFPlayer {

	// 7E FF 06 0F 00 01 01 xx xx EF
	// 0	->	7E is start code
	// 1	->	FF is version
	// 2	->	06 is length
	// 3	->	0F is command
	// 4	->	00 is no receive
	// 5~6	->	01 01 is argument
	// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
	// 9	->	EF is end code

public:

	typedef byte Buffer[10];

private:

	Stream& playerserial;

	Buffer send_buf = { 0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF };
	Buffer recv_buf;
	bool isplaying;

	DFPlayer();
	DFPlayer( const DFPlayer&);

	word GetChecksum(Buffer& buf);
	void Send();	
	void SendCmd(byte cmd, bool feedback,int arg);

	bool ReadResponse( int timeout = 100);
	bool CheckResponse();
	bool OkReturnResponse(int timeout);

public:
	DFPlayer(Stream& playerserial);

	void Play();
	void Play(int num);
	void Pause();
	void Stop();
	void Next();
	void Previous();
	void SetVolume( int volume);
	void Sleep();
	void Reset();

	bool IsPlaying();
	int GetVolume();
};

