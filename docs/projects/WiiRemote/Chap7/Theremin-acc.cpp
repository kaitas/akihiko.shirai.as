// WiiRemote[WiiYourself!]-Theremin-Acceleration by Akihiko SHIRAI 2009/05/29
// http://akihiko.shirai.as/projects/WiiRemote
// please see here for MIDI control
// http://www13.plala.or.jp/kymats/study/MULTIMEDIA/midiOutShortMsg.html

#include "stdafx.h"
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
  SetConsoleTitle(_T("WiiRemote-Theremin Acceleration version"));
  printf("WiiRemote-Theremin Acceleration version by Akihiko SHIRAI\n");
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
  printf("接続しました！\n");
  printf("\n\t  [B]打鍵 [Roll]音量 [Pitch]音階 [1]/[2]音色 [Home]終了\n");
  Sleep(1000);

  //今回はボタン＋加速度イベントが更新を伝える
  cWiiRemote.SetReportType(wiimote::IN_BUTTONS_ACCEL);
  while(!cWiiRemote.Button.Home()) {  //Homeで終了
    //RefreshStateは内部更新のために呼ばれる必要がある
    while(cWiiRemote.RefreshState() == NO_CHANGE) {
	  Sleep(1); //これがないと更新が速すぎる
    }
    switch (cWiiRemote.Button.Bits) { //ボタンごとでswitchする
      //音色(=program) [1]/[2]
      case wiimote_state::buttons::ONE :
        if(program>0) program--;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0)); //音色変更
        break;
      case wiimote_state::buttons::TWO:
        if(program<0x7F) program++;
        midiOutShortMsg(hMidiOut,MIDIMSG(0xC,0,program,0));
        break;
      default:
        //音量 [傾きPitch]
        velocity = (int)(127*(cWiiRemote.Acceleration.Orientation.Pitch+90.0f)/180.0f);
        if(velocity>0x7F) velocity=0x7f;
        if(velocity<0x00) velocity=0x00;

        //音階 [傾きRoll]
        note = (int)(127*(cWiiRemote.Acceleration.Orientation.Roll+90.0f)/180.0f);
        if(note>0x7F) note=0x7F;
        if(note<0)    note=0;

        if(cWiiRemote.Button.B()) {         //[B]打鍵
          midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,velocity));
        } else {
          midiOutShortMsg(hMidiOut,MIDIMSG(0x9,0x0,note,0));
        }

        //座標指定テキスト表示
        COORD pos = { 10,  7 };
        SetConsoleCursorPosition(console, pos);
        printf("加速度 X = %+3.4f[0x%02X]  Y = %+3.4f[0x%02X]  Z = %+3.4f[0x%02X] ",
          cWiiRemote.Acceleration.X, cWiiRemote.Acceleration.RawX,
          cWiiRemote.Acceleration.Y, cWiiRemote.Acceleration.RawY,
          cWiiRemote.Acceleration.Z, cWiiRemote.Acceleration.RawZ
          );
        pos.X=10; pos.Y=9;
        SetConsoleCursorPosition(console, pos);
        printf("姿勢推定 Pitch = %+3.4f  Roll = %+3.4f  Update=%d  ",
          cWiiRemote.Acceleration.Orientation.Pitch,
          cWiiRemote.Acceleration.Orientation.Roll,
          cWiiRemote.Acceleration.Orientation.UpdateAge
          );
        pos.X=10; pos.Y=11;
        SetConsoleCursorPosition(console, pos);
        printf("音色 = [0x%02X] , 音階 = [0x%02X] , 打鍵強度 = [0x%02X] ",
          program,note,velocity);
        break;
    }
  }
  //終了
  midiOutReset(hMidiOut);
  midiOutClose(hMidiOut);
  cWiiRemote.SetLEDs(0);
  cWiiRemote.SetRumble(0);
  cWiiRemote.Disconnect();
  printf("演奏終了\n");
  CloseHandle(console);
  return 0;
}


