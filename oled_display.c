#include "oled_display.h"
#include "logo.h"
#include "clock.h"
#include "nrf_twi_sensor.h"

bool oled_command (uint8_t command, nrf_twi_sensor_t const * p_instance)
{
  uint8_t r[2] = {0x00, command};
  ret_code_t err_code = nrf_twi_sensor_write(p_instance, command_slave_addr, r, 2, false);
  APP_ERROR_CHECK(err_code);
  nrf_delay_ms(10);
  return true;
}

void OLED_INIT(nrf_twi_sensor_t const * p_instance)
{
  int err_code;
  bool flag = false;
  text_array_old[0] = digit2text_array[digit_array[1]];
  text_array_old[1] = digit2text_array[digit_array[5]];
  text_array_old[2] = digit2text_array[10];
  text_array_old[3] = digit2text_array[digit_array[0]];
  text_array_old[4] = digit2text_array[digit_array[2]];
  text_array_old[5] = digit2text_array[10];
  text_array_old[6] = digit2text_array[digit_array[2]];
  text_array_old[7] = digit2text_array[digit_array[8]];
    // Display off
    oled_command(SET_DISPLAY_OFF, p_instance);

    // MUX
    oled_command(MUX_COMMAND, p_instance);
    oled_command(MUX_DEFAULT, p_instance);
    
    // display offset
    oled_command(DISPLAY_OFFSET_COMMAND, p_instance);
    oled_command(DISPLAY_OFFSET_DEFAULT, p_instance);
    
    // display offset
    oled_command(SET_DISPLAY_START_LINE, p_instance);

    // SEGMENT REMAP
    oled_command(SET_SEGMENT_REMAP, p_instance);

    // COM OUTPUT
    oled_command(SCAN_DIRECTION, p_instance);

    // SET COM PINS
    oled_command(COM_PINS, p_instance);
    oled_command(COM_PINS_DEFAULT, p_instance);

    // SET CONTRAST
    oled_command(CONTRAST_COMMAND, p_instance);
    oled_command(CONTRAST_DEFAULT, p_instance);
    
    // Entire display on
    oled_command(DISABLE_ENTIRE_DISPLAY_ON, p_instance);

    // invert display
    oled_command(normal_display, p_instance);

    // set osc frequency
    oled_command(SET_OSC_FREQ, p_instance);
    oled_command(OSC_FREQ_DEFAULT, p_instance);
    
    
    // enable charge pump regulator
    oled_command(ENABLE_PUMP_REGULATOR, p_instance);
    oled_command(DEFAULT_PUMP_REGULATOR, p_instance);

    // choose horizontal addressing mode
    oled_command(0x20, p_instance);
    oled_command(0x00, p_instance);

    // display on
    oled_command(SET_DISPLAY_ON, p_instance);
}

void OLED_BLANK (nrf_twi_sensor_t const * p_instance){
  uint8_t data_sig = 0x40;
  uint8_t BLANK_SCREEN[128] = {};
  ret_code_t err_code;
  for (uint8_t m =0; m<8; m++) {
    err_code = nrf_twi_sensor_reg_write(p_instance,
                                      command_slave_addr,
                                      data_sig,
                                      BLANK_SCREEN,
                                      128);
    nrf_delay_ms(20);
    APP_ERROR_CHECK(err_code);
   }
}

bool OLED_LOGO (nrf_twi_sensor_t const * p_instance){
  uint8_t data_sig = 0x40;
  ret_code_t err_code;

  oled_command(0x21, p_instance);
  oled_command(0x00, p_instance);
  oled_command(0x7F, p_instance);
  oled_command(0x22, p_instance);
  oled_command(0x00, p_instance);
  oled_command(0x07, p_instance);
  for (uint8_t m =0; m<8; m++) {
    err_code = nrf_twi_sensor_reg_write(p_instance,
                                        command_slave_addr,
                                        data_sig,
                                        smile_lab_logoBitmaps + m*128,
                                        128);
    nrf_delay_ms(10);
    APP_ERROR_CHECK(err_code);
  }
  return true;
}

bool OLED_CLOCK(nrf_twi_sensor_t const * p_instance){
  uint8_t data_sig = 0x40;
  ret_code_t err_code;
  OLED_BLANK(p_instance);
  for (uint8_t m =0; m<8; m++) {
    oled_command(0x21, p_instance);
    oled_command(0x08+m*14, p_instance);
    oled_command(0x15+m*14, p_instance);
    oled_command(0x22, p_instance);
    oled_command(0x03, p_instance);
    oled_command(0x04, p_instance);
    err_code = nrf_twi_sensor_reg_write(p_instance,
                                        command_slave_addr,
                                        data_sig,
                                        text_array[m],
                                        28);
    nrf_delay_ms(20);
    APP_ERROR_CHECK(err_code);
  }
  return true;
}

bool OLED_INVERT(nrf_twi_sensor_t const * p_instance){
  oled_command(0xA7, p_instance);
  return true;
}

bool OLED_NORMAL(nrf_twi_sensor_t const * p_instance) {
  oled_command(0xA6, p_instance);
  return true;
}