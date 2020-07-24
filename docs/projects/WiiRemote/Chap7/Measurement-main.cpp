// WiiRemote[WiiYourself!]-Measurement by Akihiko SHIRAI 2009/05/28
// http://akihiko.shirai.as/projects/WiiRemote

#include "../../wiimote.h"
#include <mmsystem.h>	// for timeGetTime()
#include <conio.h>      // for _kbhit()

int _tmain(int argc, _TCHAR* argv[])
{
	wiimote cWiiRemote;
    DWORD currentTime=0; //現在の時刻を格納する変数
    //動作周波数測定用
    DWORD startTime=0, Refreshed=0, Sampled=0;
    float duration=0.0f; //経過時間[秒]
    bool  continuous=false;
	_tprintf(_T("WiiRemote Measurement\n"));
	_tprintf(_T("contains WiiYourself! wiimote code by gl.tter\nhttp://gl.tter.org\n"));
	while(!cWiiRemote.Connect(wiimote::FIRST_AVAILABLE)) {
		_tprintf(_T("Connecting to a WiiRemote.\n"));
		Sleep(1000);
		}
	_tprintf(_T("connected.\n"));
    cWiiRemote.SetLEDs(0x0f);
	Sleep(1000);
    //何か引数が設定された場合、周期モードに設定
    if (argv[1]) {
     _tprintf(_T("ReportType continuous = true [%s]\n"), argv[1]);
     continuous = true;
    }
    //ボタンか加速度に変化があったときにリポートするよう設定
    //第2引数をtrueにすることでデータ更新を定期化(10msec程度)する
    //デフォルトはfalseでポーリング(データがあるときだけ)受信モード
	cWiiRemote.SetReportType(wiimote::IN_BUTTONS_ACCEL, continuous);
    startTime=timeGetTime();  //開始時の時間を保存
  
    //Homeがおされるまで、もしくは10秒間測定
	while(!cWiiRemote.Button.Home() && duration<10)
	  {
        while(cWiiRemote.RefreshState() == NO_CHANGE) {
            Refreshed++; //リフレッシュされた回数を記録
            Sleep(1);    //CPUを無駄に占有しないように
      }
      Sampled++; //データに変更があったときにカウントアップ
	  cWiiRemote.SetRumble(cWiiRemote.Button.B());
      _tprintf(_T("TGT:%d %+03d[msec] R:%d S:%d D:%1.0f Accel: X %+2.3f Y %+2.3f Z %+2.3f\n"),
        timeGetTime(), //現在の時刻
        timeGetTime() - currentTime,
        Refreshed, Sampled,duration,
        cWiiRemote.Acceleration.X,
        cWiiRemote.Acceleration.Y,
		cWiiRemote.Acceleration.Z);
      currentTime = timeGetTime();  
      duration = (timeGetTime()-startTime)/1000.0f;
    }
	cWiiRemote.Disconnect();
	_tprintf(_T("Disconnected.\n"));
    duration = (timeGetTime()-startTime)/1000.0f;
    printf("接続時間%4.2f秒 更新%d回 データ受信%d回\n 更新周波数%.2fHz サンプリング%.2fHz\n",
      duration, Refreshed, Sampled,
      (float)Refreshed/duration, (float)Sampled/duration);
    while (true) 
      if (_kbhit()) {break;} //何かキーを押すまで待つ
	return 0;
}
