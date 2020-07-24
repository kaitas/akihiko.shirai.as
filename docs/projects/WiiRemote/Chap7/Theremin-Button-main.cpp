// WiiRemote[WiiYourself!]-Theremin-Button by Akihiko SHIRAI 2009/05/28
// http://akihiko.shirai.as/projects/WiiRemote
// please see here for MIDI control
// http://www13.plala.or.jp/kymats/study/MULTIMEDIA/midiOutShortMsg.html

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
  printf("WiiRemote-�S�u���}�u�~�r���{��(Theremin) button version by Akihiko SHIRAI\n");
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
  printf("�ڑ����܂����I\n [1]/[2]���F [��]/[��]���K [��][��]���� [Home]�I��\n\n");
  Sleep(1000);

  //����̓{�^���C�x���g�������X�V��`����
  cWiiRemote.SetReportType(wiimote::IN_BUTTONS);
  while(!cWiiRemote.Button.Home()) {  //Home�ŏI��
    while(cWiiRemote.RefreshState() == NO_CHANGE)
	  Sleep(1); //���ꂪ�Ȃ��ƍX�V����������
    cWiiRemote.SetRumble(cWiiRemote.Button.B()); //B�ŐU��
    switch (cWiiRemote.Button.Bits) { //�{�^�����Ƃ�switch�����
      //���� [��]/[��]
      case wiimote_state::buttons::RIGHT :
        if(velocity<0x7F) velocity++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::LEFT :
        if(velocity>0) velocity--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //���F(=program) [1]/[2]
      case wiimote_state::buttons::ONE :
        if(program>0) program--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //���F�ύX
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::TWO:
        if(program<0x7F) program++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //���F�ύX
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //���K up/down
      case wiimote_state::buttons::UP :
        if(note<0x7F) note++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::DOWN:
        if(note>0) note--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //[A]/[B]�œ�������������x�炷
      case wiimote_state::buttons::_A :
      case wiimote_state::buttons::_B :
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //���̑��̃C�x���g�A�܂�{�^���𗣂����Ƃ��~���[�g�B
      default :
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
    }
    //���݂�MIDI���b�Z�[�W�𓯂��ꏊ�Ƀe�L�X�g�\��
	COORD pos = { 10, 7 };
	SetConsoleCursorPosition(console, pos);
    printf("���F = 0x%02X , ���K = 0x%02X , �Ō����x = 0x%02X\n",
          program,note,velocity);

  }
  //�I��
  midiOutReset(hMidiOut);
  midiOutClose(hMidiOut);
  cWiiRemote.SetLEDs(0);
  cWiiRemote.SetRumble(0);
  cWiiRemote.~wiimote(); //Disconnect()���܂�
  printf("���t�I��\n");
  CloseHandle(console);
  return 0;
}

