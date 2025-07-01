
import time
import math
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative
from pymavlink import mavutil 
#gazebo
vehicle = connect("/dev/ttyAMA0",wait_ready=True,timeout=100,baud=57600)

vehicle.parameters['LAND_SPEED_HIGH']= 300

def condition_yaw(heading, relative=False):

    if relative:
        is_relative = 1 #yaw relative to direction of travel
    else:
        is_relative = 0 #yaw is an absolute angle
    # create the CONDITION_YAW command using command_long_encode()
    msg = vehicle.message_factory.command_long_encode(
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_CMD_CONDITION_YAW, #command
        0, #confirmation
        heading,    # param 1, yaw in degrees
        0,          # param 2, yaw speed deg/s
        1,          # param 3, direction -1 ccw, 1 cw
        is_relative, # param 4, relative offset 1, absolute angle 0
        0, 0, 0)    # param 5 ~ 7 not used
    # send command to vehicle
    vehicle.send_mavlink(msg)


def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):

    msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # time_boot_ms (not used)
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_FRAME_LOCAL_NED, # frame
        0b0000111111000111, # type_mask (only speeds enabled)
        0, 0, 0, # x, y, z positions (not used)
        velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
        0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
        0, 0)    # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink) 

    # send command to vehicle on 1 Hz cycle
    for x in range(0,duration):	
        vehicle.send_mavlink(msg)
        time.sleep(0.036)


b=1;
i = 10
alpha= 0
c=1;
accel=0.020

while True:
    condition_yaw(1)
    altitude = vehicle.location.global_relative_frame.alt  
    if altitude > 320 and altitude <360:
	vehicle.mode = VehicleMode("GUIDED")
        send_ned_velocity(0,0,-2,250)
        break

while True:
    
    send_ned_velocity(b*math.sqrt(abs(i/1.1))*math.sin(math.radians(alpha)),math.cos(math.radians(alpha))*b*math.sqrt(abs(i/1.1)),i,1)
    altitude = vehicle.location.global_relative_frame.alt  
    i-=accel
    alpha+=0.27
    
    if i <= -2:
       i = 10
       b=b*(-1)
       alpha=alpha+180
    if altitude < 220:
        send_ned_velocity(0,0,0,350)
	i = 8
        break

while True:
    
    send_ned_velocity(b*math.sqrt(abs(i/1.1))*math.sin(math.radians(alpha)),math.cos(math.radians(alpha))*b*math.sqrt(abs(i/1.1)),i,1)
    altitude = vehicle.location.global_relative_frame.alt  
    i-=0.022
    alpha+=0.27
    
    if i <= -2:
       i = 10
       b=b*(-1)
       alpha=alpha+180
    
    if altitude < 30:
        break
    
vehicle.mode = VehicleMode("LAND")
print("Landing")
time.sleep(5)
vehicle.close()
print("Vehicle Closed")


