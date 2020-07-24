// WiiRemote[WiiYourself!]-Theremin-Acceleration by Akihiko SHIRAI 2009/05/29
// http://akihiko.shirai.as/projects/WiiRemote
// please see here for MIDI control
// http://www13.plala.or.jp/kymats/study/MULTIMEDIA/midiOutShortMsg.html

#include "stdafx.h"
#pragma comment(lib,"winmm.lib")
#include <windows.h>


//MIDI���L�̃G���f�B�A����ϊ�����}�N��
#define MIDIMSG(status,channel,data1,data2) ( (DWORD)((status<<4) | channel | (data1<<8) | (data2<<16)) )

#include "../../wiimote.h" //WiiYourself!����荞��

static HMIDIOUT hMidiOut;  //MIDI
static BYTE note=0x3C, velocity=0x40; //���K�Ɖ���
static BYTE program=0x0;   //���F

int _tmain(int argc, _TCHAR* argv[])
{
  wiimote cWiiRemote;
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTitle(_T("WiiRemote-Theremin Acceleration version"));
  printf("WiiRemote-Theremin Acceleration version by Akihiko SHIRAI\n");
  //LICENSE
  printf("contains WiiYourself! wiimote code by gl.tter\nhttp://gl.tter.org\n");
  //MIDI���J��
  midiOutOpen(&hMidiOut,MIDIMAPPER,NULL,0,CALLBACK_NULL);

  //�ŏ��ɂȂ�����WiiRemote�ɐڑ�����
  while(!cWiiRemote.Connect(wiimote::FIRST_AVAILABLE)) {
   printf("WiiRemote�ɐڑ����Ă�������(0x%02X)\n",program);
   midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0)); //�~���[�g
   Sleep(1000);
   program++;
   midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //���F�ύX
   midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity)); //�ڑ����Ȃ����і�
  }
  printf("�ڑ����܂����I\n");
  printf("\n\t  [B]�Ō� [Roll]���� [Pitch]���K [1]/[2]���F [Home]�I��\n");
  Sleep(1000);

  //����̓{�^���{�����x�C�x���g���X�V��`����
  cWiiRemote.SetReportType(wiimote::IN_BUTTONS_ACCEL);
  while(!cWiiRemote.Button.Home()) {  //Home�ŏI��
    //RefreshState�͓����X�V�̂��߂ɌĂ΂��K�v������
    while(cWiiRemote.RefreshState() == NO_CHANGE) {
	  Sleep(1); //���ꂪ�Ȃ��ƍX�V����������
    }
    switch (cWiiRemote.Button.Bits) { //�{�^�����Ƃ�switch����
      //���F(=program) [1]/[2]
      case wiimote_state::buttons::ONE :
        if(program>0) program--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //���F�ύX
        break;
      case wiimote_state::buttons::TWO:
        if(program<0x7F) program++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0));
        break;
      default:
        //���� [�X��Pitch]
        velocity = (int)(127*(cWiiRemote.Acceleration.Orientation.Pitch+90.0f)/180.0f);
        if(velocity>0x7F) velocity=0x7f;
        if(velocity<0x00) velocity=0x00;

        //���K [�X��Roll]
        note = (int)(127*(cWiiRemote.Acceleration.Orientation.Roll+90.0f)/180.0f);
        if(note>0x7F) note=0x7F;
        if(note<0)    note=0;

        if(cWiiRemote.Button.B()) {         //[B]�Ō�
          midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        } else {
          midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
        }

        //���W�w��e�L�X�g�\��
        COORD pos = { 10,  7 };
        SetConsoleCursorPosition(console, pos);
        printf("�����x X = %+3.4f[0x%02X]  Y = %+3.4f[0x%02X]  Z = %+3.4f[0x%02X] ",
          cWiiRemote.Acceleration.X, cWiiRemote.Acceleration.RawX,
          cWiiRemote.Acceleration.Y, cWiiRemote.Acceleration.RawY,
          cWiiRemote.Acceleration.Z, cWiiRemote.Acceleration.RawZ
          );
        pos.X=10; pos.Y=9;
        SetConsoleCursorPosition(console, pos);
        printf("�p������ Pitch = %+3.4f  Roll = %+3.4f  Update=%d  ",
          cWiiRemote.Acceleration.Orientation.Pitch,
          cWiiRemote.Acceleration.Orientation.Roll,
          cWiiRemote.Acceleration.Orientation.UpdateAge
          );
        pos.X=10; pos.Y=11;
        SetConsoleCursorPosition(console, pos);
        printf("���F = [0x%02X] , ���K = [0x%02X] , �Ō����x = [0x%02X] ",
          program,note,velocity);
        break;
    }
  }
  //�I��
  midiOutReset(hMidiOut);
  midiOutClose(hMidiOut);
  cWiiRemote.SetLEDs(0);
  cWiiRemote.SetRumble(0);
  cWiiRemote.Disconnect();
  printf("���t�I��\n");
  CloseHandle(console);
  return 0;
}


