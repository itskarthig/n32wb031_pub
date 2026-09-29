# N32WB03x Series Errata Sheet

**Version:** V1.1.1
**Date:** 2022.03.28
**Manufacturer:** NSING Technologies Pte. Ltd. (nsing.com.sg)

---

## 1. Errata Overview

| Module | Errata | Version B | Version C | Version D |
|---|---|:---:|:---:|:---:|
| **Bluetooth (BLE)** | 2.1: HSE as system clock | ● | ● | ● |
| | 2.2: EXTI4_12 interrupts while BLE stack is used | ● | ● | ● |
| | 2.3: LSI glitch is abnormal | ● | ● | — |
| **RCC** | 3.1: Note to use of RCC_LSCTRL register | ● | ● | ● |
| | 3.2: RCC_AHBRST's ADCRST bit is abnormal | ● | ● | ● |
| **CACHE** | 4.1: Abnormal fetch instruction on special code logic | ● | — | — |
| **LPUART** | 5.1: LSI 32K clock, 9600 baud and wake-up byte abnormal | ● | ● | ● |
| **SPI** | 6.1.1: SPI baud rate setup | ● | ● | ● |
| | 6.1.2: CRC calibration in slave mode | ● | ● | ● |
| | 6.2: SPI1 interrupts fail with BLE stack | ● | ● | ● |
| **I2C** | 7.1: Abnormal signal interference | ● | ● | ● |
| **KEYSCAN** | 8.1: Retention voltage requirement in Sleep mode | ● | ● | ● |

> ● = problem present, — = fixed

---

## 2. Bluetooth

### 2.1. HSE as System Clock

**Description:**
When the Bluetooth protocol function is used, HSE 32MHz as system clock speed cannot meet the performance requirements.

**Workaround:**
When the Bluetooth protocol function needs to be used, HSE cannot be directly used as system clock. **Select HSI as system clock instead.**

---

### 2.2. EXTI4_12 Interrupts While Bluetooth Protocol Stack Is Used

**Description:**
When the Bluetooth protocol stack function is used, the protocol will reconfigure the EXTI4_12 interrupt, which makes the configuration of EXTI4_12 interrupt during startup invalid.

**Workaround:**
When the Bluetooth protocol stack function needs to be enabled, the protocol stack initialization will configure EXTI4_12 interrupt and use the EXTI11 interrupt function. The user needs to:

1. Configure this interrupt **after** the protocol stack initialization.
2. Add EXTI11 clear flag byte in the interrupt processing function.

---

### 2.3. LSI Issue Affects Wake-up

**Description:**
For chip version B and C, when the power supply VCCRF is over 3V, there is risk of LSI glitch, which will lead to abnormal wake-up of the Bluetooth protocol stack.

**Workaround 1:**
Ensure that the power supply VCCRF is lower than 3V. Connect VCC to VCCRF after a diode to reduce the voltage. Recommended diode models: **BAV21W**, **BZT52C3V6**, **BAP1321**.

**Workaround 2:**
Use LSE as low speed clock source.

**Workaround 3:**
Use the chip of new version D.

---

## 3. Reset and Clock Control (RCC)

### 3.1. Note to Use of RCC_LSCTRL Register

**Description:**
After waking up from Sleep mode, if we operate the register `RCC_CFG`, the register `RCC_LSCTRL` will reset to the default value.

**Workaround:**
After waking up from Sleep mode, **first write to `RCC_LSCTRL`**, and then operate `RCC_CFG`.

---

### 3.2. RCC_AHBRST's Abnormal ADCRST Bit

**Description:**
Setting the `RCC_AHBPRST` register's `ADCRST` bit cannot correctly reset the ADC module.

**Workaround:**
When you need to reset the ADC module, manually assign default values to all ADC module registers.

---

## 4. System Cache Management (CACHE)

### 4.1. Abnormal Fetch Instruction When Bus Accesses Special Code Logic

**Description:**
When chip of version B implements a special instruction sequence, there is an abnormal fetch instruction, demonstrated by the core stopping. The SWD interface can access the JTAG IDCODE interface but cannot get the core ID of the chip.

**Workaround:**
Use the chip of new version D.

---

## 5. Low Power UART (LPUART)

### 5.1. LSI 32K Clock Source — 9600 Baud Rate and Wake-up Byte Are Abnormal

**Description:**
When the LPUART uses a 32K clock source, due to the baud rate being indivisible from the clock source, there will be a baud rate deviation, leading to byte detection errors during wake up, and thus failing to wake up.

**Workaround 1:**
When LSI needs to be used as LPUART clock source, **calibrate the LSI to 32.768K** for use.

**Workaround 2:**
Use **LSE** as the clock source for the LPUART.

---

## 6. Serial Peripheral Interface (SPI)

### 6.1. SPI Issues

#### 6.1.1. SPI Baud Rate Setup

**Description:**
When SPI master mode and CRC calibration function are enabled, and the SPI clock frequency is above 8MHz, the CRC calibration is abnormal.

**Workaround:**
When SPI master mode and CRC calibration function are enabled, keep the **SPI clock frequency at or below 8MHz**.

---

#### 6.1.2. CRC Calibration in Slave Mode

**Description:**
When SPI works in slave mode and has enabled CRC calibration, even if the NSS pin is at high level, if SPI receives the clock signal, it will still conduct CRC calculation.

**Workaround:**
Before CRC calibration, **clear the CRC data register first**, so that the master and slave devices are synchronized in CRC calibration.

---

### 6.2. SPI1 Interrupts Fail When Bluetooth Protocol Stack Is Used

**Description:**
When the Bluetooth protocol stack is enabled, the ROM interrupt vector table is used, but this interrupt vector table does not map out SPI1 interrupt callback function, so it cannot be called back.

**Workaround:**
Use **DMA receiver** or **SPI2 module** instead of SPI1 interrupts.

---

## 7. I2C Interface

### 7.1. Abnormal Signal Interference

**Description:**
During the I2C operation process, SCL and SDA might be disturbed by glitches during communication, resulting in abnormal communication.

**Workaround:**
Use **IO software to simulate I2C** (bit-banging).

---

## 8. Key Scan (KEYSCAN)

### 8.1. Retention Voltage Requirement for KEYSCAN in Sleep Mode

**Description:**
Under sleep mode, KEYSCAN requires higher retention voltage, or there might be risks that KEYSCAN and EXTI3 functions cannot wake up the chip.

**Workaround:**
Increase the retention voltage with the following register configuration:

```c
*(uint32_t*)0x40007014 = 0x00000814;
```

> **Note:** After using this configuration, the sleep power consumption increases by ~200nA.

---

## 9. Version History

| Version | Date | Changes |
|---|---|---|
| V1.1.0 | 2022.10.19 | Initial release |
| V1.1.1 | 2022.03.28 | Error correction |
