# Arduino autonomous car project

### Designed an autonomous vehicle that:
* Detects traffic signs (start, slope, stop) using ESP32-CAM and OpenCV
* Maintains 10 cm/sec speed via PID control despite slope changes (0-100% incline)
* Prevents lane deviations with real-time line-following algorithms
* Dynamically adjusts DC motors using mathematical modeling

### Hardware
* Arduino Uno (Motor control)
* ESP32-CAM (Sign detection)
* L298N Motor Driver
* Infrared line sensors (Track following)
* LED indicators (Slope transition feedback)
* Ultrasonic sensor (HCSR04)

### Software
* OpenCV (Sign recognition)
* Python (Image processing logic)
* Arduino C++ (Motor/sensor control)
