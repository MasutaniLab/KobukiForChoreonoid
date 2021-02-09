#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

from __future__ import print_function
import sys
import time
sys.path.append(".")

# Import RTM module
import RTC
import OpenRTM_aist

import tkjoystick

# This module's spesification
# <rtc-template block="module_spec">
tkjoystickvelocity_spec = ["implementation_id", "TkJoyStickVelocity", 
                   "type_name",         "TkJoyStickVelocity", 
                   "description",       "Sample component for MobileRobotCanvas component", 
                   "version",           "1.0", 
                   "vendor",            "MasutaniLab", 
                   "category",          "example", 
                   "activity_type",     "DataFlowComponent", 
                   "max_instance",      "10", 
                   "language",          "Python", 
                   "lang_type",         "SCRIPT",
                   ""]
# </rtc-template>

tkJoyCanvas = tkjoystick.TkJoystick()

class Position:
  def __init__(self, x = 0.0, y = 0.0, r = 0.0, th = 0.0):
    self.x = x
    self.y = y
    self.r = r
    self.th = th

position = Position()
#tkJoyCanvas = tkjoystick.TkJoystick()


class TkJoyStickVelocity(OpenRTM_aist.DataFlowComponentBase):
  def __init__(self, manager):
    OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)

    self._k = 1.0
    self.x = 0.0
    self.y = 0.0

     
  def onInitialize(self):
    self._d_velocity = RTC.TimedVelocity2D(RTC.Time(0,0), RTC.Velocity2D(0, 0, 0))
    self._velocityOut = OpenRTM_aist.OutPort("velocity", self._d_velocity)
    
    # Set OutPort buffers
    self.addOutPort("velocity",self._velocityOut)
    
    return RTC.RTC_OK


  def onShutdown(self, ec_id):
    tkJoyCanvas.destroy()
    tkJoyCanvas.quit()
    return RTC.RTC_OK


  def onExecute(self, ec_id):
    self._d_velocity.data.vx = 0.01 * self.x
    self._d_velocity.data.va = 0.01 * self.y
    self._velocityOut.write()
    
    return RTC.RTC_OK

  def set_pos(self, pos, pol):
    self.x = pos[0]
    self.y = pos[1]
    self.r = pol[0]
    self.th = pol[1]



#def MyModuleInit(manager):
#    profile = OpenRTM_aist.Properties(defaults_str=tkjoystickvelocity_spec)
#    manager.registerFactory(profile,
#                            TkJoyStickVelocity,
#                            OpenRTM_aist.Delete)
#
#    # Create a component
#    comp = manager.createComponent("TkJoyStickVelocity")



def main():
  tkJoyCanvas.master.title("TkJoyStickVelocity")
  mgr = OpenRTM_aist.Manager.init(sys.argv)
  mgr.activateManager()

  # Register component
  profile = OpenRTM_aist.Properties(defaults_str=tkjoystickvelocity_spec)
  mgr.registerFactory(profile,
                      TkJoyStickVelocity,
                      OpenRTM_aist.Delete)
  # Create a component
  comp = mgr.createComponent("TkJoyStickVelocity")

  tkJoyCanvas.set_on_update(comp.set_pos)
  mgr.runManager(True)
  tkJoyCanvas.mainloop()

if __name__ == "__main__":
  main()

