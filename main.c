/**
 * Copyright (c) 2016 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup tw_scanner main.c
 * @{
 * @ingroup nrf_twi_example
 * @brief TWI Sensor Example main file.
 *
 * This file contains the source code for a sample application using TWI.
 *
 */
#include "main.h"

static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

void OLED_SLEEP_HANDLER()
{
  display_status_old = display_status;
  display_status = 0;
  sleep_timer_flag = 0;
  oled_command(SET_DISPLAY_OFF, &m_twi_oled);
}

void OLED_CLOCK_HANDLER(){
  update_digit_array();
  map_array();

  if(display_status == 2) {
    uint8_t data_sig = 0x40;
    ret_code_t err_code;
    nrf_twi_sensor_t* p_instance = &m_twi_oled;
    for (uint8_t m=0; m<8; m++) {
      if(text_array[m] != text_array_old[m])
      {
        oled_command(0x21, p_instance);
        oled_command(0x08+m*14, p_instance);
        oled_command(0x15+m*14, p_instance);
        oled_command(0x22, p_instance);
        oled_command(0x03, p_instance);
        oled_command(0x04, p_instance);

        err_code = nrf_twi_sensor_reg_write(p_instance,
                                            command_slave_addr,
                                            data_sig,
                                            empty_digit,
                                            28);
        nrf_delay_ms(20);
        APP_ERROR_CHECK(err_code);

        err_code = nrf_twi_sensor_reg_write(p_instance,
                                            command_slave_addr,
                                            data_sig,
                                            text_array[m],
                                            28);
        nrf_delay_ms(20);
        APP_ERROR_CHECK(err_code);
        text_array_old[m] = text_array[m];
      }
    }
  }
}

/*
void Proximity_Int_Handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    uint8_t pdata, dummy;
    ret_code_t err_code;
    err_code = APDS9960_READ(PDATA, &pdata, 1);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Proximity detected: %d", pdata);

    bsp_board_led_invert(LED_G);
    nrf_delay_ms(200);
    bsp_board_led_invert(LED_G);
  
    err_code = APDS9960_READ(PICLEAR, &dummy, 1);
    APP_ERROR_CHECK(err_code);

}
*/


/**
 * @brief Function for configuring: APDS_9960_INT_PIN pin for input,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);


    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(APDS_9960_INT_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(APDS_9960_INT_PIN, true);
}

/**
 * @brief TWI initialization.
 */
void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
    switch (p_event->type)
    {
    case NRF_DRV_TWI_EVT_DONE:
        if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
        {
        }

        m_xfer_done = true;
        break;
    default:
        break;
    }
}

void twi_init_sensor(void) {
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
      .scl = ARDUINO_SCL1_PIN,
      .sda = ARDUINO_SDA1_PIN,
      .frequency = NRF_DRV_TWI_FREQ_100K,
      .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
      .clear_bus_init = false};

  err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
  APP_ERROR_CHECK(err_code);

  nrf_drv_twi_enable(&m_twi);
  // To secure correct signal levels on the pins used by the TWI master when the system is in OFF mode, and
  // when the TWI master is disabled, SDA and SCL pins must be configured in the GPIO peripheral as follows:
  // SCL As specified in PSEL.SCL: Input H0D1
  // SDA As specified in PSEL.SDA: Input H0D1
  nrf_gpio_cfg(ARDUINO_SDA1_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP,
      NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(ARDUINO_SCL1_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP,
      NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);
}

void twi_init_oled(void) {
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
      .scl = ARDUINO_SCL2_PIN,
      .sda = ARDUINO_SDA2_PIN,
      .frequency = NRF_DRV_TWI_FREQ_100K,
      .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
      .clear_bus_init = false};

  err_code = nrf_twi_mngr_init(&m_twi_oled_mngr, &twi_config);
  APP_ERROR_CHECK(err_code);
  err_code = nrf_twi_sensor_init(&m_twi_oled);
  APP_ERROR_CHECK(err_code);

  // To secure correct signal levels on the pins used by the TWI master when the system is in OFF mode, and
  // when the TWI master is disabled, SDA and SCL pins must be configured in the GPIO peripheral as follows:
  // SCL As specified in PSEL.SCL: Input H0D1
  // SDA As specified in PSEL.SDA: Input H0D1
  nrf_gpio_cfg(ARDUINO_SDA2_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP,
      NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(ARDUINO_SCL2_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP,
      NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);
}

/*
 * dev_addr -> Device address
 * reg_adddr -> Register address
 * data -> Buffer which holds data to be written
 * length -> Length of data to be written
 */
int8_t APDS9960_WRITE(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
  /* Write the data to APDS-9960 sensor by I2C protocol
   * reg_addr -> Register address
   * data -> Buffer where data read from TWI will be stored
   * length -> Length of data to be write
   */
    uint32_t err_code;
    uint8_t buffer[255] = {0};
    buffer[0] = reg_addr;
    memcpy(&buffer[1], data, length);
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, APDS_9960_ADDR, buffer, length + 1, false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);

   return err_code;
}


int8_t APDS9960_READ(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
  /* Read the data in APDS-9960 sensor by I2C protocol
   * reg_addr -> Register address
   * data -> Buffer where data read from TWI will be stored
   * length -> Length of data to be read
   */
    uint32_t err_code;
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, APDS_9960_ADDR, &reg_addr, 1, true);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);

    m_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, APDS_9960_ADDR, data, length);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);

    return err_code;

}

bool getGSTATUS(uint8_t *r)
{
      // Get the content from GSTATUS register
      uint8_t err_code = APDS9960_READ(GSTATUS, (uint8_t*) r, 1);
      if (err_code == 0) return true;
      return false;
}

bool getGFLVL(uint8_t *r)
{
  // Get the content from GFLVL register
  uint8_t err_code = APDS9960_READ(GFLVL, (uint8_t*) r, 1);
  if (err_code == 0) return true;
  return false;
}

bool getENABLE(uint8_t *r)
{
  // Get the content from ENABLE register
  uint8_t err_code = APDS9960_READ(ENABLE, (uint8_t*) r, 1);
  if (err_code == 0) return true;
  return false;
}

bool setENABLE(uint8_t r)
{
// Set value to ENABLE register
  uint8_t err_code = APDS9960_WRITE(ENABLE, (uint8_t*) &r, 1);
  if (err_code == 0) return true;
  return false;
}

int readGesture() {
  int gesture = _detectedGesture;
  _detectedGesture = GESTURE_NONE;
  return gesture;
}

int gestureFIFOAvailable(void) {
  uint8_t r;
  if (!getGSTATUS(&r))
    return -1;
  // Fill in the appropriate register
  if ((r & 1) == 0) {
    return -2;
    }
  if (!getGFLVL(&r))
    return -3;

  return (int) r;
}

bool enableGesture(void) {
  uint8_t r;
  if (!getENABLE(&r))
    return false;
  // Fill in the appropriate register
  if ((r & 0b1000101) != 0) {
    _gestureEnabled = true;
    return true;
  }
  // Fill in the appropriate register
  r |= ENABLE_GESTURE_START;
  bool res = setENABLE(r);
  _gestureEnabled = res;
  return res;
}

uint8_t readGFIFO_U(uint8_t *fifo_data, int byte_available)
{
  uint8_t err_code = APDS9960_READ(GFIFO_U, (uint8_t *) fifo_data, byte_available);
  if (err_code != 0) return 0xFF;
}

int handleGesture() {
  // May change the threshold for better use of gesture sensing
  const int gestureThreshold = 30;
  // Check if FIFO data is ready to read
  int available = gestureFIFOAvailable();


  // Read the gesture data in sensor and store to fifo_data
  if (available <= 0)
    return 0;

  uint8_t fifo_data[128];
  uint8_t bytes_read = readGFIFO_U(fifo_data, available * 4);

  // Process the gesture data and indicate the motion
  for (int i = 0; i + 3 < available * 4; i += 4) {
    uint8_t u, d, l, r;
    u = fifo_data[i];
    d = fifo_data[i + 1];
    l = fifo_data[i + 2];
    r = fifo_data[i + 3];
    NRF_LOG_INFO("%x,%x,%x,%x", u, d, l, r);

    if ((u < gestureThreshold) && (d < gestureThreshold) && (l < gestureThreshold) && (r < gestureThreshold)) {
      _gestureIn = true;
      if (_gestureDirInX != 0 || _gestureDirInY != 0) {
        int totalX = _gestureDirInX - _gestureDirectionX;
        int totalY = _gestureDirInY - _gestureDirectionY;
        NRF_LOG_INFO("OUT %d, %d", totalX, totalY);
        sleep_timer_reset();
        if (totalX < -_gestureSensitivity) {
          _detectedGesture = GESTURE_LEFT;
        }
        if (totalX > _gestureSensitivity) {
          _detectedGesture = GESTURE_RIGHT;
        }

        if (totalY < -_gestureSensitivity) {
          _detectedGesture = GESTURE_DOWN;
        }
        if (totalY > _gestureSensitivity) {
          _detectedGesture = GESTURE_UP;
        }

        _gestureDirectionX = 0;
        _gestureDirectionY = 0;
        _gestureDirInX = 0;
        _gestureDirInY = 0;
      }
      continue;
    }
    _gestureDirectionX = r - l;
    _gestureDirectionY = u - d;
    if (_gestureIn) {
      _gestureIn = false;
      _gestureDirInX = _gestureDirectionX;
      _gestureDirInY = _gestureDirectionY;
    }
  }
}

uint8_t gesture_init()
{
  // Pulse length and pulse count init
  uint8_t r = GPULSE_DEFAULT;
  int8_t err_code = APDS9960_WRITE(GPULSE, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  // Set GFIFOTH level
  err_code = APDS9960_READ(GCONF1, (uint8_t*) &r, 1);
  r |= GCONF1_DEFAULT;
  err_code = APDS9960_WRITE(GCONF1, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  // Set proximity gain, LED drive strength and waitime between detection cycles
  r = GCONF2_DEFAULT;
  err_code = APDS9960_WRITE(GCONF2, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  // Enable both sensing dimensions
  err_code = APDS9960_READ(GCONF3, (uint8_t*) &r, 1);
  r &= ~(0b00000011);
  APP_ERROR_CHECK(err_code);
  err_code = APDS9960_WRITE(GCONF3, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  // Set GMODE to 0
  err_code = APDS9960_READ(GCONF4, (uint8_t*) &r, 1);
  if (err_code != 0) return 0;
  r &= ~(0b00000001);
  err_code = APDS9960_WRITE(GCONF4, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  // Set proximity gain
  r = 0b00000100;
  err_code = APDS9960_WRITE(CONTROL, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);
  
  // Set Proximity Threshold
  r = 0x00;
  err_code = APDS9960_WRITE(PILT, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);

  r = 0x32;
  err_code = APDS9960_WRITE(PIHT, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);
  
  // Set Proximity Persistance
  r = 0xA0;
  err_code = APDS9960_WRITE(PERS, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);
  
  // Clear Proximity Interrupt
  err_code = APDS9960_READ(PICLEAR, &r, 1);
  APP_ERROR_CHECK(err_code);

  // Config GMODE exit
  err_code = APDS9960_READ(GCONF1, (uint8_t*) &r, 1);
  if (err_code != 0) return 0;
  r &= ~(0b011111);
  err_code = APDS9960_WRITE(GCONF1, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);
  // Set GMODE exit threshold
  r = 0;
  err_code = APDS9960_WRITE(GEXTH, &r, 1);
  // Enable Proximity only mode
  r = ENABLE_PROXIMITY;
  err_code = APDS9960_WRITE(ENABLE, (uint8_t*) &r, 1);
  APP_ERROR_CHECK(err_code);
  return 0b00000001;
  
}

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){

    NRF_LOG_INFO("Proximity interrupt triggered.");    
    NRF_LOG_FLUSH();
    gesture_flag = true;
    if(display_status == 0) {
      display_status = display_status_old;
      sleep_flag = false;
      if(display_status == 1) OLED_CLOCK(&m_twi_oled);
      nrf_delay_ms(200);
      oled_command(SET_DISPLAY_ON, &m_twi_oled);
      sleep_timer_reset();
    }

    // Set GMODE to 0 and start the sensor
    uint8_t r1 = 0b00000001;
    ret_code_t err_code = APDS9960_WRITE(GCONF4, (uint8_t*) &r1, 1);
    r1 = ENABLE_GESTURE_START;
    err_code = APDS9960_WRITE(ENABLE, &r1, 1);    
    APP_ERROR_CHECK(err_code);    

    while (display_status) { // Change 0 back to display_status
      nrf_delay_ms(100);
      if (gestureFIFOAvailable() > 0) {
        handleGesture();
    
        /******************************************/
        /* Control the LEDs from detected gesture */
        bool oled_success = false;
        bsp_board_leds_off();
        if (_detectedGesture == GESTURE_UP) {
          // red led, display shows inverted version
          bsp_board_led_on(LED_R);
          oled_success = OLED_INVERT(&m_twi_oled);
          if(!oled_success) abort();        
          NRF_LOG_INFO("UP.");
          nrf_delay_ms(250);
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;
        } 
        else if (_detectedGesture == GESTURE_DOWN) {
          // blue led, display shows normal version
          bsp_board_led_on(LED_B);
          oled_success = OLED_NORMAL(&m_twi_oled);
          if(!oled_success) abort();        
          NRF_LOG_INFO("DOWN");
          nrf_delay_ms(250);
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;
        } 
        else if (_detectedGesture == GESTURE_LEFT) {
          // green led, display shows logo
          bsp_board_led_on(LED_G);
          oled_success = OLED_LOGO(&m_twi_oled);
          display_status = 1;
          if(!oled_success) abort();
        
          NRF_LOG_INFO("LEFT");
          nrf_delay_ms(250);
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;

        } 
        else if (_detectedGesture == GESTURE_RIGHT) {
          // white led, display shows clock
          bsp_board_led_on(LED_R);
          bsp_board_led_on(LED_G);
          bsp_board_led_on(LED_B);
          oled_success = OLED_CLOCK(&m_twi_oled);
          display_status = 2;
          if(!oled_success) abort();
        
          NRF_LOG_INFO("Right.");
          nrf_delay_ms(250);
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;

        } 
        else {
          NRF_LOG_INFO("None");
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;
        
        }
      }
    }
    r1 = 0b100;
    err_code = APDS9960_WRITE(GCONF4, &r1, 1);
    APP_ERROR_CHECK(err_code);
    
    err_code = APDS9960_READ(AICLEAR, &r1, 1);
    r1 = ENABLE_PROXIMITY;
    err_code = APDS9960_WRITE(ENABLE, &r1, 1);      // Turn off Gesture engine

    APP_ERROR_CHECK(err_code);
}

void sleep_timer_reset(void)
{
  ret_code_t err_code;
  if(sleep_timer_flag)
  {
      err_code = app_timer_stop(m_timer_sleep);
      APP_ERROR_CHECK(err_code);
      err_code = app_timer_start(m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
      APP_ERROR_CHECK(err_code);
  }
  else
  {
      err_code = app_timer_start(m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
      sleep_timer_flag = true;     
  }
}

void sleep_timer_stop(void)
{
  ret_code_t err_code;
  if(sleep_timer_flag)
  {
      err_code = app_timer_stop(m_timer_sleep);
      APP_ERROR_CHECK(err_code);
      sleep_timer_flag = false;
  }
  else return;
}

////////////////////// LSM9DS1 //////////////////////
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t data_raw_magnetic_field;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float magnetic_field_mgauss[3];
static lsm9ds1_id_t whoamI;
static lsm9ds1_status_t reg;
static uint8_t rst;
char tx_buffer[100];
uint8_t register_address = 0x0F;
//static lsm9ds1_id_t whoamI;

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len){
  uint8_t *i2c_address = handle;
  ret_code_t err_code;
  uint16_t reg16 = reg;
  err_code = nrf_drv_twi_tx(&m_twi, *i2c_address, (uint8_t *)&reg16, 1, true);
  if (NRF_SUCCESS != err_code){
    return 0;
  }
  err_code = nrf_drv_twi_rx(&m_twi, *i2c_address, bufp, len); 
  return 0;
}

static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len){
  uint8_t *i2c_address = handle;
  ret_code_t err_code;
  uint8_t buffer[1 + len];
  memcpy(buffer, &reg, 1);
  memcpy(buffer + 1, bufp, len);
    err_code = nrf_drv_twi_tx(&m_twi, *i2c_address, buffer, len + 1, true);
    if(err_code == NRF_SUCCESS){
      NRF_LOG_INFO("Device Address and Register Address and Data sent");
    }
    NRF_LOG_FLUSH();
  return 0;
}

/////////////////////////////////////////////////////

int main(void){
  ret_code_t err_code;
  uint8_t address;
  uint8_t sample_data;
  uint32_t time_sleep = 10000;
  bool detected_device = false;
  nrf_gpio_pin_set(PIN_VDD_ENV);
  nrf_gpio_pin_set(PIN_R_PULLUP);

  nrf_gpio_cfg(PIN_VDD_ENV, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_cfg(PIN_R_PULLUP, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0H1, NRF_GPIO_PIN_NOSENSE);
  nrf_delay_ms(4);

  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("TWI scanner started.");
  NRF_LOG_FLUSH();
  twi_init_sensor();
  twi_init_oled();
  OLED_INIT(&m_twi_oled);
  map_array();
  OLED_CLOCK(&m_twi_oled);

  /**********************************************/
  /* All the initialization should be done here */
  // Please read the lab document carefully!
  bsp_board_init(BSP_INIT_LEDS);
  bsp_board_leds_off();
  // Pulse length and pulse count init
  lfclk_request();
  gesture_init();
  app_timer_init();
  gpio_init();
  sleep_timer_flag = false;
  sleep_flag = false;
  err_code = app_timer_create (&m_timer_clock, APP_TIMER_MODE_REPEATED, &OLED_CLOCK_HANDLER);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_create(&m_timer_sleep, APP_TIMER_MODE_SINGLE_SHOT, &OLED_SLEEP_HANDLER);
  APP_ERROR_CHECK(err_code);
  app_timer_start(m_timer_clock, APP_TIMER_TICKS(1000), &OLED_CLOCK_HANDLER);
  sleep_timer_reset();

  ////////////////////// LSM9DS1 //////////////////////
  uint8_t i2c_add_mag = LSM9DS1_MAG_I2C_ADD_L >> 1;
  lsm9ds1_ctx_t dev_ctx_mag;
  dev_ctx_mag.write_reg = platform_write;
  dev_ctx_mag.read_reg = platform_read;
  dev_ctx_mag.handle = (void*)&i2c_add_mag;

  /* Initialize magnetic sensors driver interface */
  uint8_t i2c_add_imu = LSM9DS1_IMU_I2C_ADD_H >> 1;
  lsm9ds1_ctx_t dev_ctx_imu;
  dev_ctx_imu.write_reg = platform_write;
  dev_ctx_imu.read_reg = platform_read;
  dev_ctx_imu.handle = (void*)&i2c_add_imu;

  /* Check device ID */
  lsm9ds1_dev_id_get(&dev_ctx_mag, &dev_ctx_imu, &whoamI);
  nrf_delay_ms(10);
  if (whoamI.imu != LSM9DS1_IMU_ID || whoamI.mag != LSM9DS1_MAG_ID){
    while(1){
      // manage here device not found
      NRF_LOG_INFO("\r\nCannot find the LSM9DS1.********\r\n");
      NRF_LOG_FLUSH();
      //printf("\r\nCannot find the LSM9DS1.********\r\n");
    }
  }
  //printf("Who am I register [IMU]: 0x%x [MAG]: 0x%x \r\n", whoamI.imu, whoamI.mag);   

  /* Restore default configuration */
  lsm9ds1_dev_reset_set(&dev_ctx_mag, &dev_ctx_imu, PROPERTY_ENABLE);
  do {
    lsm9ds1_dev_reset_get(&dev_ctx_mag, &dev_ctx_imu, &rst);
  } while (rst);

  /* Enable Block Data Update */
  lsm9ds1_block_data_update_set(&dev_ctx_mag, &dev_ctx_imu, PROPERTY_ENABLE);

  /* Set full scale */
  lsm9ds1_xl_full_scale_set(&dev_ctx_imu, LSM9DS1_4g);
  lsm9ds1_gy_full_scale_set(&dev_ctx_imu, LSM9DS1_2000dps);
  lsm9ds1_mag_full_scale_set(&dev_ctx_mag, LSM9DS1_16Ga);

  /* Configure filtering chain - See datasheet for filtering chain details */
  /* Accelerometer filtering chain */
  lsm9ds1_xl_filter_aalias_bandwidth_set(&dev_ctx_imu, LSM9DS1_AUTO);
  lsm9ds1_xl_filter_lp_bandwidth_set(&dev_ctx_imu, LSM9DS1_LP_ODR_DIV_50);
  lsm9ds1_xl_filter_out_path_set(&dev_ctx_imu, LSM9DS1_LP_OUT);
  /* Gyroscope filtering chain */
  lsm9ds1_gy_filter_lp_bandwidth_set(&dev_ctx_imu, LSM9DS1_LP_ULTRA_LIGHT);
  lsm9ds1_gy_filter_hp_bandwidth_set(&dev_ctx_imu, LSM9DS1_HP_MEDIUM);
  lsm9ds1_gy_filter_out_path_set(&dev_ctx_imu, LSM9DS1_LPF1_HPF_LPF2_OUT);

  /* Set Output Data Rate / Power mode */
  lsm9ds1_imu_data_rate_set(&dev_ctx_imu, LSM9DS1_IMU_59Hz5);
  lsm9ds1_mag_data_rate_set(&dev_ctx_mag, LSM9DS1_MAG_UHP_10Hz);
    ///////////////////////////////////////////////////////////////////

  while (true) {
    // do nothing
    //__WFI();
    err_code = APDS9960_READ(PDATA, &sample_data, 1);
    nrf_delay_ms(100);
    NRF_LOG_INFO("PDATA = %d", sample_data);
    NRF_LOG_FLUSH();

    lsm9ds1_dev_status_get(&dev_ctx_mag, &dev_ctx_imu, &reg);

      if ( reg.status_imu.xlda && reg.status_imu.gda ){
        /* Read imu data */
        memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
        memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_acceleration_raw_get(&dev_ctx_imu, data_raw_acceleration.u8bit);
        lsm9ds1_angular_rate_raw_get(&dev_ctx_imu, data_raw_angular_rate.u8bit);

        acceleration_mg[0] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[0]);
        acceleration_mg[1] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[1]);
        acceleration_mg[2] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[2]);

        angular_rate_mdps[0] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
        angular_rate_mdps[1] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
        angular_rate_mdps[2] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);

        //sprintf((char*)tx_buffer, "IMU - [mg]:%4.2f\t%4.2f\t%4.2f\t[mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
        //        acceleration_mg[0], acceleration_mg[1], acceleration_mg[2],
        //        angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
        NRF_LOG_INFO("IMU - [mg]: %4.2f\t%4.2f\t%4.2f\t[mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
                acceleration_mg[0], acceleration_mg[1], acceleration_mg[2],
                angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
        //printf(tx_buffer/*, strlen((char const*)tx_buffer)*/);
        NRF_LOG_INFO(tx_buffer);
        NRF_LOG_FLUSH();
      }

      if ( reg.status_mag.zyxda ){
        /* Read magnetometer data */
        memset(data_raw_magnetic_field.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_magnetic_raw_get(&dev_ctx_mag, data_raw_magnetic_field.u8bit);

        magnetic_field_mgauss[0] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[0]);
        magnetic_field_mgauss[1] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[1]);
        magnetic_field_mgauss[2] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[2]);

        //sprintf(tx_buffer, "MAG - [mG]:%4.2f\t%4.2f\t%4.2f\r\n",
        //        magnetic_field_mgauss[0], magnetic_field_mgauss[1], magnetic_field_mgauss[2]);
        NRF_LOG_INFO("MAG - [mG]: %4.2f\t%4.2f\t%4.2f\r\n",
                magnetic_field_mgauss[0], magnetic_field_mgauss[1], magnetic_field_mgauss[2]);

        //printf(tx_buffer/*, strlen((char const*)tx_buffer)*/);
        NRF_LOG_INFO(tx_buffer);
        NRF_LOG_FLUSH();
      }
  }
}
