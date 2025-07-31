import asyncio
from mavsdk import System
from mavsdk.offboard import (OffboardError, VelocityBodyYawspeed)
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
import time


async def main():

    
    drone= System()
    await drone.connect(system_address= "serial:///dev/ttyAMA0:57600")

    # wait for the drone to be ready 
    # print("Waiting for the drone to have GPS estimates...")
    # async for health in drone.telemetry.health():
    #     if health.is_global_position_ok:
    #         print("Global Position is OK.")
    #         break
    
    print("Arming...")
    await drone.action.arm()

    takeoff_alt= 2.0
    await drone.action.set_takeoff_altitude(takeoff_alt)

    print(f'Taking off to {takeoff_alt} meters')
    await drone.action.takeoff()
    await asyncio.sleep(20) #give the drone time to takeoff

    #LAND
    print("Landing...")
    await drone.action.land()
    await asyncio.sleep(10)
    await drone.action.disarm()
    

if __name__ == "__main__":
    picam2= Picamera2()
    video_config = picam2.create_video_configuration()
    picam2.configure(video_config)
    encoder = H264Encoder()

    picam2.start_recording(encoder, "video_ATL.h264")
    asyncio.run(main())
    picam2.stop_recording()
