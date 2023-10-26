#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#include "app_util_platform.h"
#include "boards.h"
#include "nrf_drv_twi.h"
#include "nrf_twi_sensor.h"
#include <stdio.h>
#include "nrf_drv_gpiote.h"
#include "nrfx_gpiote.h"
#include "nrf_drv_clock.h"

#include "lsm9ds1_reg.h"
#include "oled_display.h"
#include "APDS9960.h"
#include "nrf_delay.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID 0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID 1
#endif
#define TIMER_INSTANCE_ID 0
/* Number of possible TWI addresses. */
#define TWI_ADDRESSES 127
#define PIN_VDD_ENV 22  // 0<<5||22 for P0.22
#define PIN_R_PULLUP 32 // 1<<5||0 for P1.0

#define DEVICE_NAME                     "EE414"                       /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_FAST_INTERVAL           40                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_SLOW_INTERVAL           3200                                        /**< Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds). */

#define APP_ADV_FAST_DURATION           3000                                        /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION           18000                                       /**< The advertising duration of slow advertising in units of 10 milliseconds. */


#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define MESSAGE_BUFFER_SIZE             18                                          /**< Size of buffer holding optional messages in notifications. */
#define BLE_ANS_NB_OF_CATEGORY_ID       10                                          /**< Number of categories. */

#define SEC_PARAM_TIMEOUT               30  
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                20                                          /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */
#endif

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */



// Declaration of instances
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
NRF_TWI_MNGR_DEF(m_twi_oled_mngr, 5, 1);
NRF_TWI_SENSOR_DEF(m_twi_oled, &m_twi_oled_mngr, 128);
APP_TIMER_DEF(m_timer_sleep);
APP_TIMER_DEF(m_timer_clock);

// Declaration of global variables                                     /**< Number of peers currently in the whitelist. */


static volatile bool m_xfer_done = false;
static int display_status = 2; // sleep = 0, logo = 1; clock = 2
static int display_status_old;
static bool sleep_flag;
static bool sleep_timer_flag; // true if sleep timer is running and false otherwise
static bool gesture_flag = false; // true if existing a gesture
// Declaration of function in main.c

static void lfclk_request(void);
void OLED_SLEEP_HANDLER();
void OLED_CLOCK_HANDLER();
void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void gpio_init(void);
void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);
void twi_init_sensor(void);
void twi_init_oled(void);


int8_t APDS9960_READ(uint8_t reg_addr, uint8_t *data, uint16_t length);
int8_t APDS9960_WRITE(uint8_t reg_addr, uint8_t *data, uint16_t length);
bool enableGesture(void);
bool getGSTATUS(uint8_t *r);
bool getGFLVL(uint8_t *r);
bool getENABLE(uint8_t *r);
bool setENABLE(uint8_t r);
int readGesture();
int gestureFIFOAvailable(void);
uint8_t readGFIFO_U(uint8_t *fifo_data, int byte_available);
int handleGesture();
uint8_t gesture_init();
void sleep_timer_reset(void);
void sleep_timer_stop(void);
#endif