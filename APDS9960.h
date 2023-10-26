// This module contains the functions to work with APDS-9960 Sensor
#ifndef APDS9960_H
#define APDS9960_H


#define LED_SCK 0
#define LED_R 1
#define LED_B 2
#define LED_G 3
#define APDS_9960_ADDR 0x39
#define ENABLE 0x80
#define ATIME 0x81
#define WTIME 0x83
#define AILTL 0x84
#define AILTH 0x85
#define AIHTL 0x86
#define AIHTH 0x87
#define PILT 0x89
#define PIHT 0x8B
#define PERS 0x8C
#define CONFIG1 0x8D
#define PPULSE 0x8E
#define CONTROL 0x8F
#define CONFIG2 0x90
#define ID 0x92
#define STATUS 0x93
#define CDATAL 0x94
#define CDATAH 0x95
#define RDATAL 0x96
#define RDATAH 0x97
#define GDATAL 0x98
#define GDATAH 0x99
#define BDATAL 0x9A
#define BDATAH 0x9B
#define PDATA 0x9C
#define POFFSET_UR 0x9D
#define POFFSET_DL 0x9E
#define CONFIG3 0x9F
#define GPENTH 0xA0
#define GEXTH 0xA1
#define GCONF1 0xA2
#define GCONF2 0xA3			// Including wait time between gesture detection cycles
#define GOFFSET_U 0xA4
#define GOFFSET_D 0xA5
#define GOFFSET_L 0xA7
#define GOFFSET_R 0xA9
#define GPULSE 0xA6 // Set to 0b10001111 or 0x8F
#define GCONF3 0xAA
#define GCONF4 0xAB
#define GFLVL 0xAE
#define GSTATUS 0xAF
#define IFORCE 0xE4
#define PICLEAR 0xE5
#define CICLEAR 0xE6
#define AICLEAR 0xE7
#define GFIFO_U 0xFC
#define GFIFO_D 0xFD
#define GFIFO_L 0xFE
#define GFIFO_R 0xFF

#define ENABLE_GESTURE_START 0b01000101
#define ENABLE_PROXIMITY 0b00100101
#define GPULSE_DEFAULT 0b10001111
#define GCONF1_DEFAULT 0b11000000
#define GCONF2_DEFAULT 0b00101010
#define GCONF4_GESTURE_START 0b00000001



enum {
  GESTURE_NONE = -1,
  GESTURE_UP = 0,
  GESTURE_DOWN = 1,
  GESTURE_LEFT = 2,
  GESTURE_RIGHT = 3
};

bool _gestureEnabled = false;
bool _gestureIn = false;
int _gestureDirectionX = 0;
int _gestureDirectionY = 0;
int _gestureDirInX = 0;
int _gestureDirInY = 0;
int _gestureSensitivity = 30; // You may change the sensitivity for better use of gesture sensing.
int _detectedGesture = GESTURE_NONE;
#endif