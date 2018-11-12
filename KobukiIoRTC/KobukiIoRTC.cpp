/**
   @author Yasuhiro Masutani
*/

#define _USE_MATH_DEFINES
#include <cnoid/BodyIoRTC>
#include <cnoid/RateGyroSensor>
#include <cnoid/ForceSensor>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <cmath>

using namespace std;
using namespace cnoid;

namespace {

class KobukiIoRTC : public BodyIoRTC
{
public:
    KobukiIoRTC(RTC::Manager* manager);
    ~KobukiIoRTC();

    virtual bool initializeIO(ControllerIO* io) override;
    virtual bool initializeSimulation(ControllerIO* io) override;
    virtual void inputFromSimulator() override;
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id) override;
    virtual void outputToSimulator() override;
    
    // DataInPort declaration
    RTC::TimedVelocity2D m_targetVelocity;
    RTC::InPort<RTC::TimedVelocity2D> m_targetVelocityIn;

    RTC::TimedPose2D m_poseUpdate;
    RTC::InPort<RTC::TimedPose2D> m_poseUpdateIn;
    
    // DataOutPort declaration
    RTC::TimedPose2D m_currentPose;
    RTC::OutPort<RTC::TimedPose2D> m_currentPoseOut;

    RTC::TimedBooleanSeq m_bumper;
    RTC::OutPort<RTC::TimedBooleanSeq> m_bumperOut;
private:
    ControllerIO* m_io;
    BodyPtr m_ioBody;
    Link* m_wheelR;
    Link* m_wheelL;
    Link* m_clink;
    RateGyroSensor* m_gyro;
    ForceSensor* m_bumperForce;

    double m_v;
    double m_w;
    double m_x;
    double m_y;
    double m_q;
    double m_prevTime;
    double m_prevOutputTime;
    double m_iR;
    double m_iL;
    bool m_wheelFirst;
    double m_sumFx;
    double m_sumFy;
    int m_sumC;

    const double OutputDt = 0.01;       //[sec]
    const double Dt = 0.002;            //[sec]
    const double WheelRadius = 0.0352;  //[m]
    const double WheelDistance = 0.115; //[m]

    double normalizeAngle(double x) {
      if (x < -M_PI)
        x += M_PI * 2 * (double)((int)(-x / (2 * M_PI) + 0.5));
      else if (x > M_PI)
        x -= M_PI * 2 * (double)((int)(x / (2 * M_PI) + 0.5));
      return x;
    }
    int sign(double x) {
      if (x < 0) return -1;
      if (x > 0) return 1;
      return 0;
    }
    void sat(double& x, double min, double max) {
      if (x < min) {
        x = min;
      } else if (x > max) {
        x = max;
      }
    }
};

const char* spec[] =
{
    "implementation_id", "KobukiIoRTC",
    "type_name",         "KobukiIoRTC",
    "description",       "Kobuki I/O",
    "version",           "1.0",
    "vendor",            "MasutaniLab",
    "category",          "MobileRobot",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

}

KobukiIoRTC::KobukiIoRTC(RTC::Manager* manager)
    : BodyIoRTC(manager),
    m_targetVelocityIn("targetVelocity", m_targetVelocity),
    m_poseUpdateIn("poseUpdate", m_poseUpdate),
    m_currentPoseOut("currentPose", m_currentPose),
    m_bumperOut("bumper", m_bumper)
{

}


KobukiIoRTC::~KobukiIoRTC()
{

}


bool KobukiIoRTC::initializeIO(ControllerIO* io)
{
    // Set InPort buffers
    addInPort("targetVelocity", m_targetVelocityIn);
    addInPort("poseUpdate", m_poseUpdateIn);
    
    // Set OutPort buffer
    addOutPort("currentPose", m_currentPoseOut);
    addOutPort("bumper", m_bumperOut);

    return true;
}


bool KobukiIoRTC::initializeSimulation(ControllerIO* io)
{
  m_io = io;
  m_ioBody = io->body();

  m_wheelR = m_ioBody->link("wheel_right_link");
  m_wheelL = m_ioBody->link("wheel_left_link");
  m_clink = m_ioBody->link("clink");
  m_wheelR->setActuationMode(Link::JOINT_TORQUE);
  m_wheelL->setActuationMode(Link::JOINT_TORQUE);

  m_gyro = m_ioBody->findDevice<RateGyroSensor>("gyro");
  m_bumperForce = m_ioBody->findDevice<ForceSensor>("bumperForce");

  m_v = 0;
  m_w = 0;
  m_x = 0;
  m_y = 0;
  m_q = 0;
  m_prevTime = 0;
  m_bumper.data.length(3);
  m_iR = 0;
  m_iL = 0;
  m_wheelFirst = true;
  m_sumFx = 0;
  m_sumFy = 0;
  m_sumC = 0;
  return true;
}

void KobukiIoRTC::inputFromSimulator()
{
  //時間
  double currentTime = m_io->currentTime(); //要確認
  if (m_wheelFirst || m_prevTime > currentTime) {
    m_prevTime = currentTime - Dt;
    m_prevOutputTime = currentTime - OutputDt;
    m_wheelFirst = false;
  }

  //車輪の速度
  double wR = m_wheelR->dq();
  double wL = m_wheelL->dq();

  //デッドレコニング
  double v = (wL + wR) * WheelRadius / 2;
  double w = m_gyro->w().z();
  double dt = currentTime - m_prevTime;
  m_prevTime = currentTime;
  double qPrev = m_q;
  m_q += w * dt;
  m_x += v * dt * cos((m_q + qPrev) / 2);
  m_y += v * dt * sin((m_q + qPrev) / 2);

  //力センサ
  Vector3 force = m_bumperForce->f();
  //cout << force.x() << ", " << force.y() << endl;
  m_sumFx += force.x();
  m_sumFy += force.y();
  m_sumC++;

  if (currentTime - m_prevOutputTime > OutputDt - 0.9 * Dt) {
    //現在位置の出力
    coil::TimeValue ct(currentTime);
    m_currentPose.tm.sec = ct.sec();
    m_currentPose.tm.nsec = ct.usec()*1000;
    m_currentPose.data.position.x = m_x;
    m_currentPose.data.position.y = m_y;
    m_currentPose.data.heading = normalizeAngle(m_q);
    m_currentPoseOut.write();

    //バンパの出力
    double fx, fy;
    if (m_sumC == 0) {
      fx = 0;
      fy = 0;
    } else {
      fx = m_sumFx / m_sumC;
      fy = m_sumFy / m_sumC;
    }
    // cout << "fx: " << fx << "  fy: " << fy << endl;
    double a = sqrt(fx * fx + fy * fy);
    m_bumper.data[0] = false;
    m_bumper.data[1] = false;
    m_bumper.data[2] = false;
    if (a > 1.0) {
      double q = atan2(fy, fx);
      if (0.475 * M_PI < q && q < 0.875 * M_PI) {
        m_bumper.data[0] = true;
      }
      if (q < -0.75 * M_PI || 0.75 * M_PI < q) {
        m_bumper.data[1] = true;
      }
      if (-0.875 * M_PI < q && q < -0.475 * M_PI) {
        m_bumper.data[2] = true;
      }
    }
    m_bumperOut.write();
    m_sumFx = 0;
    m_sumFy = 0;
    m_sumC = 0;

    //出力時刻の記録
    m_prevOutputTime = currentTime;
  }

  //車輪の制御
  double wRTarget = (m_v + WheelDistance * m_w) / WheelRadius;
  double wLTarget = (m_v - WheelDistance * m_w) / WheelRadius;
  // cout << "wRTarget: " << wRTarget << " wLTarget: " << wLTarget << endl;
  double dR = wR - wRTarget;
  double dL = wL - wLTarget;
  m_iR += dR * dt;
  if (abs(wR) < 0.01 && abs(dR) < 0.01) {
    m_iR = 0;
  }
  m_iL += dL * dt;
  if (abs(wL) < 0.01 && abs(dL) < 0.01) {
    m_iL = 0;
  }
  const double mass = 5.386 / 2;  //[kg]
  const double tconst = 0.1;      //[s]
  const double kp =
      3.0 * mass * WheelRadius * WheelRadius / tconst;  //比例ゲイン
  const double ki = 2.0 * kp;
  const double compensation = 0.2;
  double fR, fL;
  fR = sign(wRTarget) * compensation;
  fL = sign(wLTarget) * compensation;
  double tR = -kp * dR - ki * m_iR + fR;
  double tL = -kp * dL - ki * m_iL + fL;
  const double torqueLimit = 1.5;  //[Nm]
  sat(tR, -torqueLimit, torqueLimit);
  sat(tL, -torqueLimit, torqueLimit);
  m_wheelR->u() = tR;
  m_wheelL->u() = tL;
}

RTC::ReturnCode_t KobukiIoRTC::onExecute(RTC::UniqueId ec_id)
{
  if (m_targetVelocityIn.isNew()) {
    m_targetVelocityIn.read();
    m_v = m_targetVelocity.data.vx;
    m_w = m_targetVelocity.data.va;
    //cout << m_v << ", " << m_w << endl;
  }

  if (m_poseUpdateIn.isNew()) {
    m_poseUpdateIn.read();
    m_x = m_poseUpdate.data.position.x;
    m_y = m_poseUpdate.data.position.y;
    m_q = m_poseUpdate.data.heading;
  }
  return RTC::RTC_OK;
}

void KobukiIoRTC::outputToSimulator()
{
}


extern "C"
{
    DLL_EXPORT void KobukiIoRTCInit(RTC::Manager* manager)
    {
        coil::Properties profile(spec);
        manager->registerFactory(
            profile, RTC::Create<KobukiIoRTC>, RTC::Delete<KobukiIoRTC>);
    }
};
