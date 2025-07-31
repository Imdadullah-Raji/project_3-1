import asyncio
from mavsdk import System
from mavsdk.offboard import VelocityBodyYawspeed, OffboardError
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
import time

async def main():
    drone = System()
    await drone.connect(system_address= "serial:///dev/ttyAMA0:57600")

    # wait for position lock
    # async for health in drone.telemetry.health():
    #     if health.is_global_position_ok:
    #         print("GPS locked.")
    #         break
    #ARM
    await drone.action.arm()
    await drone.action.set_takeoff_altitude(4)
    await drone.action.takeoff()
    await asyncio.sleep(10)

    # START OFFBOARD mode with 0 velocity to prep PX4
    print("Starting offboard control...")
    await drone.offboard.set_velocity_body(VelocityBodyYawspeed(0.0, 0.0, 0.0, 0.0))
    try:
        await drone.offboard.start()
    except OffboardError as e:
        print(f"Offboard failed to start: {e}")
        await drone.action.disarm()
        return
    #move forward in body frame
    speed= 2
    distance= 7 
    duration = distance/speed
    
    await drone.offboard.set_velocity_body(VelocityBodyYawspeed(speed, 0.0, 0.0, 0.0))
    await asyncio.sleep(duration)

    #stop movement
    await drone.offboard.set_velocity_body(VelocityBodyYawspeed(0.0, 0.0, 0.0, 0.0))
    await asyncio.sleep(2)
    # turn by 90 deg
    await drone.offboard.set_velocity_body(
        VelocityBodyYawspeed(0.0, 0.0, 0.0, 30.0))
    await asyncio.sleep(5)
    
    

    #stop offboard
    await drone.offboard.stop()

    print("Landing ... ")
    await drone.action.land()
    await asyncio.sleep(20)
    await drone.action.disarm()

if __name__=='__main__':
    picam2= Picamera2()
    video_config = picam2.create_video_configuration()
    picam2.configure(video_config)
    encoder = H264Encoder()

    picam2.start_recording(encoder, "video_FT.h264")
    asyncio.run(main())
    picam2.stop_recording()
