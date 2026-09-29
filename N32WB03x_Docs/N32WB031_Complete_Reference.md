# N32WB031 SoC — Complete Developer Reference

**Chip:** N32WB031 (Nations Technologies / NSING)
**Package:** QFN32 (4×4 mm)
**Ordering:** N32WB031KCQ6-1 (256KB, 1.8–3.6V) / N32WB031KEQ6-2 (512KB, 2.32–3.6V)

---

## 1. Core & Memory

| Parameter | Value |
|---|---|
| CPU | ARM Cortex-M0 |
| Max Frequency | 64 MHz |
| Flash | 256KB or 512KB (Bank A + Bank B, 256KB each) |
| SRAM | 48 KB (fully retained in Sleep mode) |
| Flash Address | `0x01000000` – `0x0107FFFF` |
| Bank A | `0x01000000` – `0x0103FFFF` (256KB, default code execution) |
| Bank B | `0x01040000` – `0x0107FFFF` (256KB) |
| TRAM | 512 bytes (read-only chip calibration, UUID, MAC) |
| Flash Page Write | 256 bytes, 2–3 ms |
| Flash Sector Erase | 4 KB, 16–30 ms |
| Flash Mode | XIP on Bank A; exits XIP for Bank B or write/erase ops |
| Qflash RAM Overhead | 804 bytes (for n32wb03x_qflash.c) |

---

## 2. BLE Radio

| Parameter | Value |
|---|---|
| Standard | BLE 5.1 + SIG Mesh |
| Modes | 1 Mbps, 2 Mbps, 125 kbps (S8 coded), 500 kbps (S2 coded) |
| TX Power | Programmable: −20 to +6 dBm |
| TX Current | 4.2 mA @ 0 dBm / 3.3V |
| RX Current | 3.8 mA @ 3.3V |
| RX Sensitivity | −96 dBm @ 1 Mbps, −93 dBm @ 2 Mbps |
| Features | AoA, AoD, RSSI, master/slave, multi-connection, DLE |
| MTU | Max 517 (usable data = MTU − 3) |
| Antenna | Single-ended interface |

**TX Power Enum:**

```c
typedef enum {
    TX_POWER_0_DBM = 0,     //  0 dBm
    TX_POWER_Neg2_DBM,      // -2 dBm
    TX_POWER_Neg4_DBM,      // -4 dBm
    TX_POWER_Neg8_DBM,      // -8 dBm
    TX_POWER_Neg15_DBM,     // -12 dBm
    TX_POWER_Neg20_DBM,     // -20 dBm
    TX_POWER_Pos2_DBM,      // +2 dBm
    TX_POWER_Pos3_DBM,      // +3 dBm
    TX_POWER_Pos4_DBM,      // +4 dBm
    TX_POWER_Pos6_DBM,      // +6 dBm
} rf_tx_power_t;
```

---

## 3. Clock System

| Clock | Frequency | Notes |
|---|---|---|
| HSI | 64 MHz | Internal RC. **Must be system clock when BLE is active** (errata 2.1) |
| HSE | 32 MHz | External crystal. Cannot be SYSCLK with BLE |
| LSI | 32 kHz | Internal RC. Risk of glitch if VCCRF > 3V on chip versions B/C (errata 2.3) |
| LSE | 32.768 kHz | External crystal. Recommended for RTC/LPUART/KEYSCAN |
| SYSCLK Max | 64 MHz | |
| APB1 Max | 32 MHz | |
| APB2 Max | 64 MHz | |
| MCO | Configurable | Various sources, divided by 4 |

**Clock Tree Summary:**

```
HSI(64M) / HSE(32M) → SYSCLK(64M max) → AHB Prescaler(/1,2,4) → HCLK
                                                                    ├→ APB1 Prescaler → PCLK1 (32M max) → USART2, I2C, LPUART, TIM3, TIM6, PWR
                                                                    └→ APB2 Prescaler → PCLK2 (64M max) → USART1, SPI1, TIM1, GPIOA/B, EXTI, AFIO
HSE/2 → BLE_CLK
LSI/LSE → RTC, KEYSCAN, LPUART, BLE wakeup, IWDG
```

---

## 4. Power Modes

| Mode | Description | Current | Wake Sources |
|---|---|---|---|
| **Active** | Full operation | — | — |
| **Idle** | CPU stopped, peripherals running | — | Any interrupt/event |
| **Standby** | Power supply normal, CORE domain off, BLE available | — | BLE, configured sources |
| **Sleep** | High-speed clock off, low-power supply, CORE + BLE off | 1.4 µA @ 3V (48KB retained) | RTC, KEYSCAN, LPUART, EXTI (8 lines), GPIO |
| **PD** | All systems shutdown | 130 nA | WAKEUP IO (PB3), NRST only |

**Sleep API:**

```c
void ns_sleep(void);                    // Call in main loop after rwip_schedule()
uint8_t ns_sleep_lock_acquire(void);    // Prevent sleep (e.g., for UART)
uint8_t ns_sleep_lock_release(void);    // Allow sleep again
__weak void app_sleep_prepare_proc(void);  // Override: pre-sleep tasks
__weak void app_sleep_resume_proc(void);   // Override: post-wake reinit
```

---

## 5. GPIO & Pin Configuration

**21 GPIOs total:** GPIOA (7 pins: PA0–PA6), GPIOB (14 pins: PB0–PB13)

**Modes:** Input floating, Input pull-up, Input pull-down, Analog, Output open-drain, Output push-pull, AF push-pull, AF open-drain.

**Key Pin Assignments:**

| Pin | Primary | Alternate Functions | Special |
|---|---|---|---|
| PA2 | GPIO | | |
| PA3 | GPIO | | |
| PA6 | GPIO | | |
| PB0 | GPIO | USART1_RTS | |
| PB1 | GPIO | SPI2_CLK, I2S2_CLK | **Default log TX (LPUART)** |
| PB2 | GPIO | LPUART_RXD | |
| PB3 | GPIO | LPUART_CTS | **WAKEUP (PD mode)** |
| PB4 | GPIO | | |
| PB5 | GPIO | USART2_RXD | |
| PB6 | GPIO | ADC5 | **Default log TX (USART1)** |
| PB7 | GPIO | ADC4 | |
| PB10 | GPIO | USART2_RXD, ADC1 | |
| PB11 | GPIO | AMIC_N | |
| PB12 | GPIO | LPUART_TXD, AMIC_BIAS | |
| PB13 | GPIO | LPUART_CTS, AMIC_P | |

**8 EXTIs can wake from Sleep mode. All I/O can be multiplexed as EXTI.**

**Interrupt priority:** User code can only use priorities **2** and **3** (BLE stack uses 0 and 1). Register ISRs via `ModuleIrqRegister`.

---

## 6. Peripherals Quick Reference

### 6.1 USART (×2: USART1, USART2)

- Rate: up to 4 Mbps
- Modes: Async, Sync, ISO7816 smartcard, IrDA, LIN, single-wire half-duplex
- DMA support

### 6.2 LPUART (×1)

- Rate: up to 9600 bps
- Low-power wake-up in Sleep mode
- Clock: LSI or LSE (use LSE or calibrate LSI to 32.768K — errata 5.1)

### 6.3 SPI (×2: SPI1, SPI2) / I2S

- Rate: up to 16 MHz, master/slave
- CRC calibration support
- **SPI1 interrupt doesn't work with BLE stack** — use DMA or SPI2 (errata 6.2)
- CRC abnormal above 8 MHz in master mode (errata 6.1.1)
- I2S audio support on both SPI ports

### 6.4 I2C (×1)

- Rate: up to 1 MHz, master/slave
- **Susceptible to glitches** — consider software I2C bit-bang (errata 7.1)

### 6.5 ADC

- 10-bit @ 1.33 Msps (or 16-bit @ 16 Ksps)
- 5 external single-ended channels, 1 differential MIC channel, 2 internal channels
- Built-in PGA: up to 128×
- MIC BIAS: adjustable 1.6V–2.3V
- **Cannot reset via RCC_AHBPRST ADCRST bit** — manually reset registers (errata 3.2)

### 6.6 Timers

| Timer | Type | Bits | Channels | Special |
|---|---|---|---|---|
| TIM1 | Advanced | 16 | 4 (3 with complementary + dead time) | Encoder, Hall sensor, break input |
| TIM3 | General-purpose | 16 | 4 | Encoder, up/down/center-aligned |
| TIM6 | Basic | 16 | 0 | Time base, DMA trigger |
| SysTick | System | 24 | — | Downcounter, RTOS tick |
| IWDG | Independent WDG | 12 | — | LSI-driven, works even if main clock fails |
| WWDG | Window WDG | 7 | — | Early warning interrupt |

### 6.7 Other Peripherals

- **DMA:** 1 controller, 5 channels
- **CRC:** CRC16 (1 HCLK cycle) + CRC32 (4 HCLK cycles), configurable initial value
- **IRC:** Infrared controller, supports all IR remote protocols
- **KEYSCAN:** 8/10/13 GPIOs → 44/65/104 keys. Needs higher retention voltage in sleep (errata 8.1): `*(uint32_t*)0x40007014 = 0x00000814` (+200nA)
- **RTC:** Perpetual calendar (leap year), alarms, periodic wakeup
- **SWD:** Serial Wire Debug (2-pin)

---

## 7. BLE API Quick Reference (ns_ble.h)

**Source:** `middlewares\Nationstech\ble_library\ns_library\ble\`

### 7.1 Initialization Sequence

```c
// 1. Stack init
struct ns_stack_cfg_t app_handler;
app_handler.ble_msg_handler = app_ble_msg_handler;
app_handler.user_msg_handler = app_user_msg_handler;
ns_ble_stack_init(&app_handler);

// 2. GAP init
struct ns_gap_params_t dev_info = {0};
memcpy(dev_info.mac_addr.addr, "\x01\x02\x03\x04\x05\x06", BD_ADDR_LEN);
dev_info.mac_addr_type = GAPM_STATIC_ADDR;
dev_info.dev_role = GAP_ROLE_PERIPHERAL;
dev_info.dev_name_len = sizeof(CUSTOM_DEVICE_NAME) - 1;
memcpy(dev_info.dev_name, CUSTOM_DEVICE_NAME, dev_info.dev_name_len);
dev_info.dev_conn_param.intv_min = MSECS_TO_UNIT(MIN_CONN_INTERVAL, MSECS_UNIT_1_25_MS);
dev_info.dev_conn_param.intv_max = MSECS_TO_UNIT(MAX_CONN_INTERVAL, MSECS_UNIT_1_25_MS);
dev_info.dev_conn_param.latency = SLAVE_LATENCY;
dev_info.dev_conn_param.time_out = MSECS_TO_UNIT(CONN_SUP_TIMEOUT, MSECS_UNIT_10_MS);
ns_ble_gap_init(&dev_info);

// 3. Register profiles
ns_ble_add_prf_func_register(app_dis_add_dis);

struct prf_task_t prf;
prf.prf_task_id = TASK_ID_DISS;
prf.prf_task_handler = &app_dis_handlers;
ns_ble_prf_task_register(&prf);

// 4. Advertising init
struct ns_adv_params_t user_adv = {0};
user_adv.adv_data_len = ADVERTISE_DATA_LEN;
memcpy(user_adv.adv_data, ADVERTISE_DATA, ADVERTISE_DATA_LEN);
user_adv.attach_name = true;
user_adv.fast_adv.enable = true;
user_adv.fast_adv.duration = CUSTOM_ADV_FAST_DURATION;
user_adv.fast_adv.adv_intv = CUSTOM_ADV_FAST_INTERVAL;
user_adv.slow_adv.enable = true;
user_adv.slow_adv.duration = CUSTOM_ADV_SLOW_DURATION;
user_adv.slow_adv.adv_intv = CUSTOM_ADV_SLOW_INTERVAL;
user_adv.ble_adv_msg_handler = app_ble_adv_msg_handler;
ns_ble_adv_init(&user_adv);

// 5. Security init (optional)
struct ns_sec_init_t sec_init = {0};
sec_init.pin_code = 123456;
sec_init.pairing_feat.auth = (SEC_PARAM_BOND | (SEC_PARAM_MITM << 2) | (SEC_PARAM_LESC << 3));
sec_init.pairing_feat.iocap = SEC_PARAM_IO_CAPABILITIES;
sec_init.bond_enable = BOND_STORE_ENABLE;
sec_init.bond_db_addr = BOND_DATA_BASE_ADDR;
sec_init.bond_max_peer = MAX_BOND_PEER;
ns_sec_init(&sec_init);
```

### 7.2 BLE API Function List

| Function | Purpose |
|---|---|
| `ns_ble_stack_init(p_handler)` | Init BLE stack with message callbacks |
| `ns_ble_gap_init(p_dev_info)` | Configure MAC, name, role, connection params |
| `ns_ble_add_prf_func_register(func)` | Register profile add function |
| `ns_ble_prf_task_register(prf)` | Register profile subtask events |
| `prf_get_itf_func_register(prf)` | Register profile interface getter (in prf.c) |
| `ns_ble_adv_init(p_adv_init)` | Init advertising parameters |
| `ns_ble_adv_start()` | Start advertising |
| `ns_ble_adv_stop()` | Stop advertising |
| `ns_ble_adv_data_set(p_dat, len)` | Set advertising data |
| `ns_ble_scan_rsp_data_set(p_dat, len)` | Set scan response data |
| `ns_ble_ex_adv_data_set(p_dat, len)` | Set extended advertising data (no local ptr!) |
| `ns_ble_scan_init(p_init)` | Init scan parameters |
| `ns_ble_start_scan()` | Start scanning |
| `ns_ble_stop_scan()` | Stop scanning |
| `ns_ble_start_init(addr, addr_type)` | Master initiates connection |
| `ns_ble_update_param(conn_param)` | Update connection parameters |
| `ns_ble_mtu_set(mtu)` | Set MTU (max 517) |
| `ns_ble_phy_set(phy)` | Set PHY (1M/2M/coded) |
| `ns_ble_active_rssi(interval)` | Start RSSI reading (0 = once) |
| `ns_ble_disconnect()` | Disconnect |
| `ns_ble_dle_set(tx_octets, tx_time)` | Set DLE params (suggested: 251, 2120) |
| `rf_tx_power_set(pwr)` | Set TX power |

### 7.3 Key BLE Messages (enum app_ble_msg)

```c
APP_BLE_OS_READY           // Stack initialized, safe to use timers
APP_BLE_GAP_CONNECTED      // Connection established
APP_BLE_GAP_DISCONNECTED   // Connection lost
APP_BLE_GAP_RSSI_IND       // RSSI value received
```

---

## 8. Security API (ns_sec.h)

| Function | Purpose |
|---|---|
| `ns_sec_init(init)` | Init security module (pairing, bonding, PIN) |
| `ns_sec_get_bond_status()` | Returns true if bonded |
| `ns_sec_get_iocap()` | Get IO capability parameter |
| `ns_sec_bond_db_erase_all()` | Erase all bonds (blocks! Don't call when connected) |

---

## 9. Software Timer API (ns_timer.h)

> **Only usable after `APP_BLE_OS_READY` message.**

| Function | Purpose |
|---|---|
| `ns_timer_create(delay, fn)` | Create one-shot timer (re-create in callback for cyclic) |
| `ns_timer_modify(timer_id, delay)` | Change timer delay |
| `ns_timer_cancel(timer_id)` | Cancel a timer |
| `ns_timer_cancel_all()` | Cancel all timers |

---

## 10. Flash API (n32wb03x_qflash.c)

```c
Qflash_Init();                                         // Must call first
Qflash_Erase_Sector(0x1040000);                        // Erase 4KB sector (addr must be 0x1000 aligned)
Qflash_Write(0x1040000, data, len);                    // Write (addr must be 0x100 aligned, page boundary)
Qflash_Read(0x1040000, buf, len);                      // Read (addr must be 4-byte aligned)
```

**TRAM System Functions:**

```c
trim_stored_t* SystemTrimValueGet(void);               // Get calibration values
void SystemTrimValueRead(uint8_t* p, uint32_t len);    // Read raw TRAM data
uint8_t* SystemGetUUID(void);                          // 16-byte chip UUID
uint8_t* SystemGetMacAddr(void);                       // 6-byte MAC (UUID bytes 6–11)
```

**Flash Precautions:**

- Read: 4B min, Write: 256B page, Erase: 4KB sector
- Erase takes 16–30ms — **if BLE connected, connection interval must exceed 30ms** or risk disconnection
- Interrupts are masked during flash ops (handled by driver)

---

## 11. Debug Logging (log.h)

**Hardware:** LPUART (PB1) or USART1 (PB6)

```c
NS_LOG_INIT();                  // Initialize
NS_LOG_DEBUG("msg %d", val);    // Debug level
NS_LOG_INFO("msg");             // Info level
NS_LOG_WARNING("msg");          // Warning level
NS_LOG_ERROR("msg");            // Error level
NS_LOG_DEINIT();                // Deinit
```

**Enable macros (set to 1 to enable):**

```c
#define NS_LOG_LPUART_ENABLE    0    // LPUART on PB1
#define NS_LOG_USART_ENABLE     0    // USART1 on PB6
#define NS_LOG_ERROR_ENABLE     0
#define NS_LOG_WARNING_ENABLE   0
#define NS_LOG_INFO_ENABLE      0
#define NS_LOG_DEBUG_ENABLE     0
#define PRINTF_COLOR_ENABLE     0
```

---

## 12. Delay Functions (ns_delay.h, ROM-implemented)

```c
delay_n_ms(uint32_t val);       // Milliseconds
delay_n_100us(uint32_t val);    // 100µs units
delay_n_10us(uint32_t val);     // 10µs units
delay_n_us(uint32_t val);       // Microseconds
delay_cycles(uint32_t cycles);  // ~(10/110) µs per cycle
```

> **Not precise.** Use hardware timers for accurate timing.

---

## 13. Firmware Update (DFU)

### Flash Partition Layout (256KB chip)

```
0x0100_0000  ┌──────────────────────┐
             │ MasterBoot (8 KB)    │  Boot entry + serial update
0x0100_2000  ├──────────────────────┤
             │ Bootsetting (4 KB)   │  Partition table
0x0100_3000  ├──────────────────────┤
             │ APP Data (4 KB)      │  Bond table + user data
0x0100_4000  ├──────────────────────┤
             │ APP1 / Bank 1        │  User program 1
             │ (112 KB)             │
0x0102_0000  ├──────────────────────┤
             │ APP2 / Bank 2        │  User program 2 (or free)
             │ (112 KB)             │
0x0103_C000  ├──────────────────────┤
             │ ImageUpdate (16 KB)  │  Single-bank OTA helper
0x0103_FFFF  └──────────────────────┘
```

### Update Methods

| Method | Speed | Flash Usage | Rollback | Stability |
|---|---|---|---|---|
| **Serial Port** | — | Full | No | High |
| **BLE Dual Bank** | Fast | 50% per app | Yes | High |
| **BLE Single Bank** | Slow | Full | No | Lower (BLE reconnect) |

### BLE OTA Service

| Component | UUID | MTU |
|---|---|---|
| IUS (Service) | `11111111-1111-1111-1111-111100011111` | — |
| RC (Receive) | `11111111-1111-1111-1111-111100021111` | 20–244 |
| CC (Command) | `11111111-1111-1111-1111-111100031111` | 20 |

### Encryption

- ECC ECDSA SHA256 NIST256P signature verification
- Enable macro: `OTA_ECC_ECDSA_SHA256_ENABLE`
- Public key stored in Bootsetting (64 bytes)

---

## 14. Critical Errata Summary

| # | Issue | Impact | Workaround |
|---|---|---|---|
| **2.1** | HSE can't be SYSCLK with BLE | BLE fails | **Use HSI as SYSCLK** |
| **2.2** | BLE stack reconfigures EXTI4_12 | User EXTI config lost | Configure EXTI **after** stack init, clear EXTI11 in ISR |
| **2.3** | LSI glitch when VCCRF > 3V (ver B/C) | Abnormal BLE wake | Use diode on VCCRF, or LSE, or chip version D |
| **3.1** | RCC_LSCTRL resets after sleep wake | LSC config lost | Write RCC_LSCTRL **before** RCC_CFG after wake |
| **3.2** | ADCRST bit doesn't work | Can't reset ADC | Manually write default values to all ADC registers |
| **5.1** | LPUART 9600 baud fail with LSI 32K | Wake-up byte errors | Calibrate LSI to 32.768K, or use LSE |
| **6.1.1** | SPI CRC fails above 8 MHz | CRC corruption | Keep SPI ≤ 8 MHz when CRC enabled |
| **6.2** | SPI1 interrupt fails with BLE | No SPI1 ISR callback | **Use DMA or SPI2** |
| **7.1** | I2C glitch interference | Communication errors | **Use software I2C (bit-bang)** |
| **8.1** | KEYSCAN needs more retention voltage | Can't wake from sleep | Set `*(uint32_t*)0x40007014 = 0x00000814` (+200nA) |

---

## 15. Programming Best Practices

1. **Don't block** — Fragmentize code; each task/poll should take minimal time. Long code blocks BLE message handling.
2. **Keep ISRs short** — Use flags/messages, handle logic in task callbacks.
3. **No peripheral calls in ISRs** — After sleep wake, ISR fires before `app_sleep_resume_proc`; peripherals aren't initialized yet.
4. **ISR priority** — User code: priority 2 or 3 only. Register via `ModuleIrqRegister`.
5. **Reinit after sleep** — High-speed peripherals (USART) turn off in sleep. Reinit in `app_sleep_resume_proc`.
6. **Flash + BLE** — If erasing flash while BLE connected, ensure connection interval > 30ms.
7. **Use HSI for SYSCLK** — Always when BLE is active.
8. **Use task callbacks** — Implement logic in message event callbacks or timed task callbacks.

---

## 16. Electrical Characteristics (Key Values)

| Parameter | Min | Typ | Max | Unit |
|---|---|---|---|---|
| VCC operating | 1.8 / 2.32 | — | 3.6 | V |
| Temperature | −40 | — | +85 | °C |
| ESD (HBM) | — | — | ±2 | kV |
| Sleep current (48KB retained) | — | 1.4 | — | µA @ 3V |
| PD current | — | 130 | — | nA |
| TX current @ 0 dBm | — | 4.2 | — | mA @ 3.3V |
| RX current | — | 3.8 | — | mA @ 3.3V |
| RX sensitivity (1M) | — | −96 | — | dBm |
| RX sensitivity (2M) | — | −93 | — | dBm |
| Max TX power | — | +6 | — | dBm |

---

## 17. Source Documents

| Document | Description |
|---|---|
| N32WB03x_API_Reference.md | BLE API function guide (ns_ble.h, ns_sec.h, ns_timer.h, ns_sleep.h, log.h, ns_delay.h) |
| N32WB03x_User_Manual.md | Full 475-page hardware reference (registers, peripherals, bit fields) |
| N32WB03x_Datasheet.md | Chip specifications, pinout, electrical characteristics |
| N32WB03x_Errata_Sheet.md | Known silicon bugs and workarounds |
| N32WB03x_FLASH_User_Guide.md | Flash operations (Qflash API) and TRAM storage |
| N32WB03x_Firmware_Update_Guide.md | OTA/serial DFU process, commands, tools, encryption |
