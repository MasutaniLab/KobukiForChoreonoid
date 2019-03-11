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
    setCartVelocity(vel);
}

//概要：行動決定のための毎回の処理
//引数：なし
//戻り値：なし
void loop()
{
    int key;
    getKey(key);
    if (key < 0) {} //何もしない
    else if (key == 'i') { vel.v += 0.1; } else if (key == ',') { vel.v -= 0.1; } else if (key == 'j') { vel.w += 0.2; } else if (key == 'l') { vel.w -= 0.2; } else if (key == ' ') { vel.v = 0; vel.w = 0; }
    setCartVelocity(vel);
}