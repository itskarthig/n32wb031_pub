# N32WB03x API Function Guide

**Version:** V1.0.1
**Date:** 2021.12.27
**Source:** Nations Technologies Inc. (nsing.com.sg)

> This document covers the N32WB03x series Bluetooth SoC chip API functions for development reference.

---

## Table of Contents

- [1. Bluetooth Application Module: ns\_ble.h](#1-bluetooth-application-module-ns_bleh)
- [2. Bluetooth Security Encryption Module: ns\_sec.h](#2-bluetooth-security-encryption-module-ns_sech)
- [3. Software Timer: ns\_timer.h](#3-software-timer-ns_timerh)
- [4. Bluetooth Sleep Module: ns\_sleep.h](#4-bluetooth-sleep-module-ns_sleeph)
- [5. Debug Information Printing Module: log.h](#5-debug-information-printing-module-logh)
- [6. Hard Delay Module: ns\_delay.h](#6-hard-delay-module-ns_delayh)
- [7. Suggestions for Bluetooth Programming](#7-suggestions-for-bluetooth-programming)

---

## 1. Bluetooth Application Module: ns\_ble.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\ble`
**Source Files:** `ns_ble.c`, `ns_ble.h`, `ns_ble_task.c`, `ns_ble_task.h`
**Introduction:** Bluetooth application related APIs, communicating between user application code and Bluetooth protocol stack.

---

### 1.1 ns\_ble\_stack\_init

**Function:** Bluetooth protocol stack initialization, registering Bluetooth message callbacks and user-defined message callbacks. In the `struct ns_stack_cfg_t` structure passed in as a parameter, `ble_msg_handler` is the Bluetooth message callback function that can handle messages declared in `enum app_ble_msg`. `user_msg_handler` is the user message callback function. Users can declare host messages, noting that the message number starts from `APP_FREE_EVE_FOR_USER`, specifically referring to `enum user_msg_id`. Users can immediately hang up a message through the function `ke_msg_send_basic`, or hang up a message at a timed interval through the function `ke_timer_set`. To have a recurring timed message, re-timed hangup of the message can be done again in message handling.

**Syntax:**

```c
void ns_ble_stack_init(struct ns_stack_cfg_t const* p_handler);
```

**Parameters:**

- `[in] p_handler` — Bluetooth application callback function configuration, see `struct ns_stack_cfg_t` definition for details.

**Return:** None

**Example:**

```c
struct ns_stack_cfg_t app_handler;
app_handler.ble_msg_handler = app_ble_msg_handler;   // user ble msg handler
app_handler.user_msg_handler = app_user_msg_handler;  // user custom msg handler
ns_ble_stack_init(&app_handler);
```

**User-defined message callback implementation:**

```c
void app_user_msg_handler(ke_msg_id_t const msgid, void const *p_param)
{
    switch (msgid)
    {
        case APP_CUSTS_TEST_EVT:
            app_usart_tx_process();
            break;
        default:
            break;
    }
}
```

**Bluetooth message callback implementation:**

```c
void app_ble_msg_handler(struct ble_msg_t const *p_ble_msg)
{
    switch (p_ble_msg->msg_id)
    {
        case APP_BLE_OS_READY:
            NS_LOG_INFO("APP_BLE_OS_READY\r\n");
            break;
        case APP_BLE_GAP_CONNECTED:
            app_ble_connected();
            break;
        case APP_BLE_GAP_DISCONNECTED:
            app_ble_disconnected();
            break;
        default:
            break;
    }
}
```

---

### 1.2 ns\_ble\_gap\_init

**Function:** Bluetooth common parameter configuration, such as Bluetooth MAC address, name, role, connection parameters, etc. See `struct ns_gap_params_t` definition for details.

**Syntax:**

```c
void ns_ble_gap_init(struct ns_gap_params_t const* p_dev_info);
```

**Parameters:**

- `[in] p_dev_info` — Bluetooth common parameter structure pointer.

**Return:** None

**Example:**

```c
struct ns_gap_params_t dev_info = {0};
memcpy(dev_info.mac_addr.addr, "\x01\x02\x03\x04\x05\x06", BD_ADDR_LEN);
dev_info.mac_addr_type = GAPM_STATIC_ADDR;
dev_info.appearance = 0;
dev_info.dev_role = GAP_ROLE_PERIPHERAL;
dev_info.dev_name_len = sizeof(CUSTOM_DEVICE_NAME) - 1;
memcpy(dev_info.dev_name, CUSTOM_DEVICE_NAME, dev_info.dev_name_len);
dev_info.dev_conn_param.intv_min = MSECS_TO_UNIT(MIN_CONN_INTERVAL, MSECS_UNIT_1_25_MS);
dev_info.dev_conn_param.intv_max = MSECS_TO_UNIT(MAX_CONN_INTERVAL, MSECS_UNIT_1_25_MS);
dev_info.dev_conn_param.latency = SLAVE_LATENCY;
dev_info.dev_conn_param.time_out = MSECS_TO_UNIT(CONN_SUP_TIMEOUT, MSECS_UNIT_10_MS);
dev_info.conn_param_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
ns_ble_gap_init(&dev_info);
```

---

### 1.3 ns\_ble\_add\_prf\_func\_register

**Function:** Register service (profile) add function. The system will later call the registered service add function to add corresponding services.

**Syntax:**

```c
bool ns_ble_add_prf_func_register(ns_ble_add_prf_func_t func);
```

**Parameters:**

- `[in] func` — Add service (profile) function, implement the function with reference to sample function `app_dis_add_dis`.

**Return:** `true` — Registration succeeded; `false` — Registration failed

**Example:**

```c
ns_ble_add_prf_func_register(app_dis_add_dis);
```

---

### 1.4 ns\_ble\_prf\_task\_register

**Function:** Register service (profile) subtask events to Bluetooth application event callback list.

**Syntax:**

```c
bool ns_ble_prf_task_register(struct prf_task_t *prf);
```

**Parameters:**

- `[in] prf` — Service (profile) subtask structure pointer, see `struct prf_task_t` definition for details.

**Return:** `true` — Registration succeeded; `false` — Registration failed

**Example:**

```c
// register application subtask to app task
struct prf_task_t prf;
prf.prf_task_id = TASK_ID_DISS;
prf.prf_task_handler = &app_dis_handlers;
ns_ble_prf_task_register(&prf);
```

---

### 1.5 prf\_get\_itf\_func\_register

**Function:** Register service (profile) task interface function getting function. Note that this function is implemented and declared in `prf.c` file along with `prf.h` file.

**Syntax:**

```c
#include "prf.h"
bool prf_get_itf_func_register(struct prf_get_func_t *prf);
```

**Parameters:**

- `[in] prf` — Service task interface getting function pointer.

**Return:** `true` — Registration succeeded; `false` — Registration failed

**Example:**

```c
// register get itf function to prf.c
struct prf_get_func_t get_func;
get_func.task_id = TASK_ID_DISS;
get_func.prf_itf_get_func = diss_prf_itf_get;
prf_get_itf_func_register(&get_func);
```

---

### 1.6 ns\_ble\_adv\_init

**Function:** Initialize BLE Bluetooth advertising parameters.

**Syntax:**

```c
void ns_ble_adv_init(struct ns_adv_params_t const* p_adv_init);
```

**Parameters:**

- `[in] p_adv_init` — Advertising initialization parameter structure pointer.

**Return:** None

**Example:**

```c
struct ns_adv_params_t user_adv = {0};
// init advertising data
user_adv.adv_data_len = ADVERTISE_DATA_LEN;
memcpy(user_adv.adv_data, ADVERTISE_DATA, ADVERTISE_DATA_LEN);
user_adv.scan_rsp_data_len = ADV_SCNRSP_DATA_LEN;
memcpy(user_adv.scan_rsp_data, ADV_SCNRSP_DATA, ADV_SCNRSP_DATA_LEN);
user_adv.attach_appearance = false;
user_adv.attach_name = true;
user_adv.ex_adv_enable = false;
user_adv.adv_phy = PHY_1MBPS_VALUE;
user_adv.directed_adv.enable = false;
user_adv.fast_adv.enable = true;
user_adv.fast_adv.duration = CUSTOM_ADV_FAST_DURATION;
user_adv.fast_adv.adv_intv = CUSTOM_ADV_FAST_INTERVAL;
user_adv.slow_adv.enable = true;
user_adv.slow_adv.duration = CUSTOM_ADV_SLOW_DURATION;
user_adv.slow_adv.adv_intv = CUSTOM_ADV_SLOW_INTERVAL;
user_adv.ble_adv_msg_handler = app_ble_adv_msg_handler;
ns_ble_adv_init(&user_adv);
```

---

### 1.7 ns\_ble\_adv\_start

**Function:** Start (enable) Bluetooth advertising.

**Syntax:**

```c
void ns_ble_adv_start(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_ble_adv_start();
```

---

### 1.8 ns\_ble\_adv\_stop

**Function:** Stop Bluetooth advertising.

**Syntax:**

```c
void ns_ble_adv_stop(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_ble_adv_stop();
```

---

### 1.9 ns\_ble\_adv\_data\_set

**Function:** Set the content of the advertising data packet.

**Syntax:**

```c
void ns_ble_adv_data_set(uint8_t* p_dat, uint16_t len);
```

**Parameters:**

- `[in] p_dat` — Set advertising packet data.
- `[in] len` — Set length of advertising packet data.

**Return:** None

**Example:**

```c
ns_ble_adv_data_set(CUSTOM_USER_ADVERTISE_DATA, CUSTOM_USER_ADVERTISE_DATA_LEN);
```

---

### 1.10 ns\_ble\_scan\_rsp\_data\_set

**Function:** Set the content of the advertising scan response data packet.

**Syntax:**

```c
void ns_ble_scan_rsp_data_set(uint8_t* p_dat, uint16_t len);
```

**Parameters:**

- `[in] p_dat` — Set advertising scan response packet data.
- `[in] len` — Set length of advertising scan response packet data.

**Return:** None

**Example:**

```c
ns_ble_scan_rsp_data_set(CUSTOM_USER_ADV_SCNRSP_DATA, CUSTOM_USER_ADV_SCNRSP_DATA_LEN);
```

---

### 1.11 ns\_ble\_ex\_adv\_data\_set

**Function:** Set the content of the extended broadcast data packet.

**Syntax:**

```c
void ns_ble_ex_adv_data_set(uint8_t* p_dat, uint16_t len);
```

**Parameters:**

- `[in] p_dat` — Set the extended broadcast packet data. **Note:** the pointer to a local variable cannot be used here.
- `[in] len` — Set the length of the extended broadcast packet data.

**Return:** None

**Example:**

```c
const static uint8_t ex_adv[] = {"\x29\xff""1234567890123456789012345678901234567890"};
ns_ble_ex_adv_data_set((uint8_t*) ex_adv, sizeof(ex_adv) - 1);
```

---

### 1.12 ns\_ble\_scan\_init

**Function:** Initialize the parameters for the BLE scan function.

**Syntax:**

```c
void ns_ble_scan_init(struct ns_scan_params_t *p_init);
```

**Parameters:**

- `[in] p_init` — Pointer to the scan function parameter structure, refer to the internal definition of `ns_scan_params_t` for details.

**Return:** None

**Example:**

```c
struct ns_scan_params_t init = {0};
static const uint8_t target_name[] = {"NS_RDTS_SERVER"};
init.type = SCAN_PARAM_TYPE;
init.dup_filt_pol = SCAN_PARAM_DUP_FILT_POL;
init.connect_enable = SCAN_PARAM_CONNECT_EN;
init.prop_active_enable = SCAN_PARAM_PROP_ACTIVE;
init.scan_intv = SCAN_PARAM_INTV;
init.scan_wd = SCAN_PARAM_WD;
init.duration = SCAN_PARAM_DURATION;
init.filter_type = SCAN_FILTER_BY_NAME;
init.filter_data = (uint8_t*)&target_name;
ns_ble_scan_init(&init);
```

---

### 1.13 ns\_ble\_start\_scan

**Function:** Start the BLE scan function.

**Syntax:**

```c
void ns_ble_start_scan(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_ble_start_scan();
```

---

### 1.14 ns\_ble\_stop\_scan

**Function:** Stop the BLE scan function.

**Syntax:**

```c
void ns_ble_stop_scan(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_ble_stop_scan();
```

---

### 1.15 ns\_ble\_start\_init

**Function:** The host device actively initiates the BLE connection. **Note:** Only the master device can call this function.

**Syntax:**

```c
void ns_ble_start_init(uint8_t *addr, uint8_t addr_type);
```

**Parameters:**

- `[in] addr` — The address of the slave device to connect to.
- `[in] addr_type` — The address type of the device to connect to (the returned scan information contains the address type).

**Return:** None

**Example:**

```c
ns_ble_start_init("\x11\x11\x11\x11\x11\x11", GAPM_STATIC_ADDR);
```

---

### 1.16 ns\_ble\_update\_param

**Function:** The master or slave device actively initiates the BLE connection parameter update request.

**Syntax:**

```c
void ns_ble_update_param(struct gapc_conn_param *conn_param);
```

**Parameters:**

- `[in] conn_param` — Pointer to the connection parameter structure.

**Return:** None

**Example:**

```c
struct gapc_conn_param conn_param;
conn_param.intv_min = 12;    // 15ms
conn_param.intv_max = 12;    // 15ms
conn_param.latency = 5;
conn_param.time_out = 500;   // 5000ms
ns_ble_update_param(&conn_param);
```

---

### 1.17 ns\_ble\_mtu\_set

**Function:** The master or slave device actively initiates the BLE MTU parameter update request. **Note:** The valid data length of the user data packet is 3 bytes less than the MTU.

**Syntax:**

```c
void ns_ble_mtu_set(uint16_t mtu);
```

**Parameters:**

- `[in] mtu` — The BLE MTU value. Maximum value is **517**.

**Return:** None

**Example:**

```c
ns_ble_mtu_set(247);  // set mtu as 247
```

---

### 1.18 ns\_ble\_phy\_set

**Function:** The master or slave device actively initiates the BLE PHY parameter update request.

**Syntax:**

```c
void ns_ble_phy_set(enum gap_phy_val phy);
```

**Parameters:**

- `[in] phy` — The BLE PHY parameter value, optional parameters refer to the declaration of `enum gap_phy_val`.

**Return:** None

**Example:**

```c
ns_ble_phy_set(GAP_PHY_125KBPS);  // set phy as coded 125kbps
```

---

### 1.19 ns\_ble\_active\_rssi

**Function:** The master or slave device actively initiates the BLE RSSI read request. The read value is returned through the `APP_BLE_GAP_RSSI_IND` message in the Bluetooth event callback function.

**Syntax:**

```c
void ns_ble_active_rssi(uint32_t interval);
```

**Parameters:**

- `[in] interval` — The BLE RSSI read interval in milliseconds. Input `0` to read only once.

**Return:** None

**Example:**

```c
void app_ble_msg_handler(struct ble_msg_t const *p_ble_msg)
{
    switch (p_ble_msg->msg_id)
    {
        case APP_BLE_OS_READY:
            NS_LOG_INFO("APP_BLE_OS_READY\r\n");
            break;
        case APP_BLE_GAP_CONNECTED:
            app_ble_connected();
            ns_ble_active_rssi(40);  // enable rssi read every 40ms
            break;
        case APP_BLE_GAP_DISCONNECTED:
            app_ble_disconnected();
            break;
        case APP_BLE_GAP_RSSI_IND:
            NS_LOG_INFO("rssi:%d\r\n", p_ble_msg->msg.p_gapc_rssi->rssi);
            break;
        default:
            break;
    }
}
```

---

### 1.20 ns\_ble\_disconnect

**Function:** The master or slave device actively initiates the BLE disconnection request. The successful disconnection will be returned through the `APP_BLE_GAP_DISCONNECTED` message in the Bluetooth event callback function.

**Syntax:**

```c
void ns_ble_disconnect(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_ble_disconnect();
```

---

### 1.21 ns\_ble\_dle\_set

**Function:** The master or slave device requests to set the DLE parameters. It is recommended to use the parameters in the example code if you need to change them.

**Syntax:**

```c
void ns_ble_dle_set(uint16_t tx_octets, uint16_t tx_time);
```

**Parameters:**

- `[in] tx_octets` — The maximum data amount of a single link data channel PDU.
- `[in] tx_time` — The maximum number of microseconds to send a single link data channel PDU.

**Return:** None

**Example:**

```c
ns_ble_dle_set(251, 2120);  // suggested parameters
```

---

### 1.22 rf\_tx\_power\_set

**Function:** The master or slave device sets the transmission power of the radio signal.

**Syntax:**

```c
void rf_tx_power_set(rf_tx_power_t pwr);
```

**Parameters:**

- `[in] pwr` — The radio signal transmit power. Options:

```c
typedef enum
{
    TX_POWER_0_DBM = 0,     /*  0 dBm  */
    TX_POWER_Neg2_DBM,      /* -2 dBm  */
    TX_POWER_Neg4_DBM,      /* -4 dBm  */
    TX_POWER_Neg8_DBM,      /* -8 dBm  */
    TX_POWER_Neg15_DBM,     /* -12 dBm */
    TX_POWER_Neg20_DBM,     /* -20 dBm */
    TX_POWER_Pos2_DBM,      /* +2 dBm  */
    TX_POWER_Pos3_DBM,      /* +3 dBm  */
    TX_POWER_Pos4_DBM,      /* +4 dBm  */
    TX_POWER_Pos6_DBM,      /* +6 dBm  */
} rf_tx_power_t;
```

**Return:** None

**Example:**

```c
rf_tx_power_set(TX_POWER_Pos4_DBM);
```

---

### 1.23 Process Functions (Internal)

These are internal process functions. **User code does not need to call them separately.**

| Function | Description |
|---|---|
| `void ns_ble_adv_fsm_next(void)` | Slave device broadcast state management function. |
| `void ns_ble_create_scan(void)` | Host creates a scan activity. Auto-created by `ns_ble_scan_init`. |
| `void ns_ble_delete_scan(void)` | Host deletes the scan activity. |
| `void ns_ble_create_init(void)` | Host creates the master connection activity (auto-created). |
| `void ns_ble_delete_init(void)` | Host deletes the connection activity. |
| `bool ns_ble_scan_data_find(uint8_t types, const uint8_t *p_filter_data, uint8_t *p_data, uint8_t len)` | Host finds specified data in returned broadcast data during scanning. Auto-called based on filter config. |
| `bool ns_ble_add_svc(void)` | Adds registered services (profiles) during initialization. |

---

## 2. Bluetooth Security Encryption Module: ns\_sec.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\sec`
**Source Files:** `ns_sec.c`, `ns_sec.h`
**Introduction:** Bluetooth security encryption related APIs, communicating between user application code and Bluetooth protocol stack.

---

### 2.1 ns\_sec\_init

**Function:** Initialize the Bluetooth security encryption module.

**Syntax:**

```c
void ns_sec_init(struct ns_sec_init_t const* init);
```

**Parameters:**

- `[in] init` — Bluetooth security module initialization parameter structure pointer. Refer to `ns_sec_init_t` for details.

**Return:** None

**Example:**

```c
struct ns_sec_init_t sec_init = {0};
sec_init.rand_pin_enable = false;
sec_init.pin_code = 123456;
sec_init.pairing_feat.auth = (SEC_PARAM_BOND | (SEC_PARAM_MITM << 2) |
                              (SEC_PARAM_LESC << 3) | (SEC_PARAM_KEYPRESS << 4));
sec_init.pairing_feat.iocap = SEC_PARAM_IO_CAPABILITIES;
sec_init.pairing_feat.key_size = SEC_PARAM_KEY_SIZE;
sec_init.pairing_feat.oob = SEC_PARAM_OOB;
sec_init.pairing_feat.ikey_dist = SEC_PARAM_IKEY;
sec_init.pairing_feat.rkey_dist = SEC_PARAM_RKEY;
sec_init.pairing_feat.sec_req = SEC_PARAM_SEC_MODE_LEVEL;
sec_init.bond_enable = BOND_STORE_ENABLE;
sec_init.bond_db_addr = BOND_DATA_BASE_ADDR;
sec_init.bond_max_peer = MAX_BOND_PEER;
sec_init.bond_sync_delay = 5000;
sec_init.ns_sec_msg_handler = NULL;
ns_sec_init(&sec_init);
```

---

### 2.2 ns\_sec\_get\_bond\_status

**Function:** Return the bonded status.

**Syntax:**

```c
bool ns_sec_get_bond_status(void);
```

**Parameters:** None
**Return:** `true` — Already bonded. `false` — Not bonded.

**Example:**

```c
bool bond_status = ns_sec_get_bond_status();
```

---

### 2.3 ns\_sec\_get\_iocap

**Function:** Return the device interface capability parameter for calling by related Bluetooth libraries. Generally user code does not need to call this.

**Syntax:**

```c
uint8_t ns_sec_get_iocap(void);
```

**Parameters:** None
**Return:** The device interface capability parameter (value set by `ns_sec_init`).

**Example:**

```c
uint8_t io_cap = ns_sec_get_iocap();
```

---

### 2.4 ns\_sec\_bond\_db\_erase\_all

**Function:** Erase all bonded devices. **Warning:** Erasing flash will block execution. It is recommended not to execute when already connected, as it may affect the connection.

**Syntax:**

```c
void ns_sec_bond_db_erase_all(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_sec_bond_db_erase_all();
```

---

### 2.5 ns\_sec\_send\_security\_req

**Function:** Send a security request to the master device. Generally user code does not need to call this.

---

### 2.6 ns\_sec\_bond\_store\_evt\_handler

**Function:** Callback function of the data storage task for bonded device information. Used internally.

---

## 3. Software Timer: ns\_timer.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\timer`
**Source Files:** `ns_timer.c`, `ns_timer.h`
**Introduction:** Software timer module, encapsulated by the `ke_timer` module in the Bluetooth protocol stack. Must be used after the protocol stack is initialized (after the `APP_BLE_OS_READY` message is sent). Since it takes more than one low-speed clock tick (~32us) to wake up from sleep before calling `ke_timer_set`.

---

### 3.1 ns\_timer\_create

**Function:** Create a software timer that calls the `fn` function after a delay of `delay` milliseconds. For a cyclic timer, create another timer in the callback handler again. **Note:** Can only be used after the protocol stack is initialized (after `APP_BLE_OS_READY`).

**Syntax:**

```c
timer_hnd_t ns_timer_create(const uint32_t delay, timer_callback_t fn);
```

**Parameters:**

- `[in] delay` — The delay time in milliseconds.
- `[in] fn` — The callback function after the delay.

**Return:** The id of the timer.

**Example:**

```c
timer_hnd_t timer_id = ns_timer_create(1000, my_timer_callback);
```

---

### 3.2 ns\_timer\_modify

**Function:** Modify the delay time of a specified timer.

**Syntax:**

```c
timer_hnd_t ns_timer_modify(const timer_hnd_t timer_id, const uint32_t delay);
```

**Parameters:**

- `[in] timer_id` — The timer id to modify.
- `[in] delay` — The modified delay time in milliseconds.

**Return:** The modified timer id.

**Example:**

```c
timer_id = ns_timer_modify(timer_id, 2000);
```

---

### 3.3 ns\_timer\_cancel

**Function:** Cancel a specified timer.

**Syntax:**

```c
void ns_timer_cancel(const timer_hnd_t timer_id);
```

**Parameters:**

- `[in] timer_id` — The timer id to cancel.

**Return:** None

**Example:**

```c
ns_timer_cancel(timer_id);
```

---

### 3.4 ns\_timer\_cancel\_all

**Function:** Cancel all created timers.

**Syntax:**

```c
void ns_timer_cancel_all(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_timer_cancel_all();
```

---

## 4. Bluetooth Sleep Module: ns\_sleep.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\sleep`
**Source Files:** `ns_sleep.c`, `ns_sleep.h`
**Introduction:** Bluetooth protocol stack sleep function module.

---

### 4.1 ns\_sleep

**Function:** Bluetooth sleep function entry. Based on the Bluetooth protocol stack working status, it will automatically enter sleep state if there is no task to execute, and resume system status after being woken up by timed task or hardware interrupt. Usually called in the `main()` function main loop, after the `rwip_schedule` dispatch function.

**Syntax:**

```c
void ns_sleep(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
ns_sleep();
```

---

### 4.2 ns\_sleep\_lock\_acquire

**Function:** Bluetooth sleep mode lock request — apply for the system not to enter sleep mode. Useful when high-speed peripherals (such as UART) are enabled and you don't want the system to turn them off during sleep.

**Syntax:**

```c
uint8_t ns_sleep_lock_acquire(void);
```

**Parameters:** None
**Return:** `true` — Lock request succeeded. `false` — Lock request failed.

**Example:**

```c
if (ns_sleep_lock_acquire())
{
    // require sleep lock success
}
```

---

### 4.3 ns\_sleep\_lock\_release

**Function:** Release Bluetooth sleep mode lock. After all sleep mode locks are released, the system can enter sleep mode when there is no pending task.

**Syntax:**

```c
uint8_t ns_sleep_lock_release(void);
```

**Parameters:** None
**Return:** `true` — Release lock successfully. `false` — Release lock failure (no lock to release).

**Example:**

```c
if (ns_sleep_lock_release())
{
    // release sleep lock success
}
```

---

### 4.4 Virtual Functions for Entering and Exiting Sleep Mode

**`__weak void app_sleep_prepare_proc(void)`**

Preset virtual function for users to reimplement tasks that need to be done **before** entering sleep.

**`__weak void app_sleep_resume_proc(void)`**

Preset virtual function for users to reimplement tasks that need to be done **after** sleep wake-up (e.g., reinitializing peripherals closed during sleep).

---

## 5. Debug Information Printing Module: log.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\log`
**Source Files:** `ns_log_lpuart.c`, `ns_log_lpuart.h`, `ns_log_usart.c`, `ns_log_usart.h`, `ns_log.h`
**Introduction:** Debug information output function module. Currently available output hardware: LPUART (default TX pin: **PB1**) and USART1 (default TX pin: **PB6**).

---

### 5.1 NS\_LOG\_INIT

**Function:** Initialize the debug information printing module.

**Syntax:**

```c
NS_LOG_INIT();
```

---

### 5.2 NS\_LOG\_DEINIT

**Function:** Deinitialize the debug information printing module.

**Syntax:**

```c
NS_LOG_DEINIT();
```

---

### 5.3 Debug Information Print Output

**Function:** Call debug information print output functions of different levels. Syntax is the same as `printf`.

```c
NS_LOG_DEBUG(...);
NS_LOG_INFO(...);
NS_LOG_WARNING(...);
NS_LOG_ERROR(...);
```

---

### 5.4 Related Enabling Macros

```c
// Whether to enable LPUART (PB1) as the output hardware for the log module
#define NS_LOG_LPUART_ENABLE      0

// Whether to enable USART (PB6) as the output hardware for the log module
#define NS_LOG_USART_ENABLE       0

// Whether to enable ERROR level output functions
#define NS_LOG_ERROR_ENABLE       0

// Whether to enable WARNING level output functions
#define NS_LOG_WARNING_ENABLE     0

// Whether to enable INFO level output functions
#define NS_LOG_INFO_ENABLE        0

// Whether to enable DEBUG level output functions
#define NS_LOG_DEBUG_ENABLE       0

// Whether to enable output of debug information with color
#define PRINTF_COLOR_ENABLE       0
```

---

## 6. Hard Delay Module: ns\_delay.h

**API Directory:** `middlewares\Nationstech\ble_library\ns_library\delay`
**Source File:** `ns_delay.h`
**Introduction:** Hard delay function module. The related functions are implemented in ROM code. Users can directly call them after including the header file. **Note:** The delay time is not accurate. For accurate delays, use hardware timer functions.

---

### 6.1 delay\_cycles

**Function:** Delay and wait for the specified number of cycles to return.

**Syntax:**

```c
void delay_cycles(uint32_t ui32Cycles);
```

**Parameters:**

- `[in] ui32Cycles` — Number of cycles to wait. One cycle takes approximately **(10/110) microseconds**.

**Example:**

```c
delay_cycles(1000);
```

---

### 6.2 delay\_n\_us

**Function:** Delay and wait for the specified number of microseconds.

**Syntax:**

```c
void delay_n_us(uint32_t val);
```

**Parameters:**

- `[in] val` — Number of microseconds to wait.

**Example:**

```c
delay_n_us(1000);
```

---

### 6.3 delay\_n\_10us

**Function:** Delay and wait for the specified number of 10-microsecond intervals.

**Syntax:**

```c
void delay_n_10us(uint32_t val);
```

**Parameters:**

- `[in] val` — Number of 10-microsecond intervals to wait.

**Example:**

```c
delay_n_10us(1000);
```

---

### 6.4 delay\_n\_100us

**Function:** Delay and wait for the specified number of 100-microsecond intervals.

**Syntax:**

```c
void delay_n_100us(uint32_t val);
```

**Parameters:**

- `[in] val` — Number of 100-microsecond intervals to wait.

**Example:**

```c
delay_n_100us(1000);
```

---

### 6.5 delay\_n\_ms

**Function:** Delay and wait for the specified number of milliseconds.

**Syntax:**

```c
void delay_n_ms(uint32_t val);
```

**Parameters:**

- `[in] val` — Number of milliseconds to wait.

**Example:**

```c
delay_n_ms(1000);
```

---

## 7. Suggestions for Bluetooth Programming

- **Avoid long-running code** — Do not continuously execute code that takes too long, as it will hinder Bluetooth message handling. Fragmentize user code so each task message or polling only takes up a small amount of time.

- **Keep ISRs short** — Do not execute interrupt service functions for too long. Implement logical code in tasks or polling by pending messages, timers, or flag bits.

- **No hardware peripheral calls in ISRs** — If an interrupt event is pending after sleep wake-up, the ISR will be called before `app_sleep_resume_proc`, and the peripheral may not yet be initialized. This will cause errors.

- **Register ISRs via `ModuleIrqRegister`** — For projects containing Bluetooth, interrupt service functions must be registered through `ModuleIrqRegister`. User code interrupt priority can only use **2** and **3** (Bluetooth protocol stack uses 0 and 1).

- **High-speed peripherals and sleep** — High-speed peripherals (such as USART) will be turned off in low power sleep mode. They need to be reinitialized by the program after wake-up before they can be used.

- **Use task callbacks** — It is recommended that user's logical code be implemented in task callback functions (message event callbacks or timed task callbacks).

---

## 8. Version History

| Version | Date | Changes |
|---|---|---|
| V1.0 | 2021.12.27 | Initial version |
