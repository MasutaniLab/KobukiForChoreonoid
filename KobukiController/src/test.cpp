//Kobukiのテスト用プログラム
#include "ise-robot.h"
#include <iostream>
#include <cmath>
#include "draw.h"
#include "util.h"
using namespace std;

Velocity vel;

//概要：行動決定のための初期設定
//引数：なし
//戻り値：なし
void setup()
{
    vel.v = 0;
    vel.w = 0;
}

//概要：行動決定のための毎回の処理
//引数：なし
//戻り値：なし
Velocity loop(const Orthogonal &pos, const Bumper &bum, int key)
{
    if (key < 0) {} //何もしない
    else if (key == '1') { vel.v = 0; vel.w = 0.3; }
    else if (key == '2') { vel.v = 0; vel.w = 0.6; }
    else if (key == '3') { vel.v = 0; vel.w = 0.9; }
    else if (key == '4') { vel.v = 0.1; vel.w = 0.3; }
    else if (key == '5') { vel.v = 0.1; vel.w = 0.6; }
    else if (key == '6') { vel.v = 0.1; vel.w = 0.9; }
    else if (key == '7') { vel.v = 0.2; vel.w = 0.3; }
    else if (key == '8') { vel.v = 0.2; vel.w = 0.6; }
    else if (key == '9') { vel.v = 0.2; vel.w = 0.9; }
    else if (key == ' ') { vel.v = 0; vel.w = 0; }
    return vel;
}