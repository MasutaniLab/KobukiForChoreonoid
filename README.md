# 車輪型移動ロボットKobukiのChoreonoid用シミュレーションモデル

大阪電気通信大学  
升谷 保博  
2018年11月27日

## はじめに

- オープンソースのロボット用統合GUIソフトウェア[Choreonoid](http://choreonoid.org/ja/)を用いて，Yujin Robot社の車輪型移動ロボット[Kobuki](http://kobuki.yujinrobot.com/)（[TurtleBot2](http://www.turtlebot.com/turtlebot2/)）の動力学シミュレーションを行うためのモデル一式です．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2015
  - OpenRTM-aist 1.1.2 64bit版
  - Choreonoid 1.7（開発版）
  - VRMLモデルは，[ROSのTurtlebotのURDF](http://wiki.ros.org/turtlebot_description)を[simtrans](http://fkanehiro.github.io/simtrans/html-ja/index.html)でVRMLに変換したものを改変し，さらに手作業でbodyファイルに変換しました．力覚センサを使って，バンパセンサを模擬しています．測域センサと深度カメラも載せています．
- ChoreonoidのBodyIoRTCのRTCモジュール（KobukiIoRTC）は独自に作成したものです．ポートの仕様は，[KobukiRTC](https://github.com/rt-net/kobuki_rtc)と互換になるようにしています．
- 以上を利用するChorenoidのプロジェクトファイルも同梱しています．

## 内容物

- ロボットと環境のbodyファイル
  - `model/turtlebot.wrl`（および，`model/*.wrl`）
  - `box.body`
  - `green-floor.body`
  - `marks.body`
  - `wall.body`
- BodyIoRTCのコントローラモジュールのソースコード一式
  - ディレクトリ `KobukiIoRTC`
- `KobukiIoRTC`と接続するRTコンポーネントの例
  - ディレクトリ `KobukiController`
- Choreonoidのプロジェクトファイル
  - `kobuki.cnoid` （障害物なし，VisionSensorIoRTC不使用）
  - `kobuki+boxes.cnoid` （障害物あり，VisionSensorIoRTC使用）

## インストール

- [OpenRTM-aist 1.1.2](http://www.openrtm.org/openrtm/ja/node/6034)をインストール．
- [Choreonoid 開発版をインストール](http://choreonoid.org/ja/manuals/1.5/install/install.html)．
  - Windowsの場合，Choreonoid本体をビルドしたVisual Stduioのバージョンや構成と，BodyIORTCのRTCモジュールをビルドするVisual Stduioのバージョンや構成が異なっていると，動作しません．
- [KobukiForChoreonoid](https://github.com/MasutaniLab/KobukiForChoreonoid)
をクローンかダウンロードする．
- `KobukiIoRTC`
  - CMake
  - ソースディレクトリはトップの下の`KobukiIoRTC`
  - ビルドディレクトリは`KobukiIoRTC/build`
  - Windowsの場合
    - ConfigureはVisual Studio 64bit
    - `KobukiIoRTC/build/KobukiIoRTC.sln`をVisual Studioで開く．
    - 構成をReleaseにしてビルド．
    - 正常終了したら，プロジェクト`INSTALL`をビルド．
  - Linuxの場合
    - `cd KobukiIoRTC; mkdir build; cd build; cmake ..; make; make install`
  - WindowsとLinuxでプロジェクトファイルを共通にするために，コントローラモジュールの動的ライブラリ（Windowsではdllファイル，Linuxではsoファイル）を`KobukiIoRTC/build/module`にインストールするようにしています．
- `wineggx`
  - Windowsの場合，`KobukiController`で利用するグラフィックスライブラリ．サブモジュールとしてクローンし，CMake，ビルドし，INSTALLをビルドする．
  - Linuxの場合は，[EGGX](https://www.ir.isas.jaxa.jp/~cyamauch/eggx_procall/index.ja.html)をインストールする．
- `KobukiController`
  - CMake
  - ソースディレクトリはトップの下の`KobukiController`
  - ビルドディレクトリは`KobukiController/build`
  - Windowsの場合
    - ConfigureはVisual Studio 64bit
    - `KobukiController/build/KobukiController.sln`をVisual Studioで開く．
    - 構成をReleaseにしてビルド．
  - Linuxの場合
    - `cd KobukiController; mkdir build; cd build; cmake ..; make`

## 使い方

- 適切なバージョンのChoreonoidを起動し，プロジェクトファイル`kobuki.cnoid`（障害物なし）または`kobuki+boxes.cnoid` （障害物あり）を読み込みます．
- メッセージビューにエラーが出ていないか確認します．
- `KobukiIoRTC`コンポーネントのポートを他のコンポーネントのポートに接続します．
- `KobukiIoRTC`コンポーネントのポートは以下の通りです．
  - 入力ポート
    - targetVelocity
      - 型: RTC::TimedVelocity2D
      - 概要： 速度指令
    - poseUpdate
      - 型: RTC::TimedPose2D
      - 概要： オドメトリの設定値

  - 出力ポート
    - currentPose
      - 型: RTC::TimedPose2D
      - 概要： オドメトリの結果
    - bumper
      - 型: RTC::TimedBooleanSeq
      - 概要： バンパの状態（長さ3）
- `KobukiIoRTC`のコンポーネントの仕様は，アールティ社が公開している[KobukiRTC
](https://github.com/rt-net/kobuki_rtc)と互換になるようにしています．

## 既知の問題・TODO

- 
