//Kobuki�̃e�X�g�p�v���O����
#include "ise-robot.h"
#include <iostream>
#include <cmath>
#include "draw.h"
#include "util.h"
using namespace std;

Velocity vel;

//�T�v�F�s������̂��߂̏����ݒ�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
void setup()
{
    vel.v = 0;
    vel.w = 0;
    setCartVelocity(vel);
}

//�T�v�F�s������̂��߂̖���̏���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
void loop()
{
    int key;
    getKey(key);
    if (key < 0) {} //�������Ȃ�
    else if (key == 'i') { vel.v += 0.1; } else if (key == ',') { vel.v -= 0.1; } else if (key == 'j') { vel.w += 0.2; } else if (key == 'l') { vel.w -= 0.2; } else if (key == ' ') { vel.v = 0; vel.w = 0; }
    setCartVelocity(vel);
}