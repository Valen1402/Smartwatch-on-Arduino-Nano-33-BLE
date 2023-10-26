    app_timer_stop(&m_timer_sleep);
    nrf_delay_ms(100);
    
    if (gestureFIFOAvailable() > 0) {
      handleGesture();

      /******************************************/
      /* Control the LEDs from detected gesture */
      bool oled_success = false;
      bsp_board_leds_off();
      nrf_drv_timer_compare_int_enable(&m_timer, NRF_TIMER_CC_CHANNEL0);
      if (_detectedGesture == GESTURE_UP) {
        // red led, display shows inverted version
        bsp_board_led_on(LED_R);
        oled_success = OLED_INVERT(&m_twi_oled);
        if(!oled_success) abort();
        app_timer_start(&m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
        nrf_delay_ms(250);
        bsp_board_leds_off();
        _detectedGesture = GESTURE_NONE;

      } 
      else if (_detectedGesture == GESTURE_DOWN) {
        // blue led, display shows normal version
        bsp_board_led_on(LED_B);
        oled_success = OLED_NORMAL(&m_twi_oled);
        if(!oled_success) abort();
        app_timer_start(&m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
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
        app_timer_start(&m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
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
        app_timer_start(&m_timer_sleep, APP_TIMER_TICKS(5000), &OLED_SLEEP_HANDLER);
        nrf_delay_ms(250);
        bsp_board_leds_off();
        _detectedGesture = GESTURE_NONE;

      } 
      else {
          bsp_board_leds_off();
          _detectedGesture = GESTURE_NONE;
          continue;