// WiiRemote[WiiYourself!]-Theremin-Button by Akihiko SHIRAI 2009/05/28
// http://akihiko.shirai.as/projects/WiiRemote
// please see here for MIDI control
// http://www13.plala.or.jp/kymats/study/MULTIMEDIA/midiOutShortMsg.html

#pragma comment(lib,"winmm.lib")
#include <windows.h>

//MIDI特有のエンディアンを変換するマクロ
#define MIDIMSG(status,channel,data1,data2) ( (DWORD)((status<<4) | channel | (data1<<8) | (data2<<16)) )

#include "../../wiimote.h" //WiiYourself!を取り込む

static HMIDIOUT hMidiOut;  //MIDI
static BYTE note=0x3C, velocity=0x40; //音階と音量
static BYTE program=0x0;   //音色

int _tmain(int argc, _TCHAR* argv[])
{
  wiimote cWiiRemote;
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  printf("WiiRemote-Терменвокс(Theremin) button version by Akihiko SHIRAI\n");
  //LICENSE
  printf("contains WiiYourself! wiimote code by gl.tter\nhttp://gl.tter.org\n");
  //MIDIを開く
  midiOutOpen(&hMidiOut,MIDIMAPPER,NULL,0,CALLBACK_NULL);

  //最初につながったWiiRemoteに接続する
  while(!cWiiRemote.Connect(wiimote::FIRST_AVAILABLE)) {
   printf("WiiRemoteに接続してください(0x%02X)\n",program);
   midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0)); //ミュート
   Sleep(1000);
   program++;
   midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //音色変更
   midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity)); //接続しないたび鳴る
  }
  printf("接続しました！\n [1]/[2]音色 [↑]/[↓]音階 [←][→]音量 [Home]終了\n\n");
  Sleep(1000);

  //今回はボタンイベントだけが更新を伝える
  cWiiRemote.SetReportType(wiimote::IN_BUTTONS);
  while(!cWiiRemote.Button.Home()) {  //Homeで終了
    while(cWiiRemote.RefreshState() == NO_CHANGE)
	  Sleep(1); //これがないと更新が速すぎる
    cWiiRemote.SetRumble(cWiiRemote.Button.B()); //Bで振動
    switch (cWiiRemote.Button.Bits) { //ボタンごとでswitchする例
      //音量 [←]/[→]
      case wiimote_state::buttons::RIGHT :
        if(velocity<0x7F) velocity++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::LEFT :
        if(velocity>0) velocity--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //音色(=program) [1]/[2]
      case wiimote_state::buttons::ONE :
        if(program>0) program--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //音色変更
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::TWO:
        if(program<0x7F) program++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //音色変更
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //音階 up/down
      case wiimote_state::buttons::UP :
        if(note<0x7F) note++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;
      case wiimote_state::buttons::DOWN:
        if(note>0) note--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //[A]/[B]で同じ音をもう一度鳴らす
      case wiimote_state::buttons::_A :
      case wiimote_state::buttons::_B :
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        break;

      //その他のイベント、つまりボタンを離したときミュート。
      default :
        midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
    }
    //現在のMIDIメッセージを同じ場所にテキスト表示
	COORD pos = { 10, 7 };
	SetConsoleCursorPosition(console, pos);
    printf("音色 = 0x%02X , 音階 = 0x%02X , 打鍵強度 = 0x%02X\n",
          program,note,velocity);

  }
  //終了
  midiOutReset(hMidiOut);
  midiOutClose(hMidiOut);
  cWiiRemote.SetLEDs(0);
  cWiiRemote.SetRumble(0);
  cWiiRemote.~wiimote(); //Disconnect()も含む
  printf("演奏終了\n");
  CloseHandle(console);
  return 0;
}

