# N32WB03x FLASH User Guide

**Version:** V1.2
**Date:** 2021.12.23
**Manufacturer:** NSING Technologies Pte. Ltd. (nsing.com.sg)

> This document covers the FLASH usage of the N32WB03x series Bluetooth SoC chips.

---

## 1. FLASH Introduction

The FLASH memory of N32WB03x contains two areas:

- **FLASH:** 256KB / 512KB — used to store user code and data
- **TRAM storage area:** up to 512 bytes — used to store chip calibration values

---

## 2. Flash

The N32WB03x chip has a 256KB or 512KB FLASH. User code is programmed into the FLASH and the kernel can directly address and run from the FLASH at runtime (XIP mode).

- **Reading Bank A** can directly access the address (FLASH stays in XIP mode).
- **Erasing/writing Bank A or B, and reading Bank B** must exit XIP mode. The library function runs in RAM, executes the FLASH operation, then returns to the FLASH area to continue running user code.

---

### 2.1 Address Range

The FLASH address range is `0x01000000` – `0x0107FFFF`, with a total space of **512KB**, divided into Bank A and Bank B (256KB each).

| Bank | Start Address | End Address | Size |
|---|---|---|---|
| **Bank A** | `0x01000000` | `0x0103FFFF` | 256 KB |
| **Bank B** | `0x01040000` | `0x0107FFFF` | 256 KB |

User code can only run in one of the Banks. **Bank A is used by default.**

```
Start address: 0x107FFFF
┌──────────────────────┐
│                      │
│       Bank B         │
│       256 KB         │
│                      │
├──────────────────────┤  Start address: 0x1040000
│                      │
│       Bank A         │
│       256 KB         │
│                      │
└──────────────────────┘  Start address: 0x1000000
```

---

### 2.2 Erase and Write Operation Time

FLASH write operations must be written **page by page** (address must be a multiple of `0x100`). Erase operations can only be done **by sector** (address must be a multiple of `0x1000`).

| Operation | Typical Value | Maximum Value | Unit |
|---|---|---|---|
| Page Write (256 bytes) | 2 | 3 | ms |
| Sector Erase (4 KB) | 16 | 30 | ms |

---

### 2.3 Read Operations

When calling read, write, and erase functions, execution jumps into RAM and exits XIP mode to operate on FLASH. Adding compilation of FLASH library `n32wb03x_qflash.c` will occupy an additional **804 bytes of RAM** to store the code.

---

### 2.4 Interface Functions

```c
void Qflash_Init(void);                                              // Must init before use
void Qflash_Erase_Sector(uint32_t address);                          // Erase sector
void Qflash_Write(uint32_t address, uint8_t* p_data, uint32_t len);  // Write
void Qflash_Read(uint32_t address, uint8_t* p_data, uint32_t len);   // Read
```

---

#### 2.4.1 Qflash_Init

**Function:** Initialize the FLASH library. This function **must be called before** calling the FLASH read/write/erase functions.

**Syntax:**

```c
void Qflash_Init(void);
```

**Parameters:** None
**Return:** None

**Example:**

```c
Qflash_Init();
```

---

#### 2.4.2 Qflash_Erase_Sector

**Function:** Erase a sector of FLASH at the specified address.

**Syntax:**

```c
void Qflash_Erase_Sector(uint32_t address);
```

**Parameters:**

- `[in] address` — FLASH address to be erased. **Must be sector starting address** (multiple of `0x1000`).

**Return:** None

**Example:**

```c
Qflash_Erase_Sector(0x1040000);
```

---

#### 2.4.3 Qflash_Write

**Function:** Write data to the specified FLASH address.

**Syntax:**

```c
void Qflash_Write(uint32_t address, uint8_t* p_data, uint32_t len);
```

**Parameters:**

- `[in] address` — FLASH address to write. **Must be page starting address** (multiple of `0x100`).
- `[in] p_data` — Pointer to the data block to be written to FLASH.
- `[in] len` — Length of data to write to FLASH.

**Return:** None

**Example:**

```c
uint8_t data[] = {"12345"};
Qflash_Write(0x1040000, (uint8_t*)data, 5);
```

---

#### 2.4.4 Qflash_Read

**Function:** Read data from the specified FLASH address.

**Syntax:**

```c
void Qflash_Read(uint32_t address, uint8_t* p_data, uint32_t len);
```

**Parameters:**

- `[in] address` — FLASH address to read. **Must be 4-byte aligned.**
- `[out] p_data` — Pointer to data block for storing data read from FLASH.
- `[in] len` — Length of data to read from FLASH.

**Return:** None

**Example:**

```c
uint8_t data[5];
Qflash_Read(0x1040000, (uint8_t*)data, 5);
```

---

### 2.5 Precautions

- **Minimum operation units:** Reading is 4 bytes, writing is 256 bytes per page, erasing is 4KB per sector.

- **Blocking behavior:** FLASH write and erase operations are blocking. Consider whether the operation time affects other code logic.

- **Bluetooth connection timing:** FLASH erase takes 16–30ms. If an erase operation needs to be performed while Bluetooth is connected, the **connection interval should exceed 30ms**, otherwise it may cause abnormal Bluetooth disconnection.

- **Interrupt masking:** Interrupts should be masked to avoid exceptions when performing FLASH operations. This step is included in the driver function.

---

## 3. TRAM Storage Area

There is a **512-byte TRAM value storage area** on the chip. The TRAM storage area is **read-only**.

---

### 3.1 TRAMvalue Structure

```c
typedef struct {
    uint32_t stote_bg_vtrim_value;
    uint32_t stote_rc28800_trim_value;
    uint32_t stote_rc32000_trim_value;
    uint32_t stote_rc32768_trim_value;
    uint32_t stote_rc64m_trim_value;
    uint32_t stote_rc96m_trim_value;
    uint32_t rc_adc_ts_25c;
    uint32_t rc_gpadc_value_3400mv;
    uint32_t rc_gpadc_value_600mv;
    uint8_t  flash_uuid[16];
} trim_stored_t;
```

Data must be read using the dedicated function `SystemTrimValueGet` to return a pointer to the structure.

---

### 3.2 Interface Functions

#### 3.2.1 SystemTrimValueRead

**Function:** Read TRIM storage area data into a user-provided buffer.

**Syntax:**

```c
void SystemTrimValueRead(uint8_t* p_data, uint32_t byte_length);
```

**Parameters:**

- `[out] p_data` — Pointer to buffer for storing the TRIM data.
- `[in] byte_length` — Number of bytes to read.

**Return:** None

**Example:**

```c
trim_stored_t trim_stored;
SystemTrimValueRead((uint8_t*)&trim_stored, sizeof(trim_stored));
```

---

#### 3.2.2 SystemTrimValueGet

**Function:** Return pointer to the `trim_stored_t` structure containing chip calibration values.

**Syntax:**

```c
trim_stored_t* SystemTrimValueGet(void);
```

**Parameters:** None

**Return:** Non-NULL pointer to `trim_stored_t` on success. `NULL` if no TRIM values have been read.

**Example:**

```c
trim_stored_t *p_trim;
p_trim = SystemTrimValueGet();
```

---

#### 3.2.3 SystemGetUUID

**Function:** Return pointer to chip UUID array. Chip UUID length is **16 bytes**.

**Syntax:**

```c
uint8_t* SystemGetUUID(void);
```

**Parameters:** None

**Return:** Non-NULL pointer to UUID array on success. `NULL` if no UUID value has been read.

**Example:**

```c
uint8_t chip_uuid[16];
memcpy(chip_uuid, SystemGetUUID(), 16);
```

---

#### 3.2.4 SystemGetMacAddr

**Function:** Return pointer to chip MAC address array. Chip MAC address length is **6 bytes**. The MAC address consists of bytes 6–11 of the chip UUID.

**Syntax:**

```c
uint8_t* SystemGetMacAddr(void);
```

**Parameters:** None

**Return:** Non-NULL pointer to MAC address array on success. `NULL` if no MAC address value has been read.

**Example:**

```c
uint8_t chip_mac[6];
memcpy(chip_mac, SystemGetMacAddr(), 6);
```

---

## 4. Version History

| Version | Date | Changes |
|---|---|---|
| V1.0 | 2021.10.26 | Initial version |
| V1.2 | 2021.12.23 | Add API function description |
