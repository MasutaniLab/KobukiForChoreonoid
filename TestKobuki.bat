@echo off
: KobukiForChoreonoid�̃e�X�g

:�l�[�~���O�T�[�r�X�̊m�F
rtls /localhost > nul
if errorlevel 1 (
  echo �l�[�~���O�T�[�o��������܂���
  pause
  exit /b 1
  rem /b�I�v�V�����͐e���I��点�Ȃ����߂ɕK�{
)

:��ƃf�B���N�g�����o�b�`�t�@�C���̂���ꏊ�֕ύX
cd %~dp0

:Choreonoid�N��
start "Choreonoid" choreonoid kobuki+boxes.cnoid

:�R���|�[�l���g�̋N��
start "" python\TkJoyStickVelocity.pyw

:�R���|�[�l���g����ϐ���
set j=/localhost/TkJoyStickVelocity0.rtc
set k=/localhost/Turtlebot-KobukiIoRTC.rtc

:���ԑ҂�
timeout 10

:�ڑ�
rtcon %j%:velocity %k%:targetVelocity

:�A�N�e�B�x�[�g
rtact %j%

echo Choreonoid�ŃV�~�����[�V�������J�n���Ă��������D

:loop
set /p ans="�I�����܂����H (y/n)"
if not "%ans%"=="y" goto loop

rtdeact %j%

:�I���irtexit�́C����������j
for %%i in (%j%) do (
  rtexit %%i
)

:Choreonoid�I��
taskkill /im choreonoid.exe
