# RTES Challange
This is the code of Spring2024 Embedded Challange
A wearable Parkinsonian tremor detector using only STM32 F429 Discovery board with its embedded gyroscope
## Team Members 
Huaqiu Liu hl5398,
Khushi Sharma ks7406,
Nitisha Shetty ns6108
## Model Choice
The Goertzel Algorithm is an essential part of our project, specifically chosen for its efficiency in detecting specific frequencies within gyroscope data. It focuses on the 3 Hz frequency, characteristic of Parkinsonian tremors, offering an optimal solution for embedded systems.
### Current Code Functionality
#### Gyroscope Data Acquisition: 
The program reads the x, y, and z-axis data from a gyroscope using SPI communication. This happens periodically based on a timer (ticker), which triggers every ten milliseconds.
#### Data Processing:
- **Scaling:** The raw gyroscope data is scaled using a predefined factor to convert it into a more useful unit (presumably radians).
- **Delay Line Filtering:** The y-axis data is then processed through a digital filter described by arrays a and b, which is designed to filter out noise or irrelevant frequencies.
#### Movement Detection:
- **Steadiness Check:** Checks if the combined absolute values of gx and gz are less than 50, a condition likely set to determine if the device is steady or in motion.
- **Detection Reliability:** It then calculates an average value (avg_gy), possibly to smooth out noise and improve the detection reliability.
#### Tremor Indication:
- **Threshold Check:** Based on the processed gyroscope data, the program determines if the detected movement corresponds to a tremor. This is done by checking if avg_gy exceeds a threshold.
- **LEDs Usage:** LEDs are used as indicators. led1 is toggled based on immediate tremor detection, and led2 is managed to indicate prolonged tremor activity or severity.
### Main Functionality
#### Tremor detection
Our device can identify the frequency and how long it last of the user's movement. By doing this, we can accurately detect if a tremor is happened, and prevent mistakenly trigger the alarm.
#### Body movement detection
Our device will detect if the user is moving since resting tremors are visible at rest and occur when a body part (usually the hand or
wrist) is completely supported, and is minimal or absent during activity. 
#### Intensity Identification
Our device can detect the intensity of a detected tremor, and use different ways to notify the situation.
We use static red light to inform a detected tremor, and a flashing red light to inform a serious tremor.

https://github.com/Unicser-tkmk/RTES_Proj/assets/67886071/71ca7128-5f8d-4f61-a730-ba415c8c51a1

