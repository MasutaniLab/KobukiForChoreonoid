# 車輪型移動ロボットKobukiのChoreonoid用シミュレーションモデル

大阪電気通信大学  
升谷 保博  
2017年12月4日

## はじめに

- オープンソースのロボット用統合GUIソフトウェア[Choreonoid](http://choreonoid.org/ja/)を用いて，Yujin Robot社の車輪型移動ロボット[Kobuki](http://kobuki.yujinrobot.com/)（[TurtleBot2](http://www.turtlebot.com/turtlebot2/)）の動力学シミュレーションを行うためのモデル一式です．
- 以下の環境で開発，動作確認しています．
  - Windows 10 64bit版
  - Visual Studio 2012
  - OpenRTM-aist 1.1.2 64bit版
  - Choreonoid 1.5
- - VRMLモデルは，[ROSのTurtlebotのURDF](http://wiki.ros.org/turtlebot_description)を[simtrans](http://fkanehiro.github.io/simtrans/html-ja/index.html)でVRMLに変換したものを改変して使っています．力覚センサを使って，バンパセンサを模擬しています．
- ChoreonoidのボディRTCのコントローラモジュールとして使うRTコンポーネント（KobukiBridge）は独自に作成したものです．ポートの仕様は，[KobukiRTC](https://github.com/rt-net/kobuki_rtc)と互換になるようにしています．
- 以上を利用するChorenoidのプロジェクトファイルも同梱しています．

## 内容物

- ロボットと環境のVRMLモデル
  - `model/*.wrl`
  - `box.wrl`
  - `green-floor.wrl`
  - `marks.wrl`
  - `wall.wrl`
- ボディRTCのコントローラモジュールのソースコード一式
  - ディレクトリ `KobukiBridge`
- ボディRTCの設定ファイル
  - `turtlebot.conf`
- Choreonoidのプロジェクトファイル
  - `kobuki.cnoid` （障害物なし）
  - `kobuki+boxes.cnoid` （障害物あり）

## インストール

- [OpenRTM-aist 1.1.2](http://www.openrtm.org/openrtm/ja/node/6034)をインストール．
- [Choreonoid 1.5をインストール](http://choreonoid.org/ja/manuals/1.5/install/install.html)．
  - 2017年10月31日にバージョン1.6.0がリリースされましたが，まだ動作確認できていません．
  - Windowsの場合，Choreonoid本体をビルドしたVisual Stduioのバージョンや構成と，ボディRTCのコントローラモジュールをビルドするVisual Stduioのバージョンや構成が異なっていると，動作しません．配布されているWindows用のインストーラは`Choreonoid-1.5.0-win64.exe`は，Visual Studio 2013でビルドされています．
- [KobukiForChoreonoid](https://github.com/MasutaniLab/KobukiForChoreonoid)
をクローンかダウンロードする．
- CMake
  - ソースディレクトリはトップの下の`KobukiBridge`
  - ビルドディレクトリは`KobukiBridge/build`
  - Windowsの場合
    - ConfigureはVisual Studio 64bit
    - `KobukiBridge/build/KobukiBridge.sln`をVisual Studioで開く．
    - 構成をReleaseにしてビルド．
    - 正常終了したら，プロジェクト`INSTALL`をビルド．
  - Linuxの場合
    - `cd KobukiBridge; mkdir build; cd build; cmake ..; make; make install`
  - WindowsとLinuxでプロジェクトファイルを共通にするために，コントローラモジュールの動的ライブラリ（Windowsではdllファイル，Linuxではsoファイル）を`KobukiBridge/build/module`にインストールするようにしています．

## 使い方

- 適切なバージョンのChoreonoidを起動し，プロジェクトファイル`kobuki.cnoid`（障害物なし）または`kobuki+boxes.cnoid` （障害物あり）を読み込みます．
- メッセージビューにエラーが出ていないか確認します．
- `KobukiBridge`コンポーネントのポートを他のコンポーネントのポートに接続します．
- `KobukiBridge`コンポーネントの外部接続用のポートは以下の通りです．
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
- `KobukiBridge`のコンポーネントの仕様は，アールティ社が公開している[KobukiRTCl
](https://github.com/rt-net/kobuki_rtc)と互換になるようにしています．

## 既知の問題・TODO

- オドメトリがずれやすいです．要調査．
