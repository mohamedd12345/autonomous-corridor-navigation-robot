# 🤖 Autonomous Corridor Navigation Robot

An embedded systems project built on the **Mbed platform** that enables a robot to autonomously navigate through corridors using a combination of ToF, ultrasonic, and IMU sensors with a Finite State Machine (FSM) control logic.

---

## 📋 Description

This robot uses three sensors to detect obstacles:
- **Left & Right** — HC-SR04 Ultrasonic sensors
- **Front** — VL53L0X Time-of-Flight (ToF) sensor

Based on sensor readings, an FSM decides the steering direction and motor speed in real time, allowing the robot to avoid walls and navigate corridors without human input.

---

## 🛠️ Hardware Components

| Component | Description |
|---|---|
| Mbed-compatible MCU | Main microcontroller (e.g. STM32 Nucleo) |
| VL53L0X | Front-facing Time-of-Flight distance sensor |
| MPU6050 | IMU for yaw angle tracking |
| HC-SR04 (x2) | Left and right ultrasonic sensors |
| Servo Motor | Steering control |
| DC Motor (x2) | Forward and reverse drive |

---

## 📌 Pin Connections

| Pin | Connected To |
|---|---|
| D14, D15 | I2C SDA/SCL (VL53L0X + MPU6050) |
| D2 | Ultrasonic Left TRIG |
| D3 | Ultrasonic Left ECHO |
| D7 | Ultrasonic Right TRIG |
| D8 | Ultrasonic Right ECHO |
| A3 | Steering Servo PWM |
| D6 | Forward Motor PWM |
| D5 | Reverse Motor PWM |

---

## 🧠 How It Works

The robot continuously reads all three sensors and converts them into binary states:

| Variable | Sensor | Threshold | Meaning |
|---|---|---|---|
| A | Left ultrasonic | < 300 mm | Left is blocked |
| B | Front ToF | < 250 mm | Front is blocked |
| C | Right ultrasonic | < 300 mm | Right is blocked |

These three binary values are encoded into a single command (`cmd = A + 2B + 4C`) and fed into an FSM with 8 possible states.

---

## 📊 FSM State Table

| CMD | A (Left) | B (Front) | C (Right) | Steering | Drive |
|---|---|---|---|---|---|
| 0 | Clear | Clear | Clear | Straight | Fast |
| 1 | Blocked | Clear | Clear | Half Right | Fast |
| 2 | Clear | Blocked | Clear | Half Left | Reverse |
| 3 | Blocked | Blocked | Clear | Full Left | Reverse |
| 4 | Clear | Clear | Blocked | Half Left | Fast |
| 5 | Blocked | Clear | Blocked | Straight | Slow |
| 6 | Clear | Blocked | Blocked | Full Right | Reverse |
| 7 | Blocked | Blocked | Blocked | Half Left | Reverse |

---

## 📂 File Structure

```
autonomous-corridor-navigation-robot/
│
└── autonomous-corridor-navigation-robot.cpp    # Main source code
└── README.md                                   # Project documentation
```

---

## ⚙️ How to Build & Flash

1. Open [Mbed Studio](https://os.mbed.com/studio/) or the [Mbed Online Compiler](https://ide.mbed.com/)
2. Create a new project and select your target board
3. Import the following libraries:
   - `VL53L0X`
   - `MPU6050`
4. Copy the contents of `autonomous-corridor-navigation-robot.cpp` into `main.cpp`
5. Click **Build** then **Flash** to upload to your board

---

## 📐 Sensor Calibration Notes

- Ultrasonic distance formula: `pulse_us / 5.8 = distance in mm`
- ToF uses **Exponential Moving Average (EMA)** smoothing:  
  `filtered = (0.7 × previous) + (0.3 × new_reading)`
- IMU yaw angle is read but reserved for future use

---

## 🚀 Future Improvements

- [ ] Use IMU yaw to drive in a straight line (PID correction)
- [ ] Add speed ramping for smoother acceleration
- [ ] Implement a mapping/logging system over serial
- [ ] Add Bluetooth remote override

---

## 📜 License

This project is licensed under the **MIT License** — free to use, modify, and distribute.

---

## 👤 Author

**Mohamed** — [@mohamedd12345](https://github.com/mohamedd12345)
