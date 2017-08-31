// -*- C++ -*-
/*!
 * @file  KobukiBridge.cpp
 * @brief ${rtcParam.description}
 * @date $Date$
 *
 * $Id$
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>
#include "KobukiBridge.h"
using namespace std;

const double OutputDt = 0.01; //[sec]
const double Dt = 0.002; //[sec]
const double WheelRadius = 0.0352; //[m]
const double WheelDistance = 0.115; //[m]
double normalizeAngle(double x);
int sign(double x);
void sat(double &x, double min, double max);

// Module specification
// <rtc-template block="module_spec">
static const char* kobukibridge_spec[] =
  {
    "implementation_id", "KobukiBridge",
    "type_name",         "KobukiBridge",
    "description",       "${rtcParam.description}",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "Mobile Robot",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
KobukiBridge::KobukiBridge(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_targetVelocityIn("targetVelocity", m_targetVelocity),
    m_poseUpdateIn("poseUpdate", m_poseUpdate),
    m_gyroIn("gyro", m_gyro),
    m_wheelVelocityIn("wheelVelocity", m_wheelVelocity),
    m_bumperForceIn("bumperForce", m_bumperForce),
    m_currentPoseOut("currentPose", m_currentPose),
    m_wheelTorqueOut("wheelTorque", m_wheelTorque),
    m_bumperOut("bumper", m_bumper)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
KobukiBridge::~KobukiBridge()
{
}



RTC::ReturnCode_t KobukiBridge::onInitialize()
{
  cout << "KobukiBridge::onInitialize()" << endl;
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("targetVelocity", m_targetVelocityIn);
  addInPort("poseUpdate", m_poseUpdateIn);
  addInPort("gyro", m_gyroIn);
  addInPort("wheelVelocity", m_wheelVelocityIn);
  addInPort("bumperForce", m_bumperForceIn);
  
  // Set OutPort buffer
  addOutPort("currentPose", m_currentPoseOut);
  addOutPort("wheelTorque", m_wheelTorqueOut);
  addOutPort("bumper", m_bumperOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KobukiBridge::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t KobukiBridge::onActivated(RTC::UniqueId ec_id)
{
  cout << "KobukiBridge::onActivated()" << endl;

  m_v = 0;
  m_w = 0;
  m_x = 0;
  m_y = 0;
  m_q = 0;
  m_prevTime = 0;
  m_wheelTorque.data.length(2);
  m_wheelVelocity.data.length(2);
  m_iR = 0;
  m_iL = 0;
  m_wheelFirst = true;
  m_sumFx = 0;
  m_sumFy = 0;
  m_sumC = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t KobukiBridge::onDeactivated(RTC::UniqueId ec_id)
{
  cout << "KobukiBridge::onDeactivated()" << endl;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t KobukiBridge::onExecute(RTC::UniqueId ec_id)
{
  //コントローラ側から入力があった場合
  if (m_targetVelocityIn.isNew()) {
    m_targetVelocityIn.read();
    m_v = m_targetVelocity.data.vx;
    m_w = m_targetVelocity.data.va;
    //cout << "m_v: " << m_v << " m_w: " << m_w << endl;
  }

  if (m_poseUpdateIn.isNew()) {
    m_poseUpdateIn.read();
    m_x = m_poseUpdate.data.position.x;
    m_y = m_poseUpdate.data.position.y;
    m_q = m_poseUpdate.data.heading;
  }

  //Choreonoid側から入力があった場合
  if (m_wheelVelocityIn.isNew() && m_gyroIn.isNew() && m_bumperForceIn.isNew()) {
    //車輪の速度
    m_wheelVelocityIn.read();
    double currentTime = m_wheelVelocity.tm.sec + 1e-9*m_wheelVelocity.tm.nsec;
    if (m_wheelFirst || m_prevTime > currentTime) {
      m_prevTime = currentTime - Dt;
      m_prevOutputTime = currentTime - OutputDt;
      m_wheelFirst = false;
    }
    double wR = m_wheelVelocity.data[0];
    double wL = m_wheelVelocity.data[1];
    //cout << "wR: " << wR << " wL: " << wL << endl;

    //デッドレコニング
    m_gyroIn.read();
    double v = (wL+wR)*WheelRadius/2;
    double w = m_gyro.data[2];

    //x, y, qの更新
    double dt = currentTime - m_prevTime;
    m_prevTime = currentTime;
    double qPrev = m_q;
    m_q += w*dt;
    m_x += v*dt*cos((m_q+qPrev)/2);
    m_y += v*dt*sin((m_q+qPrev)/2);
    
    //力センサ
    m_bumperForceIn.read();
    m_sumFx += m_bumperForce.data[0];
    m_sumFy += m_bumperForce.data[1];
    m_sumC++;

    //cout << currentTime << " " << m_prevOutputTime << " " << currentTime-m_prevOutputTime << endl;
    if (currentTime-m_prevOutputTime > OutputDt-0.9*Dt) {
      m_currentPose.tm = m_wheelVelocity.tm;
      m_currentPose.data.position.x = m_x;
      m_currentPose.data.position.y = m_y;
      m_currentPose.data.heading = normalizeAngle(m_q);
      m_currentPoseOut.write();
      //バンパセンサ
      double fx = m_sumFx/m_sumC;
      double fy = m_sumFy/m_sumC;
      //cout << "fx: " << fx << "  fy: " << fy << endl;
      double a = sqrt(fx*fx+fy*fy);
      m_bumper.data[0] = false;
      m_bumper.data[1] = false;
      m_bumper.data[2] = false;
      if ( a > 1.0 ) {
        double q = atan2(fy, fx);
        if (0.475*M_PI < q && q < 0.875*M_PI) {
          m_bumper.data[0] = true;
        }
        if (q < -0.75*M_PI || 0.75*M_PI < q) {
          m_bumper.data[1] = true;
        }
        if (-0.875*M_PI < q && q < -0.475*M_PI) {
          m_bumper.data[2] = true;
        }
      }
      m_bumperOut.write();
      m_sumFx = 0;
      m_sumFy = 0;
      m_sumC = 0;
      m_prevOutputTime = currentTime;
      //cout << "x,y,q: " << m_x << " " << m_y << " " << m_q << endl;
    }

    //車輪の制御
    double wRTarget = (m_v + WheelDistance*m_w)/WheelRadius;
    double wLTarget = (m_v - WheelDistance*m_w)/WheelRadius;
    //cout << "wRTarget: " << wRTarget << " wLTarget: " << wLTarget << endl;
    double dR = wR - wRTarget;
    double dL = wL - wLTarget;
    m_iR += dR*dt;
    if ( abs(wR) < 0.01 && abs(dR) < 0.01) {
      m_iR = 0;
    } 
    m_iL += dL*dt;
    if ( abs(wL) < 0.01 && abs(dL) < 0.01) {
      m_iL = 0;
    } 
    const double mass = 5.386/2; //[kg]
    const double tconst = 0.1; //[s]
    const double kp = 3.0*mass*WheelRadius*WheelRadius/tconst; //比例ゲイン
    const double ki = 2.0*kp;
    const double compensation = 0.2;
    double fR, fL;
    fR = sign(wRTarget)*compensation;
    fL = sign(wLTarget)*compensation;
    double tR = -kp*dR - ki*m_iR + fR;
    double tL = -kp*dL - ki*m_iL + fL;
    const double torqueLimit = 1.5; //[Nm]
    sat(tR, -torqueLimit, torqueLimit);
    sat(tL, -torqueLimit, torqueLimit);
    m_wheelTorque.data[0] = tR;
    m_wheelTorque.data[1] = tL;
    m_wheelTorqueOut.write();
  }
 
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KobukiBridge::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiBridge::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

// 引数の角度を-π～+πの間に正規化したものを返す
double
normalizeAngle(double x)
{
  if      (x<-M_PI) x += M_PI*2*(double)((int)(-x/(2*M_PI)+0.5));
  else if (x> M_PI) x -= M_PI*2*(double)((int)( x/(2*M_PI)+0.5));
  return x;
}

//引数の符号を返す
int
  sign(double x)
{
  if (x<0) return -1;
  if (x>0) return 1;
  return 0;
}

//第1引数（参照）を範囲内に収める
void
  sat(double &x, double min, double max)
{
  if (x < min) {
    x = min;
  } else if(x > max) {
    x = max;
  }
}


extern "C"
{
 
  void KobukiBridgeInit(RTC::Manager* manager)
  {
    coil::Properties profile(kobukibridge_spec);
    manager->registerFactory(profile,
                             RTC::Create<KobukiBridge>,
                             RTC::Delete<KobukiBridge>);
  }
  
};


