// WiiRemote[WiiYourself!]-Measurement by Akihiko SHIRAI 2009/05/28
// http://akihiko.shirai.as/projects/WiiRemote

#include "../../wiimote.h"
#include <mmsystem.h>	// for timeGetTime()
#include <conio.h>      // for _kbhit()

int _tmain(int argc, _TCHAR* argv[])
{
	wiimote cWiiRemote;
    DWORD currentTime=0; //���݂̎������i�[����ϐ�
    //������g������p
    DWORD startTime=0, Refreshed=0, Sampled=0;
    float duration=0.0f; //�o�ߎ���[�b]
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
    //�����������ݒ肳�ꂽ�ꍇ�A�������[�h�ɐݒ�
    if (argv[1]) {
     _tprintf(_T("ReportType continuous = true [%s]\n"), argv[1]);
     continuous = true;
    }
    //�{�^���������x�ɕω����������Ƃ��Ƀ��|�[�g����悤�ݒ�
    //��2������true�ɂ��邱�ƂŃf�[�^�X�V������(10msec���x)����
    //�f�t�H���g��false�Ń|�[�����O(�f�[�^������Ƃ�����)��M���[�h
	cWiiRemote.SetReportType(wiimote::IN_BUTTONS_ACCEL, continuous);
    startTime=timeGetTime();  //�J�n���̎��Ԃ�ۑ�
  
    //Home���������܂ŁA��������10�b�ԑ���
	while(!cWiiRemote.Button.Home() && duration<10)
	  {
        while(cWiiRemote.RefreshState() == NO_CHANGE) {
            Refreshed++; //���t���b�V�����ꂽ�񐔂��L�^
            Sleep(1);    //CPU�𖳑ʂɐ�L���Ȃ��悤��
      }
      Sampled++; //�f�[�^�ɕύX���������Ƃ��ɃJ�E���g�A�b�v
	  cWiiRemote.SetRumble(cWiiRemote.Button.B());
      _tprintf(_T("TGT:%d %+03d[msec] R:%d S:%d D:%1.0f Accel: X %+2.3f Y %+2.3f Z %+2.3f\n"),
        timeGetTime(), //���݂̎���
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
    printf("�ڑ�����%4.2f�b �X�V%d�� �f�[�^��M%d��\n �X�V���g��%.2fHz �T���v�����O%.2fHz\n",
      duration, Refreshed, Sampled,
      (float)Refreshed/duration, (float)Sampled/duration);
    while (true) 
      if (_kbhit()) {break;} //�����L�[�������܂ő҂�
	return 0;
}
