#ifndef _OLED_DISPLAY_H_
#define _OLED_DISPLAY_H_
#include <stdio.h>
#include "logo.h"
#include "clock.h"
#include "nrf_twi_sensor.h"
#include "nrf_delay.h"

#define data_slave_addr     0b0111101
#define command_slave_addr  0b0111100
#define inverse_display     0xA7
#define normal_display      0xA6
#define MUX_COMMAND         0xA8
#define MUX_DEFAULT         0x3F

#define DISPLAY_OFFSET_COMMAND      0xD3
#define DISPLAY_OFFSET_DEFAULT      0x00
#define SET_DISPLAY_START_LINE      0x40
#define SET_SEGMENT_REMAP           0xA1
#define SCAN_DIRECTION              0xC8
#define COM_PINS                    0xDA
#define COM_PINS_DEFAULT            0x12
#define CONTRAST_COMMAND            0x81
#define CONTRAST_DEFAULT            0x7F
#define DISABLE_ENTIRE_DISPLAY_ON   0xA4
#define SET_NORMAL_DISPLAY          0xA6
#define SET_OSC_FREQ                0xD5
#define OSC_FREQ_DEFAULT            0x80
#define ENABLE_PUMP_REGULATOR       0x8D
#define DEFAULT_PUMP_REGULATOR      0x14
#define SET_DISPLAY_ON              0xAF
#define SET_DISPLAY_OFF             0xAE

bool oled_command (uint8_t command, nrf_twi_sensor_t const * p_instance);
void OLED_BLANK(nrf_twi_sensor_t const * p_instance);
void OLED_INIT(nrf_twi_sensor_t const * p_instance);
bool OLED_LOGO(nrf_twi_sensor_t const * p_instance);
bool OLED_CLOCK(nrf_twi_sensor_t const * p_instance);
bool OLED_INVERT(nrf_twi_sensor_t const * p_instance);
bool OLED_NORMAL(nrf_twi_sensor_t const * p_instance);
#endif