@echo off
: KobukiForChoreonoidのテスト

:ネーミングサービスの確認
rtls /localhost > nul
if errorlevel 1 (
  echo ネーミングサーバが見つかりません
  pause
  exit /b 1
  rem /bオプションは親を終わらせないために必須
)

:作業ディレクトリをバッチファイルのある場所へ変更
cd %~dp0

:Choreonoid起動
start "Choreonoid" choreonoid kobuki+boxes.cnoid

:コンポーネントの起動
start "" python\TkJoyStickVelocity.pyw

:コンポーネント名を変数化
set j=/localhost/TkJoyStickVelocity0.rtc
set k=/localhost/Turtlebot-KobukiIoRTC.rtc

:時間待ち
timeout 10

:接続
rtcon %j%:velocity %k%:targetVelocity

:アクティベート
rtact %j%

echo Choreonoidでシミュレーションを開始してください．

:loop
set /p ans="終了しますか？ (y/n)"
if not "%ans%"=="y" goto loop

rtdeact %j%

:終了（rtexitは，引数を一つずつ）
for %%i in (%j%) do (
  rtexit %%i
)

:Choreonoid終了
taskkill /im choreonoid.exe
