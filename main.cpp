#include "mbed.h"
#include "VL53L0X.h"
#include "MPU6050.h"
// -------------------- Pin Setup --------------------
VL53L0X tof(D14, D15);      // ToF sensor (I2C)
MPU6050 imu(D14, D15);      // IMU on same I2C bus
DigitalOut trigL(D2), trigR(D7);
InterruptIn echoL(D3), echoR(D8);
PwmOut servo(A3);           // Steering servo
PwmOut motorF(D6);          // Forward motor PWM
PwmOut motorR(D5);          // Reverse motor PWM
Timer tL, tR;
// -------------------- Globals --------------------
volatile int pulseL = 0;
volatile int pulseR = 0;
float tof_filtered = 400;   // Start with a reasonable value
float imu_yaw = 0;
// -------------------- Ultrasonic ISRs --------------------
void echoL_rise() { tL.reset(); tL.start(); }
void echoL_fall() { tL.stop(); pulseL = tL.read_us(); }
void echoR_rise() { tR.reset(); tR.start(); }
void echoR_fall() { tR.stop(); pulseR = tR.read_us(); }
// -------------------- Sensor Functions --------------------
float readLeft() {
    return pulseL / 5.8f;   // mm (matches test results)
}
float readRight() {
    return pulseR / 5.8f;   // mm
}
float readFront() {
    int raw = tof.readRangeContinuousMillimeters();
    // Simple smoothing (EMA) – matches ToF test behaviour
    tof_filtered = (0.7f * tof_filtered) + (0.3f * raw);
    return tof_filtered;
}
void readIMU() {
    imu.update();
    imu_yaw = imu.getAngleZ();   // Not used in FSM yet
}
// -------------------- Steering Functions --------------------
void steerStraight()   { servo.pulsewidth_us(1500); }
void steerHalfLeft()   { servo.pulsewidth_us(1300); }
void steerFullLeft()   { servo.pulsewidth_us(1100); }
void steerHalfRight()  { servo.pulsewidth_us(1700); }
void steerFullRight()  { servo.pulsewidth_us(1900); }
// -------------------- Motor Functions --------------------
void driveFast() {
    motorF = 0.8;
    motorR = 0;
}
void driveSlow() {
    motorF = 0.4;
    motorR = 0;
}
void driveReverse() {
    motorF = 0;
    motorR = 0.5;
}
void stopDrive() {
    motorF = 0;
    motorR = 0;
}
// -------------------- Main Program --------------------
int main() {
    // Attach interrupts
    echoL.rise(&echoL_rise);
    echoL.fall(&echoL_fall);
    echoR.rise(&echoR_rise);
    echoR.fall(&echoR_fall);
    tof.startContinuous();
    imu.initialize();
    while (true) {
        // Read sensors
        float left  = readLeft();
        float front = readFront();
        float right = readRight();
        readIMU();
        // Convert to binary states (A,B,C)
        int A = (left  < 300) ? 1 : 0;
        int B = (front < 250) ? 1 : 0;
        int C = (right < 300) ? 1 : 0;
        // Encode into command (0–7)
        int cmd = A + (2 * B) + (4 * C);
        // -------------------- FSM --------------------
        switch (cmd) {
            case 0:   // Clear, Clear, Clear
                steerStraight();
                driveFast();
                break;
            case 1:   // Left blocked
                steerHalfRight();
                driveFast();
                break;
            case 2:   // Front blocked
                steerHalfLeft();
                driveReverse();
                break;
            case 3:   // Left + Front blocked
                steerFullLeft();
                driveReverse();
                break;
            case 4:   // Right blocked
                steerHalfLeft();
                driveFast();
                break;
            case 5:   // Left + Right blocked
                steerStraight();
                driveSlow();
                break;
            case 6:   // Front + Right blocked
                steerFullRight();
                driveReverse();
                break;
            case 7:   // All blocked
                steerHalfLeft();
                driveReverse();
                break;
            default:
                stopDrive();
                break;
        }
        thread_sleep_for(50);   // 50 ms loop
    }
}
