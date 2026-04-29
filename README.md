# Autonomous Corridor Navigation Robot

This project is an embedded system built on the Mbed platform. It lets a small robot navigate corridors autonomously using three sensors (left and right ultrasonic sensors, and a front Time-of-Flight sensor) and a simple finite state machine (FSM) to decide steering and motor behavior.

Overview

The robot reads distances from three sensors and converts those readings into a small set of discrete states. The FSM uses those states to decide whether to drive forward, slow down, reverse, or steer left/right. The code is intended to be compact and easy to tune.

Hardware

Component | Notes
--- | ---
Mbed-compatible MCU | Any supported board (for example, an STM32 Nucleo)
VL53L0X | Front-facing ToF distance sensor
MPU6050 | IMU used for yaw measurements (not required for basic FSM)
HC-SR04 (x2) | Left and right ultrasonic sensors
Steering servo | Controls steering angle
DC motors | One for forward drive and one for reverse/brake (controlled via PWM)

Pin connections

Pin | Function
--- | ---
D14, D15 | I2C (SDA, SCL) for VL53L0X and MPU6050
D2 | Left ultrasonic TRIG
D3 | Left ultrasonic ECHO
D7 | Right ultrasonic TRIG
D8 | Right ultrasonic ECHO
A3 | Steering servo PWM
D6 | Forward motor PWM
D5 | Reverse motor PWM

How it works

The code reads the three sensors each loop and converts the numeric distances to three binary variables:
- A: left ultrasonic indicates an obstacle if distance &lt; 300 mm
- B: front ToF indicates an obstacle if distance &lt; 250 mm
- C: right ultrasonic indicates an obstacle if distance &lt; 300 mm

These three bits are combined into a single command value: cmd = A + 2*B + 4*C. The command (0 to 7) selects one of eight FSM states which set steering and motor outputs.

FSM summary

CMD | Left (A) | Front (B) | Right (C) | Action
--- | --- | --- | --- | ---
0 | clear | clear | clear | Drive straight fast
1 | blocked | clear | clear | Steer half right, drive fast
2 | clear | blocked | clear | Steer half left, reverse
3 | blocked | blocked | clear | Steer full left, reverse
4 | clear | clear | blocked | Steer half left, drive fast
5 | blocked | clear | blocked | Stay straight, drive slow
6 | clear | blocked | blocked | Steer full right, reverse
7 | blocked | blocked | blocked | Steer half left, reverse

Files

- main.cpp — firmware source (this repository contains main.cpp)
- README.md — project documentation
- .gitignore — ignore build artifacts

Building and flashing

1. Open Mbed Studio or the Mbed Online Compiler and create a new project for your target board.
2. Add the VL53L0X and MPU6050 libraries (if you use the IMU).
3. Copy main.cpp into the project and build.
4. Flash the binary to your board.

Notes on sensors and tuning

- Ultrasonic distance: the code uses pulse_us / 5.8 to estimate distance in millimeters. That factor was chosen to match test results but may need calibration for your hardware and clock settings.
- Front ToF: readings are smoothed with a simple exponential moving average (filtered = 0.7*previous + 0.3*new). You can change the smoothing weights to make the response faster or more stable.
- The thresholds (300 mm for side sensors, 250 mm for front) are starting points. If your robot is larger or faster, reduce the thresholds or add hysteresis to avoid oscillation.

Future improvements

- Use the IMU yaw to correct and maintain heading using a PID controller.
- Add rate-limited acceleration and deceleration to make motion smoother.
- Log sensor data over serial for debugging and offline analysis.
- Add support for mapping or higher-level navigation.

License

This project is provided under the MIT License.

Author

Mohamed (GitHub: mohamedd12345)
