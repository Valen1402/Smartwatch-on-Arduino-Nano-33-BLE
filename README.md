# Smartwatch-on-Arduino-with-nRF52840
Smartwatch design running on Arduino Nano 33 BLE Sense board. The board is using nRF52840 MCU by Nordic Semiconductors, based on the ARM Cortex-M4 processor. The authors are Minh-Quang NGUYEN and Quang Huy LE, in spring 2023.

Smartwatch features:
- Automatic Time Update (using timer interrupt) and manual Time Update (synced with smartphone via Bluetooth)
- Hand Gesture control: change the OLED screen _(SSD1306)_ with Proximity and Gesture sensor _(APDS9960)_ detecting UP, DOWN, RIGHT, LEFT movement using Interrupt.
- Fall detection: detect the potential fall accident of the watcher bearer, using Accelerometer and Gyroscope _(LSM9DS1)_. Sending Alert via Bluetooth to Smartphone using Alert Service.
- For features demostration, check out the demo video (low quality):
https://drive.google.com/file/d/11LvpyeYC4P7Ks7we7ZQ3CfzgQJ9pHEWI/view

Other information:
- Platform: SEGGER Embedded Studio for ARM
- Project specification: please find the 4 pdf files which details every step to design this smartatch, named lab1-4.
- Note: Most of the source code files are presented on Github. For whole project, please check this GoogleDrive link
https://drive.google.com/uc?id=1nqgUX4rcbP2jN76JrX9EiGpa1U8Kza-Y&export=download
You then can find those source code on github at
nRF5_SDK_17.1.0_ddde560\examples\peripheral\ee414_lab3_team05\pca10056\blank\ses
- For result reproduction, one simplest way are including the whole directory nRF5_SDK_17.1.0_ddde560 in your Segger directory. This ensures all the details such as changes in SDK file and libraries are included.
