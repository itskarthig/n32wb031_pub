# N32WB03x User Manual

**Bluetooth® Low Energy Wireless SoC Series**

**Manufacturer:** Nations Technologies Inc. (nsing.com.sg)

---


N32WB03x Bluetooth® Low Energy wireless
SoC Series
User manual

Contents

2

## 1. Abbreviations in the text


### 1.1. List of abbreviations for registers

The following abbreviations are used in register descriptions:

read/write(rw)                     Software can read and write these bits.

read-only(r)                       Software can only read these bits.

write-only(w)                      Software can only write this bit, and reading this bit will return the reset
value.

read/clear(rc_w1)                  Software can read this bit or clear it by writing' 1', and writing' 0' has no
effect on this bit.

read/clear(rc_w0)                  Software can read this bit or clear it by writing' 0', and writing' 1' has no
effect on this bit.

read/clear by read(rc_r)           Software can read this bit. Reading this bit will automatically clear it to' 0'.
Writing' 0' has no effect on this bit.

read/set(rs)                       Software can read or set this bit. Writing' 0' has no effect on this bit.

read-only write trigger(rt_w)      Software can read this bit and write' 0' or' 1' to trigger an event, but it has no
effect on this bit value.

toggle(t)                          Software can only flip this bit by writing' 1', and writing' 0' has no effect on
this bit.

Reserved(Res.)                     Reserved bit, must be kept at reset value.

### 1.2. Available peripherals

For all models of N32G031 microcontroller series, the existence and number of a peripheral, please refer to
the data sheet of the corresponding model.

## 2. Memory and Bus Architecture


### 2.1. System Architecture


#### 2.1.1. Architecture of System Bus

The main system consists of the following parts:
• Two main drive units:
−    Cortex ®- M0 core system bus
−    General DMA
•   Multiple passive units:
−    Internal SRAM
−    Low power Bluetooth BLE subsystem
−    ADCCTRL
−    Bridge from AHB to AHB, connecting some AHB equipment
−    Bridge from AHB to APB (AHB2APBx), connecting all APB equipment
These are interconnected through a multi-level AHB bus architecture, as shown in Figure 2-1:

*Figure 2-1 Architecture Diagram of System Bus*


• CPU system bus: connecting the ICode/DCode bus of Cortex™-M0 core to the bus matrix, used for instruction
prefetch, data loading (constant loading and debugging access) and AHB/APB peripheral access.
• DMA bus: connecting the AHB master interface of DMA to the bus matrix which coordinates the access of
core and DMA to SRAM, flash memory and peripherals.
• The bus matrix coordinates the access arbitration, based on the rotation algorithm, between the core system bus
and the DMA master bus, and consists of 2 driver components (CPU system bus, DMA bus) and multiple
slave components (SRAM, BLE (SRAM and registers), ADCCTRL and AHB system bus 1/2). Some
peripheral devices of AHB are connected with system bus 1 through bus matrix, while system bus 2 connects
two AHB2APB bridges.
• The system includes two AHB2APB bridges, namely AHB2APB1 and AHB2APB2. Among them, APB1 contains
9 APB peripherals, and the maximum speed of PCLK is 32MHz, as compared to APB2’s 9 APB peripherals and 64MHz.

Bus Address Mapping
Bus address mapping covers all AHB and APB peripherals: AHB peripherals, APB1 peripherals, APB2
peripherals, FLASH, SRAM, etc., and the mapping is specifically presented as follows.
*Figure 2-2 Bus Address Mapping*


Vendor Specific
0 xE010_ 0000–0xFFFF_ FFFF      Reserved                      511 MB
NVIC/ SCS                                                    Reserved     0x 4002_ C 000 –0x 4002_ FFFF
BPU                    Cortex-M0 Peripheral             BLE baseband   0x 4002_ 8000 –0x 4002_ BFFF
DWT                           1 MB                         CRC         0x 4002_ 3000 –0x 4002_ 33 FF
0 xE000_ E 000–0xE000_ EFFF                                                                MODEM         0x 4002_ 2400 –0x 4002_2 FFF
0 xE000_ 2000 –0xE000_2 FFF                                                                  IRC         0x 4002_ 1C 00 –0x 4002_1 FFF
0 xE000_ 1000 –0xE000_1 FFF                                                                  RCC         0x 4002_ 1000 –0x 4002_ 13 FF

AHB
ADC         0x 4002_ 0800 –0x 4002_0 BFF
DMA          0x 4002_ 0000 –0x 4002_ 03 FF

0x 4003_ 2000–0x5FFF_ FFFF      Reserved                                                  SPI2_I2S2      0x 4001_ 4400 –0x 4001_ 47 FF
0x 4002_ 0000 –0x 4003_1 FFF   AHB Peripheral              Reserved2GB                     USART1        0x 4001_ 3800 –0x 4001_3BFF
0x 4001_ 5800 –0x 4001_ FFFF    Reserved                                                    TIM1         0x 4001_ 2C 00 –0x 4001_2 FFF
0x 4001_ 0000 –0x 4001_57FF                                                               SPI1_I2S1      0x 4001_ 2000 –0x 4001_ 23 FF
APB 2 Peripheral
0x 4000_ 7800 –0x 4000_ FFFF                                                              KEYSCAN        0x 4001_ 1400 –0x 4001_17FF
Reserved
0x 4000_ 0000 –0x 4000_77FF                                                                 GPIOB        0x 4001_ 0 C 00 –0x 4001_0 FFF
APB 1 Peripheral
GPIOA        0x 4001_ 0800 –0x 4001_0 BFF

APB2
EXTI         0x 4001_ 0400 –0x 4001_ 07 FF
AFIO         0x 4001_ 0000 –0x 4001_ 03 FF

Peripheral0.5 GB                    PWR        0x 4000_ 7000 –0x 4000_ 73 FF
0x 2000_C 000–0x3FFF_ FFFF      Reserved                                                                0x 4000_ 5400 –0x 4000_ 57 FF
I2C
0x 2000_ 0000–0x2000_ BFFF       SRAM                                                      LPUART       0x 4000_ 4800 –0x 4000_4 BFF
USART2       0x 4000_ 4400 –0x 4000_ 47 FF

APB1
SRAM0.5 GB                      IWDG        0x 4000_ 3000 –0x 4000_ 33 FF
WWDG         0x 4000_ 2C 00 –0x 4000_2 FFF
RTC        0x 4000_ 2800 –0x 4000_ 2BFF
0x 0100_ 0000–0x 010F_ FFFF     FLASH                                                       TIM6        0x 4000_ 1000 –0x 4000_ 13 FF
TIM3        0x 4000_ 0400 –0x 4000_ 07 FF
CODE0.5 GB

*Table 2-1 Address List of Peripheral Register*


Address Range                                                       Peripheral                                   Bus

0x4002_C000 – 0x4002_FFFF                                                        Reserved

0x4002_8000 – 0x4002_BFFF                                                 BLE baseband

0x4002_3000 – 0x4002_33FF                                                          CRC

0x4002_2400 – 0x4002_2FFF                                                        MODEM
AHB
0x4002_1C00 – 0x4002_1FFF                                                          IRC

0x4002_1000 – 0x4002_13FF                                                          RCC

0x4002_0800 – 0x4002_0BFF                                                          ADC

0x4002_0000 – 0x4002_03FF                                                         DMA

0x4001_4400 – 0x4001_47FF                                                    SPI2_I2S2
APB2
0x4001_3800 – 0x4001_3BFF                                                        USART1

Address Range                                         Peripheral                        Bus

0x4001_2C00 – 0x4001_2FFF                                       TIM1

0x4001_2000 – 0x4001_23FF                                     SPI1_I2S1

0x4001_0C00 – 0x4001_0FFF                                      GPIOB

0x4001_0800 – 0x4001_0BFF                                      GPIOA

0x4001_0400 – 0x4001_07FF                                       EXTI

0x4001_0000 – 0x4001_03FF                                       AFIO

0x4000_7000 – 0x4000_73FF                                       PWR

0x4000_5400 – 0x4000_57FF                                        I2C

0x4000_4800 – 0x4000_4BFF                                     LPUART

0x4000_4400 – 0x4000_47FF                                     USART2

0x4000_3000 – 0x4000_33FF                                       IWDG                           APB1

0x4000_2C00 – 0x4000_2FFF                                      WWDG

0x4000_2800 – 0x4000_2BFF                                        RTC

0x4000_1000 – 0x4000_13FF                                       TIM6

0x4000_0400 – 0x4000_07FF                                       TIM3

##### 2.1.1.1. Start address and configuration

The system starts operation after jumping from ROM to the start address of FLASH 0x0100_ 0000 constantly. The system
vector table is included in the ROM address by default.
In order to run the interrupt service program in FLASH or SRAM, the software can map VECTOR to the corresponding
space through configuration by the register PWR_ VTOR_ REG.

### 2.2. Memory System

Program memory, data memory, register and I/O ports are incorporated in the same 4GB linear address space. Data bytes
are stored in the memory in little-endian format. The lowest address byte in a word is considered to be the least significant
byte of the word, while the highest address byte is the most significant byte. The specification of program memory and
data memory is illustrated as follows.

#### 2.2.1. SRAM

SRAM is mainly used for code running, storing variables and data or stacks, with a maximum capacity of 48KB, during
program execution.

SRAM supports byte, halfword, and word read and write access..
SRAM supports code running and can run programs at full speed in SRAM. The maximum address range of SRAM is
0x2000 0000~0x2000 BFFF.

SRAM can maintain data normally in many working modes (Active/Idle/Standby/Sleep) except PD
mode. PowerSwitch power off can be configured in SRAM Standby/Sleep mode.
The main characteristics include:
• The maximum capacity is 48KB in total;
• Support read and write byte/half word/word;
• Both CPU and DMA are accessible;
• Support retention function under low power consumption;
• Run at system clock frequency;

## 3. Power Supply Control (PWR)

### 3.1. Introduction

The working voltage (VCC) of this MCU is 1.8/2.32V~3.6V, and mainly has 2 analog/digital power supply areas
(VCC, VCCRF). Please refer to Figure 3-1 Power Supply Diagram for details.
As the power control module of the whole device, PWR is primarily used to control the MCU to enter different
power modes and enable it to be awakened by other events or interrupts. This MCU supports Active, Idle, Standby,
Sleep, and PD mode.
This MCU power supply architecture is designed with Buck DC/DC cascaded LDO two-stage power supply
structure, and is suitable for low power consumption application scenarios since it allows for the high efficiency
of DC/DC and the low noise/small ripple voltage of LDO.
DC/DC supports PCD peak current control mode and PFM modulation mode and can work in DCM discontinuous
current mode (with small load).
LDO supplies power to each module based on distributed power supply mode, so as to improve the communication
quality of BLE TX&RX and the power isolation between radio modules.
*Figure 3-1 Diagram of Power Supply*


VCC
0.1uF
10uF

VCCRF      VDD_FLASH

0.1uF
3.3V

2.2uF
1uF(2)

GND

VDCDC         X032MP
1uF                                                                                          12pF(1)
VDCDCRF                       32MHz
1uF                                                  XO32MM
SWITCH
2.2uH                                         12pF(1)

0           47pF
RFIOP
NC             NC

1.6nH
N32WB03x
VDD_PA
NC

(a) VDCDC/VDCDCRF provided with internal DCDC power supply

(b) VDCDC/VDCDCRF provided with external power supply

(1)
Different crystals or resonators usually require different load capacitance CL, and the CL selected must match the crystal
or resonator used.
(2)
In the case of a low requirement of ripple, welding is unnecessary for the 1uF capacitor.
DC/DC: Buck DC level converter supplies power to the internal LDO_SYS and LDOs_RF by maintaining the external
power input voltage in the range of 1.8/2.32V～3.6V, improving the system power conversion efficiency, and saving
dynamic power consumption.
Digital system power supply: The voltage regulator supports three running modes: normal mode, low power consumption
mode and power down mode.
•       Normal mode (Active, Idle, Standby)
Voltage regulator is powered by LDO_SYS, mainly used in Active mode, Idle mode and Standby mode. DCDC,
LDO_SYS, and LDO_RET are ON.
•       Low power consumption mode (Sleep)
LDO_RET in VCC domain of voltage regulator is used in low power consumption mode and Sleep mode. DCDC
OFF and LDO_ SYS are OFF, LDO_RET is ON.
•       Power down mode (PD)

In PD mode, the voltage regulator is in Power down mode. DCDC, LDO_SYS and LDO_RET are completely OFF.

#### 3.1.1. Power supply

Introduction of different power domains as below.
•        VCC: the input voltage range of 1.8/2.32 V~3.6 V, mainly providing power input for DCDC, LDO, IO, clock and
reset system.
•        VCCRF: the input voltage range of 1.8/2.32 V~3.6 V, supplying power for most analog and RF peripherals.
•        VDD_ FLASH: No external power supply is required, except a 2.2μF external capacitance.

### 3.2. Power Supply Management

This MCU supports five power modes: Active, Idle, Standby, Sleep, and PD. Different modes have different performance
and power consumption.
The system operation or shutdown under different working modes is presented in the figure below:

Working Mode
Power Supply
Active            Idle                 Standby              Sleep              PD

Power supply system              ON               ON                Partially ON     Run in small numbers      OFF

HSE ON
HSE/HSI          ON               ON                                        OFF                OFF
Clock                                                                 HSI OFF

LSE/LSI       Available         Available             Available           Available            OFF

CPU CORE                         ON        Stop the core clock          OFF                 OFF                OFF

SYS_CLK                          ON               ON                    OFF                 OFF                OFF

Periphery interface           Available         Available               OFF                 OFF                OFF

FLASH                            ON             Standby                Standby        Deep Power-down          OFF

Hold/Partially      Hold/Partially
RAM[1] [2] RETENTION            Hold              Hold                                                         OFF
hold/OFF            hold/OFF

Register                        Hold              Hold              Partially hold      Partially hold         OFF

RF                            Available         Available             Available             OFF                OFF

Interrupt     Available         Available               OFF                 OFF                OFF

EXTI
(RTC+LPUAR       Available         Available             Available           Available            OFF
T+KEYSCAN
+8IO)
Wakeup
WKUP1            Available         Available             Available           Available         Available

RESET Pin
(wakeup +      Available         Available             Available           Available         Available
reset)

Notes:
(1)
48KB SYS SRAM, 8KQSPIC SRAM and 16KB EM SRAM are used for RAM.

(2)
SD/DS mode can be selected for 32KB SYS SRAM and 16KB SYS SRAM under sleep and standby mode;
Please refer to the table below for the working modes:
*Table 3-1 Working Mode*


Mode         Condition                                 Entry                            Exit

CPU startup                               Power-on, system reset, low      Enter Idle, Standby, Sleep and PD mode
Active
All peripherals are configurable          power consumption wake-up

Wakeup:
CPU enters sleep mode and the
1) SLEEPING = 1,                 1) If entering through WFI or ISR return, any
core stops. All peripherals are
NVIC interrupt allows to exit CPU
configurable.                             SLEEPDEEP = 0,
2)     If     entering     through      WFE,
The voltage regulator operates in         SLEEPONEXIT = 0,
SEVEONPEND=0, any event from the
Idle         normal mode.
WFI/WFE                          external interrupt/event line EXTI can wake
FLASH is in Standby state.                                                 up CPU.
2) SLEEPING = 1,
Wakeup        source:   Any       NVIC                                     3)     If     entering     through      WFE,
SLEEPDEEP            =      0,
interrupt, EXTI interrupt or event,                                        SEVONPEND=1, any peripheral interrupt
SLEEPONEXIT = 1,
and RESET can wake up the CPU.                                             (include disabled in NVIC) can wake up
No interrupt waiting when ISR    EXTI events
returns

CPU deep sleep mode.                                                       Wakeup:

Voltage     regulator   operates    in                                     1) When MCU entering STANDBY mode

normal mode.                                                               with WFI instruction, it can be waken up by
any interrupt from the external interrupt event
Both CPU and peripheral interfaces
line EXTI, which may be external GPIO
are shut down.
interrupt     or     internal     peripherals.
Optionally SRAM can hold all or                                            Corresponding NVIC interrupt enable needs
part of the data.                         WFI/WFE: 1）                      to be activated.

HSE/HSI/LSE/LSI switch can be             SLEEPDEEP = 1                    When MCU entering STANDBY mode with
Standby[1]   configured.                               2）PDSTANDBY＝0                    WFE instruction, can be waken up by any
event from the external interrupt event line
The register is partially held.           3) No interrupt (WFI) or event
EXTI as long as SEVEONPEND=0.
BB and RF are available.                  (WFE) is set
3)When MCU entering STANDBY mode
All IO Retention and all GPIO states                                       with WFE instruction, can be waken up by
are retained.                                                              any external interrupt (include disabled in

Wakeup source: EXTI interrupt or                                           NVIC) as long as SEVEONPEND=1.

event and RESET can wake up                                                The peripheral interrupt status bit and EXTI
CPU.                                                                       interrupt pending bit should be cleared

After wakeup, start HSI and HSE                                            through the software.

Mode       Condition                                 Entry                            Exit

and activate the code from the place                                       EXTI events can wake it up.
where it is suspended.

CPU deep sleep mode.
Wakeup:
Voltage     regulator     operates   in
normal mode.                                                               1) N32WB03x, when entering SLEEP mode
with WFI instruction, can be waken up by any
Both CPU and peripheral interfaces
interrupt from the external interrupt event
are shut down.
line EXTI, which may be external GPIO
Optionally SRAM can hold all or                                            interrupt     or    internal    peripherals.
part of the data.                                                          Corresponding NVIC interrupt enable needs
to be activated.
HSE/HSI      OFF,       and   /LSE/LSI
ON/OFF can be configured.                 WFI/WFE:                         N32WB03x, when entering SLEEP mode

1) SLEEPDEEP = 1                 with WFE instruction, can be waken up by
The register is partially held.
any event from the external interrupt event
Sleep[1]                                             2) SLEEPS＝1
VDDD_BB, LDOs_RF, BB and RF                                                line EXTI as long as SEVEONPEND=0.
are OFF.                                  3) No interrupt (WFI) or event
3) N32WB03x, when entering SLEEP mode
FLASH is powered off.                     (WFE) is set
with WFE instruction, can be waken up by

All IO Retention and all GPIO states                                       any external interrupt (include disabled in

are retained.                                                              NVIC) as long as SEVEONPEND=1.

Wakeup source: EXTI interrupt or                                           The peripheral interrupt status bit and EXTI

event and RESET can wake up                                                interrupt pending bit should be cleared

CPU.                                                                       through the software.

After wakeup, start HSI and HSE                                            EXTI events can wake it up.

and activate the code from the place
where it is suspended.

The voltage regulator is closed.

CPU and peripheral interfaces are
closed.

SRAM data is lost.                        WFI/WFE:

HSE/HSI/LSI/LSI is closed.                3) SLEEPDEEP = 1

PD         The register is not held.                 4) PDSTANDBY＝1                   WKUP1 rising edge, RESET

BB and RF off.                            3) No interrupt (WFI) or event
(WFE) is set
FLASH is powered off.

Except for the wake-up source
RESET/WKUP1, other IO ports are
in high impedance state.

Mode     Condition                           Entry                             Exit

BB and RF off.

FLASH is powered off.

Except for the wake-up source
RESET/WKUP1, other IO ports are
in high impedance state.

Note(1): In the Standby and Sleep modes, the code can continue to run from the stop position after wakeup.

### 3.3. Low Power Consumption Mode

By default, MCU is in active mode after system or power reset. When it is unnecessary to run the CPU (for example, when
waiting for external events), several low power consumption modes can be used to save power. Users can select the
optimum low power consumption mode based on low power consumption, short start time and available wake-up sources.
Four characteristics of low power consumption modes:
•     Idle mode (the core stops, and all peripherals, including Cortex ®- M0 core peripherals, such as NVIC, system tick
clock (SysTick) are still running).
•     Standby mode (voltage regulator still operates in normal mode, CORE is powered off, and BLE and RF can operate).
•     Sleep mode (most clocks are turned off, voltage regulators operate in low power consumption mode, and CORE, BLE
and RF are powered off).
•     PD mode (VDDD power down mode, VCC hold, a WAKEUP IO and RESET can wake up).
In addition, one of the following methods can be used to reduce the power consumption in running mode:
•     Lower the system clock
•     Turn off unused peripheral clocks on the APB and AHB bus

#### 3.3.1. After Entering the User mode for Active mode

the first time, the core system and BLE subsystem remain Active, and CORE system working mode is controlled by the
CPU.
The working mode of BLE subsystem is controlled by CPU and baseband. SYS RAM/FLASH is activated by default after
power on.

#### 3.3.2. Idle Mode

##### 3.3.2.1. Entering idle mode

Enter Idle mode by executing WFI (Wait Interrupt) or WFE (Wait Event) instructions and SLEEPDEEP=0. According to
SLEEPONEXIT bit value in the control register of Cortex®-M0 system, two options can be used to select the Idle mode
entry mechanism:
•     Sleep now: If the SLEEPONEXIT bit is cleared, WFI or WFE instructions will be executed immediately, and the
system will enter Idle mode promptly.
•     Sleep on exit: If SLEEPONEXIT is in position 1, the system will enter Idle mode immediately when exiting from the
lowest priority interrupt handler.
In Idle mode, all I/O pins remain in the same state/function as in running mode.

##### 3.3.2.2. Exiting idle mode

If WFI instruction is used to enter Idle mode, any peripheral interrupts responded by the Nested Vector Interrupt Controller
(NVIC) can wake the device from Idle mode.
If WFE instruction is used to enter Idle mode, MCU will exit Idle mode immediately when an event occurs. Wakeup events
can be generated in the following ways:
•    Enable an interrupt in the peripheral control register instead of in NVIC, and enable SEVONPEND bit in the control
register of Cortex®-M0 system. When MCU recovers from WFE, peripheral interrupt pending bit and peripheral
NVIC interrupt channel pending bit (in NVIC interrupt clear pending register) must be cleared.
•    Configure an external or internal EXTI event mode. When the CPU recovers from WFE, because the pending bit
corresponding to the event line is not set, it is unnecessary to clear the peripheral interrupt pending bit and the
peripheral NVIC interrupt channel pending bit (in the NVIC interrupt clear pending register). This mode provides the
shortest wake-up time, because there is no time lost in interrupt entry or exit.

#### 3.3.3. Standby Mode

CPU and peripheral interfaces are closed in Standby mode.
In Standby mode, only LSI, LSE and HSE can be used. However, SRAM data can be retained, and the register contents
are partially saved.
In Standby mode, all I/O pins remain in the same state and function as in running mode.

##### 3.3.3.1. Entering Standby mode

When entering Standby mode, you need to set SLEEPDEEP=1.
Configure PWR_ CR1. PMU_ MODE_ EN＝1 and PWR_ CR1. PMU_ MODE＝3’b001 。
Execute WFI or WFE instruction to enter Standby mode.
Users can enable the relevant EXTI wake-up source in advance.
In Standby mode, the following characteristics can be selected by programming each control bit:
•    RTC: It can be activated through the RTCEN bit in register RCC_LSCTRL
•    Internal RC oscillator (LSI RC): it can be activated through the LSIEN bit in register RCC_LSCTRL
•    External crystal oscillator (LSE OSC): it can be activated through the LSEEN in register RCC_LSCTRL
•

##### 3.3.3.2. Exiting Standby mode

When MCU enter STANDBY mode with WFI instruction, it can be waken up by any interrupt from the external interrupt
event line EXTI, which may be external GPIO interrupt or internal peripherals. Corresponding NVIC interrupt enable needs
to be activated.
When MUC enter STANDBY mode with WFE instruction, it can be waken up by any event from the external interrupt
event line EXTI as long as SEVEONPEND=0.
When MCU enter STANDBY mode with WFE instruction, it can be waken up by any external interrupt (include disabled
in NVIC) as long as SEVEONPEND=1.
RESET falling edge as EXTI interrupt line wakes up CPU, and the system will not reset.
After the Standby mode is waken up, the program will continue to execute from the sleep location.

#### 3.3.4. Sleep Mode

In Sleep mode CPU, peripheral interfaces, and BLE subsystem are closed.
In Sleep mode, the system stops running, and only part of the wake-up logic and registers are reserved to respond to wake-
up operations. Users can choose to hold SRAM or not.
High speed clock stops and 32KHz low speed clock works. FLASH enters deep sleep mode or power off.
In Sleep mode, all I/O pins remain in the same state and function as in running mode.

##### 3.3.4.1. Entering Sleep mode

Configure the value of BLE DEEPSLEEP control register (address: 0x4002_8030) to 0x07 (with BB in Sleep mode). Read
PWR_CR1. BLE_OSC_EN=0x0, and confirm that BLE has entered Sleep mode.
Configure PWR_CR1. PMU_ MODE_EN＝0x1 and PWR_CR1. PMU_MODE＝0x2。Execute WFI or WFE instruction.
When FLASH and APB are idle, enter the Sleep mode. The user can enable the wake-up source of relevant EXTI in
advance.

##### 3.3.4.2. Exiting Sleep mode

When MCU enter SLEEP mode with WFI instruction, it can be waken up by any interrupt from the external interrupt
event line EXTI, which may be external GPIO interrupt or internal peripherals. Corresponding NVIC interrupt enable
needs to be activated.

When MCU entering SLEEP mode with WFE instruction, it can be waken up by any event from the external interrupt
event line EXTI as long as SEVEONPEND=0.

When MCU entering SLEEP mode with WFE instruction, it can be waken up by any external interrupt (disabled in NVIC)
as long as SEVEONPEND=1. It is required to clear the peripheral interrupt status bit and EXTI interrupt pending bit with
the software, and the EXTI event can wake up CPU.

The RESET falling edge as the EXTI interrupt line wakes up the CPU on the Sleep mode, the system will not reset. After
Sleep mode is waken up, the program will continue to execute from the sleep position.

The system is waken up, followed by BLE, and the CPU runs in Active mode.

#### 3.3.5. PD mode

PD mode allows for lower power consumption and is based on Cortex®-M0 deep sleep mode, in which mode CPU and
all peripherals are OFF.
The main voltage regulator and HSE/HSI/LSI/LSI clock source are OFF. Except RESET/WKUP1, other IO ports are in
high resistance state.

##### 3.3.5.1. Entering PD mode

When entering PD mod, set SLEEPDEEP=1.
Configure PWR_ CR1. PMU_ MODE_ EN＝0x1 and PWR_ CR1. PMU_ MODE＝0x4.

##### 3.3.5.2. Exiting PD mode

In case of external reset (RESET pin) and rising edge event of WKUP1 pin, MCU exits PD mode, and all registers will
reset after waking from PD state.
After wake-up from PD mode, code execution is equivalent to the execution after reset (reading reset vector, etc.).

### 3.4. PWR Register

#### 3.4.1. Diagram of PWR Register

*Table 3-2 PWR Register Overview*


Offs   Registe
et          r

PWR_MODE
Reserved
PWR_
000        CR1
h
Reset
0   0   0   0 0   0 0    0       0      0   0   0    0   0       0   0      0         0      0    0   0     0        0   0   0   0          1   0   0   0     0        0
Value

BLE_STA
Reserved

Reserved

Reserved
004    PWR_

TE
h         CR2

Reset
0   0   0   0 0   0 0    0       0      0   0   0    0   0       0   0      0         0      1    0   0     0        0   0   1   0          1   1   0   0     0        0
Value
VTOR_RE

VTOR
G

_REG
030
h
Reset
0   0   0   0 0   0 0    0       0      0   1   0    0   0       0   0      0         0      0    0   0     0        0   0   0   0          0   0   1   0     0        0
Value

#### 3.4.2. Power Control Register1 (PWR_CR1)

Offset address: 0x00
Reset value: 0x0000 0020

Bit Field        Name                                         Description

[31:6]           Reserved                                     Retention

[5]         BLE_OSC_EN      Before the CPU enters Sleep mode, it is required to query that the bit is 0,
ensuring that BLE has entered Sleep mode.

1: BLE system has exited Sleep mode;

0: BLE system is in Sleep mode.

[4]         Reserved        Retention

[3]         PWR_MODE_EN     Enable bit for entering low power consumption mode.

[2:0]       PWR_MODE        Selection of system low power consumption mode

000: Active mode

001: Standby mode

010: Sleep mode

100: PD mode

Others: reserved

#### 3.4.3. Power Control Register2 (PWR_CR2)

Offset address: 0x04
Reset value: 0x0000 20b0

Bit Field   Name             Description

[31:16]     Reserved         Retention

[15:13]     BLE_STATE        BLE system status

000:ble_ power_ On power on status;

001:ble_active normal run status;

100:ble_sleep ble in Sleep state;

Others: reserved

[12:8]      Reserved         Retention

[7]         Reserved         Must be maintained as: 1

[6:4]       Reserved         Retention

[3]         PAD_STA          Status control bit of PAD in Standby/Sleep mode

0: PAD hold

1: PAD high resistance

[2]         Reserved         Reserved, the value is 0 by default

[1]         CORE_16KMEM_LP   Mode selection of core_16K RAM in Standby/Sleep mode

0:core_ Ram 16K in SRAM DeepSleep mode (data retention)

1:core_ Ram 16K in power-off mode (data not retained)

[0]         CORE_32KMEM_LP   Mode selection of core_32K RAM in Standby/Sleep mode

0:core_ Ram 32K in SRAM DeepSleep mode (data retention)

1:core_ Ram 32K in power-off mode (data not retained)

#### 3.4.4. Interrupt Vector Address Remap Register (VTOR_REG)

Offset address: 0x30
Reset value: 0x0002 0008

Bit Field   Name                              Description

[31:0]      VTOR_REG                          Interrupt vector address Remap

[31]: VTOR Remap enable;

[30:0]: Remap address;

## 4. Reset and Clock Control (RCC)


### 4.1. Reset Control Unit

N32WB03x supports the following three reset modes:
•      Power reset
•      System reset
•      Low power reset

#### 4.1.1. Power Reset

A power reset will occur in case of any one of the following events:
•       Power on/down reset VDDD_ POR (POR/PDR reset)
•       Return from PD power
down mode Power reset will
reset all registers.
The reset source will eventually act on the RESET pin and a low level will remain during the reset process. The
reset entry vector is fixed at the address 0x0000_ 0004. For more details, refer to Table 6.1 Vector Table.

#### 4.1.2. System Reset

Except for some specific registers in the RCC and VDDD_AON power domain (PWR/RTC/RCC/AFIO/AFEC),
system reset will reset all registers to their reset state.
Any one of the following events will lead to a system reset:
•       Low level on RESET pin (external reset)
•       Window watchdog count termination (WWDG reset)
•       Independent watchdog count termination (IWDG reset)
•       Software reset (SCLKSW reset)
•       BOR reset (FLASH version)
The source of reset event can be identified by checking the reset status flag bit in the RCC_CTRLSTS control status register.

##### 4.1.2.1. Software reset

Software reset can be generated by setting the SYSRESETREQ bit in Cortex®-M0 Application Interrupt and Reset
Control Register. Refer to Cortex®-M0 technical reference manual for further information.
Reset signals inside the chip will be output on the RESET pin, and the pulse generator ensures that each (external or internal)
reset source has at least 20 μs pulse delay, and will generate reset pulse when RESET pin is pulled down to generate external
reset.

*Figure 4-1 Reset Circuit*


VDD

RESET                                                    System reset
Filter
PAD

B
O
Pulse
R                   OR                   generator
(min 20us)
W
W
D
G

I
W
D
G
Software

### 4.2. Clock Control Unit

Two different clock sources can be used to drive the system clock (SYSCLK):
•   HSI oscillator clock (64MHz)
•   HSE oscillator clock (32MHz)
Two secondary clock sources are used by these devices:
•   LSI oscillator clock (32KHz)
•   LSE oscillator clock (32.768KHz)
LSI can be used to drive independent watchdog IWDG and drive RTC, KEYSCAN and LPUART through
program selection.
LSE can be used to drive RTC, KEYSCAN and LPUART through program selection.
LSI/LSE is also used to wake up the system automatically from Idle/BLE_SLEEP/Standby/Sleep/PD mode.
Any of the clock sources, when not in use, can be independently turned on or off to optimize system power consumption.
After the system is powered on and reset, HSI and HSE are enabled by default, and HSI is selected by default as the
system clock.
*Figure 4-2 Clock Tree*


> **Note: (1) Refer to the chapter “Electrical characteristics” in the corresponding product datasheet for the**
characteristics of internal and external clock sources.
Users can configure the frequencies of AHB and APB (APB1 and APB2) domains through multiple
prescalers. The maximum allowable frequency of AHB, APB1 and APB2 domain is 64MHz.
Except for the following cases, all peripheral clocks are derived from the system clock (SYSCLK)
•   ADC clock is obtained by dividing the AHB clock.
•   LPUART working clock may come from one of the following three sources, which can be configured by the software:
−    LSI clock
−    LSE clock
−    APB1 clock (PCLK)
•   RTCCLK clock source can be provided by LSE or LSI clock
−     LSE clock
−     LSI clock
•   The clock source of IWDG is LSI oscillator.

RCC serves as the external clock of Cortex system timer (SysTick) after 8 frequency division through AHB clock (HCLK).
By controlling SysTick and setting status register, users can select the above clock or Cortex (HCLK) clock as SysTick
clock.
The frequency allocation of timer clock is automatically set by the hardware according to the following 2 conditions:
•   If the corresponding APB pre-scale coefficient is 1, the timer’s clock frequency is consistent with the frequency of
APB bus,
•   otherwise the timer’s clock frequency is set to twice the frequency of APB bus connected to it.

#### 4.2.1. HSE Clock

High speed external clock signal (HSE) is generated by the following two clock sources: HSE external crystal or external
clock.
In order to reduce the distortion of clock output and shorten the stability time of startup, the crystal and load capacitor must
be as close to the oscillator pin as possible, and the load capacitance value must be adjusted according to the oscillator
selected.
32MHz external oscillator can provide more accurate master clock input for system clock. Users can refer to the Electrical
Characteristics section of the datasheet for further information.
The HSERDF bit in the clock control register RCC_CTRLCTRL (directly from analog HSE) is used to indicate whether
the high-speed external oscillator is stable. During startup, the clock is not released until this bit is set to 1 by the hardware.
If interrupt is allowed in the clock interrupt register RCC_CLKINT, corresponding interrupt will be generated.
By setting the HSEEN bit in RCC_CTRL in the clock control register, HSE crystal can be enabled and disabled, and it is
enabled by default.

#### 4.2.2. HSI Clock

The HSI clock signal, which is generated by the internal RC oscillator, can directly serve as the system clock input.
Without any external devices, HSI RC oscillator can provide a system clock with shorter starting time than HSE crystal
oscillator. However, the former system clock still has a relatively poor frequency accuracy even after calibration.
The manufacturing process determines that different chips differ in RC oscillator frequency, which is typically calibrated
to 0.5% (25℃) and less 5% within the full temperature range.
The accuracy of RC oscillator will be affected when the application by users is based on different voltages or ambient
temperatures, and HSI frequency can be adjusted through the HSITRIM [6:0] bit in the clock control register.
The HSIRDF bit in the clock control register is used to indicate whether the HSI RC oscillator is stable. In the process of
clock startup, 128 HSI cycle startup counts are generated, and HSI RC output clock is not released until this bit is set to 1
by the hardware.
The startup and shutdown of HSI RC can be implemented by the HSIEN bit in the clock control register.
Note 1: HSI or HSI/2 can be selected internally and HSI/2 is used by default, users start HSI TRIM to 64M through software.

#### 4.2.3. HSI Calibration

HSE clock calibrates HSI clock (HSI or HSI/2 optionally):
•   Startup condition: after each system reset release or low power consumption wake-up, and when HSE output maintains
stable, the hardware automatically performs a correction count and stores the count result in a register. Software
initialization calibrates HSI (the count result is read when count is done , and the software rewrites the TRIM value of
HSI clock frequency according to the frequency conversion relationship with RC clock, and the hardware then
performs correction count).
It also supports HSI calibration when the system is active. By configuring different TRIM values or START bits of
HSI, the software starts a correction count (the corresponding clock should be started and the clock output is confirmed
stable).
•   Working principle of HSI clock calibration counter
Two counters are used. When the correction starts, the first counter starts counting by using HSE clock, ends

counting after 1,024 beats, and generates the counting done signal.
During this counting period, the second counter starts counting by using HSI clock, and when counting is Done, stores
counting result in the register for query by CPU.
•   Frequency conversion relationship between counting results and RC clock
Actual frequency of HSI clock (HSI or HSI/2 optionally) (unit: MHz)=(RC64M_Cnt/1024) * 32

#### 4.2.4. LSE Clock

LSE crystal, a 32.768KHz low-speed external crystal, provides a low-power and accurate clock source for real-time clock
or other timing functions.
The startup and shutdown of LSE crystal is implemented by the LSEEN bit in the low speed clock control register
(RCC_LSCTRL).
LSERD in the low-speed clock control register (RCC_LSCTRL) indicates whether the LSE crystal oscillator is stable.In
the startup phase, during the clock startup, 1024 LSE cycle startup counts are generated,. The LSE clock signal is not
released until this bit is set to 1 by the hardware. If interrupt is allowed in the clock interrupt register, an interrupt request
can be generated.

#### 4.2.5. External Clock Source (LSE bypass)

In this mode, an external clock source with a maximum frequency of 1MHz must be provided. Users can select this mode
by setting LSEBP bit and LSEEN=0 in the low speed clock control register (CC_LSCTRL). The external clock signal
(square wave, sine wave or triangle wave) with 50% duty cycle must be connected to OSC32K_ IN pin.

#### 4.2.6. LSI Clock

LSI can provide clock for IWDG and AWU in STANDBY or Sleep mode. LSI clock frequency is about 32KHz. Refer to
the Electrical Characteristics section of the datasheet for further information.
The startup and shutdown of LSI can be implemented through the LSIEN bit in the control/status register
(RCC_CTRLSTS).
LSIRD bit in the control/status register (RCC_CTRLSTS) indicates whether low speed internal oscillation is stable. During
clock startup, 8 LSE cycle startup counts are generated, and the clock signal is not released until this bit is set to 1 by the
hardware. If interrupt is allowed in the clock interrupt register (RCC_CLKINT), corresponding LSI interrupt will be
generated.

#### 4.2.7. LSI Calibration

Frequency offset can be compensated by calibrating the internal low-speed oscillator LSI, so as to obtain the RTC time
base with acceptable accuracy and the timeout of independent watchdog (IWDG) (when these peripherals use LSI as the
clock source).
LSE clock calibrates by HSI clock
•   Startup condition: It supports LSI calibration when the system is active. By configuring different TRIM values or
START bits of LSI, the software starts a correction count (the corresponding clock should be started and the clock
output is confirmed stable).
The hardware automatically performs a correction count and stores the count result in a register. Software initialization
calibrates LSI (the count result is read when count is done, and the software rewrites the TRIM value of LSI clock
frequency according to the frequency conversion relationship with RC clock, and the hardware then performs
correction count).
> **Note: For LSI correction, the software shall query the count done flag after configuring TRIM value or START.**
•   Working principle of LSI clock calibration counter
Two counters are used. When the correction starts, the first counter starts counting by using LSI clock, ends counting
after 20 beats for LSI_TRIM_CFG, and generates the counting done signal.
During this counting period, the second counter starts counting by using HSE clock, and when counting is done, stores
counting result in the register for query by CPU.
•   Frequency conversion relationship between counting results and RC clock
Actual frequency of LSI clock (unit: KHz)=32000 * LSI_ TRIM_ CFG/ RC32K_ Cnt

#### 4.2.8. Selection of System Clock (SYSCLK)

After system reset, HSI oscillator is selected as the system clock. The software selects clock source through CFG
control bit SCLKSW, and the clock source, when being directly used as the system clock, cannot be stopped.
The switch from one clock source to another will only occur when the target clock source is ready (after the delay
in the startup and stabilization phase). When the selected clock source is not ready, the system clock switching
will not occur until the target clock source is ready.
The status bit in the clock control register (RCC_CTRL) indicates which clock is ready and which is currently
used as the system clock.

#### 4.2.9. RTC Clock

By setting the LSXSEL bit in low speed clock control register (RCC_LSCTRL), RTCCLK clock can be provided
by LSE or LSI clock.

#### 4.2.10. Watchdog clock

If the independent watchdog has been started by the software, the LSI oscillator will be forced to open and cannot be
closed. The clock is supplied to IWDG after LSI oscillator is stabilized.

#### 4.2.11. LPUART clock

In normal operation mode LPUART clock supports two clock sources: LSI_LSE_CLK and LPUART_PCLK.
Because PCLK will be closed in the low power consumption mode, the software should switch LPUART clock to
LSI_LSE_CLK before entering the low power consumption mode.

#### 4.2.12. Clock output

The microcontroller allows for outputting clock signal to the external MCO pin.
The corresponding GPIO port register must be configured with the corresponding function, and the following
seven clock signals can be selected as MCO clock:
•    SYSCLK
•    HCLK
•    HSI
•    HSE
•    LSI
•    LSE
•    AUDIOPLL
Clock selection is controlled by the MCO bit in the clock configuration register (RCC_CFG). For the MCO pin
output, the clock is fixed with 4 frequency division output.

### 4.3. RCC Register


#### 4.3.1. Register Overview

*Table 4-1 RCC Register Overview*


Offs   Regist
et      er

HSITRIM_6
Reserved

Reserved

Reserved
RCC_

_0
CTRL
000
h
Reset
0    0         0    0         0     0         0           0   0          0   0          0      0        0   1   1   0      0                  0    0                1    1    0         1          0      0              0          0     0      0       1   1
Value

APB2PRES

APB1PRES

AHBPRES
Reserved

Reserved

Reserved

Reserved
RCC_
MCO

CFG
004
h
Reset
0    0         0    0         0     0         0           0   0          0   0          0      0        0   0   0   0      0                  0    0                0    0    0         0          0      0              0          0     0      0       0   0
Value

RCC_
Reserved

Reserved

Reserved
CLKIN
008      T
h
Reset
0    0         0    0         0     0         0           0   0          0   0          0      0        0   0   0   0      0                  0    0                0    0    0         0          0      0              0          0     0      0       0   0
Value

RCC_
Reserved

Reserved

APB2
00C    PRST
h
Reset
0    0         0    0         0     0         0           0   0          0   0          0      0        0   0   0   0      0                  0    0                0    0    0         0          0      0              0          0     0      0       0   0
Value

RCC_
Reserved

Reserved

Reserved

Reserved

Reserved

Reserved

APB1
010    PRST
h
Reset
0    0         0    0         0     0         0           0   0          0   0          0      0        0   0   0   0      0                  0    0                0    0    0         0          0      0              0          0     0      0       0   0
Value

RCC_
Reserved

Reserved

014    AHBP
h     CLKE
N

Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   1   0      1              0   0
Value

RCC_

Reserved

Reserved
APB2
PCLK
018
EN
h

Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   0   0      0              0   0
Value

RCC_
Reserved

Reserved

Reserved

Reserved

Reserved

Reserved
APB1
PCLK
01C
EN
h

Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   0   0      0              0   0
Value

RCC_
Reserved

Reserved
LSITRIM
LSCT
020    RL
h
Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        1   1   0   0    0         0         0   1   1   1   1          0            0              0   0   0      0              1   1
Value

RCC_
Reserved

Reserved
CTRL
024   STS
h
Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   1   1      0              0   0
Value

RCC_
Reserved

Reserved

AHBP
RST
28h

Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   0   0      0              0   0
Value
Reserved

Reserved

Reserved

RCC_
CFG2
02C
h
Reset
0    0         0   0   0   0   0          0   0          0    0         0            0        0   0   0   0    0         0         0   0   0   0   0          0            0              0   0   0      0              0   0
Value

LSI_TRIM_
Reserved

Reserved
OSCF

CFG
CCR
030
h
Reset
0   0     0   0   0   0   0         0    0          0        0     0     0       0      0      0       0   0   0   1               0     1   0       0    0   0        0          0   0   0   0   0
Value

Reserved
OSCF
CSR
34h

Reset
0   0     0   0   0   0   0         0    0          0        0     0     0       0      0      0       0   0   0   0               0     0   0       0    0   0        0          0   0   0   0   0
Value

OSCF
Reserved

LSICNT
CLSIC
NT
38h

Reset
0   0     0   0   0   0   0         0    0          0        0     0     0       0      0      0       0   0   0   0               0     0   0       0    0   0        0          0   0   0   0   0
Value

OSCF
Reserved

HSICNT
CHSI
03C   CNT
h
Reset
0   0     0   0   0   0   0         0    0          0        0     0     0       0      0      0       0   0   0   0               0     0   0       0    0   0        0          0   0   0   0   0
Value

DBGM
Reserved

CU_C
044    R
h
Reset
0   0     0   0   0   0   0         0    0          0        0     0     0       0      0      0       0   0   0   0               0     0   0       0    0   0        0          0   0   0   0   0
Value

#### 4.3.2. Clock Control Register (RCC_CTRL)

Offset address: 0x00
Reset value: 0x0003 0d03

Bit Field   Name          Description

31:25       Reserved      Always read as 0.

24          AUDIOPLLEN    AUDIOPLL enable bit
Set to 1 or clear to zero by software.
0: Close AUDIOPLL
1: Open AUDIOPLL

23:18       Reserved      Always read as 0.

17          HSERDF        External high-speed clock ready bit.
Hardware will be set to 1 after HSE is ready. After the HSEEN bit is cleared, 100ns is
needed to clear.
0: HSE not ready
1: HSE ready

16          HSEEN         External high-speed clock enable.
Set to 1 or clear to zero by software.
Cleared by hardware when entering Sleep or PD mode.
When returning from Sleep mode, the hardware will set 1 to start HSE oscillator.
This bit cannot be cleared when HSE serves as the system clock.
0: Turn off HSE oscillator
1: Turn on HSE oscillator

15          HSITRIM_7     Select HSI frequency 0: 64MHz

14:8        HSITRIM_6_0   The correction value of internal high-speed clock, written by the software, is used to
calibrate the frequency of internal HSI RC oscillator.
See HSI Calibration section for details
HSITRIM <7:0>:
00000000:37.62MHz；
00000001:37.8MHz；
00001101:40.13MHz；
01010010:63.41 MHz；
01010011:63.97 MHz；
01010100:64.44 MHz；
01111110:101.1MHz；
01111111:102.6MHz；

7:2         Reserved      Always read as 0.

1           HSIRDF        Internal high-speed clock ready flag bit.
The hardware will be set to 1 after HSI stabilization. The bit requires 6 internal oscillator
clock cycles for clearing after the HSIEN bit is cleared.

0           HSIEN         Internal high-speed clock enable bit.
Set to 1 or clear to zero by software.

Bit Field              Name   Description
Cleared by hardware when entering Sleep or PD mode.
The hardware will set 1 to start the HSI oscillator when returning from Sleep mode.
This bit cannot be cleared when HSI is used as the system clock;
0: Turn off HSI oscillator
1: Turn on HSI oscillator

#### 4.3.3. Clock Configuration Register (RCC_CFG)

Offset address: 0x04
Reset value: 0x0000 0000

Bit Field        Name         Description

31:29            Reserved     Always read as 0.

28:25            MCO          MCU clock output

Set and clear by software.

000: No clock

001: LSI clock

010: LSE clock

011: System clock (SYSCLK) selected

100: HSI frequency division clock

101: HSE clock

110: HCLK bus clock

111: AUDIOPLL clock

Notes:

The clock output may be truncated when the MCO clock source is started and switched.

When the system clock is output to the MCO pin, ensure that the output clock frequency
does not exceed 50MHz (the highest frequency of I/O port).

24:17            Reserved     Always read as 0.

16               HSISRC       HSI clock source of the system.

Bit Field   Name       Description

Set to 1 or clear to zero by software. This bit can only be written when HSI is off.

0: HSI 2 frequency division as system clock input

1: HSI non frequency division as system clock input

15:14       Reserved   Always read as 0.

13:11       APB2PRES   High speed APB (APB2) prescale.

Set or reset by the software, configure the prescale coefficient of APB2 clock PCLK2.
The clock frequency of APB2 must not exceed 64MHz.

0xx: HCLK non frequency division

100: HCLK 2 frequency division

101: HCLK 4 frequency division

110: HCLK 8 frequency division

111: HCLK 16 frequency division

10:8        APB1PRES   Low speed APB (APB1) prescale.

Set or reset by the software, configure the prescale coefficient of APB1 clock PCLK1.
The clock frequency of APB1 must not exceed 32MHz.

0xx: HCLK non frequency division

100: HCLK 2 frequency division

101: HCLK 4 frequency division

110: HCLK 8 frequency division

111: HCLK 16 frequency division

7:6         Reserved   Always read as 0.

5:4         AHBPRES    AHB prescale.

Set or reset by the software, configure the prescale coefficient of HCLK of AHB clock.

0x: SYSCLK non frequency division

10: SYSCLK 2 frequency division

11: SYSCLK 4 frequency division

3           Reserved   Always read as 0.

2           SCLKSTS    System clock switching status

It is set and cleared by the hardware to indicate which clock source is used as the system

Bit Field        Name        Description

clock.

0: HSI oscillator is used as system clock

1: HSE oscillator is used as system clock

1                Reserved    Always read as 0.

0                SCLKSW      System clock switch

Set or cleared by the software, select the SYSCLK source.

0: HSI is selected as system clock

1: HSE is selected as system clock

#### 4.3.4. Clock Interrupt Register (RCC_CLKINT)

Offset address: 0x08
Reset value: 0x0000 0000

Bit Field        Name        Description

31:20            Reserved    Always read as 0.

19               HSERDICLR   HSE ready interrupt clear bit.

Set 1 by the software to clear the HSERDIF flag bit.

0: Not used

1: Clear HSERDIF interrupt flag bit

18               HSIRDICLR   HSI ready interrupt clear bit.

Set 1 by the software to clear the HSERDIF flag bit.

0: Not used

1: Clear HSIRDIF interrupt flag bit

17               LSERDICLR   LSE ready interrupt clear bit.

Set 1 by the software to clear the HSERDIF flag bit.

Bit Field   Name        Description

0: Not used

1: Clear LSERDIF interrupt flag bit

16          LSIRDICLR   LSI ready interrupt clear bit.

Set 1 by the software to clear the HSERDIF flag bit.

0: Not used

1: Clear LSIRDIF interrupt flag bit

15:12       Reserved    Always read as 0.

11          HSERDIEN    HSE ready interrupt enable bit.

Set to 1 or clear by the software and enable or disable HSE Ready Interrupt.

0: Disable HSE ready interrupt

1: Enable HSE ready interrupt

10          HSIRDIEN    HSI ready interrupt enable bit.

Set to 1 or clear by the software and enable or disable HSI Ready Interrupt.

0: Disable HSI ready interrupt

1: Enable HSI ready interrupt

9           LSERDIEN    LSE ready interrupt enable bit.

Set to 1 or clear by the software and enable or disable LSE Ready Interrupt.

0: Disable LSE ready interrupt

1: Enable LSE ready interrupt

8           LSIRDIEN    LSI ready interrupt enable bit.

Set to 1 or clear by the software and enable or disable LSI Ready Interrupt.

0: Disable LSI ready interrupt

1: Enable LSI ready interrupt

7:4         Reserved    Always read as 0.

3           HSERDIF     HSE ready interrupt enable bit.

When HSERDIEN is set to 1 and the external high-speed clock is ready, the hardware
will set this bit to 1.

This bit is cleared by software by setting the HSERDICLR bit.

Bit Field        Name        Description

0: No clock ready interrupt is generated by HSE oscillator

1: Clock ready interrupt is generated by HSE oscillator

2                HSIRDIF     HSI ready interrupt enable bit.

When HSIRDIEN is set to 1 and the external high-speed clock is ready, the hardware
will set this bit to 1.

The HSIRDICLR bit 1 is cleared by the software.

0: No clock ready interrupt is generated by HSI oscillator

1: Clock ready interrupt is generated by HSI oscillator

1                LSERDIF     LSE ready interrupt enable bit.

When LSERDIEN is set to 1 and the external high-speed clock is ready, the hardware
will set this bit to 1.

The LSERDICLR bit 1 is cleared by the software.

0: No clock ready interrupt is generated by LSE oscillator

1: Clock ready interrupt is generated by LSE oscillator

0                LSIRDIF     LSI ready interrupt enable bit.

When LSIRDIEN is set to 1 and the external high-speed clock is ready, the hardware
will set this bit to 1.

The LSIRDICLR bit 1 is cleared by the software.

0: No clock ready interrupt is generated by LSI oscillator

1: Clock ready interrupt is generated by LSI oscillator

#### 4.3.5. APB2 Peripheral Reset Register (RCC_APB2PRST)

Offset address: 0x0c
Reset value: 0x0000 0000

Reset value:     Name        Description
0x0000
31:15            Reserved    Always read as 0.
0000Bit Field
14               USART1RST   USART1 reset
Set to 1 or clear to zero by software.

Bit Field        Name       Description
0: No effect
1: Reset USART1

13               Reserved   Always read as 0.

12               TIM1RST    TIM1 timer reset
Set to 1 or clear to zero by software.
0: No effect
1: Reset TIM1

11               Reserved   Always read as 0.

10               SPI2RST    SPI2 reset
Set and clear by software.
0: Clear reset
1: reset SPI2

9                SPI1RST    SPI1 reset
Set and clear by software.
0: Clear reset
1: Reset SPI1

8:4              Reserved   Always read as 0.

3                IOPBRST    GPIO port B reset.
Set to 1 or clear to zero by software.
0: No effect
1: Reset GPIO Port B

2                IOPARST    GPIO port A reset.
Set to 1 or clear to zero by software.
0: No effect
1: Reset GPIO Port A

1                Reserved   Always read as 0.

0                AFIORST    Auxiliary function IO reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset auxiliary function IO

#### 4.3.6. APB1 Peripheral Reset Register (RCC_APB1PRST)

Offset address: 0x10
Reset value: 0x0000 0000

Bit Field   Name        Description

31:29       Reserved    Always read as 0.

28          PWRRST      PWR reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset PWR

27:22       Reserved    Always read as 0.

21          I2CRST      I2C reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset I2C1

20:18       Reserved    Always read as 0.

17          USART2RST   USART2 reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset USART2

16:12       Reserved    Always read as 0.

11          WWDGRST     Window watchdog reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset window watchdog

10:5        Reserved    Always read as 0.

4           TIM6RST     TIM6 timer reset.

Set to 1 or clear to zero by software.

Bit Field        Name       Description

0: No effect

1: Reset TIM6 timer

3:2              Reserved   Always read as 0.

1                TIM3RST    TIM3 timer reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset TIM3 timer

0                Reserved   Always read as 0.

#### 4.3.7. AHB Peripheral Clock Enable Register (RCC_AHBPCLKEN)

Offset address: 0x14

Reset value: 0x0000 0014

Bit Field        Name       Description

31:15            Reserved   Always read as 0.

14               IRCTRLEN   IRCTRL clock enable.

Set to 1 or clear to zero by software.

0: Disable IRCTRL clock

1: Enable IRCTRL clock

13               Reserved   Always read as 0.

12               ADCEN      ADC clock enable.

Set to 1 or clear to zero by software.

0: Disable ADC clock

1: Enable ADC clock

11:7             Reserved   Always read as 0.

Bit Field        Name       Description

6                CRCEN      CRC clock enable.

Set to 1 or clear to zero by software.

0: Disable CRC clock

1: Enable CRC clock

5                Reserved   It defaults to 0.

4                Reserved   Always read as 1.

3                Reserved   Always read as 0.

2                SRAMEN     SRAM clock enable.

Set to 1 or clear to zero by software.

0: Disable SRAM clock during Idle mode

1: Enable SRAM clock during Idle mode

1                Reserved   Always read as 0.

0                DMAEN      DMA clock enable.

Set to 1 or clear to zero by software.

0: Disable DMA clock

1: Enable DMA clock

#### 4.3.8. APB2 Peripheral Clock Enable Register (RCC_APB2PCLKEN)

Offset address: 0x18

Reset value: 0x0000 0000

Bit Field        Name       Description

31:15            Reserved   Always read as 0.

14               USART1EN   USART1 clock enable.

Set to 1 or clear to zero by software.

Bit Field   Name       Description

0: Disable USART1 clock

1: Enable USART1 clock

13          Reserved   Always read as 0.

12          TIM1EN     TIM1 timer clock enable.

Set to 1 or clear to zero by software.

0: DisableTIM1 timer clock

1: Enable TIM1 timer clock

11          Reserved   Always read as 0.

10          SPI2EN     SPI2 clock enable.

Set to 1 or clear to zero by software.

0: Disable SPI2 clock

1: Enable SPI2 clock

9           SPI1EN     SPI1 clock enable.

Set to 1 or clear to zero by software.

0: Turn off SPI1 clock

1: Enable SPI1 clock

8:4         Reserved   Always read as 0.

3           IOPBEN     GPIO port B clock enable.

Set to 1 or clear to zero by software.

0: Disable the clock of GPIO port B

1: Enable the clock of GPIO port B

2           IOPAEN     GPIO port A clock enable.

Set to 1 or clear to zero by software.

0: Disable the clock of GPIO port A

1: Enable the clock of GPIO port A

1           Reserved   Always read as 0.

0           AFIOEN     Auxiliary function IO clock enable.

Bit Field        Name       Description

Set to 1 or clear to zero by software.

0: Disable the auxiliary function IO clock

1: Enable the auxiliary function IO clock

#### 4.3.9. APB1 Peripheral Clock Enable Register (RCC_APB1PCLKEN)

Offset address: 0x1C

Reset value: 0x0000 0000

Bit Field        Name       Description

31:29            Reserved   Always read as 0.

28               PWREN      Power interface clock enable.

Set to 1 or clear to zero by software.

0: Disable power interface clock

1: Enable power interface clock

27:22            Reserved   Always read as 0.

21               I2CEN      I2C clock enable.

Set to 1 or clear to zero by software.

0: Disable I2C clock

1: Enable I2C clock

20:18            Reserved   Always read as 0.

17               USART2EN   USART2 clock enable.

Set to 1 or clear to zero by software.

0: Disable USART2 clock

1: Enable USART2 clock

16:12            Reserved   Always read as 0.

Bit Field        Name        Description

11               WWDGEN      Window watchdog clock enable.

Set to 1 or clear to zero by software.

0: Disable window watchdog clock

1: Enable window watchdog clock

10:5             Reserved    Always read as 0.

4                TIM6EN      TIM6 timer clock enable.

Set to 1 or clear to zero by software.

0: Disable TIM6 timer clock

1: Enable TIM6 timer clock

3:2              Reserved    Always read as 0.

1                TIM3EN      TIM3 timer clock enable.

Set to 1 or clear to zero by software.

0: Disable TIM3 timer clock

1: Enable TIM3 timer clock

0                Reserved    Always read as 0.

#### 4.3.10. Low Speed Clock Control Register (RCC_LSCTRL)

Offset address: 0x20
Reset value: 0x0006 0F03

Bit Field        Name        Description

31               Reserved    Always read as 0.

30               KEYSCANEN   KEYSCAN clock enable.

Set to 1 or clear to zero by software.

0: Disable KEYSCAN clock

Bit Field   Name        Description

1: Enable KEYSCAN clock

29          Reserved    Always read as 0.

28          LPUARTRST   LPUART reset.

Set to 1 or clear to zero by software.

0: No effect

1: Reset LPUART

27          LPUARTEN    LPUART clock enable.

Set to 1 or clear to zero by software.

0: Disable LPUART clock

1: Enable LPUART clock

26          LPUARTSEL   LPUART clock source selection bit

This bit is set and cleared by the software

0: APB1 clock is selected

1: LSI_ LSE_ CLK clock

25          RTCRST      RTC software reset

0: No effect

1: Reset RTC

24          RTCEN       RTC clock enable

Set to 1 and clear by software.

0: RTC clock disabled

1: RTC clock enabled

23:19       Reserved    Always read as 0.

18:8        LSITRIM     LSI frequency trimming control bit

7:6         Reserved    Always read as 0.

5           LSXSEL      Low speed clock source selection.

The software set the bit to select LSI_LSE_CLK clock source.

0: Select LSI oscillator

Bit Field        Name                 Description

1: Select LSE oscillator

> **Note: This bit affects the low speed clock of RTC, PWR, BLE, KEY and LPUART;**

4                LSEBP                External low speed clock oscillator bypass.

The software sets 1 bypass LSE.

0: LSE clock is not bypassed

1: LSE clock is bypassed

3                LSERD                External low speed clock oscillator is ready.

Set to 1 or clear by the hardware and indicate whether the LSE oscillator is ready. After
LSEEN is cleared, this bit has to be cleared by 6 cycles of external low-speed oscillator.

0: External 32KHz oscillator is not ready

1: External 32KHz oscillator has been ready

2                LSEEN                External low speed clock oscillator enable bit.

Set to 1 or clear to zero by software.

0: Turn off the external 32KHz oscillator

1: Turn on the external 32KHz oscillator

1                LSIRD                Internal low speed clock oscillator is ready.

Set to 1 or clear by the hardware and indicate whether the LSI oscillator is ready. After
LSIEN is cleared, This bit requires 3 periods of the internal low-speed oscillator to be
cleared

0: Internal 32KHz oscillator is not ready

1: Internal 32KHz oscillator has been ready

0                LSIEN                Internal low speed clock oscillator enable bit.

Set to 1 or clear to zero by software.

0: Turn off the internal 32KHz oscillator

1: Turn on the internal 32KHz oscillator

> **Note: If RCC_ CFG register needs to be written after Sleep mode wake-up, RCC_ LSCTRL register must be**
rewritten first, that is, read and write the read value.

#### 4.3.11. Control/Status Register (RCC_CTRLSTS)

Offset address: 0x24
Reset value: 0x00000018

Bit Field   Name       Description

31:8        Reserved   Always read as 0.

7           WWDGRSTF   Window watchdog reset flag

Set 1 when the window watchdog is reset by the hardware.

Reset and clear by writing the RMRSTF bit.

0: Without window watchdog reset

1: With window watchdog reset

6           IWDGRSTF   Independent watchdog reset flag

Set 1 when the independent watchdog is reset by the hardware.

Reset and clear by writing the RMRSTF bit.

0: Without independent watchdog reset

1: With independent watchdog reset

5           SFTRSTF    Software reset flag

Set 1 when software is reset by hardware.

Reset and clear by writing the RMRSTF bit.

0: Without software reset

1: With software reset

4           PORRSTF    POR/PDR reset flag

Set 1 when POR/PDR is reset by hardware.

Clear by writing the RMRSTF bit

0: Without POR/PDR reset

1: POR/PDR reset occurs. When this bit value is 1, other reset flags are ignored

3           PINRSTF    PIN reset flag

Set 1 when RESET pin is RESET by hardware.

Reset and clear by writing the RMRSTF bit.

Bit Field        Name        Description

0: Without RESET pin reset

1: With RESET pin reset

2:1              Reserved    Always read as 0.

0                RMRSTF      REMOVE reset flag

The reset flag is cleared by the software.

0: No effect

1: Clear these reset flags

Software clearing+cancel clear (RMRSTF writes 1 and then 0) is required

#### 4.3.12. AHB Peripheral Reset Register (RCC_AHBPRST)

Offset address: 0x28
Reset value: 0x0000 0000

Bit Field        Name        Description

31:15            Reserved    Always read as 0.

14               IRCTRLRST   IRCTRL interface reset

Set and clear by software.

0: Clear reset

1: Reset IRCTRL interface

13               Reserved    Always read as 0.

12               ADCRST      ADC interface reset

Set and clear by software.

0: Clear reset

1: Reset ADC interface

11:0             Reserved    Always read as 0.

#### 4.3.13. Clock Configuration Register 2 (RCC_CFG2)

Offset address: 0x2c
Reset value: 0x0000 0000

Bit Field        Name         Description

31               TIMCLK       TIMCLK: Timer1 clock source selection

0: APB2 frequency division clock is selected as TIM1_ CLK input clock

1: SYSCLK clock is selected as TIM1_ CLK input clock

30:10            Reserved     Always read as 0.

9:5              Reserved     Always read as 0.

4                ADCSEL       ADC CLK clock source selection.

Set to 1 or clear to zero by software.

0: Select AUDIOPLL clock as the input clock of ADC

1: Select HSE_ DIV8 clock as the input clock of ADC

> **Note: Disable clock source before modifying the ADC clock source**

3:0              Reserved     Always read as 0.

#### 4.3.14. OSCFC Control Register (OSCFCCR)

Offset address: 0x30
Reset value: 0x0000 1400

Bit Field        Name                         R/W            Description

[31:15]          Reserved                     R              Always read as 0.

[14:8]           LSI_TRIM_CFG            WR             LSI TRIM count times configuration

[7:2]            Reserved                R              Always read as 0.

[1]              HSI_CalibStart          W              Start HSI calibration, read back to 0

[0]              LSI_CalibStart          W              Start LSI calibration, read back to 0

#### 4.3.15. OSCFC Status Register (OSCFCSR)

Offset address: 0x34

Reset value: 0x0000 0000

Bit Field             Name         R/W                   Function

[31:2]                Reserved     R                     Always read as 0.

HSI count completion output flag

> **Note: After enabling the HSI_CalibStart bit, it takes 2**
[1]                   HSICNTDONE   R
microseconds until the HSI count completion flag
(HSICNTDONE) in the OSCFCSR is set.

[0]                   LSICNTDONE   R                     HSI count completion output flag

#### 4.3.16. Counter Register (OSCFCLSICNT)

Offset address: 0x38
Reset value: 0x0000 0000

Bit                   Name         R/W                   Function

[31:18]               Reserved     R                     Always read as 0.

[17:0]                LSICNT       R                     Count result of LSI clock correction

#### 4.3.17. Counter Register (OSCFCHSICNT)

Offset address: 0x3C
Reset value: 0x0000 0000

Bit Field             Name             R/W                   Description

[31:12]               Reserved         R                     Always read as 0.

[11:0]                HSICNT           R                     Count result of HSI clock correction

#### 4.3.18. DBGMCU_CR Register

Offset address: 0x44
Reset value: 0x0000 0000

Bit Field      Name              R/W           Description

[31:6]         Reserved          R             Always read as 0.

Stop SMBUS timeout mode when the core stops.

Set to 1 or clear to zero by software.
[5]            I2C1_TIMEOUT      RW
0: Same as operation in normal mode;

1: Freeze the timeout control of SMBUS.

The counter stops working when the core enters the debug state.

Set to 1 or clear to zero by software.
[4]            TIM6_STOP         RW
0: The counter of the selected timer still works normally;

1: The counter of the selected timer stops working.

The counter stops working when the core enters the debug state.
[3]            TIM3_STOP         RW
Set to 1 or clear to zero by software.

0: The counter of the selected timer still works normally;

1: The counter of the selected timer stops working.

The counter stops working when the core enters the debug state.

Set to 1 or clear to zero by software.
[2]               TIM1_STOP               RW
0: The counter of the selected timer still works normally;

1: The counter of the selected timer stops working.

The debug window watchdog stops working when the core enters
the debug state.

[1]               WWDG_STOP               RW                Set to 1 or clear to zero by software.

0: The window watchdog counter still works normally;

1: The window watchdog counter stops working.

The watchdog stops working when the core enters the debug state.

Set to 1 or clear to zero by software.
[0]               IWDG_STOP               RW
0: The watchdog counter still works normally;

1: The watchdog counter stops working.

## 5. GPIO and AFIO

### 5.1. Summary

GPIO Stands for “General Purpose Input/Output”,AFIO Stands for “Alternate Function Input/Output”.This design
supports up tp 21 GPIOs, divided into 2 groups (GPIOA/GPIOB), GPIOA have 7 pins, GPIOB has 14 pins,.GPIO
ports share pins with other peripherals,users can flexibly configure according to their needs.Each GPIO pin can
be configured by software as output (push-pull or open drain), input (with or without pull-up or pull-down) or
alternate peripheral function ports (output/input).Except for ports with analog input function, all GPIO pins have
the ability to pass through a large current.
GPIO ports have the following characteristics:
•         Each GPIO port can be individually configured into multiple modes by software
−    Input floating
−    Input pull-up
−    Input pull-down
−    Analog function
−    Open drain output and pull-up/pull-down can be configured
−    Push-pull output and pull-up/pull-down can be configured
−    Push-pull alternate function and pull-up/pull-down can be configured
−    Open-drain alternate function and pull-up/pull-down can be configured
•         Individual bit set or bit clear function

•     All I/O supports external interrupt function
•     All I/O supports low power mode wake-up, rising or falling edge configurable
−    8 EXTIs can be used to wake up from SLEEP mode, and all I/Os can be reused as EXTIs
−    PB3 wake-up I/O can be used for PD mode wake-up, the maximum I/O filter time is 1us
•     Support software remapping I/O alternate function
•     Support GPIO lock mechanism, reset the lock state to clear
Each I/O port bit can be programmed arbitrarily, but I/O port registers must be accessed as 32-bit words (16-bit
half-word or 8-bit byte access is not allowed). The following figure shows the basic structure of an I/O port.
*Figure 5-1 Basic Structure of I/O Ports*


VDD

Write
Bit set/clear register                                                                            P-MOS
Output data                                                                             VDD       VDD
register
Output
Read / Write                                                                control                    On
Off     Pull
N-MOS             up           diode
Alternate function output

From on-chip peripheral
Push-pull, open-drain or disabled             VSS                                            I/O Pin

Pull
On                    diode
down
Off
On / Off
Read                                 Input data
register                                                                              VSS       VSS

TTL Schmitt trigger
Alternate function input

To on-chip peripheral

Analog Input

### 5.2. Function Description

#### 5.2.1. I/O Mode Configuration

The mode control of I/O is set by the configuration registers GPIOx_PMODE, GPIOx_POTYPE and
GPIOx_PUPD (x=A,B). The configuration in different operation modes is shown in the following table:
*Table 5-1 I/O Port Configuration Table*


PMODE[1:0]                 POTYPE                            PUPD[1:0]                                                      I/O configuration

0                 0                         0                 General-purpose output push-pull

0                 0                         1                 General-purpose output push-pull + pull-up

0                 1                         0                 General-purpose output push-pull + pull-down

01                             0                 1                         1                 Reserved

1                 0                         0                 General-purpose output open-drain

1                 0                         1                 General-purpose output open-drain + pull-up

1                 1                         0                 General-purpose output open-drain + pull-down

PMODE[1:0]           POTYPE                PUPD[1:0]                                      I/O configuration

1          1               1          Reserved

0          0               0          Alternate function + push-pull

0          0               1          Alternate function + push-pull + pull-up

0          1               0          Alternate function + push-pull + pull-down

0          1               1          Reserved
10
1          0               0          Alternate function open-drain

1          0               1          Alternate function open-drain + pull-up

1          1               0          Alternate function open-drain + pull-down

1          1               1          Reserved

x          0               0          Input floating

x          0               1          Input pull-up
00
x          1               0          Input pull-down

x          1               1          Reserved

x          0               0          Analog

x          0               1
11
x          1               0          Reserved

x          1               1

In addition, the GPIOx_DS.DSy bit can be used to configure the high/low drive strength, and the GPIOx_SR.SRy
bit can be used to configure the high/low slew rate.
The input and output characteristics of I/O under different configurations are shown in the following table:
*Table 5-2 I/O List Of Functional Features Of The Pin*


Feature                 GPIO Input         GPIO Output                Analog Function         Alternate Function

Configuration according to
Output buffer                Disabled              Enabled                  Disabled
peripheral function

Disabled,Output is
Schmitt trigger              Enabled               Enabled                                             Enable
forced to 0

Configuration according to
PULL UP/DOWN/FLOATING                Configured            Configured                Disabled
peripheral function

Can be configured,
GPIO output 0 when                                 Can be configured, GPIO
output data is "0", high                           output 0 when output data is
OPEN DRAIN                   Disabled       resistance of GPIO              Disabled
"0", high resistance of GPIO
when "1"                                              when "1"

Feature                               GPIO Input        GPIO Output           Analog Function                Alternate Function

Can be configured,
GPIO output 0 when                                  Can be configured, GPIO
output data is "0",                               output 0 when output data is
PUSH PULL MODE                                   Disabled   GPIO output 1 when          Disabled
"0", GPIO output 1 when
output data is "1”                                     output data is "1”

Input data register (I/O status)                    Readable           Readable            Reads out 0                               Readable

Output data register(Output value)                      Invalid   Readable and written         Invalid                                 Readable

##### 5.2.1.1. Input mode

When the I / O port is configured as input mode:
•     Output buffer is disabled
•     The schmitt trigger input is activated.
•     Whether the pull-up and pull-down resistors are connected depends on the configuration of the GPIOx_PUPD register.
•     The data appearing on the I/O pins is sampled into the input data register on every APB2 clock
•     I/O status is obtained by read access to the input data register
*Figure 5-2 Input Floating / Pull-up / Pull-down Configuration Mode.*


Write                                                                                             VDD       VDD
Bit set/clear register
Output data
register                                               On
Off     Pull
Read / Write                                                                             up           diode

I/O Pin

Pull
On                    diode
down
Off
On
Read                                Input data
register                                                     VSS       VSS
TTL Schmitt trigger

##### 5.2.1.2. Output mode

When the I/O port is configured as output mode:

•     The schmitt trigger input is activated.

•     Whether the pull-up and pull-down resistors are connected depends on the configuration of the GPIOx_PUPD register

•     The output buffer is activated

−    Open drain mode: '0' on the output register activates the N-MOS, the pin outputs a low level. while '1'
on the output register puts the port in a high resistance state (PMOS is never activated).

−   Push-pull mode: '0' on the output register activates the N-MOS, the pin outputs a low level. While '1' on the
output register activates the P-MOS, the pin outputs a high level.

•   The data appearing on the I/O pins is sampled into the input data register every APB2 clock.

•   Read access to input data register to get I/O status.

•   Read access to the output data register to get the last written value.

*Figure 5-3 Output Mode*


VDD

Write                                                                                                                  VDD       VDD
Bit set/clear register
register                                                                    On
control
Off     Pull
N-MOS            up           diode
Read / Write

Push-pull or open-drain VSS                                           I/O Pin

Pull
On                    diode
down
Off
On
Read                                Input data
register                                                                         VSS       VSS

TTL Schmitt trigger

##### 5.2.1.3. Alternate function mode

When the I/O port is configured as alternate function mode:
•   The schmitt trigger input is activated.
•   Whether the pull-up and pull-down resistors are connected, depending on the configuration of the
GPIOx_PUPD register.
•   In open-drain or push-pull configuration, the output buffer is controlled by the peripheral.
•   Signal-driven output buffers for built-in peripherals.
•   At each APB2 clock cycle, the data appearing on the I/O pin is sampled into the input data register.
•   Read access to input data register to get I/O status.
•   Read access to the output data register to get the last written value.

*Figure 5-4 Alternate Function Mode*


VDD

Write
Bit set/clear register                                                                               P-MOS
Output data                                                                        VDD       VDD
register
Output
control                            On
Read / Write
Off     Pull
N-MOS            up           diode
From on-chip peripheral

Alternate function output
Push-pull or open-drain    VSS
I/O Pin

Pull
On                    diode
down
Off
On
Read                                      Input data
register                                                                         VSS       VSS

TTL Schmitt trigger
Alternate function input

To on-chip peripheral

##### 5.2.1.4. Analog function mode

When the I/O Port is configured as analog function mode:

•   The pull-up and pull-down resistors are disabled.

•   Read access to the input data register gets the value “0”.

•   The output buffer is disabled.

•   Schmitt trigger input is disabled and output value is forced to '0' (achieves zero consumption on each analog I/O pin).

*Figure 5-5 Analog Function Mode with High Impedance*


Write
Bit set/clear register
Output data                                                                                                      VDD
register

Read / Write

Analog                                                                                                                                                    diode

From on-chip peripheral
I/O Pin

diode

Off
Read                                             Input data                   0
register                                                                                                       VSS

TTL Schmitt trigger
Analog

To on-chip peripheral

#### 5.2.2. Status After Reset

During and just after reset, the alternate function mapping is not turned on, and the I / O port is configured as
analog function mode (GPIOx_PMODE.PMODEx [1:0] = 11b). However, there are the following exceptions to
the signal.

•   RESET default non-GPIO functions:
−     RESET pull-up input
•   After reset, the default configuration of the pins related to the debugging system is the SWD interface I/O configuration:
−     PA4: SWCLK is placed in input pull-down mode
−     PA5: SWDIO is placed in input pull-up mode

#### 5.2.3. Individual Bit Setting and Bit Clearing

By writing '1' to the bit to be changed in the set register (GPIOx_PBSC) and reset register (GPIOx_PBC), the
individual bit operation of the data register (GPIOx_POD) can be realized, and one or more bits can be set/reset.
The bit written with '1' is set or cleared accordingly, and the bit not written with '1' will not be changed. The
software does not need to disable interrupts, and is completed in a single APB2 write operation.

#### 5.2.4. External Interrupt /Wakeup Line

All ports have external interrupt capability, which can be configured in the EXTI module:
•   The port must be configured in input mode.
•   All ports can be configured for IDLE/STANDBY/SLEEP mode wake-up, supporting rising or
falling edge configurable.
•   PB3, can be used for PD mode wake-up
•   General purpose I/O ports are connected to 8 external interrupt/event lines as shown in Figure 6-2,
configured by registers AFIO_EXTI_CFGx.

#### 5.2.5. Alternate Function

When the port is configured as AFIO, the port are used as peripheral alternate function mode. The port bit
configuration register (GPIOx_AFL/GPIOx_AFH, GPIOx_PMODE, GPIOx_POTYPE and GPIOx_PUPD)
must be configured before use, reuse input or output is determined by the peripheral.

##### 5.2.5.1. Software remapping I/O alternate function

To expand the flexibility of alternate peripheral functions under different device packages, some peripheral
alternate functions can be remapped to other pins. Each I/O has up to 8 alternate functions (AF0~AF7). After
reset, AFSELy is selected as AF0 by default. The I/O alternate function can be remapped by software configuring
the corresponding registers (GPIOx_AFL/ GPIOx_AFH).
At this time, the alternate functions are no longer mapped to their original pins(For the I/O alternate function
of the peripheral, if it is remapped to a different pin, then the input is remapping choose one of multiple, and the
output will be connected to the remapped position, and the original position will be disconnected).
5.2.5.1.1. SWD alternate function I/O remapping
*Table 5-3 I/O List Of Functional Features Of The Pin*


Alternate Function                            I/O                                        Remapping

SWDIO                                    PA5                                            AF0

PB7                                            AF6

SWCLK                                    PA4                                            AF0

PB6                                            AF6

##### 5.2.5.2. TIMx alternate function I/O remapping


5.2.5.2.1. TIM1 alternate function I/O remapping
*Table 5-4 TIM1 Alternate Function I/O Remapping*


Alternate Function                    I/O                                  Remapping

TIM1_ETR                           PA5                                    AF1

TIM1_BKIN                          PA6                                    AF1

PB0                                    AF1
TIM1_CH1
PB8                                    AF1

PB1                                    AF1
TIM1_CH2
PB9                                    AF1

PB2                                    AF1
TIM1_CH3
PB10                                    AF1

PB3                                    AF1
TIM1_CH4
PB11                                    AF1

TIM1_CH1N                         PB12                                    AF1

TIM1_CH2N                         PB13                                    AF1

TIM1_CH3N                          PA4                                    AF1

5.2.5.2.2. TIM3 alternate function I/O remapping
*Table 5-5 TIM3 Alternate Function I/O Remapping*


Alternate Function                    I/O                                  Remapping

TIM3_CH1                           PB4                                    AF2

TIM3_CH2                           PB5                                    AF2

TIM3_CH3                           PB6                                    AF2

TIM3_CH4                           PB7                                    AF2

##### 5.2.5.3. USARTx alternate function I/O remapping

5.2.5.3.1. USART1 alternate function I/O remapping
*Table 5-6 USART1 Alternate Function I/O Remapping*


Alternate Function                    I/O                                 Remapping

PB1                                   AF3
USART1_CTS
PB9                                   AF3

PB0                                   AF3
USART1_RTS
PB8                                   AF3

PA4                                   AF3
USART1_TX
PB6                                   AF4

PA5                                   AF3
USART1_RX
PB7                                   AF4

USART1_CK                           PA6                                   AF3

5.2.5.3.2. USART2 alternate function I/O remapping
*Table 5-7 USART2 Alternate Function I/O Remapping*


Alternate Function                    I/O                                 Remapping

PB4                                   AF3
USART2_TX
PA6                                   AF2

PB5                                   AF3
USART2_RX
PB10                                  AF3

USART2_CK                           PB13                                  AF3

USART2_RTS                          PB11                                  AF3

USART2_CTS                          PB12                                  AF3

##### 5.2.5.4. LPUART alternate function I/O remapping

*Table 5-8 LPUART Alternate Function I/O Remapping*


Alternate Function                    I/O                                 Remapping

PB13                                  AF2
LPUART_CTS
PB3                                   AF4

PB10                                  AF2
LPUART_RTS
PB0                                   AF4

LPUART_TX                           PB12                                  AF2

Alternate Function                       I/O                                   Remapping

PB1                                     AF4

PB11                                    AF2
LPUART_RX
PB2                                     AF4

##### 5.2.5.5. I2C alternate function I/O remapping

*Table 5-9 I2C1 Alternate Function I/O Remapping*


Alternate Function                       I/O                                   Remapping

PB7                                     AF3
I2C_SCL
PB9                                     AF2

PB6                                     AF3
I2C_SDA
PB8                                     AF2

PB10                                    AF4
I2C_SMBA
PB11                                    AF4

##### 5.2.5.6. SPI/I2S alternate function I/O remapping


5.2.5.6.1. SPI1/I2S1 alternate function I/O remapping
*Table 5-10 SPI1/I2S Alternate Function I/O Remapping*


Alternate Function                        I/O                                Remapping

SPI1_I2S1_NSS_WS                         PA0                                   AF1

SPI1_I2S1_SCK_CK                         PA1                                   AF1

SPI1_I2S1_MISO_MCK                         PA3                                   AF1

SPI1_I2S1_MOSI_SD                         PA2                                   AF1

5.2.5.6.2. SPI2 alternate function I/O remapping
*Table 5-11 SPI2 Alternate Function I/O Remapping*


Alternate Function                         I/O                             Remapping

PB0                               AF2
SPI2_I2S2_ NSS_WS
PB7                               AF1

PB1                               AF2
SPI2_I2S2_ SCK_CK
PB4                               AF1

PB3                               AF2
SPI2_I2S2 _MISO_MCK
PB5                               AF1

PB2                               AF2
SPI2 _I2S2_MOSI_SD
PB6                               AF1

##### 5.2.5.7. IRC alternate function I/O remapping

*Table 5-12 IRC Alternate Function I/O Remapping*


Alternate Function                             I/O                          Remapping

IRC_TX                                 PB4                               AF2

•   PB4 can be used as IRC_TX or TIM3_CH1 pin(default).
•    Turn on IRC by setting the IR_ENABLE bit of the IR_CTRL register.

##### 5.2.5.8. KEYSCAN alternate function I/O remapping

*Table 5-13 KEYSCAN Alternate Function I/O Remapping*


Alternate Function                          I/O                               Remapping

KEY1                                  PA0                                 AF5

KEY2                                  PA1                                 AF5

KEY3                                  PA2                                 AF5

KEY4                                  PA3                                 AF5

KEY5                                  PA6                                 AF5

KEY6                                 PB10                                 AF5

KEY7                                  PB8                                 AF5

KEY8                                  PB9                                 AF5

KEY9                                  PA4                                 AF5

KEY10                                 PA5                                 AF5

KEY11                                 PB0                                 AF5

Alternate Function                               I/O                                       Remapping

KEY12                                       PB1                                           AF5

KEY13                                       PB2                                           AF5

##### 5.2.5.9. BLE alternate function I/O remapping

*Table 5-14 BLE Alternate Function I/O Remapping*


Alternate Function                               I/O                                       Remapping

PA_LDO_EN                                      PB3                                           AF6

ANT_SW1                                      PB3                                           AF7

ANT_SW2                                      PB6                                           AF7

ANT_SW3                                      PB7                                           AF7

ANT_SW4                                      PB1                                           AF7

ANT_SW5                                      PB2                                           AF7

ANT_SW6                                      PB4                                           AF7

ANT_SW7                                      PB5                                           AF7

##### 5.2.5.10. RCC alternate function I/O remapping

*Table 5-15 RCC Alternate Function I/O Remapping*


Alternate Function                               I/O                                       Remapping

MCO                                        PB5                                           AF4

##### 5.2.5.11. OSC32K_IN/OSC32K_OUT alternate function I/O remapping

*Table 5-16 OSC32_IN/OSC32_OUT Alternate Function I/O Remapping*


Alternate Function                               I/O                                       Remapping

OSC32_IN                                     PB8                                           AF0

OSC32_OUT                                     PB9                                           AF0

PB8~PB9 two pins can be used as LSE crystal/external clock mode or other alternate function mode:
•     PB8 and PB89 can be used for LSE (OSC32_IN, OSC32_OUT) pins.
•     Turn on the LSE function by setting the RCC_LSCTRL.LSEEN and RCC_LSCTRL.LSEBP bits.
*Table 5-17 PB8/PB9 Alternate Function Remapping*


PB8 and PB9                                      Condition                                  PAD Mode Configure

LSE crystal mode           RCC_LSCTRL.LSEEN bit is enabled, alternate mode is on              Analog function mode

RCC_LSCTRL.LSEEN bit is disabled, RCC_LSCTRL.LSEBP is
LSE external clock mode               enabled, alternate mode is enabled                      Analog function mode(PB8)

It can only be used in GPIO mode when the LSE is turned off
Other function mode        and the VDDD power supply is turned off without entering the   The mode of the other function is

low power mode (PD).                      determined by the application

The default is analog function mode; PB8 and PB9 decide which mode and I/O function they are in
according to RCC_LSCTRL.LSEEN, RCC_LSCTRL.LSEBP, chip mode signal, GPIOx_PMODE,
GPIOx_POTYPE and GPIOx_PUPD.

#### 5.2.6. I/O Configuration of Peripherals

*Table 5-18 ADC*


ADC                                             PAD Configuration

ADC                                            Analog function mode

*Table 5-19 TIM1*


TIM1 Pin                          Configuration                    PAD Configuration Mode

Channel x input capture                 Input floating
TIM1_CHx
Output compare channel x             Alternate function push-pull

TIM1_CHxN                  Complementary output channel x          Alternate function push-pull

TIM1_BKIN                            Brake input                           Input floating

TIM1 ETR                    External trigger clock input                  Input floating

*Table 5-20 TIM3*


TIM3 Pin                          Configuration                    PAD configuration mode

Input capture channel x                     Input floating
TIM3_CHx
Output compare channel x             Alternate function push-pull

*Table 5-21 USART*


USART Pin                         Configuration                       PAD Configuration

Full duplex mode                 Alternate function push-pull
USART_TX
Half duplex mode                 Alternate function push-pull

Full duplex mode                 Input floating or Input pullup
USART_RX
Half duplex mode              Unused, can be used as general I/O.

USART_CK                         Synchronous mode                  Alternate function push-pull

USART_RTS                      Hardware flow control               Alternate function push-pull

USART_CTS                      Hardware flow control               Input floating or Input pullup

*Table 5-22 LPUART*


LPUARTx Pin                         Configuration                       PAD Configuration

LPUART_TX                         Full duplex mode                 Alternate function push-pull

LPUARTx Pin                 Configuration                              PAD Configuration

LPUART_RX                  Full duplex mode                        Input floating or Input pullup

LPUART_RTS              Hardware flow control                       Alternate function push-pull

LPUART_CTS              Hardware flow control                      Input floating or Input pullup

*Table 5-23 I2C*


I2C Pin                    Configuration                             PAD Configuration

I2C_SCL                      I2C clock                           Alternate function open-drain

I2C_SDA                       I2C data                           Alternate function open-drain

I2C_SMBA                     SMBA data                             Alternate function push-pull

*Table 5-24 SPI*


SPI Pin                   Configuration                              PAD Configuration

Master mode                            Alternate function push-pull
SPIx_SCLK
Slave mode                                   Input floating

Full duplex mode / Master mode                   Alternate function push-pull

Input floating or Input pullup or Alternate
Full duplex mode / Slave mode
function push-pull
SPIx_MOSI
Simple bidirectional data line / Master mode          Alternate function push-pull

Simple bidirectional data line / Slave mode       Unused, can be used as general I/O.

Input floating or Input pullup or Alternate
Full duplex mode / Master mode
function push-pull

Full duplex mode / Slave mode                   Alternate function push-pull
SPIx_MISO
Simple bidirectional data line / Master mode      Unused, can be used as general I/O.

Simple bidirectional data line / Slave mode           Alternate function push-pull

Alternate function push-pull(no pull-down
Hardware Master/ Slave mode
or pull-up/pull-up/pull-down)

Alternate function push-pull（When acting
SPIx_NSS
Hardware Master /NSS output enable           as the master, NSS can choose idle high
impedance or idle as 1）

Software mode                       Unused, can be used as general I/O.

*Table 5-25 IIS*


IIS Pin                  Configuration                              PAD Configuration

Master mode                           Alternate function push-pull
IISx_WS
Slave mode                                  Input floating

Master mode                           Alternate function push-pull
I2Sx_CK
Slave mode                                  Input floating

Transmitter                          Alternate function push-pull
I2Sx_SD
Input floating or Input pullup or Input
Receiver
pulldown

Master mode                           Alternate function push-pul
I2Sx_MCK
Slave mode                       Unused, can be used as general I/O.

*Table 5-26 IRC*


IRC                                      PAD Configuration

IRC_TX                                  Alternate function push-pull

*Table 5-27 KEYSCAN*


KEYSCAN                                        PAD Configuration

KEYx                 Input pullup or Input pulldown or Alternate function push-pull

*Table 5-28 BLE*


BLE                                       PAD Configuration

PA_LDO_EN                                   Alternate function push-pull

ANT_SWx                                    Alternate function push-pull

*Table 5-29 Other*


Pin                 Alternate function                            PAD configuration

MCO                      Clock output                          Alternate function push-pull

Input floating or input with pull-up or input
EXTI input line         External interrupt input
with pull-down

#### 5.2.7. GPIO Locking Mechanism

The locking mechanism is used to freeze the I/O configuration to prevent accidental changes. When a lock
(LOCK) procedure is executed on a port bit, the configuration of the port cannot be changed until the next
reset, refer to the port configuration lock register GPIOx_PLOCK.
•   PLOCKK is GPIOx_PLOCK[16],only after the PLOCKK is operated in the correct sequence w1->w0-
>w1->r0 (where r0 must be), it will become 1; after that, it will only become 0 after a system reset.
•   PLOCKy is GPIOx_PLOCK[15:0] ,can only be modified when PLOCKK = 0
•   PLOCKK is only written simultaneously with non-zero GPIOx_PLOCK[15:0], the sequence w1->w0-
>w1->r0 is valid; During the sequence writing process, GPIOx_PLOCK[15:0] remains unchanged.
•   As long as PLOCKK = 0, the bits of GPIOx_PMODE / GPIOx_POTYPE / GPIOx_PUPD /
GPIOx_AFL / GPIOx_AFH can be modified, they are not affected by GPIOx_PLOCK.PLOCK[15:0]
configuration.
•   PLOCKK = 1, GPIOx_PMODE/GPIOx_POTYPE/GPIOx_PUPD/GPIOx_AFL/GPIOx_AFH are controlled
by GPIOx_PLOCK[15:0]. Corresponding to PLOCKy (y = 0…15) = 1, it is a lock configuration and cannot
be modified; PLOCKy = 0, it can be modified.
•   If the lock sequence operation is wrong, then it must be redone (w1-> w0-> w1-> r0) to initiate the lock
operation again.

### 5.3. GPIO Registers

These peripheral registers must be operated as 32-bit words.

#### 5.3.1. GPIO Register Overview

*Table 5-30 GPIO Register Overview*


Of       Register
fs
et

00      GPI    x=
PMODE13[1:0]

PMODE12[1:0]

PMODE11[1:0]

PMODE10[1:0]

PMODE9[1:0]

PMODE8[1:0]

PMODE7[1:0]

PMODE6[1:0]

PMODE5[1:0]

PMODE4[1:0]

PMODE3[1:0]

PMODE2[1:0]

PMODE1[1:0]

PMODE0[1:0]
Reserved

Reserved

0h      Ox_    A,
PM      B
ODE

Res    x=   0      0       0      0       0        0         0        0         0        0         0        0         0       0         0        0        0       0         1       1         1       0         1       0         1       1         1       1         1       1         1       1
et     A
Valu
e     x=   0      0       0      0       1        1         1        1         1        1         1        1         1       1         1        1        1       1         1       1         1       1         1       1         1       1         1       1         1       1         1       1
B

00      GPI    x=
Reserved

4h      Ox_    A,
POT     B
YPE

Res    x=   0      0       0      0       0        0         0        0         0        0         0        0         0       0         0        0        0       0         0       0         0       0         0       0         0       0         0       0         0       0         0       0
et     A
Valu
e     x=   0      0       0      0       0        0         0        0         0        0         0        0         0       0         0        0        0       0         0       0         0       0         0       0         0       0         0       0         0       0         0       0
B

00      GPI    x=
Reserved

8h      Ox_    A,
SR     B

Res    x=   0      0       0      0       0        0         0        0         0        0         0        0         0       0         0        0        0       0         0       1         1       1         1       1         1       1         1       1         1       1         1       1
et     A
Valu
e     x=   0      0       0      0       0        0         0        0         0        0         0        0         0       0         0        0        0       0         1       1         1       1         1       1         1       1         1       1         1       1         1       1
B

00      GPI    x=
PUPD13[1:0]

PUPD12[1:0]

PUPD11[1:0]

PUPD10[1:0]

PUPD9[1:0]

PUPD8[1:0]

PUPD7[1:0]

PUPD6[1:0]

PUPD5[1:0]

PUPD4[1:0]

PUPD3[1:0]

PUPD2[1:0]

PUPD1[1:0]

PUPD0[1:0]
Reserved

Reserved

C       Ox_    A,
h     PUP     B
D

Res    x=   0      0       0   0   0   0   0   0   0   0   0   1   0   1   0   1   0   0   0   0   0   1   1   0   0   0   0   0   0   0   0   0
et    A
Valu
e     x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
B

01   GPI    x=
Reserved

0h   Ox_    A,
PID    B

Res    x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
et    A
Valu
e     x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
B

01   GPI    x=
Reserved

4h   Ox_    A,
POD    B

Res    x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
et    A
Valu
e     x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
B

01   GPI    x=
8h   Ox_    A,
PBS    B
C

Res    x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
et    A
Valu
e     x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
B

01   GPI    x=
Reserved

C    Ox_    A,
h    PLO    B
CK

Res    x=   0      0       0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
et    A

Valu
x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
e
B

AFSEL7[3:0]

AFSEL6[3:0]

AFSEL5[3:0]

AFSEL4[3:0]

AFSEL3[3:0]

AFSEL2[3:0]

AFSEL1[3:0]

AFSEL0[3:0]
02   GPI    x=
0h   Ox_    A,
AFL    B

Res    x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
et    A
Valu
e     x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
B                                                               AFSEL13[3:0]

AFSEL12[3:0]

AFSEL11[3:0]

AFSEL10[3:0]

AFSEL9[3:0]

AFSEL8[3:0]
02   GPI    x=
Reserved

Reserved

4h   Ox_    B
AFH

Res    x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
et    B
Valu
e

02   GPI    x=
Reserved

DS13

DS12

DS11

DS10

DS9

DS8

DS7

DS6

DS5

DS4

DS3

DS2

DS1

DS0
8h   Ox_    A,
PBC    B

Res    x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
et    A
Valu
e     x=   0       0         0   0   0       0         0    0     0        0         0    0     0        0         0    0    0        0         0    0    0        0         0    0    0       0         0    0    0       0         0    0
B

02   GPI    x=
Reserved

C    Ox_    A,
h    DS     B

Res    x=   0       0         0   0   0       0         0    1     0        1         0    1     0        1         0    1    0        1         0    1    0        1         0    1    0       1         0    1    0       1         0    1
et    A
Valu
e     x=   0       0         0   0   0       1         0    1     0        1         0    1     0        1         0    1    0        1         0    1    0        1         0    1    0       1         0    1    0       1         0    1
B

#### 5.3.2. GPIO Port Mode Description Register (GPIOx_PMODE)


Offset address : 0x00
Reset value : 0x00003AFF (x=A); 0x0FFF FFFF (x=B)

Bit Field             Name          Description

31:30                 PMODEy[1:0]   Mode of port GPIOx (x = A,B) pin PINy:

29:28                               00: Input mode

27:26                               01: General output mode

25:24                               10: Alternate function mode

23:22                               11: Analog function mode

21:20                               Note: when x = A, y = 0…6;

19:18                               When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;
17:16

15:14

13:12

11:10

9:8

7:6

5:4

3:2

1:0

#### 5.3.3. GPIO Port Type Definition (GPIOx_POTYPE)


Offset address : 0x04
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name        Description

31:16                  Reserved    Reserved,the reset value must be maintained

15:0                   POTy        Output type of port GPIOx (x = A,B) pin PINy:

0: Output push-pull mode (state after reset)

1: Output open-drain mode

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;

#### 5.3.4. GPIO Slew Rate Configuration Register (GPIOx_SR)

Offset address : 0x08
Reset value : 0x0000 1FFF(x= A); 0x0000 3FFF(x=B)

Bit Field              Name           Description

31:16                  Reserved       Reserved,the reset value must be maintained

15:0                   SRy            Slew rate configuration bits for port GPIOx (x = A,B) pin PINy

0：Fast slew rate

1：Slow slew rate

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;

#### 5.3.5. GPIO Port Pull-up/Pull-down Register (GPIOx_PUPD)

Offset address : 0x0C
Reset value : 0x0015 0600(x= A); 0x0000 0000(x= B)

Bit Field              Name           Description

31:30                  PUPDy[1:0]     Pull-up and pull-down mode of port GPIOx (x = A,B) pin PINy:

29:28                                 00: no pull-up/pull-down

Bit Field              Name        Description

27:26                              01: Pull up

25:24                              10: Pull down

23:22                              11: Reserved

21:20                              Note: when x = A, y = 0…6;

19:18                              When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;
17:16

15:14

13:12

11:10

9:8

7:6

5:4

3:2

1:0

#### 5.3.6. GPIO Port Input Data Register (GPIOx_PID)

Offset address : 0x10
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name        Description

31:16                  Reserved    Reserved,the reset value must be maintained

15:0                   PIDy        Input data of port GPIOx (x = A,B) pin PINy

These bits are read-only, and the read value is the state of the corresponding
I/O port.

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits
are read-only;

#### 5.3.7. GPIO Port Output Data Register (GPIOx_POD)

Offset address : 0x14
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name        Description

31:16                  Reserved    Reserved,the reset value must be maintained

15:0                   PODy        Output data of port GPIOx (x = A,B) pin PINy

These bits are readable or writable by software, and the corresponding POD bits can
be independently set/cleared.

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;.

#### 5.3.8. GPIO Port Bit Set/Clear Register (GPIOx_PBSC)

Offset address : 0x18
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name        Description

31:14                  Reserved    Reserved,the reset value must be maintained

13:0                   PBSy        Set bit y of port GPIOx (x = A,B)

These bits can only be written.

0: Does not affect the corresponding PODy bit

1: Set the corresponding PODy bit to 1

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;

#### 5.3.9. GPIO Port Configuration Lock Register (GPIOx_ PLOCK)

Offset address : 0x1C
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name        Description

31:17                  Reserved    Reserved,the reset value must be maintained

16                     PLOCKK      Lock key

This bit can be read at any time, it can only be modified by the lock key write
sequence.

0: Port configuration lock key is not active

1: The port configuration lock key bit is activated, and the GPIOx_PLOCK register is
locked before the next system reset.

Lock key write sequence:

write 1 -> write 0 -> write 1 -> read 0 -> (read 1)

The last read 1 can be omitted, but can be used to confirm that the lock key has been
activated.

> **Note: The value of PLOCK[15:0] cannot be changed while operating the lock key**
write sequence. Any errors in the operation lock key write sequence will not activate
the lock key.

15:0                   PLOCKy      Configuration lock bit for port GPIOx (x = A,B) pin PINy

These bits are readable and writable but can only be written when the PLOCKK bit is
0.

0: Do not lock the configuration of the port

1: Lock the configuration of the port

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…13, the remaining bits are reserved, and the reserved bits are
read-only;

#### 5.3.10. GPIO Alternate Function Low Register (GPIOx_AFL)

Offset address : 0x20
Reset value : 0x00000000 (x = A,B)

Bit Field              Name          Description

31:28                  AFSELy[3:0]   Alternate function configuration bits for port GPIOx (x = A,B) pins PINy (y = 0…7)

27:24                                0000: AF0

23:20                                0001: AF1

19:16                                0010: AF2

15:12                                0011: AF3

11:8                                 0100: AF4

7:4                                  0101: AF5

3:0                                  0110: AF6

0111: AF7

Other:reserved

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…7,the remaining bits are reserved, and the reserved bits are read-
only;

#### 5.3.11. GPIO Alternate Function High Register (GPIOx_AFH)

Offset address : 0x24
Reset value : 0x00000000 (x = B)

Bit Field              Name          Description

31:28                  AFSELy[3:0]   Alternate Function Configuration Bits for Port GPIOx (x = B) Pins PINy (y = 8…13)

27:24                                0000: AF0

23:20                                0001: AF1

19:16                                0010: AF2

15:12                                0011: AF3

11:8                                 0100: AF4

7:4                                  0101: AF5

3:0                                  0110: AF6

0111: AF7

> **Note: when x = B, y = 8…13;**

#### 5.3.12. GPIO Port Bit Clear Register (GPIOx_PBC)

Offset address : 0x28
Reset value : 0x0000 0000 (x=A,B)

Bit Field              Name          Description

31:16                  Reserved      Reserved,the reset value must be maintained

15:0                   PBCy          Clear bit y of port GPIOx (y = 0…15)

These bits can only be written.

0: No effect on the corresponding PODy bit

1: Clear the corresponding PODy bit to 0

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…7,the remaining bits are reserved, and the reserved bits are read-
only;

#### 5.3.13. GPIO Driver Strength Configuration Register (GPIOx_DS)

Offset address : 0x2C
Reset value : 0x0155 5555(x= A), 0x05555555(x= B)

Bit Field              Name          Description

31:16                  Reserved      Reserved,the reset value must be maintained

15:0                   DSy           Drive capability configuration bits for port GPIOx (x = A,B) pins PINy(y = 0…13)

00: 2mA drive capability

10: 4mA drive capability

01: 8mA drive capability

11: 12mA drive capability

> **Note: when x = A, y = 0…6;**

When x = B, y = 0…7,the remaining bits are reserved, and the reserved bits are read-
only;

### 5.4. AFIO Registers


#### 5.4.1. AFIO Register Overview

*Table 5-31 AFIO Register Overview*


O
R
f
e
f
gi
s
st
e
er
t

A
FI
O
Reserved

Reserved
_
C
F
0   G
0
0   R

h   e
s
et
0   0   0   0   0   0   0   0   0   0       0      0   0   0    0   0     0   0       0   0   0   0   0   0   0   0   0          0   0   0   0
V
al
u
e

0

#### 5.4.2. AFIO Configuration Register (AFIO_CFG)

Offset address : 0x00
Reset value : 0x0000 0000

Bit Field              Name                          Description

31:12                  Reserved                      Reserved,the reset value must be maintained

11                     SPI1_NSS                      NSS mode selection bit of SPI1 (NSS is configured in AFIO push-pull
mode).

0: NSS is high impedance when idle

1: NSS is high level when idle

10                     SPI2_NSS                      NSS mode selection bit of SPI2 (NSS is configured in AFIO push-pull
mode).

0: NSS is high impedance when idle

1: NSS is high level when idle

9                      BOOT_REG                      Indicates if the BOOT pin is tied to 0 or 1

8:0                    Reserved                      Reserved,the reset value must be maintained

#### 5.4.3. AFIO External Interrupt Configuration Register 1 (AFIO_EXTI_CFG1)

Offset address : 0x04
Reset value : 0x0000 0000

Bit Field                Name             Description

31:14                    Reserved         Reserved,the reset value must be maintained

13:12                    EXTI3_CFG[1:0]   EXTI3 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI3 external interrupt.

Bit Field              Name             Description

00: PA[5] pin
01: PB[3] pin
10: Reserved
11: Reserved

11:10                  Reserved         Reserved,the reset value must be maintained

9:8                    EXTI2_CFG[1:0]   EXTI2 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI2 external interrupt.

00: PA[6] pin

01: PB[2] pin

10: Reserved

11: Reserved

7:6                    Reserved         Reserved,the reset value must be maintained

5:4                    EXTI1_CFG[1:0]   EXTI1 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI1 external interrupt.

00: PA[2] pin

01: PA[3] pin

10: PB[1] pin

11: Reserved

3:2                    Reserved         Reserved,the reset value must be maintained

1:0                    EXTI0_CFG[1:0]   EXTI0 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI0 external interrupt.

00: PA[0] pin

01: PA[1] pin

10: PB[0] pin

11: Reserved

#### 5.4.4. AFIO External Interrupt Configuration Register 2 (AFIO_EXTI_CFG2)

Offset address : 0x08
Reset value : 0x0000 0000

Bit Field   Name             Description

31:14       Reserved         Reserved,the reset value must be maintained

13:12       EXTI7_CFG[1:0]   EXTI7 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI7 external interrupt.

00: PB[13] pin

01: PB[7] pin

10: PB[8] pin

11: Reserved

11:10       Reserved         Reserved,the reset value must be maintained

9:8         EXTI6_CFG[1:0]   EXTI6 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI6 external interrupt.

00: PB[12] pin
01: PB[6] pin
10: PA[4] pin
11: Reserved

7:6         Reserved         Reserved,the reset value must be maintained

5:4         EXTI5_CFG[1:0]   EXTI5 configuration

These bits are readable and writable by software and are used to select the input
source for the EXTI5 external interrupt.

00: PB[10] pin

01: PB[11] pin

10: PB[5] pin

11: Reserved

3:2         Reserved         Reserved,the reset value must be maintained

1:0         EXTI4_CFG[1:0]   EXTI4 configuration

Bit Field   Name   Description

These bits are readable and writable by software and are used to select the input
source for the EXTI4 external interrupt.

00: PB[9] pin

01: PB[4] pin

10: Reserved

11: Reserved

## 6. Interrupts and Events

### 6.1. Nested Vectored Interrupt Controller

Features
■    32 maskable interrupt channels (excluding 16 Cortex®-M0 interrupt lines);
■    4 programmable priorities (using 2-bit interrupt priorities);
■    Low-latency exception and interrupt handling;
■    Power management control;
■    The realization of system control register;
The Nested Vector Interrupt Controller (NVIC) is tightly coupled with the processor core, enabling low-latency
interrupt handling and efficient handling of late interrupts. The Nested Vector Interrupt Controller manages
interrupts including core exceptions.

#### 6.1.1. SysTick Calibration Value Register

The system tick calibration value is fixed at 8000. When the system tick clock is set to 8MHz (the maximum
value of HCLK/8), 1 ms time reference is generated.

#### 6.1.2. Interrupt and Exception Vectors

*Table 6-1 Vector Table*


Position   Priority    Priority Type                Name                             Description                Address

-          -              -          -                              Reserved                                0x0000 0000

-          -3           Fixed        Reset                          Reset                                   0x0000 0004

-          -2           Fixed        NMI                            Non-maskable interrupt,BOR interrupt    0x0000 0008

-          -1           Fixed        HardFault                      All types of errors (fault)             0x0000 000C

System services invoked by SWI
-          3          Settable       SVCall                                                                 0x0000 002C
directives

System service requests that can be
-          5          Settable       PendSV                                                                 0x0000 0038
pending

-          6          Settable       SysTick                        System tick timer                       0x0000 003C

0          7          Settable       WWDG                           Window watchdog interrupt               0x0000 0040

BLE SW triggers interrupts, generated
1          8          Settable       BLE_SW_IRQ                                                             0x0000 0044
based on SW requests

RTC interrupt connected to EXTI lines
2          9          Settable       RTC                                                                    0x0000 0048
8 and 9

Settable                                      BLE half-slot reference interrupts,
3         10                         BLE_HSLOT_IRQ                                                          0x0000 004C
generated every 312.5us in active

Position   Priority   Priority Type              Name                             Description                  Address

mode

4         11         Settable      FLASH                         Flash global interrupt                    0x0000 0050

5         12         Settable      RCC                           RCC global interrupt                      0x0000 0054

6         13         Settable      EXTI0_1                       The EXTI line [1:0] interrupt             0x0000 0058

7         14         Settable      EXTI2_3                       The EXTI line [3:2] interrupt             0x0000 005C

8         15         Settable      EXTI4_12                      The EXTI line [12:4] interrupt            0x0000 0060

Settable                                    BLE fine target timer interrupt, value
9         16                       BLE_FINETGT_IRQ                                                         0x0000 0064
set by register

10         17         Settable      BLE-FIFO_IRQ                  BLE FIFO interrupt                        0x0000 0068

11         18         Settable      DMA_CH1_2_3_4                 DMA channel 1/2/3/4 interrupt             0x0000 006C

12         19         Settable      DMA_CH5                       DMA channel 5 interrupt                   0x0000 0070

TIM1 brakes, updates, triggers and
13         20         Settable      TIM1_BRK_UP_TRG_COM                                                     0x0000 0074
communication interrupt

14         21         Settable      TIM1_CC                       TIM1 capture comparison interrupt         0x0000 0078

15         22         Settable      -                             Reserved                                  0x0000 007C

16         23         Settable      TIM3                          TIM3 global interrupt                     0x0000 0080

Settable                                    BLE error interrupt, generated when
the CPU and RW_BT-LE systems
17         24                       BLE_ERROR_IRQ                                                           0x0000 0084
attempt to access the same memory
space at the same time

BLE encryption/decryption interrupt,
18         25         Settable      BLE_CRYPT_IRQ                 generated when encryption/decryption      0x0000 0088
terminates, and controlled by register

BLE timestamp target 1 interrupt,
BLE_TIMESTAMP_TGT1
19         26         Settable                                    generated at a defined instant, with an   0x0000 008C
_IRQ
accuracy of 0.5us

20         27         Settable      TIM6                          /TIM6 global interrupt                    0x0000 0090

21         28         Settable      ADC                           ADC global interrupt                      0x0000 0094

Position   Priority   Priority Type                 Name                            Description                    Address

22         29          Settable      SPI2_I2S2                       SPI2_I2S2 global interrupt                0x0000 0098

23         30          Settable      I2C                             I2C global interruption                   0x0000 009C

BLE timestamp target 1 interrupt,
BLE_TIMESTAMP_TGT2_I
24         31          Settable                                      generated at a defined instant, with an   0x0000 00A0
RQ
accuracy of 0.5us

25         32          Settable      SPI1_I2S1                       SPI1_I2S1 global interrupt                0x0000 00A4

BLE sleep mode interrupt, generated
at a predetermined wake-up time
26         33          Settable      BLE_SLP_IRQ                                                               0x0000 00A8
(connected to the EXTI line

11）

27         34          Settable      KEYSCAN                         KEYSCAN interrupt                         0x0000 00AC

28         35          Settable      USART1                          USART1 global interrupt                   0x0000 00B0

LPUART global interrupt (connected
29         36          Settable      LPUART                                                                    0x0000 00B4
to EXTI line 10)

30         37          Settable      USART2                          USART2 global interrupt                   0x0000 00B8

31         38          Settable      IRC                             IRC global interrupt                      0x0000 00BC

### 6.2. External Interrupt/Event Controller (EXTI)

#### 6.2.1. Introduction

The external interrupt/event controller contains 14 edge detection circuits that generate interrupt/event triggers.
Each input line can be independently configured with pulse or pending input types, and three trigger event types
including rising edge, falling edge or double edge, which can also be independently shielded. Interrupt requests
that hold the state line in the pending register can be cleared by writing '1' in the corresponding bit of the pending
register.

#### 6.2.2. Main Fatures

The main features of EXTI controller are as follows:
•       Supports 14 software interrupt/event requests
•       Interrupts/events corresponding to each input line can be configured to trigger or mask independently
•       Each interrupt line has an independent state bit
•       Support for pulse or pending input types
•       3 trigger events are supported: rising edge, falling edge, and double edge
•       Can wake up to exit low power mode

*Figure 6-1 Extenal Interrupt/Event Controller Block Diagram*


AMBA APB BUS

Peripheral Interface                                                 PCLK2

32                   32                32                  32               3
2

Falling               Rising          Software
Interrupt        Pending
trigger              trigger         interrupt
mark           request
seletion             seletion           event
register        register
register             register          register

14                  14             14                                 14          14          To NVIC interrupt
controller

14

Input                                                                                                     Pulse
Edge detect circuit                                                                 generator
line
14                                              14                       14

Event
mask
register

#### 6.2.3. Functional Description

EXTI contains 14 interrupts, 8 from I/O pins and 6 from internal modules. To generate interrupts, the NVIC interrupt
channel of the external interrupt controller must be configured to enable the appropriate interrupt line. Select rising edge,
falling edge, or double edge trigger event types by edge trigger configuration registers EXTI_RT_CFG and EXTI_FT_CFG,
and write '1' to the corresponding bit of interrupt masking register EXTI_IMASK to allow interrupt requests. When a preset
edge trigger polarity is detected on the external interrupt line, an interrupt request is generated and the corresponding
pending bit is set to '1'. Writing '1' to the corresponding bit of the pending register clears the interrupt request.
To generate events, the corresponding event line must be configured and enabled. According to the desired edge detection
polarity, set up the rise/fall edge trigger configuration register, while writing '1' in the corresponding bit of the event
masking register to allow interrupt requests. When a preset edge occurs on an event line, an event request pulse is generated
and the corresponding pending bit is not set to '1'.
In addition, interrupt/event requests can also be generated by software by writing a '1' in the software interrupt/event register.
•    Hardware interrupt configuration, select and configure 14 lines as interrupt sources as required:
−    Configure the mask bit (EXTI_IMASK) for 14 interrupt lines.
−    Configure the selected interrupt line trigger configuration bits (EXTI_RT_CFG and EXTI_FT_CFG);
−    Configure the enable and mask bits of the NVIC interrupt channel corresponding to the external interrupt
controller so that the requests in the 14 interrupt lines can be correctly responded to.
•    Hardware event configuration: Select 13 lines as event sources as required:
−    Configure the mask bit (EXTI_EMASK) for 13 event lines.
−    Configure the trigger configuration bits for the selected event line (EXTI_RT_CFG and EXTI_FT_CFG).
•    Software interrupt/event configuration, select 13 lines as software interrupt/event lines as required:

−     Configure 13 interrupt/event line mask bits (EXTI_IMASK and EXTI_EMASK).
−     Configure the request bit of the software interrupt event register (EXTI_SWIE).

#### 6.2.4. EXTI line mapping

*Figure 6-2 External Interrupt Generic I/O Mapping*


EXTI0_CFG[1:0]contro l           EXTI1_CFG[1:0]contro l               EXTI2_CFG[1:0] contro l               EXTI3_CFG[1:0]contro l

PA0                                 PA2

AFIO_EXTI_CFG1    PA1                                 PA3                              PA6                                   PA5
EXTI0                            EXTI1                                EXTI2                                 EXTI3
registe r                                       PB1                              PB2
PB0
PB3

EXTI4_CFG[1:0]contro l           EXTI5_CFG[1:0]contro l               EXTI6_CFG[1:0] contr ol           EXTI7_CFG[1:0]contro l

AFIO_EXTI_CFG2                                         PB10                             PB12                                  PB13
registe r                               EXTI4                            EXTI5                                 EXTI6                                EXTI7
PB9                                 PB11                             PB6                                   PB7

PB4                                 PB5                                                                    PB8
PA4

To configure external interrupts/events on the GPIO line using AFIO_EXTI_CFG1~2, the AFIO clock must
be enabled first. general I/O ports are connected to 8 external interrupt/event lines as shown above. The
connection mode of the other 6 EXTI lines is as follows:
•       EXTI line 8 is connected to the RTC alarm wake up event
•       EXTI line 9 is connected to the RTC wake up event
•       EXTI line 10 is connected to the LPUART wake up event
•       EXTI line 12 is connected to the RESET wake up event,only rising edge triggering is supported
•       EXTI line 13 is connected to the KEYSCAN wake up event

### 6.3. EXTI Registers

EXTI base address: 0x40010400

#### 6.3.1. EXTI register overview

*Table 6-2 EXTI Register Overview*


Offse       Register
31

30

29

28

27

26

25

24

23

22

21

20

19

18

17

16

15

t

EXTI_IMASK
000h                                                                                                          Reserved
Reset Value

EXTI_EMAS
K
004h                                                                                                          Reserved
Reset Value

Offse     Register

31

30

29

28

27

26

25

24

23

22

21

20

19

18

17

16

15
t

EXTI_RT_CF
G
008h                                                                       Reserved
Reset Value

EXTI_FT_CF
G
00Ch                                                                        Reserved
Reset Value

EXTI_SWIE
010h                                                                       Reserved
Reset Value

EXTI_PEND
014h                                                                       Reserved
Reset Value

Offse     Register
31

30

29

28

27

26

25

24

23

22

21

20

19

18

17

16

15
t

000h EXTI_IMASK                                                            Reserved

#### 6.3.2. Interrupt Mask Register(EXTI_IMASK)

Address offset : 0x00

Reset value : 0x00000000

Bit Field                 Name            describe

31:14                     Reserved        Reserved,the reset value must be maintained.

13:0                      IMASKx          Interrupt mask on line x.

0: Mask the interrupt request from line x;

1: open the interrupt request from line x

#### 6.3.3. Event Mask Register(EXTI_EMASK)

Address offset : 0x04

Reset value : 0x00000000

Bit Field                 name       describe

31:14                     Reserved   Reserved,the reset value must be maintained.

13:0                      EMASKx     Event masking on line x.

0: Masking the event request from line x;

1: open the event request from line x

#### 6.3.4. Rising Edge Trigger Selection Register(EXTI_RT_CFG)

Address offset : 0x08

Reset value : 0x00000000

Bit Field                 Name       Describe

31:14                     Reserved   Reserved,the reset value must be maintained.

13:0                      RT_CFGx    The rising edge trigger configuration bit.from line x

0: Disable rising edge trigger on input line x (interrupts and events).

1: Allow rising edge trigger on input line x (interrupts and events) .

#### 6.3.5. Falling Edge Trigger Selection Register(EXTI_FT_CFG)

Address offset : 0x0C

Reset value : 0x00000000

Bit Field              Name       Describe

31:14                  Reserved   Reserved,the reset value must be maintained.

13:0                   FT_CFGx    The falling edge on line x triggers the configuration bit.

0: Disable falling edge trigger on input line x. (interrupts and events)

1: Allow falling edge trigger on input line x (interrupts and events)

#### 6.3.6. Software Interrupt Enable Register(EXTI_SWIE)

Address offset : 0x10

Reset value : 0x00000000

Bit Field              Name       Describe

31:14                  Reserved   Reserved,the reset value must be maintained.

13:0                   SWIEx      Software interrupt on line x

When the bit is' 0 ', writing '1' sets the corresponding pending bit in EXTI_PEND. If
this interrupt is allowed in EXTI_IMASK and EXTI_EMASK, an interrupt will be
generated.

> **Note: This bit can be cleared to '0' by writing '1' to clear the corresponding bit of**
EXTI_PEND.

#### 6.3.7. Interrupt Request Pending Register (EXTI_PEND)

Address offset : 0x14

Reset value : 0x00000000

Bit Field              Name              Describe

31:14                  Reserved          Reserved,the reset value must be maintained.

13:0                   PENDx             Pending bit on line x.

0: No pending request occurred.

1: A pending trigger request has occurred.

This bit is set to '1' when a selected edge trigger event occurs on the external interrupt
line. It can be cleared by writing '1' to the bit, or by changing the polarity of the edge
detection.

## 7. DMA Controller


### 7.1. Introduction

Direct memory access (DMA) is used to provide high-speed data transfers between peripheral and memory or
between memory and memory. Data can be moved quickly through DMA without CPU intervention, which
saves CPU resources for other operations.
The DMA controller has 5 logic channels. Each logic channel is to serve memory access requests from single or
multiple peripherals. Internal arbiter controls the priority of different DMA channels.

### 7.2. Main Features

•      5 DMA channels which can be configured independently.
•      Configurable data transmit length, the maximum length is 65535 bytes.
•      Each channel is directly connected to a dedicated hardware DMA request with support for software
triggering and configuration
•      The priority among multiple requests can be set by software programming (four levels: highest, high,
medium and low). When the priority setting is equal, it is determined by the hardware. The smaller the
channel number, the higher the priority..
•      Independent data source and transfer width (byte, halfword, fullword) of the destination data area,
simulating the process of packing and unpacking. Source and destination addresses must be aligned to the
data transfer width.
•      Supports circular mode transfers (for handling buffers or continuous data transfers).
•      Each channel has 3 independent event flags (Transfer complete, Half transfer, Transfer error), these 3
event flags generate a single interrupt request through XOR operation.
•      Support two transfer types which are Memory-to-Peripheral and Peripheral-to-Memory.
•      SRAM, APB1, APB2, CRC can be used as source and destination for access.

### 7.3. Block Diagram

*Figure 7-1 DMA Block Diagram*


### 7.4. Function Description

DMA controller and Cortex™-M0 core share the same system data bus, perform a direct memory data transfer
When CPU and DMA access the same target (RAM or peripheral) at the same time, DMA request will suspend
CPU from accessing the system bus for several cycles, and the bus arbiter will perform cyclic scheduling. This
allows the CPU to get at least half of the system bus (memory or peripheral) bandwidth.

#### 7.4.1. DMA Operation

After an event occurs, the peripheral sends a request signal to the DMA controller. The DMA controller handles
requests according to the channel's priority. When the DMA controller starts to access the requesting peripheral, the
DMA controller immediately sends it an acknowledge signal. The peripheral immediately releases its request
when it gets an acknowledgment signal from the DMA controller.Once the peripheral releases the request, the
DMA controller deactivates the acknowledge signal at the same time. The peripheral can start the next cycle if
there are more requests.
Each DMA data transfer consists of three operations:
•    Data is fetched from the peripheral data register or from the memory address indicated by the current
peripheral/memory address register. The starting bus matrix address for the first transfer is the peripheral base
address or memory unit specified by the DMA_PADDRx or DMA_MADDRx register.
•    Store data to the peripheral data register or the memory address indicated by the current peripheral/memory
address register. The start address of the first transfer is the peripheral base address or memory unit
specified by the DMA_PADDRx or DMA_MADDRx register.
•   Perform a decrement of the DMA_TXNUMx register, which contains the number of outstanding operations.

#### 7.4.2. Arbiter

The DMA controller has 5 channels, and each channel corresponds to DMA requests from different peripherals.
Although each channel can receive multiple peripheral requests, it can only receive one at a time, and cannot receive
multiple requests at the same time.
By default, the five channels have the same software priority, all of which are 0, and the smaller the hardware
logical channel number, the higher the priority. The arbiter initiates peripheral/memory accesses based on the
priority of channel requests. Priority management in 2 stages:
•    Software: The priority of each channel can be set in DMA_CHCFGx registers, there are 4 levels:
−    Very high priority
−    High priority
−    Medium priority
−    Low priority
•    Hardware:If 2 requests have the same software priority, the lower numbered lane has higher priority than the
higher numbered lane. For example, channel 2 takes precedence over channel 4.

#### 7.4.3. DMA Channels

Each channel can perform DMA transfer between the peripheral register at the specified address and the memory
address. The number of data transferred by DMA is programmable, and the maximum supported value is
65535. A register containing the number of data items to transfer, decremented after each transfer.

##### 7.4.3.1. Programmable data bit width

Peripheral and memory transfer data bit width supports byte, half-word and word, which can be programmed
through DMA_CHCFGx.PSIZE and DMA_CHCFGx.MSIZE.

##### 7.4.3.2. Address pointer incrementation

Peripheral and memory pointers can optionally be auto-incremented after each transfer by setting the PINC and
MINC flags in the DMA_CHCFGx registers.When set to Incremental mode, the next address to be transferred will
be the previous address plus an increment value of 1, 2 or 4 depending on the selected data width.The address
for the first transfer is the address stored in the DMA_PADDRx/DMA_MADDRx registers. During the transfer,
these registers keep the initial value, software cannot change and read the address currently being transferred (it
is in the internal current peripheral/memory address register).
When the channel is configured in acyclic mode, no DMA operations will occur after the transfer ends (the
transfer count becomes 0). To start a new DMA transfer, the transfer number needs to be rewritten in the
DMA_TXNUMx register with the DMA channel turned off.
•    In cyclic mode, at the end of the last transfer, the content of the DMA_TXNUMx register will be
automatically reloaded to its initial value, and the internal current peripheral/memory address register will
also be reloaded to the initial base address set by the DMA_PADDRx/DMA_MADDRx register.

##### 7.4.3.3. Channel configuration procedure

The following is the process of configuring DMA channel x (x represents the channel number) :

1. Set the address of the peripheral register in the DMA_PADDRx register. When a peripheral data transfer

request occurs, this address will be the source or destination of the data transfer.

2. Set the address of the data memory in the DMA_MADDRx registers. When a peripheral data transfer request

occurs, the transferred data will be read from or written to this address.

3. Set the amount of data to transfer in the DMA_TXNUMx registers. After each data transfer, this value is decremented.

4. Set the priority of the channel in the PRIOLVL[1:0] bits of the DMA_CHCFGx register.

5. Set the direction of data transfer, cycle mode, incremental mode of peripherals and memory, data width of


peripherals and memory, half-transfer interrupt or transfer complete interrupt in the DMA_CHCFGx register.

6. Set the CHEN bit of the DMA_CHCFGx register to enable the channel.

Once a DMA channel is enabled, it can respond to DMA requests from peripherals connected to the channel.
When half of the data is transferred, the half-transfer flag (HTXF) is set to 1, and an interrupt request will be generated
when the half-transfer interrupt bit (HTXIE) is enabled. After the data transfer is completed, the transfer complete flag
(TXCF) is set to 1, and when the transfer complete interrupt bit (TXCIE) is enabled, an interrupt request will be generated.

##### 7.4.3.4. Circular mode

The circular mode is used to process circular buffers and continuous data transmission (such as ADC scan
mode). The DMA_CHCFGx.CIRC is used to enable this function. When the cyclic mode is activated, if the
number of data to be transferred becomes 0, it will automatically be restored to the initial value when
configuring the channel, and the DMA operation will continue.
If the user wants to turn off the circular mode, the user needs to write 0 to DMA_CHCFGx.CHEN to disable
the DMA channel, and then write 0 to DMA_CHCFGx.CIRC (when DMA_CHCFGx.CHEN is 1, other bits in the
DMA_CHCFGx register cannot be rewritten).

##### 7.4.3.5. Memory-to-memory mode

The operation of the DMA channel can proceed without peripheral request, this operation is memory-to-memory mode.
After the MEM2MEM bit in the DMA_CHCFGx register is set, the DMA transfer will start as soon as the
software starts the DMA channel by setting the CHEN bit in the DMA_CHCFGx register. The DMA transfer
ends when the DMA_TXNUMx registers go to 0. Memory-to-memory mode cannot be used concurrently with
cycle mode.

#### 7.4.4. Programmable Data Transfer Width, Alignment and Data Endianness

When DMA_CHCFGx.PSIZE and DMA_CHCFGx.MSIZE are different, the DMA module aligns the data
according to the below.
*Table 7-1 Programmable Data Width and Endian Operation (when PINC = MINC = 1)*


Destina- Number
Source
tion       of         Source:                             Transfer Operations                 Destination:
Width
Width    Transfer                                                                              Address / data
Address / data                         (R: Read, W: Write)
(bit)
(bit)     (bit)

0x0 / B0      1: R B0 [7:0] @0x0, W B0 [7:0] @0x0                          0x0 / B0

0x1 / B1      2: R B1 [7:0] @0x1, W B1 [7:0] @0x1                          0x1 / B1
8        8         4
0x2 / B2      3: R B2 [7:0] @0x2, W B2 [7:0] @0x2                          0x2 / B2

0x3 / B3      4: R B3 [7:0] @0x3, W B3 [7:0] @0x3                          0x3 / B3

0x0 / B0      1: R B0 [7:0] @0x0, W 00B0 [15:0] @0x0                     0x0 / 00B0

0x1 / B1      2: R B1 [7:0] @0x1, W 00B1 [15:0] @0x2                     0x2 / 00B1
8       16         4
0x2 / B2      3: R B2 [7:0] @0x2, W 00B2 [15:0] @0x4                     0x4 / 00B2

0x3 / B3      4: R B3 [7:0] @0x3, W 00B3 [15:0].5                        0x6 / 00B3
@0x6

0x0 / B0     1: R B0 [7:0] @0x0, W 000000B0 [31:0] @0x0             0x0 / 000000B0

0x1 / B1     2: R B1 [7:0] @0x1, W 000000B1 [31:0] @0x4             0x4 / 000000B1
8    32   4
0x2 / B2     3: R B2 [7:0] @0x2, W 000000B2 [31:0] @0x8             0x8 / 000000B2

0x3 / B3     4: R B3 [7:0] @0x3, W 000000B3 [31:0] @0xC             0xC / 000000B3

0x0 / B1B0     1: R B1B0 [15:0] @0x0, W B0 [7:0] @0x0                    0x0 / B0

0x2 / B3B2     2: R B3B2 [15:0] @0x2, W B2 [7:0] @0x1                    0x1 / B2
16   8    4
0x4 / B5B4     3: R B5B4 [15:0] @0x4, W B4 [7:0] @0x2                    0x2 / B4

0x6 / B7B6     4: R B7B6 [15:0] @0x6, W B6 [7:0] @0x3                    0x3 / B6

0x0 / B1B0     1: R B1B0 [15:0] @0x0, W B1B0 [15:0] @0x0                0x0 / B1B0

0x2 / B3B2     2: R B3B2 [15:0] @0x2, W B3B2 [15:0] @0x2                0x2 / B3B2
16   16   4
0x4 / B5B4     3: R B5B4 [15:0] @0x4, W B5B4 [15:0] @0x4                0x4 / B5B4

0x6 / B7B6     4: R B7B6 [15:0] @0x6, W B7B6 [15:0] @0x6                0x6 / B7B6

0x0 / B1B0     1: R B1B0 [15:0] @0x0, W 0000B1B0 [31:0] @0x0          0x0 / 0000B1B0

0x2 / B3B2     2: R B3B2 [15:0] @0x2, W 0000B3B2 [31:0] @0x4          0x4 / 0000B3B2
16   32   4
0x4 / B5B4     3: R B5B4 [15:0] @0x4, W 0000B5B4 [31:0] @0x8          0x8 / 0000B5B4

0x6 / B7B6     4: R B7B6 [15:0] @0x6, W 0000B7B6 [31:0] @0xC          0xC / 0000B7B6

0x0 / B3B2B1B0   1: R B3B2B1B0 [31:0] @0x0, W B0 [7:0] @0x0                0x0 / B0

0x4 / B7B6B5B4   2: R B7B6B5B4 [31:0] @0x4, W B4 [7:0] @0x1                0x1 / B4
32   8    4
0x8 / BBBAB9B8   3: R BBBAB9B8 [31:0] @0x8, W B8 [7:0] @0x2                0x2 / B8

0xC / BFBEBDBC 4: R BFBEBDBC [31:0] @0xC, W BC [7:0] @0x3                  0x3 / BC

0x0 / B3B2B1B0   1: R B3B2B1B0 [31:0] @0x0, W B1B0 [15:0] @0x0            0x0 / B1B0

0x4 / B7B6B5B4   2: R B7B6B5B4 [31:0] @0x4, W B5B4 [15:0] @0x2            0x2 / B5B4
32   16   4
0x8 / BBBAB9B8   3: R BBBAB9B8 [31:0] @0x8, W B9B8 [15:0] @0x4            0x4 / B9B8

0xC / BFBEBDBC 4: R BFBEBDBC [31:0] @0xC, W BDBC [15:0] @0x6              0x6 / BDBC

0x0 / B3B2B1B0   1: R B3B2B1B0 [31:0] @0x0, W B3B2B1B0 [31:0] @0x0      0x0 / B3B2B1B0

0x4 / B7B6B5B4   2: R B7B6B5B4 [31:0] @0x4, W B7B6B5B4 [31:0] @0x4      0x4 / B7B6B5B4
32   32   4
0x8 / BBBAB9B8   3: R BBBAB9B8 [31:0] @0x8, W BBBAB9B8 [31:0] @0x8      0x8 / BBBAB9B8

0xC / BFBEBDBC 4: R BFBEBDBC [31:0] @0xC, W BFBEBDBC [31:0] @0xC        0xC / BFBEBDBC

##### 7.4.4.1. Operate an AHB device that does not support byte or halfword write

When the DMA module starts an AHB byte or halfword write operation, the data will be repeated in the unused
portion of the HWDATA[31:0] bus. Therefore, if DMA writes in bytes or halfwords to an AHB device that does
not support byte or halfword write operations (that is, HSIZE is not suitable for this module), no error will occur,
and the DMA will write 32 bits as per the following two examples HWDATA data:
•   When HSIZE=halfword, write halfword '0xABCD', DMA will set HWDATA bus to '0xABCDABCD'.
•   When HSIZE=byte, write byte '0xAB', DMA will set HWDATA bus to '0xABABABAB.
Assuming that the AHB/APB bridge is an AHB 32-bit slave device, which does not handle the HSIZE parameter,
it will transfer any byte or halfword on the AHB to the APB in 32 bits as follows:
•   A write operation of byte data '0xB0' to address 0x0 (or 0x1, 0x2 or 0x3) on AHB will be converted to a write
operation of word data '0xB0B0B0B0' to address 0x0 on APB.
•   A write operation of half-word data '0xB1B0' to address 0x0 (or 0x2) on AHB will be converted to a write
operation of word data '0xB1B0B1B0' to address 0x0 on APB.
For example, if you want to write to the APB backing register (16-bit register aligned with 32-bit address), you
need to configure the memory data source width (MSIZE) as '16-bit' and the peripheral target data width (PSIZE)
as '32-bit'

#### 7.4.5. Error Management

DMA access to a reserved address area will cause DMA transmission errors. When an error occurs,the
hardware will automatically clear the EN bit of the channel configuration register (DMA_CHCFGx)
corresponding to the error channel, and the channel operation is stopped. At this time, the transmission error
interrupt flag (TEIF) corresponding to the channel in the DMA_IFR register will be set.If the transfer error
interrupt enable bit is set in the DMA_CHCFGx register, an interrupt will be generated.

#### 7.4.6. Interrupt

Each DMA channel can generate interrupts on DMA transfer halfway, transfer complete, and transfer errors. For
application flexibility, these interrupts are enabled by setting different bits of the register.
*Table 7-2 DMA Interrupt Request*


Interrupt event             Event flag bit        Enable control bit

Half transfer                 HTXF                     HTXIE

Transfer complete               TXCF                     TXCIE

Transfer error                ERRF                     ERRIE

#### 7.4.7. DMA request mapping

Totally there are 28 DMA requests from all the peripherals,available from peripherals (TIMx[x=1, 3, 6], ADC,
SPI1/I2S1, SPI2/I2S2, I2C, LPUART and USART).
The DMA controller has 5 independent channels, and each channel is configurable to the request
of the peripheral,implemented by configuring the DMA channel x channel select register.
When a channel CH_CHSEL[4:0]=NUM, the channel selects the peripheral request corresponding to Ch
[NUM] The following table shows the input request channels corresponding to the peripherals:

DMAC Request Channel        Peripheral DMA Request
Ch0                         ADC_DMA
Ch1                         Reserved
Ch2                         Usart1_tx
Ch3                         Usart1_rx
Ch4                         Lpuart_tx
Ch5                         Lpuart_rx
Ch6                         Usart2_tx
Ch7                         Usart2_rx
Ch8                         Spi1_tx
Ch9                         Spi1_rx
Ch10                        Spi2_tx
Ch11                        Spi2_rx
Ch12                        I2c_tx
Ch13                        I2c_rx
Ch14                        Tim1_ch1
Ch15                        Tim1_ch2
Ch16                        Tim1_ch3
Ch17                        Tim1_ch4
Ch18                        Tim1_com
Ch19                        Tim1_up
Ch20                        Tim1_trig
Ch21                        Tim3_ch1
Ch22                        Tim3_ch3
Ch23                        Tim3_ch4
Ch24                        Tim3_up
Ch25                        Tim3_trig
Ch26                        Tim6
Ch27                        Reserved

### 7.5. DMA Registers


#### 7.5.1. DMA Register Overview

*Table 7-4 DMA Register Overview*


Of
Registe
fs
r
et

DMA_I
NTSTS
00
Reserved
0h
Reset
0   0    0     0   0      0   0                  0   0                0   0                0   0   0   0   0   0   0   0   0
Value

DMA_I
NTCLR
00
Reserved
4h
Reset
0   0    0     0   0      0   0                  0   0                0   0                0   0   0   0   0   0   0   0   0
Value

DMA_
PRIOLVL[1:0]

MSIZE[1:0]

CHCF                                                                                                                                           PSIZE[1:0]
00 G1
Reserved
8h
Reset
0   0                  0   0                0   0                0   0   0   0   0   0   0   0   0
Value

DMA_T
XNUM                                                                                                                                            NDTX[15:0]
00      1
Reserved
Ch
Reset
0      0   0                  0   0                0   0                0   0   0   0   0   0   0   0   0
Value

DMA_P
ADDR[31:0]
ADDR1
01
0h
Reset
0   0   0   0   0    0   0      0   0     0    0   0   0   0    0     0   0      0   0                  0   0                0   0                0   0   0   0   0   0   0   0   0
Value

Of
Registe
fs
r
et

DMA_
MADD                                                                        ADDR[31:0]
01 R1
4h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
CHSEL                                                                                                                                                                         CH_SEL[4:0]
01      1
Reserved
8h
Reset
0   0     0    0      0   0
Value

DMA_
PRIOLVL[1:0]

MSIZE[1:0]

PSIZE[1:0]
CHCF
01 G2
Reserved
Ch
Reset
0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_T
XNUM                                                                                                                                          NDTX[15:0]
02     2
Reserved
0h
Reset
0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_P
ADDR[31:0]
ADDR2
02
4h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
MADD                                                                        ADDR[31:0]
02 R2
8h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

Of
Registe
fs
r
et

DMA_
02
CHSEL                                                                                                                                                                         CH_SEL[4:0]
Ch
2
Reserved

Reset
0   0     0    0      0   0
Value

DMA_

PRIOLVL[1:0]

MSIZE[1:0]

PSIZE[1:0]
CHCF
03 G3
Reserved
0h
Reset
0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_T
XNUM                                                                                                                                          NDTX[15:0]
03     3
Reserved
4h
Reset
0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_P
ADDR[31:0]
ADDR3
03
8h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
MADD                                                                        ADDR[31:0]
03 R3
Ch
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
CHSEL                                                                                                                                                                         CH_SEL[4:0]
04      3
Reserved
0h
Reset
0   0     0    0      0   0
Value

Of
Registe
fs
r
et

DMA_

PRIOLVL[1:0]

MSIZE[1:0]

PSIZE[1:0]
CHCF
04 G4
Reserved
4h
Reset
0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_T
XNUM                                                                                                                                          NDTX[15:0]
04     4
Reserved
8h
Reset
0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_P
ADDR[31:0]
ADDR4
04
Ch
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
MADD                                                                        ADDR[31:0]
05 R4
0h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

DMA_
CHSEL                                                                                                                                                                         CH_SEL[4:0]
05      4
Reserved
4h
Reset
0   0     0    0      0   0
Value

DMA_
PRIOLVL[1:0]

MSIZE[1:0]

PSIZE[1:0]

CHCF
05 G5
Reserved
8h
Reset
0   0                  0   0                0   0                0   0   0   0   0     0    0      0   0
Value

Of
Registe
fs
r
et

DMA_T
XNUM                                                                                                             NDTX[15:0]
05     5
Reserved
Ch
Reset
0      0   0   0   0   0   0     0   0      0   0   0     0    0      0   0
Value

DMA_P
ADDR[31:0]
ADDR5
06
0h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0   0   0   0   0     0   0      0   0   0     0    0      0   0
Value

DMA_
MADD                                                                        ADDR[31:0]
06 R5
4h
Reset
0   0   0   0   0   0   0    0   0     0   0   0    0   0     0     0   0      0   0   0   0   0   0     0   0      0   0   0     0    0      0   0
Value

DMA_
06
CHSEL                                                        Reserved                                                                     CH_SEL[4:0]
8h
5

#### 7.5.2. DMA Interrupt Status Register (DMA_INTSTS)

Address offset: 0x00

Reset value: 0x0000 0000

Bit Field                     Name                   Description

19/15/11/7/3                  ERRFx                  Transfer error flag for channel x (x=1…5).

Set by hardware, software writes DMA_INTCLR corresponding bit to 1 to clear.

0: Transfer error no happened on channel x.

Bit Field             Name     Description

1: Transfer error happened on channel x.

18/14/10/6/2          HTXFx    Half transfer flag for channel x (x=1…5).

Set by hardware, software writes DMA_INTCLR corresponding bit to 1 to clear.

0: Half transfer not yet done on channel x.

1: Half transfer was done on channel x.

17/13/9/5/1           TXCFx    Transfer complete flag for channel x (x=1…5).

Set by hardware, software writes DMA_INTCLR corresponding bit to 1 to clear.

0: Transfer not yet done on channel x.

1: Transfer was done on channel x.

16/12/8/4/0           GLBFx    Global flag for channel x (x=1…5).

Set by hardware, software writes DMA_INTCLR corresponding bit to 1 to clear.

0: No transfer error, half transfer or transfer done event happen on channel x.

1: One of transfer error, half transfer or transfer done event happen on channel x.

#### 7.5.3. DMA Interrupt Flag Clear Register (DMA_INTCLR)

Address offset: 0x04

Reset value: 0x0000 0000

Bit Field             Name     Description

19/15/11/7/3          CERRFx   Clear transfer error flag for channel x (x=1…5).

Software can set this bit to clear ERRF of corresponding channel.

0: No action.

1: Reset DMA_INTSTS.ERRF bit of corresponding channel.

18/14/10/6/2          CHTXFx   Clear half transfer flag for channel x (x=1…5).

Software can set this bit to clear HTXF of corresponding channel.

0: No action.

Bit Field             Name              Description

1: Reset DMA_INTSTS.HTXF bit of corresponding channel.

17/13/9/5/1           CTXCFx            Clear transfer complete flag for channel x (x=1…5).

Software can set this bit to clear TXCF of corresponding channel.

0: No action.

1: Reset DMA_INTSTS.TXCF bit of corresponding channel.

16/12/8/4/0           CGLBFx            Clear global event flag for channel x (x=1…5).

Software can set this bit to clear GLBF of corresponding channel.

0: No action.

1: Reset GLBFx,TXCFx,HTXFx,ERRFx bit of DMA_INTSTS.register

#### 7.5.4. DMA Channel x Configuration Register (DMA_CHCFGx)

> **Note: The x is channel number, x = 1…5**

Address offset: 0x08+20 * (x–1)

Reset value: 0x0000 0000

Bit Field      Name                        Description

31:15          Reserved                    Reserved, the reset value must be maintained.

14             MEM2MEM                     Memory to memory mode.

Software can enable/disable memory-to-memory mode.

0: Non-memory-to-memory mode.

1: Memory-to-memory mode.

13:12          PRIOLVL[1:0]                Channel priority.

Software can configure channel priority.

00: Low

01: Medium

10: High

Bit Field   Name         Description

11: Very high

11:10       MSIZE[1:0]   Memory data size.

Software can configure data size read/write from/to memory address.

00: 8-bits

01: 16-bits

10: 32-bits

11: Reserved

9:8         PSIZE[1:0]   Peripheral data size.

Software can configure data size read/write from/to peripheral address.

00: 8-bits

01: 16-bits

10: 32-bits

11: Reserved

7           MINC         Memory increment mode.

Software can enable/disable memory address increment mode.

0: Memory address won't increase with each transfer.

1: Memory address increase with each transfer.

6           PINC         Peripheral increment mode.

Software can enable/disable peripheral address increment mode.

0: Peripheral address won't increase with each transfer.

1: Peripheral address increase with each transfer.

5           CIRC         Circular mode.

Software can set/clear this bit.

0: Channel will stop after one round of transfer.

1: Channel configure as circular mode.

4           DIR          Data transfer direction

Software can set/clear this bit.

0: Data transfer from Peripheral to Memory

1: Data transfer from Memory to Peripheral.

Bit Field      Name                        Description

3              ERRIE                       Transfer error interrupt enable.

Software can enable/disable transfer error interrupt.

0: Disable transfer error interrupt of channel x.

1: Enable transfer error interrupt of channel x.

2              HTXIE                       Half transfer interrupt enable.

Software can enable/disable half transfer interrupt.

0: Disable half transfer interrupt of channel x.

1: Enable half transfer interrupt of channel x.

1              TXCIE                       Transfer complete interrupt enable.

Software can enable/disable transfer complete interrupt.

0: Disable transfer complete interrupt of channel x.

1: Enable transfer complete interrupt of channel x.

0              CHEN                        Channel enable.

Software can set/reset this bit.

0: Disable channel.

1: Enable channel.

#### 7.5.5. DMA Channel x Transfer Number Register (DMA_TXNUMx)

> **Note: The x is channel number, x = 1…5**

Address offset: 0x0C+20 * (x–1)

Reset value: 0x0000 0000
This register cannot be written when the channel is enabled (CHEN=1 of DMA_CHCFGx)。

Bit Field              Name             Description

31:16                  Reserved         Reserved, the reset value must be maintained.

Bit Field             Name              Description

15:0                  NDTX              Number of data to transfer.

The value ranges from 0 to 65535. This register can only be written when the channel
is disabled. This register becomes read-only after the channel is turned on and
indicates the number of bytes remaining to be transferred. The register contents are
decremented after each DMA transfer. After the data transfer is completed, the
contents of the register will be automatically reloaded to the initial setting value.

When the content of the register is 0, no data transfer will occur regardless of whether
the channel is open or not.

#### 7.5.6. DMA Channel x Peripheral Address Register (DMA_PADDRx)

> **Note: The x is channel number, x = 1…5**
Address offset: 0x10+20 * (x–1)
Reset value: 0x0000 0000
This register can only be written if the channel is disabled (DMA_CHCFGx.CHEN = 0).In cycle mode, at the
end of the last transfer, the internal current peripheral address register is also reloaded to the initial base
address set by the DMA_PADDRx register

Bit field             Name              Description

31:0                  ADDR              Peripheral address.
If DMA_CHCFGx.PSIZE equal to 01(16 bits), DMA ignores bit 0 of
ADDR.Operations are automatically aligned to halfword addresses.
If DMA_CHCFGx.PSIZE equal to 10(32 bits) DMA will ignore bit [1:0] of
ADDR.Operations are automatically aligned to word addresses.

#### 7.5.7. DMA channel x Memory Address Register (DMA_MADDRx)

> **Note: The x is channel number, x = 1…5**
Address offset: 0x14+20 * (x–1)
Reset value: 0x0000 0000
This register can only be written if the channel is disabled (DMA_CHCFGx.CHEN = 0).In cycle mode, when
the last transfer ends, the internal current memory address register is also reloaded to the initial base
address set by the DMA_MADDRx register.

Bit Field              Name              Description

31:0                   ADDR              ADDR Memory address.The source or destination address of the data transfer.
If DMA_CHCFGx.MSIZE equal to 01(16 bits), DMA ignores bit 0 of ADDR
Operations are automatically aligned to halfword addresses.
If DMA_CHCFGx.MSIZE equal to 10(32bits) DMA will ignore bit [1:0] of
ADDR.Operations are automatically aligned to word addresses.

#### 7.5.8. DMA channel x channel request select register (DMA_CHSELx)

> **Note: The x is channel number, x = 1…5**
Address offset: 0x18+20 * (x–1)
Reset value: 0x0000 0000
This register is used to manage the DMA channel mapped by the DMA peripheral request.

Bit Field              Name              Description

31:6                   Reserved          Reserved, the reset value must be maintained.

5:0                    CH_SEL[4:0]       DMA channel request selection

0: dma_req0

……

27: dma_req27

For the mapping of peripheral DMA requests to DMA input request channel numbers,
please refer to Table7-3

## 8. CRC Calculation Unit

### 8.1. CRC Introduction

This module integrates the functions of CRC32 and CRC16, and the cyclic redundancy check (CRC)
calculation unit obtains any CRC calculation result according to a fixed generator polynomial. In other
applications, CRC technology is mainly used to verify the correctness and integrity of data transmission or data
storage. EN/IEC 60335-1 provides a method to verify the integrity of flash memory. CRC calculation unit can
calculate the identifier of the software when the program is running, then compare it with the reference identifier
generated during connection, and then store it in the specified memory space.

### 8.2. CRC Main Features

#### 8.2.1. CRC32 module

•   CRC32(X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X+1)
•   32 bits of data to be checked and 32 bits of output check code.
•   CRC calculation time: 4 AHB clock cycles (HCLK)
•   General-purpose 8-bit register (can be used to store temporary data)

#### 8.2.2. CRC16 module

•   CRC16(X16+X15+X2+1)
•   There are 8 bits of data to be checked and 16 bits of output check code.
•   CRC calculation time: 4 AHB clock cycle (HCLK)
•   The verification initial value can be configured, and the size end of the data to be verified can be configured.
•   Support 8bit LRC check value generation
The following figure is the block diagram of CRC unit.
*Figure 8-1 CRC Calculation Unit Block Diagram*


CRC
CRC32

CRC16
Ctrl&Data
Regs
AHB
AHBInf                           LRC
BUS

### 8.3. CRC Function Description

#### 8.3.1. CRC32

CRC calculation unit contains one 32-bit data register:
•    Writing this register to input CRC data.
•    Reading this register to get the calculated CRC result.
Every writing operation of this data register triggers the calculation of this new data with the previous
calculation result (CRC calculation is performed on the whole 32-bit word rather than byte by byte).
Supports back-to-back writes or sequential write-read operations.
CRC_CRC32DAT can be re-initialized to 0xFFFFFFFF by setting CRC_CRC32CTRL.RESET. This operation
does not affect the data in register CRC_CRC32IDAT.

#### 8.3.2. CRC16

CRC_CRC16CTRL.ENDHL controls little endian or big endian.
To clear the result of the last CRC operation, set CRC_CRC16CTRL.CLR to 1 or CRC_CRC16D to 0.
The initial value of CRC calculation can be configured by writing the CRC_CRC16D register. By default, the initial value

is the result of the last calculation.
LRC calculation is the same as CRC calculation. Both are carried out at the same time. CRC or LRC can be
read out depending on needs. If the initial value needs to be set, the LRC register should be configured first.

### 8.4. CRC Registers

#### 8.4.1. CRC Register Overview

The following table lists the registers and reset values of CRC.
*Table 8-1 CRC Register Overview*


Of
Regist
fs
er
et

CRC3
CRC32DAT[31:0]
2DAT
00
0h
Reset
1   1   1   1   1   1   1    1   1     1     1    1   1     1    1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1    1   1
Value

CRC3
CRC32IDAT[7:0]
2IDAT
00
Reserved
4h
Reset
0   0   0   0   0   0    0   0
Value

CRC3
2CTR
00      L
Reserved
8h
Reset
0
Value

CRC1
6CTR
00      L
Reserved
Ch
Reset
0    0
Value

CRC1
CRC16DAT[7:0]
6DAT
01
Reserved
0h
Reset
0   0   0   0   0   0    0   0
Value

01 CRC1
Reserved                                                             CRC16D[15:0]
4h 6D

Of
Regist
fs
er
et

Reset
0   0   0   0   0   0   0   0   0   0   0     0    0      0   0   0
Value

LRC                                                                                                            LRCDAT[7:0]
01
Reserved
8h Reset
0   0   0     0    0      0   0   0
Value

#### 8.4.2. CRC32 Data Register (CRC_CRC32DAT)

Address offset: 0x00

Reset value: 0xFFFF FFFF

Bit Field     Name             Description

31:0          CRC32DAT[31:0] CRC32 Data register.
The written data is the CRC value to be checked. The read data is the CRC calculation result. Only 32-
bit operations are supported.

#### 8.4.3. CRC32 Independent Data Register (CRC_CRC32IDAT)

Address offset: 0x04
Reset value: 0x0000 0000

Bit Field     Name             Description

31:8          Reserved         Reserved,the reset value must be maintained

7:0           CRC32IDAT[7:0] Independent 8-bit data register.

General 8 bits data register. It is for temporary stored 1-byte data. CRC_ CRC32CTRL.RESET bit reset

Bit Field   Name              Description

signal will not impact this register.

> **Note: This register is not a part of CRC calculation and can be used to store any data.**

#### 8.4.4. CRC32 Control Register (CRC_CRC32CTRL)

Address offset: 0x08

Reset value: 0x0000 0000

Bit Field   Name           Description

31:1        Reserved       Reserved,the reset value must be maintained

0           RESET          RESET signal.

It can reset CRC32 module and set data register to be 0xFFFF_FFFF. This reset can only write 1, and
hardware will clear to 0 automatically.

#### 8.4.5. CRC16 Control Register (CRC_CRC16CTRL)

Address offset: 0x0C

Reset value: 0x0000 0000

Bit Field          Name             Description

31:3               Reserved         Reserved,the reset value must be maintained

2                  CLR              Clear CRC16 results.
0: Not clear.
1: Clear to default value 0x0000. Set this bit to 1 will only maintain 1 clock cycle, hardware will
clear automatically. (Software read always 0).

Bit Field          Name          Description

1                  ENDHL         Data to be verified start to calculate from MSB or LSB.
0: From MSB to LSB
1: From LSB to MSB
This bit is only for data to be verified.

0                  Reserved      Reserved,the reset value must be maintained

> **Note: 8-bits, 16-bits and 32-bits operations are supported.**

#### 8.4.6. CRC16 Input Data Register (CRC_CRC16DAT)

Address offset: 0x10
Reset value: 0x0000 0000

Bit Field        Name                Description

31:8             Reserved            Reserved,the reset value must be maintained

7:0              CRC16DAT[7:0]       Data to be verified.

> **Note: 8-bits, 16-bits and 32-bits operations are supported.**

#### 8.4.7. CRC Cyclic Redundancy Check Code Register (CRC_CRC16D)

Address offset: 0x14

Reset value: 0x0000 0000

Bit Field       Name                  Description

31:16           Reserved              Reserved,the reset value must be maintained

15:0            CRC16D[15:0]          16-bit value of cyclic redundancy result data.

Every time the software writes the CRC16DAT register, the 16-bit calculated data from
CRC16 is updated in this register.

> **Note: 8-bits, 16-bits and 32-bits operations are supported (8-bit operations must be performed twice in a row to**
ensure that 16-bit initial values are configured properly)

#### 8.4.8. LRC Result Register (CRC_LRC)

Address offset: 0x18

Reset value: 0x0000 0000

Bit Field           Name            Description

31:8                Reserved        Reserved,the reset value must be maintained

7:0                 LRCDAT[7:0]     LRC check value register.

Software need to write initial value before use. And then each writing data to CRCDR will be
XOR with LCR register value. The result will be stored in LRC. Software read the result. It
should be cleared before next use.

## 9. Advanced-Control Timers (TIM1)

### 9.1. TIM1 Introduction

The advanced control timers (TIM1) is mainly used in the following occasions: counting the input signal,
measuring the pulse width of the input signal and generating the output waveform, etc.
Advanced timers have complementary output function with dead-time insertion and break function. Suitable for
motor control.

### 9.2. Main Features of TIM1

•      16-bit auto-reload counters. (It can realize up-counting, down-counting, up/down counting)
•      16-bit programmable prescaler. (The frequency division factor can be configured with any value between 1 and 65536)
•      Programmable Repetition Counter
•      TIM1 up to 6 channels.
•      4 capture/compare channels, the working modes are PWM output, ouput compare, one-pulse mode
output, input capture.
•      The events that generate the interrupt/DMA are as follows:
−    Update event
−    Trigger event
−    Input capture
−    Output compare

−        Break input
•      Complementary outputs with adjustable dead-time
−        For TIM1, channel 1,2,3 support this feature
•      Timer can be controlled by external signal
•      Incremental (quadrature) encoder interface: used for tracking motion and resolving rotation direction and position
•      Hall sensor interface: used to do three-phase motor control
*Figure 9-1 Block Diagram of TIM1*


BIT
Polarity
TIMx_BKIN                    BRK
selection

Clock failure event From clock controller CSS(Clock Security System)
PVD abnormal (Power supply voltage detection)
LOOKUP(Core Hardfault)
Comparator polarity

CK_TIM18 from RCC                            Internal clock(CK_INT)
To another timer, ADC

Polarity selection                              ETRF                                                     TRGO
Edge detector                                                         Trigger controller
TIMx_ETR pin
ETR                       Prescaler
Input filter                                             TIT                                      Reset, enable,
COMP_TIM_
up/down, count
OCREFCLR
Slave mode
controller
TRC
TI1F_ED                                  TRGI
CK_PSC

Encoder mode
Psc Prescaler

CK_CNT
Reptition counter
Update
TI1F_ED

event            Auto-reload                           CNT Counter
UDIT

CCxIT
TI1FP1                                          （Input）                                                                        OC1    TIMx_CH1
XOR                                                                                                                                 CCxIT
TI1                          TI1FP2                                                                                  （Output）                               OC1N   TIMx_CH1N
TIMx_CH1                                                                       IC1    Prescaler
edge detector
Capture/Compare x                                Output Control   OC2    TIMx_CH2
TRC                             CCx Event

Input filter &     TI2FP1                                                                 register              OCxREF           Complementary)
TIMx_CH2               TI2                                                    IC2                                                                                                        OC3
edge detector      TI2FP2                                          ICxPS                                                                                  TIMx_CH3
TRC                                                                                                                  OC3N   TIMx_CH3N
TI3FP3
Input filter &
ETRF

TIMx_CH3               TI3                                TI3FP4              IC3                                                                                                        OC4    TIMx_CH4
edge detector
TRC
TI4FP3
TIMx_CH4               TI4            Input filter &     TI4FP4               IC4
edge detector                                                                     Capture/Compare x
TRC                                                  (x = 5,6) register         OCxREF                                OC5
Output Control

ETRF
OC6

The event                                             Interrupt and DMA output

The capture channel 1 input can come from IOM or comparator output

### 9.3. TIM1 Function Description

#### 9.3.1. Time-base Unit

The advanced timer consists of a prescaler, counter, auto-reload register, and repetition counter. The counter can
count up, down, or bidirectionally (up and down). The prescaler divides the clock input to the counter.
When the time base unit is operating, software can still read and write to the counter, auto-reload register, and
prescaler register at any time.
The time base unit includes:

•   Prescaler register (TIMx_PSC)
•   Counter register (TIMx_CNT)
•   Auto-reload register (TIMx_AR)
•   Repetition counter register (TIMx_REPCNT)
The auto-reload register has a preload function. Reading and writing to the auto-reload register will access the
preload register. Depending on the setting of the auto-reload preload enable bit (ARPEN) in the TIMx_CTRL1
register, the contents of the preload register are transferred to the shadow register immediately or at each update
event (UEV). The following conditions generate an update event:
•   When the counter reaches the overflow condition (underflow condition when counting down) and when the
UPDIS bit in the TIMx_CTRL1 register is equal to 0, an update event is generated.
•   Generated by software.
The clock output CK_CNT of the prescaler drives the counter, and CK_CNT is only valid when the counter
enable bit (CNTEN) in the TIMx_CTRL1 register is set. (For more details on enabling the counter, please refer to
the description of the slave mode of the controller).
Note that the counter starts counting one clock cycle after the CNTEN bit of the TIMx_CTRL register is set.

##### 9.3.1.1. Prescaler description

The TIMx_PSC register consists of a 16-bit counter that can be used to divide the counter clock frequency by any
factor between 1 and 65536. It can be changed on the fly as it is buffered. The prescaler value is only taken into
account at the next update event.
*Figure 9-2 Counter Timing Diagram with Prescaler Division Change from 1 to 4*


CNTEN

CK_PSC

Timer Clock = CK_CNT

Counter register     87       88 89 8A 8B 8C               00                   01

Update event（UEV）

Prescaler controller register       0                              3

Write a new value in TIMx_PSC

Prescaler counter                      0           0    1    2   3   0       1    2   3

Prescaler buffer                      0                                 3

#### 9.3.2. Counter Mode

##### 9.3.2.1. Up-counting mode

In up-counting mode, the counter will count from 0 to the value of the register TIMx_AR, then it resets to 0. And a counter
overflow event is generated.
If the TIMx_CTRL1.UPRS bit (select update request) and the TIMx_EVTGEN.UDGN bit are set, an update event
(UEV) will generate. And TIMx_STS.UDITF will not be set by hardware, therefore, no update interrupts or update
DMA requests are generated. This setting is used in scenarios where you want to clear the counter but do not
want to generate an update interrupt.
Depending on the update request source is configured in the TIMx_CTRL1.UPRS, When an update event
occurs, all registers are updated and the TIMx_STS.UDITF is set:
•    The repetition counter reloads the contents of the TIMx_REPCNT
•    Update auto-reload shadow registers with preload value(TIMx_AR), when TIMx_CTRL1.ARPEN = 1.
•    The prescaler shadow register is reloaded with the preload value(TIMx_PSC).
To avoid updating the shadow registers when new values are written to the preload registers, you can disable the
update by setting TIMx_CTRL1.UPDIS=1.
When an update event occurs, the counter will still be cleared and the prescaler counter will also be set to 0
(but the prescaler value will remain unchanged).
The figure below shows some examples of the counter behavior and the update flags for different division factors
in the up-counting mode.

*Figure 9-3 Timing Diagram of Up-counting. The internal clock divider factor = 2/N*


Internal clock
divided by 2                           CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register         0034    0035 0036   0000   0001        0002   0003

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

Internal clock
CK_PSC
divided by N
Timer clock = CK_CNT

Counter register       1F           20                         00

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

*Figure 9-4 Timing Diagram of the Up-counting, Update event when ARPEN=0/1*


ARPEN = 0                  CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    31      32 33 34 35 36 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register     FF                      36

Change AR value
Write a new value in TIMx_AR

ARPEN = 1
CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    F0      F1 F2 F3 F4 F5 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register      F5                            36

Write a new value in TIMx_AR

Auto-reload shadow register      F5                             36

##### 9.3.2.2. Down-counting mode

If using the down-counting mode, the counter will start counting down from the auto-loaded value (the value
in the TIMx_AR counter) to 0, then restart from the auto-loaded value and generate a counter underflow event.

If a repetition counter is used, an update event (UEV) will be generated after the down-counting has been
repeated the number of times set in the repetition counter register (TIMx_REPCNT), otherwise an update event is
generated each time the counter underflows.

An update event can also be generated by setting the UDGN bit in the TIMx_EVTGEN register (either by
software or using the slave mode controller).

To avoid updating the shadow registers when writing a new value to the preload register, the UPDIS bit
in the TIMx_CTRL1 register can be set to disable update events. Therefore, no update event will be generated
until the UPDIS bit is cleared to 0. However, the counter will still restart counting from the current auto-load value,
and the prescaler counter will restart from 0 (but the prescaler factor remains unchanged).

Additionally, if the UPRS bit (update request selection) in the TIMx_CTRL1 register is set, setting the UDGN
bit will generate an update event UEV but will not set the UDITF flag (thus not generating an interrupt or DMA
request). This is to avoid simultaneously generating update and capture interrupts when a capture event occurs and
the counter is cleared.

After an update event occurs, all registers are updated, and the hardware simultaneously (depending on the UPRS
bit) sets the update flag bit (UDITF bit in the TIMx_STS register).

•   The repetition counter is reset to the content of the TIMx_REPCNT register

•   The prescaler buffer is loaded with the preloaded value (value in the TIMx_PSC register).

•   The current auto-reload register is updated with the preloaded value (content of the TIMx_AR register).

> **Note: The auto-reload is updated before the counter is reloaded, so the next cycle will be the expected value.**

The figure below shows some examples of the counter behavior and the update flags for different division factors
in the down-counting mode.

Here are some examples of counter operation at different clock frequencies when TIMx_AR=0x36.

*Figure 9-5 Timing Diagram of the Down-counting, internal clock divided factor = 2/N*


Internal clock divided by
2                                   CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register   0002      0001   0000    0036   0035   0034   0033

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Internal clock divided by
N                               CK_PSC

Timer clock = CK_CNT

Counter register       20        1F            00             36

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

##### 9.3.2.3. Center-aligned mode

In center-aligned mode, the counter increments from 0 to the value (TIMx_AR) – 1, a counter overflow event is
generated. It then counts down from the auto-reload value (TIMx_AR) to 1 and generates a counter underflow
event. Then the counter resets to 0 and starts counting up again.

In this mode, the TIMx_CTRL1.DIR direction bits have no effect and the count direction is updated and
specified by hardware. Center-aligned mode is valid when the TIMx_CTRL1. CAMSEL bit is not equal to "00".

The update events can be generated each time the counter overflows and each time the counter underflows.
Alternatively, an update event can also be generated by setting the TIMx_EVTGEN. UDGN bit (either by software
or using a slave mode controller). In this case, the counter restarts from 0, as does the prescaler's counter.

Please note: if the update source is a counter overflow, auto-reload update before reloading the counter.

*Figure 9-6 Timing Diagram of the Center-aligned, Internal clock divided factor =2/N*


Internal clock                   CK_PSC
divided by 2
CNTEN

Timer clock = CK_CNT

Counter register        0003        0002   0001   0000   0001        0002   0003

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Internal clock divided
by N          CK_PSC

Timer clock = CK_CNT

Counter register       20          1F            01                 00

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

*Figure 9-7 A Center-aligned (ARPEN = 1)*


Counter
underflow                              CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register        06        05 04 03 02 01 00 01 02 03 04 05 06 07

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register             FD                   36

Write a new value in TIMx_AR
Auto-reload active register          FD                           3
6

Counter                               CNTEN
overflow
CK_PSC

Counter register       F7        F8 F9 FA FB FC 36 35 34 33 32 31 30 2F

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register             FD                   36

Write a new value in TIMx_AR

Auto-reload active register           FD                           36

#### 9.3.3. Repetition Counter

The "Time-base unit" section 9.3.1 explained how the update event (UEV) is generated when the counter
overflows/underflows. However, in fact, it can only be generated when the repeat count reaches 0. This feature
is very useful for generating PWM signals. This means that every N times the counter overflows or underflows,
data is transferred from the preload register to the shadow register (TIMx_AR auto-reload register,
TIMx_PSC preload register, and capture/compare register TIMx_CCDATx in compare mode), where N is the
value in the TIMx_REPCNT repeat count register.
The repeat counter decrements when any of the following conditions are met:

•    In the up-counting mode, each time the counter reaches the maximum value, an overflow occurs.
•    In down-counting mode, each time the counter decrements to the minimum value, an underflow occurs.
•    In center-aligned mode, each time the counter overflows or underflows.
The repeat counter is automatically loaded, and the repeat rate is defined by the value in the TIMx_REPCNT register (see
*Figure 9-21). When the update event is generated by software (by setting the UDGN bit in TIMx_EVTGEN) or by the*

slave mode controller hardware, the update event occurs immediately regardless of the repeat counter value, and the content of
the TIMx_REPCNT register is reloaded into the repeat counter.
*Figure 9-8 Repeat Count Sequence Diagram in Down-counting Mode*


CK_PSC

CNTEN
Timer clock=
CNT_CLK
Counter     03   02    01   00   36   35 ... 01   00   36   35 ... 01   00   36   35 ... 01   00   36   35 ... 01   00   36   35 ... 01   00   36   35
register

Underflow

Overflow

TIMx_REPCNT = 0x0
UDITF

TIMx_REPCNT = 0x1
UDITF

TIMx_REPCNT = 0x2
UDITF

Software clear

*Figure 9-9 Repeat Count Sequence Diagram in up-counting mode*


CK_PSC

CNTEN
Timer clock
= CK_CNT

CNT_REG       33   34    35   36   00   01 ... 35   36   00   01 ... 35   36   00   01 ... 35   36   00   01 ... 35   36   00   01 ... 35   36   00   01

Underflow

Overflow

TIMx_REPCNT = 0x0
UDITF

TIMx_REPCNT = 0x1
UDITF

TIMx_REPCNT = 0x2
UDITF

Software clear

*Figure 9-10 Repeat Count Sequence Diagram in Center-aligned Mode*


CK_PSC

CNTEN
Timer clock
= CK_CNT
Counter      03   02   01   00   01   02 ... 35   36   35   34 ... 01   00   01   02 ... 35   36   35   34 ... 01   00   01   02 ... 35   36   35   34
register

Underflow

Overflow

TIMx_REPCNT = 0x0
UDITF

TIMx_REPCNT = 0x1
UDITF

TIMx_REPCNT = 0x2
UDITF

Software clear

#### 9.3.4. Clock Selection

•    The internal clock of Advanced-control timers : CK_INT
•    Two kinds of external clock mode :
−      external input pin
−      external trigger input ETR
•    Internal trigger input (ITRx): one timer is used as a prescaler for another timer.

##### 9.3.4.1. Internal clock source (CK_INT)

If the slave mode controller is disabled (SMSEL=000), then the CNTEN, DIR (TIMx_CTRL1 register), and
UDGN bits (TIMx_EVTGEN register) are the de facto control bits and can only be modified by software
(the UDGN bit is still automatically cleared). As long as the CNTEN bit is written to '1', the prescaler clock is
provided by the internal clock

CK_INT.
The figure below shows the operation of the control circuit and up-counter in general mode without a prescaler.
*Figure 9-11 Control Circuit in Normal Mode, Internal Clock Divided by 1*


CEN=CNTEN

Internal clock

UDGN

CNT_INIT

Counter clock = CK_CNT=CK_PSC

Counter register         61      62 63 64 65 66 00 01 02 03 04 05 06 07

##### 9.3.4.2. External clock source mode 1

*Figure 9-12 TI2 External Clock Connection Example*


TI2
Filter
(TIMx_CCMOD1.ICF[3:0])

Edge Detector

TIMx_SMCTRL.     CK_INT rising
TSEL[2:0]
Polarity Selection                                                          Internal clock mode
（TIMx_CCEN.CC2P）                       ITRx
TI1_ED          TRGI rising             External clock mode 1
TI1FP1
TI2FP2                                                          CK_PSC
ETRF
ETRF rising
External clock mode 2
TI2F rising or falling
or
Encoder mode
TI1F rising or falling
TIMx_SMCTRL.S
MSEL[2:0]
TIMx_SMCTRL.
EXCEN

This mode is selected by configuring TIMx_SMCTRL.SMSEL=111. The counter can be configured to count on
the rising or falling edge of the clock at the selected input.
For example, to configure up-counting mode to count on the rising edge of the clock at the TI2 input, the
configuration steps are as follows:
•    Configure TIMx_CCMOD1.CC2SEL equal to ‘01’, CC2 channel is configured as input, IC2 is mapped to TI2
•    Configure TIMx_CCEN.CC2P equal to ‘0’, select clock rising edge polarity
•    To select input filter bandwidth by configuring TIMx_CCMOD1.IC2F[3:0] (if filter is not needed, keep
IC2F bit at ‘0000’)
•    Configure TIMx_SMCTRL.SMSEL equal to ‘111’, select timer external clock mode 1
•    Configure TIMx_SMCTRL.TSEL equal to ‘110’, select TI2 as the trigger input source
•    Configure TIMx_CTRL1.CNTEN equal to ‘1’ to start the counter
> **Note: The capture prescaler is not used for triggering, so it does not need to be configured**
When the rising edge of the timer clock occurs at TI2=1, the counter counts once and the TIMx_STS .TITF flag is
pulled high.
The delay between the rising edge of TI2 and the actual clock of the counter depends on the resynchronization
circuit at the input of TI2.
*Figure 9-13 Control Circuit in External Clock Mode 1*


CNTEN

TI2

Timer clock = CK_CNT=CK_PSC

Counter register                64                          65              66

TITF

Write TITF=0

##### 9.3.4.3. External clock source mode 2

This mode is selected by TIMx_SMCTRL .EXCEN equal to 1. The counter can count on every rising or falling
edge of the external trigger input ETR.

The following figure is a schematic diagram of the external trigger input module in External clock source mode 2

*Figure 9-14 External Trigger Input Block Diagram*


TI1F rising or falling
or
TI2F rising or falling   Encode mode
Polarity Selection
ETR pin      TIMx_SMCTRL.EXTP
TRGI rising          External clock
mode 1

Prescaler                  ETRP                              ETRF rising          External clock
/1,/2,/4,/8                                                                       mode 2

TIMx_SMCTRL.EXTPS[1:0]                                            CK_INT rising         Internal clock
fDTS
mode
Filter Down counter
TIMx_SMCTR.EXTF[3:0]
TIMx_SMCTRL.EXCE
N
TIMx_SMCTRL.SMSE
L[2:0]

For example, use the following configuration steps to make the up counter count every 2 rising edges on ETR.

•      Since no filter is needed in this case, make TIMx_SMCTRL .EXTF[3:0] equal to ‘0000’

•      Configure the prescaler by making TIMx_SMCTRL.EXTPS[1:0] equal to ‘01’

•      Select the polarity on ETR pin by setting TIMx_SMCTRL.EXTP equal to ‘0’, The rising edge of ETR is valid

•      External clock mode 2 is selected by setting TIMx_SMCTRL .EXCEN equal to ‘1’

•      Turn on the counter by setting TIMx_CTRL1. CNTEN equal to ‘1’

The counter counts every 2 rising edges of ETR. The delay between the rising edge of ETR and the actual clock
to the counter is due to a resynchronization circuit on the ETRP signal.

*Figure 9-15 Control Circuit in External Clock Mode 2*


CNTEN

fCK _INT
ETR

ETRP

ETRF

Counter clock = CK_CNT=CK_PSC

34                       35
Counter register                                                                     36

#### 9.3.5. Capture/Compare Channels

Capture/compare channels include capture/compare registers and shadow registers. The input section consists of
digital filters, multiplexers and prescalers. The output section includes comparators and output controls.

The input signal TIx is sampled and filtered to generate the signal TIxF. A signal (TIxF_rising or TIxF_falling)
is then generated by the edge detector of the polarity select function, the polarity of which is selected by the
TIMx_CCEN.CCxP bits. This signal can be used as a trigger input for the slave mode controller. At the same
time, the signal ICx is sent to the capture register after frequency division. The following figure shows a block
diagram of a capture/compare channel.

*Figure 9-16 Capture/Compare Channel (example: channel 1 input stage)*


From slave mode controller TRC

TI2FP1                            Divider
TI2F_Rising                                                 /1,/2,/4,/8
From channel 2                                                         IC1                     IC1PSC
TI1FP1
TI2F_Falling                                               TIMx_CCMOD1.
Polarity Selection                                            IC1PSC[1:0]
TIMx_CCEN.CC2P            TIMx_CCMOD1.CC1SEL[3:0]         TIMx_CCEN.CC1EN
Filter Down
counter
TIMx_CCMOD1.IC1F[
3:0]                       Edge Detector
TI1F_Rising
TI1                 TI1F                                                 To the slave
TI1F_Falling
fDTS                                        Polarity Selection
mode controller
TIMx_CCEN.CC1P

TI1F_ED

The output part generates an intermediate waveform OCxRef (active high) as reference. The polarity acts at the end
of the chain.

*Figure 9-17 Capture/Compare Channel 1 Main Circuit*


CC1SEL[1] CC1SEL[0]                 IC1PSC CC1EN
Input
mode

Read CCDAT1H
S                   TIM1_EVTGEN.CC1GN

Read CCDAT1L R

Read in
APB Bus                                          progress

MCU Peripheral interface

16 bit
transfer         High 8-bits       Capture/                         Capture/
compare                          compare                 Counter
Low 8-bits    preload register                  shadow register

Output                      Comparator

Write CCDAT1H
mode
S     Write in
progress
CNT>CCDAT1
Write CCDAT1L                                                                                     CNT=CCDAT1
R

CC1SEL[0] CC1SEL[1]                           TIM1_CCMOD1.OC1PEN
UEV
From time base unit

*Figure 9-18 Output Part of Channelx (x= 1,2,3, take channel 1 as example)*


To the master mode controller
Output enable
circuit
Polarity
Seletion        TIM1_CCEN.CC1NEN    OC1
ETRF         Ocref_clr_int                                                             TIM1_CCEN.CC1NEN
TIM1_CCEN.
CC1P
TIM1_CCEN.CC1EN
TIM1_CCEN.CC1EN                              TIM1_BKDT.MOEN
0
CNT=CCR1                                                             0    x0                                           TIM1_BKDT.OSSI
OC1REFC
CNT>CCR1                                                                  01                           1
TIM1_BKDT.OSSR

Output mode
OC1ERF                       Dead-time               11
controller                                    generator                                            Polarity
TIM1_CCMOD1.OC1CEN   OCxERF                                                                            Seletion
TIM1_CCMOD1.OC1M
D[2:0]           Output selector          TIM1_BKDT.DTG[7:0]        11                          TIM1_CCEN.        Output enable
CC1NP
TIM1_CCMOD1.OC1CEN                                   10                           0                   circuit
TIM1_CCMOD1           OC1REFC           0
.OC1MD[2:0]                                     0x
TIM1_CCEN.CC1NEN   OC1N
TIM1_CCEN.CC1NEN
1               TIM1_CCEN.CC1EN
TIM1_CCEN.CC1EN
TIM1_BKDT.MOEN
TIM1_BKDT.OSSI
TIM1_BKDT.OSSR

*Figure 9-19 Output Part of Channelx (x= 4)*


To the master mode controller

Output enable
Polarity                circuit
selection
TIM1_CCEN.
CC4P                                      OC4
ETRF        Ocref_clr                                                                                     TIM1_CCEN.CC4EN
OC4 REF                                      0                    TIM1_BKDT.MOEN
CNT=CCDAT4
TIM1_BKDT.OSSI
CNT>CCDAT4                                                              1                    TIM1_CTRL2.OI4

Output mode
controller
TIM1_CCMOD2.OC2M
D[2:0]

Reads and writes always access preloaded registers when capturing/comparing. The two specific working
processes are as follows:

In capture mode, the capture is actually done in the shadow register, and then the value in the shadow register is
copied into the preload register.

In compare mode, as opposed to capture mode, the value of the preload register is copied into the shadow register,
which is compared with the counter.

#### 9.3.6. Input Capture Mode

In capture mode, the TIMx_CCDATx registers are used to latch the counter value after the ICx signal detects.

There is a capture interrupt flag TIMx_STS.CCxITF, which can issue an interrupt or DMA request if the
corresponding interrupt enable is pulled high.

The TIMx_STS. CCxITF bit is set by hardware when a capture event occurs and is cleared by software or by
reading the TIMx_CCDATx register.

The overcapture flag TIMx_STS.CCxOCF is set equal to 1 when the counter value is captured in the TIMx_CCDATx
register and TIMx_STS.CC1ITF is already pulled high. Unlike the former, TIMx_STS.CCxOCF is cleared by writing 0 to
it.

To achieve a rising edge of the TI1 input to capture the counter value into the TIMx_CCDAT1 register, the configuration
flow is as follows:

•     To select a valid input:

Configure TIMx_CCMOD1.CC1SEL to ‘01’. At this time, the input is the CC1 channel, and IC1 is mapped to TI1.

•     Program the desired input filter duration:

Define the sampling frequency of the TI1 input and the length of the digital filter by configuring the
TIMx_CCMODx.ICxF bits. Example: If the input signal jitters up to 5 internal clock cycles, we must choose a filter
duration longer than these 5 clock cycles. When 8 consecutive samples (sampled at fDTS frequency) with the new level
are detected, we can validate the transition on TI1. Then configure TIMx_CCMOD1. IC1F to ‘0011’.

•     By configuring TIMx_CCEN .CC1P=0, select the rising edge as the valid transition polarity on the TI1 channel.

•     Configure the input prescaler. In this example, configure TIMx_CCMOD1.IC1PSC= ‘00’ to disable the prescaler
because we want to capture every valid transition.

•     Enable capture by configuring TIMx_CCEN. CC1EN = ‘1’.

If you want to enable DMA request, you can configure TIMx_DINTEN.CC1DEN=1.If you want enable related interrupt
request, you can configureTIMx_DINTEN.CC1IEN bit=1

#### 9.3.7. PWM Input Mode

There are some differences between PWM input mode and normal input capture mode, including:

•     Two ICx signals are mapped to the same TIx input.

•     The two ICx signals are active on edges of opposite polarity.

•     Select one of two TIxFP signals as trigger input.

•     The slave mode controller is configured in reset mode.

For example, the following configuration flow can be used to know the period and duty cycle of the PWM signal on TI1
(It depends on the frequency of CK_INT and the value of the prescaler).

•     Configure TIMx_CCMOD1.CC1SEL equal to ‘01’ to select TI1 as valid input for TIMx_CCDAT1.

•     Configure TIMx_CCEN.CC1P equal to ‘0’ to select the active polarity of filtered timer input 1(TI1FP1), valid on the
rising edge.

•     Configure TIMx_CCMOD1.CC2SEL equal to ‘10’ select TI1 as valid input for TIMx_CCDAT2.

•     Configure TIMx_CCEN.CC2P equal to 1 to select the valid polarity of filtered timer input 2(TI1FP2), valid on the
falling edge.

•     Configure TIMx_SMCTRL.TSEL=101 to select Filtered timer input 1 (TI1FP1) as valid trigger input.

•     Configure TIMx_SMCTRL.SMSEL=100 to configure the slave mode controller to reset mode.

•     Configure TIMx_CCEN. CC1EN=1 and TIMx_CCEN.CC2EN=1 to enable capture.

*Figure 9-20 PWM Input Mode Timing*


TI1

TIMx_CNT             0004      0000         0001      000             0003       0004      0000
2

TIMx_CCDAT1
0004

TIMx_CCDAT2                                                   0002

IC1 capture                           IC2 capture           IC1 capture
IC2 capture                           Pulse width                Period
Reset counter                         measurement           measurement

Because of only filter timer input 1 (TI1FP1) and filter timer input 2 (TI2FP2) are connected to the slave mode
controller, the PWM input mode can only be used with the TIMx_CH1/TIMx_CH2 signals.

#### 9.3.8. Forced Output Mode

Software    can     force    output   compare      signals   to     active    or    inactive   level   directly,   in   output   mode (TIMx_CCM

User can set TIMx_CCMODx. OCxMD=101 to force the output compare signal to active level. And the OCxREF
will be forced high, OCx get opposite value to CCxP polarity bit. On the other hand, user can set
TIMx_CCMODx. OCxMD=100 to force the output compare signal to inactive level.

The values of the TIMx_CCDATx shadow register and the counter still comparing with each other in this mode.

The comparison between the output compare register TIMx_CCDATx and the counter TIMx_CNT has no
effect on OCxREF. And the flag still can be set. Therefore, the interrupt and DMA requests still can be sent.

#### 9.3.9. Output Compare Mode

User can use this mode to control the output waveform, or to indicate that a period of time has elapsed.

When the capture/compare register and the counter have the same value, the output compare function’s
operations are as follow:

•     TIMx_CCMODx.OCxMD is for output compare mode, and TIMx_CCEN.CCxP is for output polarity.
When the compare matches, if set TIMx_CCMODx.OCxMD=000, the output pin will keep its level;if
set TIMx_CCMODx.OCxMD=001, the output pin will be set active;if set TIMx_CCMODx.OCxMD=010,
the output pin will be set inactive;if set TIMx_CCMODx.OCxMD=011, the output pin will be set to toggle.

•     Set TIMx_STS.CCxITF.

•    If user set TIMx_DINTEN.CCxIEN, a corresponding interrupt will be generated.

•    If user set TIMx_DINTEN.CCxDEN and set TIMx_CTRL2.CCDSEL to select DMA request, and DMA
request will be sent.

User can set TIMx_CCMODx.OCxPEN to choose capture/compare shawdow regisete using capture/compare
preload registers(TIMx_CCDATx) or not.

The time resolution is one count of the counter.

In one-pulse mode, the output compare mode can also be used to output a single

pulse. Here are the configuration steps for output compare mode:

•    First of all, user should select the counter clock.

•    Secondly, set TIMx_AR and TIMx_CCDATx with desired data.

•    If user need to generate an interrupt, set TIMx_DINTEN.CCxIEN.

•    Then select the output mode by set TIMx_CCEN.CCxP, TIMx_CCMODx.OCxMD, TIMx_CCEN.CCxEN, etc.

•    At last, set TIMx_CTRL1.CNTEN to enable the counter.

User can update the output waveform by setting TIMx_CCDATx at any time, as long as the preload register is not
enabled. Otherwise the TIMx_CCDATx shadow register will be updated at the next update event.

Here is an example.

*Figure 9-21 Output Compare Mode, Toggle on OC1*


TIM1_CNT           0069       006A        006B                         8800     8801

TIM1_CCDAT1                                 006A                             8801

Write 8801h in CCDAT1
register

OC1REF=OC1

Match detected on CCDAT1
Interrupt generated if enabled

#### 9.3.10. PWM mode

User can use PWM mode to generate a signal whose duty cycle is determined by the value of the TIMx_CCDATx register

and whose frequency is determined by the value of the TIMx_AR register. And depends on the value of
TIMx_CTRL1.CAMSEL, the TIM can generate PWM signal in edge-aligned mode or center-aligned mode.

User can set PWM mode 1 or PWM mode 2 by setting TIMx_CCMODx. OCxMD=110 or setting
TIMx_CCMODx.          OCxMD=111.        To    enable    preload    register,   user   must     set   corresponding
TIMx_CCMODx.OCxPEN. And then set TIMx_CTRL1.ARPEN to auto-reload preload register eventually.

User can set polarity of OCx by setting TIMx_CCEN.CCxP. On the other hand, to enable the output of OCx, user
need to set the combination of the value of CCxEN, CCxNEN, MOEN, OSSI, and OSSR in TIMx_CCEN and
TIMx_BKDT.

The values of TIMx_CNT and TIMx_CCDATx are always compared with each other when the TIM is under PWM mode.

Only if an update event occurs, the preload register will transfer to the shadow register. Therefore user must reset
all the registers by setting TIMx_EVTGEN.UDGN before the counter starts counting..

##### 9.3.10.1. PWM center-aligned mode

If user set TIMx_CTRL1.CAMSEL equal 01, 10 or 11, the PWM center-aligned mode will be active. The setting
of the compare flag depends on the value of TIMx_CTRL1.CAMSEL. There are three kinds of situation that the
compare flag is set, only when the counter counts up, only when the counter counts down, or when the counter
counts up and counts down. User should not modified TIMx_CTRL1.DIR by software, it is updated by hardware.

Examples of center-aligned PWM waveforms is as follow, and the setting of the waveform are: TIMx_AR=8,
PWM mode 1, the compare flag is set when the counter counts down corresponding to TIMx_CTRL1.
CAMSEL=01.

*Figure 9-22 Center-aligned PWM Waveform (AR=8)*


Notes that user should know when using center-aligned mode are as follow:

•    It depends on the value of TIMx_CTRL1.DIR that the counter counts up or down. Cautions that the DIR and CAMSEL
bits should not be changed at the same time.

•    User should not write the counter while running in center-aligned mode, otherwise it will cause unexpected results.
Here are some example:

−   If the value written into the counter is 0 or is the value of TIMx_AR, the direction will be updated but the update
event will not be generated.

−   If the value written into the counter is greater than the value of auto-reload, the direction will not be updated.

•    To be on the safe side, user is suggested setting TIMx_EVTGEN.UDGN to generate an update by software before
starting the counter, and not writing the counter while it is running.

##### 9.3.10.2. PWM edge-aligned mode

There are two kinds of configuration in edge-aligned mode, up-counting and down-counting.

•    Up-counting

User can set TIMx_CTRL1.DIR=0 to make counter counts up. Here is an example for PWM mode1.

When TIMx_CNT < TIMx_CCDATx, the reference PWM signal OCxREF is high. Otherwise it will be low. If the compare
value in TIMx_CCDATx is greater than the auto-reload value, the OCxREF will remains 1. Conversely, if the compare
value is 0, the OCxREF will remains 0.

When TIMx_AR=8, the PWM waveforms are as follow.

*Figure 9-23 Edge-aligned PWM Waveform (APR=8)*


Counter register                 0   1     2   3     4   5   6   7   8   0   1

1
CCDATx>8 OCXREF
CCxIF

CCDATx=8 OCXREF

CCxIF

OCXREF
CCDATx=4

CCxIF

OCXREF 0
CCDATx=0
CCxIF

•    Down-counting

User can set TIMx_CTRL1.DIR=1 to make counter counts down. Here is an example for PWM mode1.

When TIMx_CNT > TIMx_CCDATx, the reference PWM signal OCxREF is low. Otherwise it will be high. If the compare
value in TIMx_CCDATx is greater than the auto-reload value, the OCxREF will remains 1.

> **Note: If the nth PWM cycle CCDATx shadow register >= AR value, the shadow register value of CCDATx in the (n+1)th**
PWM cycle is 0. At the moment when the counter is 0 in the (n+1)th PWM cycle, although the value of the counter =
CCDATx shadow register = 0 and OCxREF = '0', no compare event will be generated.

#### 9.3.11. One-pulse mode

In the one-pulse mode (ONEPM), a trigger signal is received, and a pulse tPULSE with a controllable pulse width is generated
after a controllable delay tDELAY. The output mode needs to be configured as output compare mode or PWM mode. After
selecting one-pulse mode, the counter will stop counting after the update event UEV is generated.

*Figure 9-24 Example of One-pulse Mode*


TI2

TIMx_AR

TIMx_CCDAT1

Counter

0
t

tDELAY      tPULSE

OCxREF

OC1

The following is an example of a one-pulse mode:

A rising edge trigger is detected from the TI2 input, and a pulse with a width of tPULSE is generated on OC1 after a delay of
tDELAY.

•   Counter configuration: count up, counter TIMx_CNT < TIMx_CCDAT1 ≤ TIMx_AR;

•   TI2FP2 is mapped to TI2, TIMx_CCMOD1.CC2SEL= ‘01’; TI2FP2 is configured for rising edge detection,
TIMx_CCEN.CC2P= ‘0’;

•   TI2FP2 acts as the trigger (TRGI) of the slave mode controller and starts the counter, TIMx_SMCTRL.TSEL= ‘110’,
TIMx_SMCTRL.SMSEL= ‘110’ (trigger mode);

•   TIMx_CCDAT1 writes the count value to be delayed (tDELAY), TIMx_AR - TIMx_CCDAT1 is the count value of the
pulse width tPULSE;

•   Configure TIMx_CTRL1.ONEPM=1 to enable single pulse mode, configure TIMx_CCMOD1.OC1MD = ‘111’to
select PWM2 mode;

•   Wait for an external trigger event on TI2, and a one pulse waveform will be output on OC1;

##### 9.3.11.1. Special case: OCx fast enable:

In one-pulse mode, an edge is detected through the TIx input, and triggers the start of the counter to count to the comparison
value and then output a pulse. These operations limit the minimum delay tDELAY that can be achieved.

You can set TIMx_CCMODx.OCxFEN=1 to turn on OCx fast enable, after triggering the rising edge, the OCxREF signal
will be forced to be converted to the same level as the comparison match occurs immediately, regardless of the comparison

result. OCxFEN fast enable only takes effect when the channel mode is configured for PWM1 and PWM2 modes.

#### 9.3.12. Clearing the OCxREF Signal on an External Event

If user set TIMx_CCMODx.OCxCEN=1, high level of ETRF input can be used to driven the OCxREF signal to
low, and the OCxREF signal will remains low, until the next UEV happens. Only output compare and PWM
modes can use this function. This cannot be used when it is in forced mode.

Here is an example for it. To control the current, user can connect the ETR signal to the output of a comparator,
and the operation for ETR should be as follow:

•     Set TIMx_SMCTRL.EXTPS=00 to disable the external trigger prescaler.

•     Set TIMx_SMCTRL.EXCEN=0 to disable the external clock mode 2.

•     Set TIMx_SMCTRL.EXTP and TIMx_SMCTRL.EXTF to configure the external trigger polarity and external
trigger filter according to the need.

Here is an example for the case that when ETRF input becomes high, the behavior of OCxREF signal for
different value of OCxCEN. Timer is set to be in PWM mode in this case.

*Figure 9-25 Clearing the OCxREF of TIMx*


ETRF

(CCDATx)

Counter(CNT)

OCxREF
(OCxCEN=＇0＇)

OCxREF
(OCxCEN=＇１＇)

ETRF becomes       ETRF still
high               high

#### 9.3.13. Complementary Outputs with Dead-time Insertion

Advanced-control timer can output two complementary signals, and manage the switching-off and switching-on of outputs.
This is called dead-time. User should adjust dead-time depending on the devices connected to the outputs and their
characteristics.

User can select the polarity of outputs by setting TIMx_CCEN.CCxP and TIMx_CCEN.CCxNP. And this selection is
independently for each output.

User can control the complementary signals OCx and OCxN by setting the combination of several control bits, which are
TIMx_CCEN.CCxEN, TIMx_CCEN.CCxNEN, TIMx_BKDT.MOEN, TIMx_CTRL2.OIx, TIMx_CTRL2.OIxN,
TIMx_BKDT.OSSI, and TIMx_BKDT.OSSR. When switching to the IDLE state, the dead-time will be activated.

If user set TIMx_CCEN.CCxEN and TIMx_CCEN.CCxNEN at the same time, a dead-time will be insert. If there is a
break circuit, the TIMx_BKDT.MOEN should be set too. There are 10-bit dead-time generators for each channel.

Reference waveform OCxREF can generates 2 outputs OCx and OCxN. And if OCx and OCxN are active high, the OCx
ouput signal is the same as the reference signal and the OCxN output signal is the opposite of the reference signal. However,
OCx output signal will be delayed relative to the reference rising edge and the OCxN output signal will be delayed relative
to the reference falling edge. If the delay is greater than the width of the active OCx or OCxN output, the corresponding
pulse will not generated.

The relationships between the output signals of the dead-time generator and the reference signal OCxREF are as follow.

Assume that TIMx_CCEN.CCxP=0, TIMx_CCEN.CCxNP=0, TIMx_BKDT.MOEN=1, TIMx_CCEN.CCxEN=1,
TIMx_CCEN.CCxNEN=1.

*Figure 9-26 Complementary Output with Dead-time Insertion*


User can set TIMx_BKDT.DTGN to programme the dead-time delay for each of the channels.

##### 9.3.13.1. Redirecting OCxREF to OCx or OCxN

User can set TIMx_CCEN.CCxEN and TIMx_CCEN.CCxNEN to re-directed OCxREF to the OCx output or to OCxN
output, in output mode.

Here are two ways to use this function. When the complementary remains at its inactive level, user can use this function to
send a specific waveform, such as PWM or static active level. User can also use this function to set both outputs in their
inactive level or both outputs active and complementary with dead-time.

If user set TIMx_CCEN.CCxEN=0 and TIMx_CCEN.CCxNEN=1, it will not complemented, and OCxN will become
active when OCxREF is high. On the other hand, if user set TIMx_CCEN.CCxEN=1 and TIMx_CCEN.CCxNEN=1, OCx
will become active when OCxREF is high. On the contrary, OCxN will become active when OCxREF is low.

#### 9.3.14. Break Function

The output enable signals and inactive levels will be modified when setting the corresponding control bits when using the
break function. However, the output of OCx and OCxN cannot at the active level at the same time no matter when, that is,
(CCxP^OIx) ^(CCxNP^OIxN)!=0.

When multiple break signals are enabled, each break signal constitutes an OR logic. Here are some signal which can be
the source of breaking.

•   The break input pin

•   A clock failure event, generated by the clock security system in the clock controller.

•   A PVD failure event.

•   Core Hardfault event.

•   The output signal of the comparator (configured in the comparator module, high level break).

•   By software through the TIMx_EVTGEN.BGN.

The break circuit will be disable after reset. And the MOEN bit will be low. User can set TIMx_BKDT.BKEN to enable
the break function. The polarity of break input signal can be selected by setting TIMx_BKDT.BKP. User can modify the
TIMx_BKDT.BKEN and TIMx_BKDT.BKP at the                    same time. After      user set the TIMx_BKDT.BKEN and
TIMx_BKDT.BKP, there is 1 APB clock cycle delay before the option take effect. Therefore, user need to wait 1 APB
clock cycle to read back the value of the written bit.

The falling edge of MOEN can be asynchronous, so between the actual signal and the synchronous control bit, there set a
resynchronization circuit. This circuit will cause a delay between the asynchronous and the synchronous signal. When user
set TIMx_BKDT.MOEN while it is low, user need to insert a delay before reading the value. Because an asynchronous
signal was written but user read the synchronous signal.

The behaviors that after a break occurs are as follow:

•    TIMx_BKDT.MOEN will be cleared asynchronously, and then the outputs will be put in inactive state, idle state or
reset state. The state of output is selected by setting TIMx_BKDT.OSSI. This will take effect even if the MCU
oscillator is off.

•    Once TIMx_BKDT.MOEN=0, the output of each output channel will be driven with the level programmed in
TIMx_CTRL2.OIx. Timer will release the enable outputs(taken over by GPIO controller) if TIMx_BKDT.OSSI=0,
otherwise it will remains high.

•    If user choose to use complementary outputs, the behaviors of TIM are as follow

−    Depends on the polarity, the outputs will be set in reset state first. It is an asynchronous option so it still
works even if there is no clock provided to the timer.

−    The dead-time generator will reactivated if the timer clock is still provided, and drive the outputs
according to the value of TIMx_CTRL2.OIx and TIMx_CTRL2.OIxN after the dead-time when (CCxP ^
OIx) ^ (CCxNP^OIxN)!
= 0, that is, the OCx and OCxN still cannot be driven to active level at the same time. Note that the dead-
time will be longer than usual because of the resynchronization on MOEN (almost 2 cycles of ck_tim).

−    Timer will release the output control if TIMx_BKDT.OSSI=0. Otherwise, if the enable output was high,
it will remain high. If it was low, it will become high when TIMx_CCEN.CCxEN or
TIMx_CCEN.CCxNEN is high.

•    If TIMx_DINTEN.BIEN=1, when TIMx_STS.BITF=1, an interrupt will be generated.

•    If user set TIMx_BKDT.AOEN, the TIMx_BKDT.MOEN will be set automatically when the next UEV
happened. User can use this to regulate. If user did not set TIMx_BKDT.AOEN, the TIMx_BKDT.MOEN
will remain low until been set 1 again. At this situation, user can use this for security. User can connect the
break input to thermal sensors, alarm for power drivers, or other security components.

•    When the break input is active, TIMx_BKDT.MOEN cannot be set automatically or by software at the
same time, and the TIMx_STS.BITF cannot be cleared. Because the break inputs are active on level.

To insure the security of application, the break circuit has the write protection function, and there is break input and
output management too. It allow user to freeze some parameters, such as dead-time duration, OCx/OCxN polarities
and state when disabled, OCxMD configurations, break enable and polarity. User can choose one of the 3 levels
of protection to use by setting TIMx_BKDT.LCKCFG. However, the TIMx_BKDT.LCKCFG can only be written
once after an MCU reset.

An example for output behavior in response to a break is as follow

*Figure 9-27 Output Behavior in Response to a Break*


Break in

OCxREF

OCx                                                                                            =OIx

Delay                     Delay            Delay
OCxN
=OIxN
(CCxENN=1,CCxNEN=1,CCxP=0,CCxNP=0,OIx= ~OIxN)

OCx                                                                                            =OIx

Delay
OCxN                                                                                           =OIxN
(CCxEN=1,CCxNEN=0,CCxP=0,CCxNP=0,OIx=~OIxN)

OCx

OCxN
(CCxEN=1,CCxNEN=0,CCxP=0,CCxNP=0,OIx=OIxN)

#### 9.3.15. Debug Mode

When the microcontroller is in debug mode (the Cortex-M0 core halted), depending on the
DBG_CTRL.TIMx_STOP configuration in the PWR module, the TIMx counter can either continue to work
normally or stop. See Section 4.3.18 register for details.

#### 9.3.16. TIMx and External Trigger Synchronization

TIMx timers can be synchronized by a trigger in slave modes (reset, trigger and gated).

##### 9.3.16.1. Slave mode: Reset mode

In reset mode, the trigger event can reset the counter and the prescaler updates the preload registers
TIMx_AR, TIMx_CCDATx, and generates the update event UEV (TIMx_CTRL1.UPRS=0).

The following is an example of a reset mode:

•    Channel 1 is configured as input to detect the rising edge of TI1
(TIMx_CCMOD1.CC1SEL=01, TIMx_CCEN.CC1P=0);

•    The slave mode is selected as reset mode (TIMx_SMCTRL.SMSEL=100), and the trigger input is selected
as TI1 (TIMx_SMCTRL.TSEL=101);

•    Start counter(TIMx_CTRL1.CNTEN = 1).

After starting the timer, when TI1 detects a rising edge, the counter resets and restarts counting, and the trigger
flag is set (TIMx_STS.TITF=1);

The delay between the rising edge on TI1 and the actual reset of the counter is due to the resynchronization circuit
on TI1 input.

*Figure 9-28 Control Circuit in Reset Mode*


Counter clock = ck_cnt = ck_psc

Counter register          60 61 62 63 64 65 66 00 01 02 03 00 01 02 03

TI1

UDGN

TITF

##### 9.3.16.2. Slave mode: Trigger mode

In trigger mode, the trigger event (rising edge/falling edge) of the input port can trigger the counter to start

counting. The following is an example of a trigger pattern:

•   Channel 2 is configured as input to detect the rising edge of TI2
(TIMx_CCMOD1.CC2SEL=01, TIMx_CCEN.CC2P=0);

•   Select    from    mode    to   trigger    mode    (TIMx_SMCTRL.SMSEL=110),              select   TI2    for    trigger   input (TIMx_SMC

When TI2 detects a rising edge, the counter starts counting, and the trigger flag is set (TIMx_STS.TITF=1);

The delay between the rising edge on TI2 and the actual start of the counter is due to the resynchronization circuit
on TI2 input.

*Figure 9-29 Control Circuit in Trigger Mode*


Counter clock=CN_CNT=CK_PSC

Counter register                          64                            65 66 67 68

CNTEN

TI2

TITF

##### 9.3.16.3. Slave mode: Gated mode

In gate control mode, the level polarity of the input port can control whether the counter

counts. The following is an example of a gated pattern:

•   Channel 1 is configured as input detection active low on TI1 (TIMx_CCMOD1.CC1SEL=01, TIMx_CCEN.CC1P=1);

•   Select the slave mode as the gated mode (TIMx_SMCTRL.SMSEL=101), and select TI1 as the
trigger input (TIMx_SMCTRL.TSEL=101);

•   Start counter(TIMx_CTRL1.CNTEN=1).

When TI1 detects that the level changes from low to high, the counter stops counting, and when TI1 detects that
the level changes from high to low, the counter starts counting, and the trigger flag will be set when it starts
or stops counting (TIMx_STS.TITF=1);

The delay between the rising edge on TI1 and the actual stop of the counter is due to the resynchronization circuit
on TI1 input.

*Figure 9-30 Control Circuit in Gated Mode*


Counter clock=CK_CNT=CK_PSC

Counter register        30 31 32 33                 34              35 36 37 38

CNTEN

TI1

TITF

Clear TITF

##### 9.3.16.4. Slave mode: Trigger Mode + External Clock Mode 2

In reset mode, trigger mode and gate control mode, the counter clock can be selected as external clock mode 2,
and the ETR signal is used as the external clock source input. At this time, the trigger selection needs to
select non-ETRF (TIMx_SMCTRL.TSEL=111).

Here is an example:

•   Channel 1 is configured as input to detect the rising edge of TI1 (TIMx_CCMOD1.CC1SEL=01,
TIMx_CCEN.CC1P=0);

•   Enable external clock mode 2 (TIMx_SMCTRL.EXCEN=1), select rising edge for external trigger polarity
(TIMx_SMCTRL.EXTP=0), select slave mode as trigger mode (TIMx_SMCTRL.SMSEL=110), select TI1 for
trigger input (TIMx_SMCTRL.TSEL=101);

When TI1 detects a rising edge, the counter starts counting on the rising edge of ETR, and the trigger flag
is set (TIMx_STS.TITF=1);

*Figure 9-31 Control Circuit in Trigger Mode + External Clock Mode2*


ETR

Counter clock=CK_CNT=CK_PSC

Counter register                        64                                65           66

CNTEN

TI1

TITF

#### 9.3.17. Timer Synchronization

All TIM timers are internally connected for timer synchronization or chaining. For more details, see Error!
Reference s ource not found..

#### 9.3.18. Six-step PWM Generation

In order to modify the configuration of all channels at the same time, the configuration of the next step can be set in
advance (the preloaded bits are OCxMD, CCxEN and CCxNEN). When a COM commutation event occurs, the
OCxMD, CCxEN, and CCxNEN preload bits are transferred to the shadow register bits.

COM commutation event generation method:

•   The software sets TIMx_EVTGEN.CCUDGN;

•   Generated by hardware on the rising edge of TRGI;

When a COM commutation event occurs, the TIMx_STS.COMITF flag will be set, enabling interrupts
(TIMx_DINTEN.COMIEN)                 will   generate       interrupts,     and      enabling      DMA         requests
(TIMx_DINTEN.COMDEN) will generate DMA requests.

The following figure shows the output timing diagram of OCx and OCxN when a COM commutation event occurs
in three different configurations:

*Figure 9-32 6-step PWM Generation, COM Example (OSSR=1)*


(CCRx)

Counter (CNT)

OCxREF

write COM=1

COM event

CCxEN=1                      write OCxMD=100
CCxNEN=0                                                                 CCxEN=1
OCxMD=100(forced inactive)                                               CCxNEN=0
OCx                                                                                     OCxMD=100

OCxN

write CCxNEN=0
CCxEN=1                      和 OCxMD=100
CCxNEN=0                                                                 CCxEN=1
OCxMD=100(forced inactive)                                               CCxNEN=0
OCx                                                                                     OCxMD=100

OCxN

write CCxNEN=1
CCxEN=1                      和 OCxMD=101                                 CCxEN=0
CCxNEN=0                                                                 CCxNEN=1
OCxMD=100(forced inactive)                                               OCxMD=101
Ocx

OCxN

#### 9.3.19. Encoder Interface Mode

The methods for selecting the encoder interface mode are:

•    If the counter only counts on the edge of TI2, set SMSEL=001 in the TIMx_SMCTRL register.

•    If it only counts on the edge of TI1, set SMSEL=010.

•    If the counter counts on both TI1 and TI2 edges, set SMSEL=011.

The polarity of TI1 and TI2 can be selected by setting the CC1P and CC2P bits in the TIMx_CCEN register; the
input filter can also be programmed if needed.

The two inputs, TI1 and TI2, are used as the interface for the incremental encoder. Referring to Table 9-1,
assuming the counter has been started (CNTEN=1 in the TIMx_CTRL1 register), the counter is driven by each valid
transition on TI1FP1 or TI2FP2. TI1FP1 and TI2FP2 are the signals of TI1 and TI2 after passing through the
input filter and polarity control; if there is no filtering and phase shifting, then TI1FP1=TI1, TI2FP2=TI2.

According to the transition sequence of the two input signals, counting pulses and direction signals are
generated. Depending on the transition sequence of the two input signals, the counter counts up or down, and the
hardware sets the DIR bit in the TIMx_CTRL1 register accordingly. Regardless of whether the counter counts
based on TI1, TI2, or both TI1 and TI2, a transition on either input (TI1 or TI2) will recalculate the DIR bit.

The encoder interface mode is basically equivalent to using an external clock with direction selection. This means
that the counter only counts continuously between 0 and the auto-reload value of the TIMx_AR register (depending
on the direction, either counting from 0 to AR or from AR to 0). Therefore, TIMx_AR must be configured before
starting the count; similarly, the capture, comparator, prescaler, repeat counter, trigger output characteristics, etc.
still work as usual.

> **Note: Encoder mode and external clock mode 2 are not compatible and cannot operate simultaneously.**

In this mode, the counter is automatically modified according to the speed and direction of the incremental
encoder, so the content of the counter always indicates the position of the encoder. The counting direction
corresponds to the direction of rotation of the connected sensor. The following table lists all possible
combinations, assuming that TI1 and TI2 do not change simultaneously.

*Table 9-1 Counting Direction Versus Encoder Signals*


Level on Opposite Signals          TI1FP1 Signal                   TI2FP2 Signal

Active Edge           (TI1FP1 forTI2,
Rising          Falling         Rising          Falling
TI2FP2 for TI1)

Counting only at TI1             High              Counting down    Counting up     Don't count     Don't count

Low                Counting up    Counting down    Don't count     Don't count

Counting only at TI2             High               Don't count     Don't count     Counting up    Counting down

Low                Don't count     Don't count    Counting down    Counting up

Counting on                 High              Counting down    Counting up     Counting up    Counting down

TI1 and TI2
Low                Counting up    Counting down Counting down      Counting up

Here is an example of an encoder with dual edge triggering selected to suppress input jitter:

•   IC1FP1 is mapped to TI1 (TIMx_CCMOD1.CC1SEL= ‘01’), IC1FP1 is not inverted (TIMx_CCEN.CC1P= ‘0’);

•   IC1FP2 is mapped to TI2 (TIMx_CCMOD2.CC2SEL= ‘01’), IC2FP2 is not inverted (TIMx_CCEN.CC2P= ‘0’);

•   The input is valid on both rising and falling edges (TIMx_SMCTRL.SMSEL = ‘011’);

•   Enable counter TIMx_CTRL1.CNTEN= ‘1’;

*Figure 9-33 Example of Counter Operation in Encoder Interface Mode*


Forward              Jitter         Backward        Jitter

TI1

TI2

Counter

Up                    Down

The following figure shows the example of counter behavior when IC1FP1 polarity is inverted (CC1P=
'1', other configurations are the same as above)

*Figure 9-34 Encoder Interface Mode Example with IC1FP1 Polarity Inverted*


forward             jitter          backward      jitter

TI1

TI2

Counter

down                                   up

#### 9.3.20. Interfacing with Hall sensor

Connect the Hall sensor to the three input pins (CC1, CC2 and CC3) of the timer, and then select the XOR function
to pass the inputs of TIMx_CH1, TIMx_CH2 and TIMx_CH3 through the XOR gate as the output of TI1 to
channel 1 for capture signal.

The timer needs to be configured as the reset mode in slave mode (TIMx_SMCTRL.SMSEL= ‘100’); the edge
of the trigger select TI1 triggers TI1F_ED (TIMx_SMCTRL.TSEL= ‘100’), any change in the Hall 3 inputs
will trigger the counter to recount, so it is used as a time reference; the capture/compare channel 1 is configured to
capture the TRC signal in capture mode (TIMx_CCMOD1.CC1SEL= ‘11’), which is used to calculate the
two input time intervals, thereby reflecting the motor speed.

Select timer channel 2 to output pulses to the advanced timer to trigger the COM event of the advanced timer to
update the control bits of the output PWM. The trigger selection of the advanced timer needs to select the
corresponding internal trigger signal (TIMx_SMCTRL.TSEL="ITRx"), the capture/compare preload control bit
needs to be configured to support preload (TIMx_CTRL2.CCPCTL=1) and support the rising edge of TRGI Trigger
an update (TIMx_CTRL2.CCUSEL=1).

This example is shown in the following figure.

*Figure 9-35 Example of Hall Sensor Interface*


### 9.4. TIMx Register Description(x=1)

For abbreviations used in registers, see section 1.1

These peripheral registers can be operated as half word (16-bits) or one word (32-bits).

#### 9.4.1. Register Overview

*Table 9-2 Register Overview*


Of
Registe
fs
r
et

CAMSEL[1:0]
CLKD[1:0]
TIMx_
CTRL1
Reserved

00
0h
Reset
0   0   0                                   0   0           0               0   0   0                 0      0   0   0     0          0
Value

MMSEL[2:0]
TIMx_
CTRL2
Reserved

00
4h
Reset
0        0        0         0                0   0   0           0               0   0   0                 0      0   0   0                0
Value

SMSEL[2:0]
EXTPS[1:0]

EXTF[3:0]

TSEL[2:0]
TIMx_S
MCTRL
Reserved

00
8h
Reset
0    0         0                0   0   0           0               0   0   0                 0      0       0     0          0
Value

TIMx_
DINTE
Reserved

00      N
Ch
Reset
0         0                0   0   0           0               0   0   0                 0      0   0   0     0          0
Value

TIMx_S
TS
Reserved

Reserved

01
0h
Reset
0   0                                   0   0   0           0                   0   0                 0      0   0   0     0          0
Value
Reserved

01 TIMx_E
4h VTGEN

Of
Registe
fs
r
et

Reset
0   0               0      0   0                 0   0                 0
Value

TIMx_
CCMO

CC2SEL[1:0]

CC1SEL[1:0]
OC2MD[2:0]

OC1MD[2:0]
D1

Output
Reserved

compar
e mode

Reset
0   0           0          0   0                 0   0                 0   0   0               0      0   0                 0   0                 0
01 Value
8h
TIMx_
CCMO

CC2SEL[1:0]

CC1SEL[1:0]
IC2PSC[1:0]

IC1PSC[1:0]
IC2F[3:0]

IC1F[3:0]
D1
Input
Reserved

capture
mode

Reset
0   0           0          0   0                 0   0                 0   0   0               0      0   0                 0   0                 0
Value

TIMx_
CCMO
CC4SEL[1:0]

CC3SEL[1:0]
OC4MD[2:0]

OC3MD[2:0]

D2
Output
Reserved

01
compar
Ch
e mode

Reset
0   0           0          0   0                 0   0                 0   0   0               0      0   0                 0   0                 0
Value

TIMx_
CCMO
CC4SEL[1:0]

CC3SEL[1:0]
IC4PSC[1:0]

IC3PSC[1:0]
Reserved

IC4F[3:0]

IC3F[3:0]

01 D2
Ch Input
capture
mode

Of
Registe
fs
r
et

Reset
0          0   0   0   0   0   0    0   0      0   0   0   0   0   0   0
Value

TIMx_
CCEN
Reserved

Reserved

Reserved
02
0h
Reset
0    0                0   0                  0   0   0   0   0    0   0      0   0   0   0   0   0   0
Value

TIMx_
CNT[15:0]
CNT
02
Reserved
4h
Reset
0          0   0   0   0   0   0    0   0      0   0   0   0   0   0   0
Value

TIMx_P
PSC[15:0]
SC
02
Reserved
8h
Reset
0          0   0   0   0   0   0    0   0      0   0   0   0   0   0   0
Value

TIMx_A
02                                                                                                    AR[15:0]
R
Ch                        Reserved
Reset
1          1   1   1   1   1   1    1   1      1   1   1   1   1   1   1
Value

TIMx_
REPC                                                                                                             REPCNT[7:0]
03 NT
Reserved
0h
Reset
0      0   0   0   0   0   0   0
Value

TIMx_
CCDAT                                                                                        CCDAT1[15:0]
03     1
Reserved
4h
Reset
0          0   0   0   0   0   0    0   0      0   0   0   0   0   0   0
Value

Of
Registe
fs
r
et

TIMx_
CCDAT                                                            CCDAT2[15:0]
03     2
Reserved
8h
Reset
0   0   0   0   0   0   0                  0   0    0         0    0    0     0   0   0
Value

TIMx_
CCDAT                                                            CCDAT3[15:0]
03     3
Reserved
Ch
Reset
0   0   0   0   0   0   0                  0   0    0         0    0    0     0   0   0
Value

TIMx_
CCDAT                                                            CCDAT4[15:0]
04     4
Reserved
0h
Reset
0   0   0   0   0   0   0                  0   0    0         0    0    0     0   0   0
Value
LCKCFG[1:0]

TIMx_B
DTGN[7:0]
KDT
04
Reserved
4h
Reset
0   0   0   0   0   0   0                  0   0    0         0    0    0     0   0   0
Value

TIMx_
DBLEN[4:0]                                             DBADDR[4:0]
DCTRL
Reserved

Reserved

04
8h
Reset
0   0   0   0                  0                       0    0     0   0   0
Value

TIMx_
DADD                                                                 BURST[15:0]
04     R
Reserved
Ch
Reset
0   0   0   0   0   0   0                  0   0    0         0    0    0     0   0   0
Value

#### 9.4.2. Control Register 1 (TIMx_CTRL1)

Offset address: 0x00

Reset value: 0x0000 0000

Bit Field     Name        Description

31:18         Reserved    Reserved, the reset value must be maintained

17            PBKPEN      PVD as BKP enable

0: Disable

1: Enable

16            LBKPEN      LockUp as BKP enable

0: Disable

1: Enable

15            CLRSEL      OCxREF clear selection

0: Select the external OCxREF clear from ETR

1: Select the internal OCxREF clear from comparator

14:12         Reserved    Reserved, the reset value must be maintained

11            C1SEL       Channel 1 selection

0: Select external CH1 signal from IOM

1: Select internal CH1 signal from COMP

10            IOMBKPEN    Enabling IOM as BKP

0: Enable

1: Disable

9:8           CLKD[1:0]   Clock division

CLKD[1:0] indicates the division ratio between CK_INT (timer clock) and DTS (clock used
for dead-time generator and digital filters (ETR, TIx))

00: tDTS = tCK_INT

Bit Field   Name          Description

01: tDTS = 2 × tCK_INT

10: tDTS = 4 × tCK_INT

11: Reserved, do not use this configuration

7           ARPEN         ARPEN: Auto-reload preload enable

0: Shadow register disable for TIMx_AR register

1: Shadow register enable for TIMx_AR register

6:5         CAMSEL[1:0]   Center-aligned mode selection

00: Edge-aligned mode. TIMx_CTRL1.DIR specifies up-counting or down-counting.

01: Center-aligned mode 1. The counter counts in center-aligned mode, and the output
compare interrupt flag bit is set to 1 when down-counting.

10: Center-aligned mode 2. The counter counts in center-aligned mode, and the output
compare interrupt flag bit is set to 1 when up-counting.

11: Center-aligned mode 3. The counter counts in center-aligned mode, and the output
compare interrupt flag bit is set to 1 when up-counting or down-counting.

> **Note: Switching from edge-aligned mode to center-aligned mode is not allowed when the**
counter is still enabled (TIMx_CTRL1.CNTEN = 1).

4           DIR           Direction

0: Up-counting

1: Down-counting

> **Note: This bit is read-only when the counter is configured in center-aligned mode or encoder**
mode.

3           ONEPM         One-pulse mode

0: Disable one-pulse mode, the counter counts are not affected when an update event occurs.

1: Enable one-pulse mode, the counter stops counting when the next update event occurs
(clearing TIMx_CTRL1.CNTEN bit)

2           UPRS          Update request source

This bit is used to select the UEV event sources by software.

0: If update interrupt or DMA request is enabled, any of the following events will generate an
update interrupt or DMA request:

–     Counter overflow/underflow

–     The TIMx_EVTGEN.UDGN bit is set

Bit Field     Name         Description

–     Update generation from the slave mode controller

1: If update interrupt or DMA request is enabled, only counter overflow/underflow will
generate update interrupt or DMA request

1             UPDIS        Update disable

This bit is used to enable/disable the Update event (UEV) events generation by software.

0: Enable UEV. UEV will be generated if one of following condition been fulfilled:

–     Counter overflow/underflow

–     The TIMx_EVTGEN.UDGN bit is set

–     Update generation from the slave mode controller

Shadow registers will update with preload value.

1: UEV disabled. No update event is generated, and the shadow registers (AR, PSC, and
CCDATx) keep their values. If the TIMx_EVTGEN.UDGN bit is set or a hardware reset is
issued by the slave mode controller, the counter and prescaler are reinitialized.

0             CNTEN        Counter Enable

0: Disable counter

1: Enable counter

> **Note: external clock, gating mode and encoder mode can only work after**
TIMx_CTRL1.CNTEN bit is set in the software. Trigger mode can automatically set
TIMx_CTRL1.CNTEN bit by hardware.

#### 9.4.3. Control Register 2 (TIMx_CTRL2)

Offset address: 0x04

Reset value: 0x0000 0000

Bit Field      Name       Description

31:19          Reserved   Reserved, the reset value must be maintained

18             OI6        Output idle state 6 (OC6 output). See TIMx_CTRL2.OI1 bit.

Bit Field   Name         Description

17          Reserved     Reserved, the reset value must be maintained

16          OI5          Output idle state 5 (OC5 output). See TIMx_CTRL2.OI1 bit.

15          Reserved     Reserved, the reset value must be maintained

14          OI4          Output idle state 4 (OC4 output). See TIMx_CTRL2.OI1 bit.

13          OI3N         Output idle state 3 (OC3N output). See TIMx_CTRL2.OI1N bits.

12          OI3          Output idle state 3 (OC3 output). See TIMx_CTRL2.OI1 bit.

11          OI2N         Output idle state 2 (OC2N output). See TIMx_CTRL2.OI1N bits.

10          OI2          Output idle state 2 (OC2 output). See TIMx_CTRL2.OI1 bit.

9           OI1N         Output Idle state 1 (OC1N Output)

0: When TIMx_BKDT.MOEN = 0, after dead-time OC1N = 0

1: When TIMx_BKDT.MOEN = 0, after dead-time OC1N = 1

8           OI1          Output Idle state 1

0: When TIMx_BKDT.MOEN = 0, if OC1N is implemented, after dead-time OC1 = 0

1: When TIMx_BKDT.MOEN = 0, if OC1N is implemented, after dead-time OC1 = 1

7           TI1SEL       TI1 selection

0: TIMx_CH1 pin connected to TI1 input.

1: TIMx_CH1, TIMx_CH2, and TIMx_CH3 pins are XOR connected to the TI1 input.

6:4         MMSEL[2:0]   Master Mode Selection

These 3 bits (TIMx_CTRL2. MMSEL [2:0]) are used to select the synchronization information
(TRGO) sent to the slave timer in the master mode. Possible combiNSING are as follows:

000: Reset –When the TIMx_EVTGEN.UDGN is set or a reset is generated by the slave mode
controller, a TRGO pulse occurs. And in the latter case, the signal on TRGO is delayed
compared to the actual reset.

001: Enable - The TIMx_CTRL1.CNTEN bit is used as the trigger output (TRGO). Sometimes
you need to start multiple timers at the same time or enable slave timer for a period of time.

The counter enable signal is set when TIMx_CTRL1.CNTEN bit is set or the trigger input in
gated mode is high.

Bit Field       Name        Description

When the counter enable signal is controlled by the trigger input, there is a delay on TRGO
except if the master/slave mode is selected (see the description of the TIMx_SMCTRL.MSMD
bit).

010: Update - The update event is selected as the trigger output (TRGO). For example, a master
timer clock can be used as a slave timer prescaler.

011: Compare pulse - Triggers the output to send a positive pulse (TRGO) when the
TIMx_STS.CC1ITF is to be set (even if it is already high), when a capture or a comparison
succeeds.

100: Compare - OC1REF signal is used as the trigger output (TRGO).

101: Compare - OC2REF signal is used as the trigger output (TRGO).

110: Compare - OC3REF signal is used as the trigger output (TRGO).

111: Compare - OC4REF signal is used as the trigger output (TRGO).

3               CCDSEL      Capture/compare DMA selection

0: When a CCx event occurs, a DMA request for CCx is sent.

1: When an update event occurs, a DMA request for CCx is sent.

2               CCUSEL      Capture/compare control update selection

0: If TIMx_CTRL2.CCPCTL = 1, they can only be updated by setting CCUDGN bits

1: If TIMx_CTRL2.CCPCTL = 1, they can be updated by setting CCUDGN bits or a rising edge
on TRGI.

> **Note: This bit only applied to channels with complementary outputs.**

1               Reserved    Reserved, the reset value must be maintained

0               CCPCTL      Capture/ Compare preloaded control

0: No preloading of CCxEN, CCxNEN and OCxMD bits occurs.

1: Preloading of CCxEN, CCxNEN and OCxMD bits occurs. they are updated only when a
commutation event COM occurs (TIMx_EVTGEN.CCUDGN bit set or rising edge on TRGI
depending on CCUSEL bit)

> **Note: This bit only applied to channels with complementary outputs.**

9.4.1        Slave Mode Control Register (TIMx_SMCTRL)
Offset address: 0x08

Reset value: 0x0000

Bit Field   Name         Description

15          EXTP         External trigger polarity

This bit is used to select whether the trigger operation is to use ETR or the inversion of ETR.

0: ETR active at high level or rising edge.

1: ETR active at low level or falling edge.

14          EXCEN        External clock enable

This bit is used to enable external clock mode 2, and the counter is driven by any active edge on
the ETRF signal in this mode.

0: External clock mode 2 disable.

1: External clock mode 2 enable.

Note 1: When external clock mode 1 and external clock mode 2 are enabled at the same time, the
input of the external clock is ETRF.

Note 2: The following slave modes can be used simultaneously with external clock mode 2: reset
mode, gated mode and trigger mode; However, TRGI cannot connect to ETRF
(TIMx_SMCTRL.TSEL ≠ '111').

Note 3: Setting the TIMx_SMCTRL.EXCEN bit has the same effect as selecting external clock
mode 1 and connecting TRGI to ETRF (TIMx_SMCTRL.SMSEL = 111 and TIMx_SMCTRL.TSEL
= 111).

13:12       EXTPS[1:0]   External trigger prescaler

The frequency of the external trigger signal ETRP must be at most 1/4 of TIMxCLK frequency.
When a faster external clock is input, a prescaler can be used to reduce the frequency of ETRP.

00: Prescaler disable

01: ETRP frequency divided by 2

10: ETRP frequency divided by 4

11: ETRP frequency divided by 8

Bit Field   Name        Description

11:8        EXTF[3:0]   External trigger filter

These bits are used to define the frequency at which the ETRP signal is sampled and the
bandwidth of the ETRP digital filtering. In effect, the digital filter is an event counter that
generates a validate output after consecutive N events are recorded.

0000: No filter, sampling at fDTS

0001: fSAMPLING = fCK_INT, N = 2

0010: fSAMPLING = fCK_INT, N = 4

0011: fSAMPLING = fCK_INT, N = 8

0100: fSAMPLING = fDTS/2, N = 6

0101: fSAMPLING = fDTS/2, N = 8

0110: fSAMPLING = fDTS/4, N = 6

0111: fSAMPLING = fDTS/4, N = 8

1000: fSAMPLING = fDTS/8, N = 6

1001: fSAMPLING = fDTS/8, N = 8

1010: fSAMPLING = fDTS/16, N = 5

1011: fSAMPLING = fDTS/16, N = 6

1100: fSAMPLING = fDTS/16, N = 8

1101: fSAMPLING = fDTS/32, N = 5

1110: fSAMPLING = fDTS/32, N = 6

1111: fSAMPLING = fDTS/32, N = 8

7           MSMD        Master/ Slave mode

0: No action

1: Events on the trigger input (TRGI) are delayed to allow a perfect synchronization between the
current timer (via TRGO) and its slaves. This is useful when several timers are required to be
synchronized to a single external event.

6:4         TSEL[2:0]   Trigger selection

These 3 bits are used to select the trigger input of the synchronous counter.

000: Internal trigger 0 (ITR0) 100: TI1 edge detector (TI1F_ED)

001: Internal trigger 1 (ITR1) 101: Filtered timer input 1 (TI1FP1)

010: Internal trigger 2 (ITR2) 110: Filtered timer input 2 (TI2FP2)

011: Internal trigger 3 (ITR3) 111: External triggered Input (ETRF)

Bit Field   Name          Description

For more details on ITRx, see Table 9-3.

> **Note: These bits must be changed only when not in use (e. g. TIMx_SMCTRL.SMSEL=000) to**
avoid false edge detection at the transition.

3           Reserved      Reserved, the reset value must be maintained

2:0         SMSEL[2:0]    Slave mode selection

When an external signal is selected, the active edge of the trigger signal (TRGI) is linked to the
selected external input polarity (see input control register and control register description)

000: Disable slave mode. If TIMx_CTRL1.CNTEN = 1, the prescaler is driven directly by the
internal clock.

001: Encoder mode 1. According to the level of TI2FP2, the counter up-counting or down-
counting on the edge of TI1FP1.

010: Encoder mode 2. According to the level of TI1FP1, the counter up-counting or down-
counting on the edge of TI2FP2.

011: Encoder mode 3. According to the input level of another signal, the counter up-counting or
down-counting on the edges of TI2FP1 and TI2FP2.

100: Reset mode. On the rising edge of the selected trigger input (TRGI), the counter is
reinitialized and the shadow register is updated.

101: Gated mode. When the trigger input (TRGI) is high, the clock of the counter is enabled. Once
the trigger input becomes low, the counter stops counting, but is not reset. In this mode, the start
and stop of the counter are controlled.

110: Trigger mode. When a rising edge occurs on the trigger input (TRGI), the counter is started
but not reset. In this mode, only the start of the counter is controlled.

111: External clock mode 1. The counter is clocked by the rising edge of the selected trigger input
(TRGI).

> **Note: Do not use gated mode if TI1F_ED is selected as the trigger input**
(TIMx_SMCTRL.TSEL=100). This is because TI1F_ED outputs a pulse for each TI1F transition,
whereas gated mode checks the level of the triggered input.

*Table 9-3 TIMx Internal Trigger Connection*


Slave timer    ITR0 (TSEL =            ITR1 (TSEL =            ITR2 (TSEL =               ITR3 (TSEL =
000)                    001)                    010)                    011)

TIM1            NA                    NA               TIM3       NA

TIM8           TIM1                   NA               NA         NA

#### 9.4.4. DMA/Interrupt Enable Registers (TIMx_DINTEN)

Offset address: 0x0C

Reset value: 0x0000

Bit Field   Name       Description

15          Reserved   Reserved, the reset value must be maintained

14          TDEN       Trigger DMA request enable

0: Disable trigger DMA request

1: Enable trigger DMA request

13          COMDEN     COM DMA request enable

0: Disable COM DMA request

1: Enable COM DMA request

12          CC4DEN     Capture/Compare 4 DMA request enable

0: Disable capture/compare 4 DMA request

1: Enable capture/compare 4 DMA request

11          CC3DEN     Capture/Compare 3 DMA request enable

0: Disable capture/compare 3 DMA request

1: Enable capture/compare 3 DMA request

10          CC2DEN     Capture/Compare 2 DMA request enable

0: Disable capture/compare 2 DMA request

1: Enable capture/compare 2 DMA request

9           CC1DEN     Capture/Compare 1 DMA request enable

0: Disable capture/compare 1 DMA request

1: Enable capture/compare 1 DMA request

Bit Field   Name     Description

8           UDEN     Update DMA request enable

0: Disable update DMA request

1: Enable update DMA request

7           BIEN     Break interrupt enable

0: Disable break interrupt

1: Enable break interrupt

6           TIEN     Trigger interrupt enable

0: Disable trigger interrupt

1: Enable trigger interrupt

5           COMIEN   COM interrupt enable

0: Disable COM interrupt

1: Enable COM interrupt

4           CC4IEN   Capture/Compare 4 interrupt enable

0: Disable capture/compare 4 interrupt

1: Enable capture/compare 4 interrupt

3           CC3IEN   Capture/Compare 3 interrupt enable

0: Disable capture/compare 3 interrupt

1: Enable capture/compare 3 interrupts

2           CC2IEN   Capture/Compare 2 interrupt enable

0: Disable capture/compare 2 interrupt

1: Enables capture/compare 2 interrupts

1           CC1IEN   Capture/Compare 1 interrupt enable

0: Disable capture/compare 1 interrupt

1: Enables capture/comparing 1 interrupt

0           UIEN     Update interrupt enable

0: Disable update interrupt

1: Enables update interrupt

#### 9.4.5. Status registers (TIMx_STS)

Offset address: 0x10

Reset value: 0x0000 0000

Bit Field   Name          Description

31：18       Reserved      Reserved, the reset value must be maintained

17          CC6ITF        Capture/Compare 6 interrupt flag

See TIMx_STS.CC1ITF description.

16          CC5ITF        Capture/Compare 5 interrupt flag

See TIMx_STS.CC1ITF description.

15：13       Reserved      Reserved, the reset value must be maintained

12          CC4OCF        Capture/Compare 4 overcapture flag

See TIMx_STS.CC1OCF description.

11          CC3OCF        Capture/Compare 3 overcapture flag

See TIMx_STS.CC1OCF description.

10          CC2OCF        Capture/Compare 2 overcapture flags

See TIMx_STS.CC1OCF description.

9           CC1OCF        Capture/Compare 1 overcapture flag

This bit is set by hardware only when the corresponding channel is configured in input
capture mode. Cleared by software writing 0.

0: No overcapture occurred

1: TIMx_STS.CC1ITF was already set when the value of the counter has been
captured in the TIMx_CCDAT1 register.

8           Reserved      Reserved, the reset value must be maintained

7           BITF          Break interrupt flag

Bit Field   Name     Description

This bit is set by hardware once the brake input is active. This bit is cleared by
software when the brake input becomes inactive.

0: No break event occurred

1: An active level has been detected

6           TITF     Trigger interrupt flag

This bit is set by hardware when an active edge is detected on the TRGI input when
the slave mode controller is in a mode other than gated. This bit is set by hardware
when any edge in gated mode is detected. This bit is cleared by software.

0: No trigger event occurred

1: Trigger interrupt occurred

5           COMITF   COM interrupt flag

This bit is set by hardware once a COM event is generated (when
TIMx_CCEN.CCxEN, TIMx_CCEN.CCxNEN, TIMx_CCMOD1.OCxMD have been
updated). This bit is cleared by software.

0: No COM event occurred

1: COM interrupt pending

4           CC4ITF   Capture/Compare 4 interrupt flag

See TIMx_STS.CC1ITF description.

3           CC3ITF   Capture/Compare 3 interrupt flag

See TIMx_STS.CC1ITF description.

2           CC2ITF   Capture/Compare 2 interrupt flag

See TIMx_STS.CC1ITF description.

1           CC1ITF   Capture/Compare 1 interrupt flag

When the corresponding channel of CC1 is in output mode:

Except in center-aligned mode, this bit is set by hardware when the counter value is
the same as the compare value (see TIMx_CTRL1.CAMSEL bit description). This bit
is cleared by software.

0: No match occurred.

1: The value of TIMx_CNT is the same as the value of TIMx_CCDAT1.

Bit Field   Name       Description

When the value of TIMx_CCDAT1 is greater than the value of TIMx_AR, the
TIMx_STS.CC1ITF bit will go high if the counter overflows (in up-counting and
up/down-counting modes) and underflows in down-counting mode.

When the corresponding channel of CC1 is in input mode:

This bit is set by hardware when the capture event occurs. This bit is cleared by
software or by reading TIMx_CCDAT1.

0: No input capture occurred.

1: Input capture occurred. Counter value has captured in the TIMx_CCDAT1. An edge
with the same polarity as selected has been detected on IC1.

0           UDITF      Update interrupt flag

This bit is set by hardware when an update event occurs under the following
conditions:

–    When TIMx_CTRL1.UPDIS = 0, and repeat counter value overflow or
underflow (An update event is generated when the repeat counter equals 0).

–    When TIMx_CTRL1.UPRS = 0, TIMx_CTRL1.UPDIS = 0, and set the
TIMx_EVTGEN.UDGN bit by software to reinitialize the CNT.

–    When TIMx_CTRL1.UPRS = 0, TIMx_CTRL1.UPDIS = 0, and the counter
CNT is reinitialized by the trigger event. (See TIMx_SMCTRL Register
description)

This bit is cleared by software.

0: No update event occurred

1: Update interrupt occurred

#### 9.4.6. Event Generation Registers (TIMx_EVTGEN)

Offset address: 0x14

Reset values: 0x0000

Bit Field   Name       Description

15：8        Reserved   Reserved, the reset value must be maintained

Bit Field   Name     Description

7           BGN      Break generation

This bit can generate a brake event when set by software. And at this time
TIMx_BKDT.MOEN = 0, TIMx_STS.BITF = 1, if the corresponding interrupt and
DMA are enabled, the corresponding interrupt and DMA will be generated. This bit is
automatically cleared by hardware.

0: No action

1: Generated a break event

6           TGN      Trigger generation

This bit can generate a trigger event when set by software. And at this time
TIMx_STS.TITF = 1, if the corresponding interrupt and DMA are enabled, the
corresponding interrupt and DMA will be generated. This bit is automatically cleared
by hardware.

0: No action

1: Generated a trigger event

5           CCUDGN   Capture/Compare control update generation

This bit is set by software. And if TIMx_CTRL2.CCPCTL = 1 at this time, the
CCxEN, CCxNEN and OCxMD bits are allowed to be updated. This bit is
automatically cleared by hardware.

0: No action

1: Generated a COM event

> **Note: This bit is only valid for channels with complementary outputs.**

4           CC4GN    Capture/Compare 4 generation

See TIMx_EVTGEN.CC1GN description.

3           CC3GN    Capture/Compare 3 generation

See TIMx_EVTGEN.CC1GN description.

2           CC2GN    Capture/Compare 2 generation

See TIMx_EVTGEN.CC1GN description.

Bit Field   Name                Description

1           CC1GN               Capture/Compare 1 generation

This bit can generate a capture/compare event when set by software. This bit is
automatically cleared by hardware.

When the corresponding channel of CC1 is in output mode:

The TIMx_STS.CC1ITF flag will be pulled high, if the corresponding interrupt and
DMA are enabled, the corresponding interrupt and DMA will be generated.

When the corresponding channel of CC1 is in input mode:

TIMx_CCDAT1 will capture the current counter value, and the TIMx_STS.CC1ITF
flag will be pulled high, if the corresponding interrupt and DMA are enabled, the
corresponding interrupt and DMA will be generated. If The TIMx_STS.CC1ITF is
already pulled high, pull TIMx_STS.CC1OCF high.

0: No action

1: Generated a CC1 capture/compare event

0           UDGN                Update generation

This bit can generate an update event when set by software. And at this time the counter
will be reinitialized, the prescaler counter will be cleared, the counter will be cleared in center-
aligned or up-counting mode, but take TIMx_AR in down-counting mode the value of the
register. This bit is automatically cleared by hardware.

0: No action

1: Generated an update event

#### 9.4.7. Capture/Compare Mode Register 1 (TIMx_CCMOD1)

Offset address: 0x18

Reset value: 0x0000

Channels can be used for input (capture mode) or output (compare mode), and the direction of the channel is
defined by the corresponding CCxSEL bit. The other bits of the register act differently in input and output modes.
OCx describes the function of a channel in output mode, ICx describes the function of a channel in input mode.
Hence, please note that the same bit can have different meanings for output mode and for input mode.

Output compare mode:

Bit Field   Name                  Description

15          OC2CEN                Output Compare 2 clear enable

Bit Field   Name          Description

14:12       OC2MD[2:0]    Output Compare 2 mode

11          OC2PEN        Output Compare 2 preload enable

10          OC2FEN        Output Compare 2 fast enable

9:8         CC2SEL[1:0]   Capture/compare 2 selection

These bits are used to select the input/output and input mapping of the channel

00: CC2 channel is configured as output

01: CC2 channel is configured as input, IC2 is mapped on TI2

10: CC2 channel is configured as input, IC2 is mapped on TI1

11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC2SEL is writable only when the channel is off (TIMx_CCEN.CC2EN = 0).**

7           OC1CEN        Output Compare 1 clear enable

0: OC1REF is not affected by ETRF input level

1: OC1REF is cleared immediately when the ETRF input level is detected as high

6:4         OC1MD[2:0]    Output Compare 1 mode

These bits are used to manage the output reference signal OC1REF, which
determines the values of OC1 and OC1N, and is valid at high levels, while the active
levels of OC1 and OC1N depend on the TIMx_CCEN.CC1P and
TIMx_CCEN.CC1NP bits.

000: Frozen. Comparison between TIMx_CCDAT1 register and counter TIMx_CNT
has no effect on OC1REF signal.

001: Set channel 1 to the active level on match. When TIMx_CCDAT1 =
TIMx_CNT, OC1REF signal will be forced high.

010: Set channel 1 as inactive level on match. When TIMx_CCDAT1 = TIMx_CNT,
OC1REF signal will be forced low.

011: Toggle. When TIMx_CCDAT1 = TIMx_CNT, OC1REF signal will be toggled.

100: Force to inactive level. OC1REF signal is forced low.

101: Force to active level. OC1REF signal is forced high.

110: PWM mode 1 - In up-counting mode, if TIMx_CNT < TIMx_CCDAT1,
OC1REF signal of channel 1 is high, otherwise it is low. In down-counting mode, if
TIMx_CNT > TIMx_CCDAT1, OC1REF signal of channel 1 is low, otherwise it is

Bit Field   Name          Description

high.

111: PWM mode 2 - In up-counting mode, if TIMx_CNT < TIMx_CCDAT1,
OC1REF signal of channel 1 is low, otherwise it is high. In down-counting mode, if
TIMx_CNT > TIMx_CCDAT1, OC1REF signal of channel 1 is high, otherwise it is
low.

Note 1: In PWM mode 1 or PWM mode 2, the OC1REF level changes only when the
comparison result changes or when the output compare mode is switched from
frozen mode to PWM mode.

3           OC1PEN        Output Compare 1 preload enable

0: Disable preload function of TIMx_CCDAT1 register. Supports write operations to
TIMx_CCDAT1 register at any time, and the written value is effective immediately.

1: Enable preload function of TIMx_CCDAT1 register. Only read and write
operations to preload registers. When an update event occurs, the value of
TIMx_CCDAT1 is loaded into the active register.

Note 1: Only when TIMx_CTRL1.ONEPM = 1(In one-pulse mode), PWM mode can
be used without verifying the preload register, otherwise no other behavior can be
predicted.

2           OC1FEN        Output Compare 1 fast enable

This bit is used to speed up the response of the CC output to the trigger input event.

0: CC1 behaves normally depending on the counter and CCDAT1 values, even if the
trigger is ON. The minimum delay for activating CC1 output when an edge occurs
on the trigger input is 5 clock cycles.

1: An active edge of the trigger input acts like a comparison match on CC1 output.
Therefore, OC is set to the comparison level regardless of the comparison result. The
delay time for sampling the trigger input and activating the CC1 output is reduced to
3 clock cycles.

OCxFEN only works if the channel is configured in PWM1 or PWM2 mode.

1：0         CC1SEL[1:0]   Capture/compare 1 selection

These bits are used to select the input/output and input mapping of the channel

00: CC1 channel is configured as output

01: CC1 channel is configured as input, IC1 is mapped on TI1

10: CC1 channel is configured as input, IC1 is mapped on TI2

11: CC1 channels are configured as inputs and IC1 is mapped to TRC. This mode is
only active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

Bit Field     Name           Description

> **Note: CC1SEL is writable only when the channel is off (TIMx_CCEN.CC1EN = 0).**

Input capture mode:

Bit Field Name             Description

15:12        IC2F[3:0]     Input Capture 2 Filter

11:10        IC2PSC[1:0]   Input Capture 2 Prescaler

9:8          CC2SEL[1:0]   Capture/Compare 2 selection

These bits are used to select the input/output and input mapping of the channel

00: CC2 channel is configured as output

01: CC2 channel is configured as input, IC2 is mapped on TI2

10: CC2 channel is configured as input, IC2 is mapped on TI1

11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC2SEL is writable only when the channel is off (TIMx_CCEN.CC2EN = 0).**

7:4          IC1F[3:0]     Input Capture 1 filter

These bits are used to define sampling frequency of TI1 input and the length of digital
filter. The digital filter is an event counter that generates an output transition after N
events are recorded.

0000: No filter, sampling at fDTS frequency

0001: fSAMPLING = fCK_INT, N = 2

0010: fSAMPLING = fCK_INT, N = 4

0011: fSAMPLING = fCK_INT, N = 8

0100: fSAMPLING = fDTS/2, N = 6

0101: fSAMPLING = fDTS/2, N = 8

0110: fSAMPLING = fDTS/4, N = 6

0111: fSAMPLING = fDTS/4, N = 8

1000: fSAMPLING = fDTS/8, N = 6

Bit Field Name                  Description

1001: fSAMPLING = fDTS/8, N = 8

1010: fSAMPLING = fDTS/16, N = 5

1011: fSAMPLING = fDTS/16, N = 6

1100: fSAMPLING = fDTS/16, N = 8

1101: fSAMPLING = fDTS/32, N = 5

1110: fSAMPLING = fDTS/32, N = 6

1111: fSAMPLING = fDTS/32, N = 8

3:2       IC1PSC[1:0]           Input Capture 1 prescaler

These bits are used to select the ratio of the prescaler for IC1 (CC1 input).

When TIMx_CCEN.CC1EN = 0, the prescaler will be reset.

00: No prescaler, capture is done each time an edge is detected on the capture input

01: Capture is done once every 2 events

10: Capture is done once every 4 events

11: Capture is done once every 8 events

1:0       CC1SEL[1:0]           Capture/Compare 1 selection

These bits are used to select the input/output and input mapping of the channel

00: CC1 channel is configured as output

01: CC1 channel is configured as input, IC1 is mapped on TI1

10: CC1 channel is configured as input, IC1 is mapped on TI2

11: CC1 channel is configured as input, IC1 is mapped to TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC1SEL is writable only when the channel is off (TIMx_CCEN.CC1EN = 0).**

#### 9.4.8. Capture/Compare Mode Register 2 (TIMx_CCMOD2)

Offset address: 0x1C

Reset value: 0x0000

See the description of the CCMOD1 register

above Output comparison mode:

Bit Field     Name          Description

15            OC4CEN        Output compare 4 clear enable

14:12         OC4MD[2:0]    Output compare 4 mode

11            OC4PEN        Output compare 4 preload enable

10            OC4FEN        Output compare 4 fast enable

9:8           CC4SEL[1:0]   Capture/Compare 4 selection

These bits are used to select the input/output and input mapping of the channel

00: CC4 channel is configured as output

01: CC4 channel is configured as input, IC4 is mapped on TI4

10: CC4 channel is configured as input, IC4 is mapped on TI3

11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC4SEL is writable only when the channel is off (TIMx_CCEN.CC4EN = 0).**

7             OC3CEN        Output compare 3 clear enable

6:4           OC3MD[2:0]    Output compare 3 mode

3             OC3PEN        Output compare 3 preload enable

2             OC3FEN        Output compare 3 fast enable

1:0           CC3SEL[1:0]   Capture/Compare 3 selection

These bits are used to select the input/output and input mapping of the channel

00: CC3 channel is configured as output

01: CC3 channel is configured as input, IC3 is mapped to TI3

10: CC3 channel is configured as input, IC3 is mapped on TI4

11: CC3 channel is configured as input, IC3 is mapped to TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC3SEL is writable only when the channel is off (TIMx_CCEN.CC3EN = 0).**

Input capture mode:

Bit Field Name             Description

15:12      IC4F[3:0]       Input Capture 4 filter

11:10      IC4PSC[1:0]     Input Capture 4 Prescaler

9:8        CC4SEL[1:0]     Capture/Compare 4 selection

These bits are used to select the input/output and input mapping of the channel

00: CC4 channel is configured as output

01: CC4 channel is configured as input, IC4 is mapped on TI4

10: CC4 channel is configured as input, IC4 is mapped on TI3

11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC4SEL is writable only when the channel is off (TIMx_CCEN.CC4EN = 0).**

7:4        IC3F[3:0]       Input Capture 3 filter

3:2        IC3PSC[1:0]     Input Capture 3 Prescaler

1:0        CC3SEL[1:0]     Capture/compare 3 selection

These bits are used to select the input/output and input mapping of the channel

00: CC3 channel is configured as output

01: CC3 channel is configured as input, IC3 is mapped to TI3

10: CC3 channel is configured as input, IC3 is mapped on TI4

11: CC3 channel is configured as input, IC3 is mapped to TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC3SEL is writable only when the channel is off (TIMx_CCEN.CC3EN = 0).**

#### 9.4.9. Capture/Compare Enable Registers (TIMx_CCEN)

Offset address: 0x20

Reset value: 0x0000 0000

Bit Field   Name       Description

31:22       Reserved   Reserved, the reset value must be maintained

21          CC6P       Capture/Compare 6 output polarity

See TIMx_CCEN.CC1P description.

20          CC6EN      Capture/Compare 6 output enable

See TIMx_CCEN.CC1EN description.

19：18       Reserved   Reserved, the reset value must be maintained

17          CC5P       Capture/Compare 5 output polarity

See TIMx_CCEN.CC1P description.

16          CC5EN      Capture/Compare 5 output enable

See TIMx_CCEN.CC1EN description.

15:14       Reserved   Reserved, the reset value must be maintained

13          CC4P       Capture/Compare 4 output polarity

See TIMx_CCEN.CC1P description.

12          CC4EN      Capture/Compare 4 output enable

See TIMx_CCEN.CC1EN description.

11          CC3NP      Capture/Compare 3 Complementary output polarity

See TIMx_CCEN.CC1NP description.

10          CC3NEN     Capture/Compare 3 complementary output enable

See TIMx_CCEN.CC1NEN description.

9           CC3P       Capture/Compare 3 output polarity

See TIMx_CCEN.CC1P description.

8           CC3EN      Capture/Compare 3 output enable

See TIMx_CCEN.CC1EN description.

7           CC2NP      Capture/Compare 2 complementary output polarity

See TIMx_CCEN.CC1NP description.

6           CC2NEN     Capture/Compare 2 complementary output enable

Bit Field   Name     Description

See TIMx_CCEN.CC1NEN description.

5           CC2P     Capture/Compare 2 output polarity

See TIMx_CCEN.CC1P description.

4           CC2EN    Capture/Compare 2 output enable

See TIMx_CCEN.CC1EN description.

3           CC1NP    Capture/Compare 1 complementary output polarity

0: OC1N active high

1: OC1N active low

2           CC1NEN   Capture/Compare 1 complementary output enable

0: Disable - Disable output OC1N signal. The level of OC1N depends on the value of
these bits TIMx_BKDT.MOEN, TIMx_BKDT.OSSI, TIMx_BKDT.OSSR,
TIMx_CTRL2.OI1, TIMx_CTRL2.OI1N and TIMx_CCEN.CC1EN.

1: Enable - Enable output OC1N signal. The level of OC1N depends on the value of
these bits TIMx_BKDT.MOEN, TIMx_BKDT.OSSI, TIMx_BKDT.OSSR,
TIMx_CTRL2.OI1, TIMx_CTRL2.OI1N and TIMx_CCEN.CC1EN.

1           CC1P     Capture/Compare 1 output polarity

When the corresponding channel of CC1 is in output mode:

0: OC1 active high

1: OC1 active low

When the corresponding channel of CC1 is in input mode:

At this time, this bit is used to select whether IC1 or the inverse signal of IC1 is used as
the trigger or capture signal.

0: non-inverted: Capture action occurs when IC1 generates a rising edge. When used as
external trigger, IC1 is non-inverted.

1: inverted: Capture action occurs when IC1 generates a falling edge. When used as
external trigger, IC1 is inverted.

> **Note: If TIMx_BKDT.LCKCFG = 3 or 2, these bits cannot be modified.**

Bit Field          Name            Description

0                  CC1EN           Capture/Compare 1 output enable

When the corresponding channel of CC1 is in output mode:

0: Disable - Disable output OC1 signal. The level of OC1 depends on the value of these
bits TIMx_BKDT.MOEN, TIMx_BKDT.OSSI, TIMx_BKDT.OSSR,
TIMx_CTRL2.OI1, TIMx_CTRL2.OI1N and TIMx_CCEN.CC1NEN.

1: Enable - Enable output OC1 signal. The level of OC1N depends on the value of these
bits TIMx_BKDT.MOEN, TIMx_BKDT.OSSI, TIMx_BKDT.OSSR,
TIMx_CTRL2.OI1, TIMx_CTRL2.OI1N and TIMx_CCEN.CC1NEN.

When the corresponding channel of CC1 is in input mode:

At this time, this bit is used to disable/enable the capture function.

0: Disable capture

1: Enable capture

*Table 9-4 Output Control bits of Complementary OCx and OCxN Channels with Break Function*


Control bits                                Output State1)

MOEN     OSSI       OSSR   CCxEN   CCxNEN   OCx Output state                                 OCxN Output state

Output disabled（not driven by timer）             Output disabled（not driven by timer）
0      0       0
OCx=0,OCx_EN=0                                   OCxN=0,OCxN_EN=0

Output disabled（not driven by timer）             OCxREF + polarity,
0      0       1
OCx=0,OCx_EN=0                                   OCxN= OCxREF xor CCxNP,OCxN_EN=1

OCxREF + polarity,                               Output disabled（not driven by timer）
0      1       0
OCx= OCxREF xor CCxP,OCx_EN=1                    OCxN=0,OCxN_EN=0

1         X                                                                             Complementary to OCxREF + polarity + dead-
0      1       1     OCxREF + polarity + dead-time,OCx_EN=1
time,OCxN_EN=1

Output disabled（not driven by timer）             Output disabled（not driven by timer）
1      0       0
OCx=CCxP,OCx_EN=0                                OCxN=CCxNP,OCxN_EN=0

Off-state (Output enabled with inactive state)   OCxREF + polarity,
1      0       1
OCx=CCxP,OCx_EN=1                                OCxN= OCxREF xor CCxNP,OCxN_EN=1

1      1       0     OCxREF + polarity,                               Off-state (Output enabled with inactive state)

Control bits                                Output State1)

MOEN     OSSI      OSSR   CCxEN    CCxNEN   OCx Output state                                  OCxN Output state

OCx= OCxREF xor CCxP, OCx_EN=1                    OCxN=CCxNP,OCxN_EN=1

OCxREF + polarity + dead-time,                    Complementary to OCxREF + polarity + dead-time,
1         1      1
OCx_EN=1                                          OCxN_EN=1

0               0      0
Output disabled（not driven by timer）

0               0      1
Asynchronously: OCx=CCxP, OCx_EN=0, OCxN=CCxNP,OCxN_EN=0；

0               1      0     Then if the clock is present: OCx=OIx and OCxN=OIxN after a dead-time, when (CCxP ^ OIx) ^ (CCxNP^OIxN)! =
0.
0               1      1
0                X
1               0      0
Off-state (Output enabled with inactive state)
1               0      1
Asynchronously: OCx=CCxP, OCx_EN=1, OCxN=CCxNP,OCxN_EN=1；

1               1      0     Then if the clock is present: OCx=OIx and OCxN=OIxN after a dead-time, when (CCxP ^ OIx) ^ (CCxNP^OIxN)! =
0
1               1      1

Note(1). If both outputs of a channel are not used (CCxEN = CCxNEN = 0), OIx, OIxN, CCxP and CCxNP
must all be cleared.

The status of external I/O pins connected to complementary OCx and OCxN channels depends on the OCx and
OCxN channel states and GPIO and AFIO registers.

#### 9.4.10. Counters (TIMx_CNT)

Offset address: 0x24

Reset value: 0x0000

Bit Field         Name               Description

15:0              CNT[15:0]          Counter value

#### 9.4.11. Prescaler (TIMx_PSC)

Offset address: 0x28

Reset value: 0x0000

Bit Field   Name         Description

15:0        PSC[15:0]    Prescaler value

Counter clock fCK_CNT = fCK_PSC/ (PSC [15:0] +1).

Each time an update event occurs, the PSC value is loaded into the active prescaler
register.

#### 9.4.12. Auto-reload Register (TIMx_AR)

Offset address: 0x2C

Reset values: 0xFFFF

Bit Field   Name         Description

15:0        AR[15:0]     Auto-reload value

These bits define the value that will be loaded into the actual auto-reload register.

See Section 9.3.1 for more details.

When the TIMx_AR.AR [15:0] value is null, the counter does not work.

#### 9.4.13. Repeat Count Registers (TIMx_REPCNT)

Offset address: 0x30

Reset value: 0x0000

Bit Field   Name        Description

15:8        Reserved    Reserved, the reset value must be maintained

Bit Field   Name           Description

7:0         REPCNT[7:0]    Repetition counter value

Repetition counter is used to generate the update event or update the timer registers only
after a given number (N+1) cycles of the counter, where N is the value of
TIMx_REPCNT.REPCNT . The repetition counter is decremented at each counter
overflow in up-counting mode, at each counter underflow in down-counting mode or at
each counter overflow and at each counter underflow in center-aligned mode. Setting the
TIMx_EVTGEN.UDGN bit will reload the content of TIMx_REPCNT.REPCNT
and generate an update event.

#### 9.4.14. Capture/Compare Register 1 (TIMx_CCDAT1)

Offset address: 0x34

Reset value: 0x0000

Bit Field   Name           Description

15:0        CCDAT1[15:0]   Capture/Compare 1 value

1) CC1 channel is configured as output:

CCDAT1 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC1 output.

If the preload feature is not selected in TIMx_CCMOD1.OC1PEN bit, the written value
is immediately transferred to the active register. Otherwise, this preloaded value is
transferred to the active register only when an update event occurs.

2) CC1 channel is configured as input:

CCDAT1 contains the counter value transferred by the last input capture 1 event (IC1).

When configured as input mode, register CCDAT1 and CCDDAT1 are only readable.

When configured as output mode, register CCDAT1 and CCDDAT1 are readable and
writable.

#### 9.4.15. Capture/Compare Register 2 (TIMx_CCDAT2)

Offset address: 0x38

Reset value: 0x0000

Bit Field   Name           Description

15:0        CCDAT2[15:0]   Capture/Compare 2 values

3) CC2 channel is configured as output:

CCDAT2 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC2 output.

If the preload feature is not selected in TIMx_CCMOD1.OC2PEN bit, the written value
is immediately transferred to the active register. Otherwise, this preloaded value is
transferred to the active register only when an update event occurs.

4) CC2 channel is configured as input:

CCDAT2 contains the counter value transferred by the last input capture 2 event (IC2).

When configured as input mode, register CCDAT2 and CCDDAT2 are only readable.

When configured as output mode, register CCDAT2 and CCDDAT2 are readable and
writable.

#### 9.4.16. Capture/Compare Register 3 (TIMx_CCDAT3)

Offset address: 0x3C

Reset value: 0x0000

Bit Field   Name            Description

15:0        CCDAT3[15:0]    Capture/Compare 3 value

5) CC3 channel is configured as output:

CCDAT3 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC3 output.

If the preload feature is not selected in TIMx_CCMOD2.OC3PEN bit, the written
value is immediately transferred to the active register. Otherwise, this preloaded value
is transferred to the active register only when an update event occurs.

6) CC3 channel is configured as input:

CCDAT3 contains the counter value transferred by the last input capture 3 event
(IC3).

When configured as input mode, register CCDAT3 and CCDDAT3 are only readable.

When configured as output mode, register CCDAT3 and CCDDAT3 are readable and
writable.

#### 9.4.17. Capture/Compare Register 4 (TIMx_CCDAT4)

Offset address: 0x40

Reset value: 0x0000

Bit Field    Name                  Description

15:0         CCDAT4[15:0]          Capture/Compare 4 value

7) CC4 channel is configured as output:

CCDAT4 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC4 output.

If the preload feature is not selected in TIMx_CCMOD2.OC4PEN bit, the written
value is immediately transferred to the active register. Otherwise, this preloaded value
is transferred to the active register only when an update event occurs.

8) CC4 channel is configured as input:

CCDAT4 contains the counter value transferred by the last input capture 4 event
(IC4).

When configured as input mode, register CCDAT4 and CCDDAT4 are only readable.

When configured as output mode, register CCDAT4 and CCDDAT4 are readable and
writable.

#### 9.4.18. Break and Dead-time Registers (TIMx_BKDT)

Offset address: 0x44

Reset value: 0x0000

> **Note:     AOEN, BKP, BKEN, OSSI, OSSR, and DTGN [7:0] bits can all be write protected depending on the LOCK**
configuration, and it is necessary to configure all of them on the first write to the TIMx_BKDT register.

Bit Field   Name   Description

15          MOEN   Main Output enable

This bit can be set by software or hardware depending on the TIMx_BKDT.AOEN bit,
and is asynchronously cleared to '0' by hardware once the brake input is active. It is only
valid for channels configured as outputs.

0: OC and OCN outputs are disabled or forced to idle state.

1: OC and OCN outputs are enabled if TIMx_CCEN.CCxEN or TIMx_CCEN.CCxNEN
bits are set. For more details, see Section 9.4.10，Capture/Compare enable registers
(TIMx_CCEN).

14          AOEN   Automatic output enable

0: Only software can set TIMx_BKDT.MOEN;

1: Software sets TIMx_BKDT.MOEN; or if the break input is not active, when the next
update event occurs, hardware automatically sets TIMx_BKDT.MOEN.

13          BKP    Break input polarity

0: Low level of the brake input is valid

1: High level of the brake input is valid

> **Note: Any write to this bit requires an APB clock delay to take effect.**

12          BKEN   Break enable

0: Disable brake input (BRK and CCS clock failure events)

1: Enable brake input (BRK and CCS clock failure events)

> **Note: Any write to this bit requires an APB clock delay to take effect.**

11          OSSR   Off-state Selection for Run Mode

This bit is used when TIMx_BKDT.MOEN=1 and the channel is a complementary
output.

The OSSR bit does not exist in timer without complementary outputs.

0: When inactive, OCx/OCxN outputs are disabled (OCx/OCxN enable output signal =
0)

1: When inactive, OCx/OCxN outputs are enabled with their inactive level as soon as
CCxEN = 1 or CCxNEN = 1. Then, OCx/OCxN enable output signal = 1

For more details, see Section 9.4.10, capture/compare enablement registers
(TIMx_CCEN).

Bit Field   Name          Description

10          OSSI          Off-state Selection for Idle Mode

This bit is used when TIMx_BKDT.MOEN=0 and the channels configured as outputs.

0: When inactive, OCx/OCxN outputs are disabled (OCx/OCxN enable output signal =
0)

1: When inactive, OCx/OCxN outputs are enabled with their idle level as soon as
CCxEN = 1 or CCxNEN = 1. Then, OCx/OCxN enable output signal = 1

For more details, see Section 9.4.10, capture/compare enablement registers
(TIMx_CCEN).

9:8         LCKCFG[1:0]   Lock Configuration

These bits offer a write protection against software errors.

00:

–     No write protected.

01:

–     LOCK Level 1

TIMx_BKDT.DTGN, TIMx_BKDT.BKEN, TIMx_BKDT.BKP, TIMx_BKDT.AOEN,
TIMx_CTRL2.OIx, TIMx_CTRL2.OIxN bits enable write protection.

10:

–     LOCK Level 2

Except for register write protection in LOCK Level 1 mode, TIMx_CCEN.CCxP and
TIMx_CCEN.CCxNP (If the corresponding channel is configured in output mode),
TIMx_BKDT.OSSR and TIMx_BKDT.OSSI bits also enable write protection.

11:

–     LOCK Level 3

Except for register write protection in LOCK Level 2, TIMx_CCMODx.OCxMD and
TIMx_CCMODx.OCxPEN bits (If the corresponding channel is configured in output
mode) also enable write protection.

> **Note: After the system reset, the LCKCFG bit can only be written once. Once written to**
the TIMx_BKDT register, LCKCFG will be protected until the next reset.

7:0         DTGN [7:0]    Dead-time Generator

These bits define the dead-time duration between inserted complementary outputs. The
relationship between the DTGN value and the dead time is as follows::

DTGN[7:5] = 0xx:

Bit Field   Name          Description

dead time = DTGN[7:0] × (tDTS)

DTGN[7:5] = 10x:

dead time =(64+DTGN[5:0]) × (2 × tDTS)

DTGN[7:5]=110:

dead time =(32+DTGN[4:0]) × (8 × tDTS)

DTGN [then] = 111:

dead time = (32 + DTGN [4:0]) × (16 × tDTS)

tDTS value see TIMx_CTRL1.CLKD [1:0].

#### 9.4.19. DMA Control Register (TIMx_DCTRL)

Offset address: 0x48

Reset value: 0x0000

Bit Field   Name           Description

15:9        Reserved       Reserved, the reset value must be maintained, kept at 0.

12:8        DBLEN[4:0]     DMA Burst Length

This bit field defines the number DMA will accesses (write/read) TIMx_DADDR
register.

00000:1 time transfer

00001: 2 times transfers

00010: 3 times transfers

…

10001: 18 times transfers

7:5         Reserved       Reserved, the reset value must be maintained.

4:0         DBADDR[4:0]    DMA Base Address

This bit field defines the first address where the DMA accesses the TIMx_DADDR
register.

When access is done through the TIMx_DADDR first time, this bit-field specifies the
address you just access. And then the second access to the TIMx_DADDR, you will

Bit Field   Name           Description

access the address of “DMA Base Address + 4”

00000: TIMx_CTRL1,

00001: TIMx_CTRL2,

00010: TIMx_SMCTRL,

…
10001：TIMx_BKDT

10010：TIMx_DCTRL

#### 9.4.20. DMA Transfer Buffer Register (TIMx_DADDR)

Offset address: 0x4C

Reset value: 0x0000

Bit Field    Name          Description

15:0         BURST[15:0]   DMA access buffer.

When a read or write operation is assigned to this register, the register located at the
address range (DMA base address + DMA burst length × 4) will be accessed.

DMA base address = The address of TIM_CTRL1 + TIMx_DCTRL. DBADDR * 4；

DMA burst len = TIMx_DCTRL.DBLEN + 1.

Example:

If TIMx_DCTRL.DBLEN = 0x3(4 transfers), TIMx_DCTRL.DBADDR = 0xD
(TIMx_CCDAT1), DMA data length = half word, DMA memory address = buffer
address in SRAM, DMA peripheral address = TIMx_DADDR address.

When an event occurs, TIMx will send requests to the DMA, and transfer data 4 times.

For the first time, DMA access to the TIMx_ DADDR register will be mapped to
access TIMx_CCDAT1 register;

For the second time, DMA access to the TIMx_ DADDR register will be mapped to
access TIMx_CCDAT2 register;

……

For the fourth time, DMA access to the TIMx_ DADDR register will be mapped to
access TIMx_CCDAT4 register;

## 10. General-Purpose Timers (TIM3)


### 10.1. General-Purpose Timers Introduction

The general-purpose timers (TIM3) is mainly used in the following occasions: counting the input signal,
measuring the pulse width of the input signal and generating the output waveform, etc.

### 10.2. Main Features of General-Purpose Timers

•    16-bit auto-reload counter. (It can implement up-counting, down-counting, up/down counting)

•    16-bit programmable prescaler. (The frequency division factor can be configured with any value between 1 and 65536)

•    TIM3 up to 4 channels

•    Channel’s working modes: PWM output, ouput compare, one-pulse mode output, input capture

•    The events that generate the interrupt/DMA are as follows:

−    Update event

−    Trigger event

−    Input capture

−    Output compare

•    Timer can be controlled by external signal

•    Timers are linked internally for timer synchronization or interconnection

•    Incremental (quadrature) encoder interface: used for tracking motion and resolving rotation direction and position

•    Hall sensor interface: used to do three-phase motor control

*Figure 10-1 Block Diagram of TIMx（x=3）*


TIMxCLK from RCC                        Internal clock(CK_INT)
To another timer, ADC

Polarity selection                           ETRF                                                     TRGO
Edge detector                                                      Trigger controller
TIMx_ETR pin
ETR                    Prescaler
Input filter                                          TIT                                      Reset, enable,
COMP_TIM_
up/down, count
OCREFCLR
Slave mode
controller
TRC
TI1F_ED                                TRGI
CK_PSC

Encoder mode
Psc Prescaler

CK_CNT
Update
event

UDIT     Auto-reload                           CNT Counter
TI1F_ED

CCxIT
TI1FP1                                        （Input）
XOR                                                                                                                          CCxIT                                 OC1
TI1FP2                                                                                                                             TIMx_CH1
Input filter &                                                                                           （Output）
TIMx_CH1                  TI1   edge detector                        IC1    Prescaler
TRC                            CCx Event          Capture/Compare x
Output Control   OC2   TIMx_CH2
（Input）            (x = 1，2，3，4)
TIMx_CH2            TI2
Input filter &     TI2FP1                                                           register                 OCxREF
edge detector      TI2FP2            IC2                        ICxPS
TRC                                                                                                                 OC3   TIMx_CH3
TI3FP3
ETRF

Input filter &
TIMx_CH3            TI3                            TI3FP4            IC3                                                                                                        OC4   TIMx_CH4
edge detector
TRC
TI4FP3
TIMx_CH4           TI4          Input filter &     TI4FP4            IC4
edge detector               TRC

The event                                              Interrupt and DMA output

The capture channel 1 input can come from IOM or comparator output

### 10.3. General-Purpose Timers Description


#### 10.3.1. Time-base Unit

The time base unit mainly includes: prescaler, counter and auto-reload. When the time base unit is operating, the
software can read and write the corresponding registers (TIMx_PSC, TIMx_CNT and TIMx_AR) at any time.

Depending on the setting of the auto-reload preload enable bit (TIMx_CTRL1.ARPEN), the value of the preload
register is transferred to the shadow register immediately or at each update event UEV. An update event is
generated when the counter reaches the overflow/underflow condition and it can be generated by software when
TIMx_CTRL1.UPDIS=0. The counter CK_CNT is valid only when the TIMx_CTRL1.CNTEN bit is set. The
counter starts counting one clock cycle after the TIMx_CTRL1.CNTEN bit is set.

##### 10.3.1.1. Prescaler description

The TIMx_PSC register consists of a 16-bit counter that can be used to divide the counter clock frequency by
any factor between 1 and 65536. It can be changed dynamically at runtime as the controller has a buffer . The
prescaler value is only taken into account at the next update event.

*Figure 10-2 Counter Timing Diagram with Prescaler Division Change From 1 to 4*


CNTEN

CK_PSC

Timer Clock = CK_CNT

Counter register    87       88 89 8A 8B 8C               00                   01

Update event（UEV）

Prescaler controller register     0                               3

Write a new value in TIMx_PSC

Prescaler counter                     0           0    1    2   3   0       1    2   3

Prescaler buffer                     0                                 3

#### 10.3.2. Counter Mode

##### 10.3.2.1. Up-counting mode

In up-counting mode, the counter will count from 0 to the value of the register TIMx_AR, then it resets to 0. And a counter
overflow event is generated.

If the TIMx_CTRL1.UPRS bit (select update request) and the TIMx_EVTGEN.UDGN bit are set, an update event (UEV)
will generate And TIMx_STS.UDITF will not be set by hardware, therefore, no update interrupts or update DMA requests
are generated. This setting is used in scenarios where you want to clear the counter but do not want to generate an update
interrupt.

Depending on the update request source is configured in TIMx_CTRL1.UPRS, When an update event occurs, all registers
are updated and the TIMx_STS.UDITF is set:

•    Update auto-reload shadow registers with preload value(TIMx_AR), when TIMx_CTRL1.ARPEN = 1.

•    The prescaler shadow register is reloaded with the preload value(TIMx_PSC).

To avoid updating the shadow registers when new values are written to the preload registers, you can disable the update by
setting TIMx_CTRL1.UPDIS=1.

When an update event occurs, the counter will still be cleared and the prescaler counter will also be set to 0 (but the
prescaler value will remain unchanged).
The figure below shows some examples of the counter behavior and the update flags for different division
factors in the up-counting mode.

*Figure 10-3 Timing Diagram of Up-counting. The internal clock divider factor = 2/N*


Internal clock
divided by 2                           CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register        0034        0035   0036   0000   0001        0002   0003

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

Internal clock
CK_PSC
divided by N
Timer clock = CK_CNT

Counter register       1F          20                               00

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

*Figure 10-4 Timing Diagram of the up-counting, update event when ARPEN=0/1*


ARPEN = 0                  CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    31      32 33 34 35 36 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register     FF                      36

Change AR value
Write a new value in TIMx_AR

ARPEN = 1
CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    F0      F1 F2 F3 F4 F5 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register      F5                            36

Write a new value in TIMx_AR

Auto-reload shadow register      F5                             36

##### 10.3.2.2. Down-counting mode

In down-counting mode, the counter will decrement from the value of the register TIMx_AR to 0, then restart from the
auto-reload value and generate a counter underflow event.

The process of configuring update events and updating registers in down-counting mode is the same as in up-counting
mode, see 10.3.2.1.

The figure below shows some examples of the counter behavior and the update flags for different division factors in the
down-counting mode.

*Figure 10-5 Timing Diagram of the down-counting, internal clock divided factor = 2/N*


Internal clock divided by
2                                   CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register   0002      0001   0000    0036   0035   0034   0033

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Internal clock divided by
N                                 CK_PSC

Timer clock = CK_CNT

Counter register        20        1F            00             36

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

##### 10.3.2.3. Center-aligned mode

In center-aligned mode, the counter increments from 0 to the value (TIMx_AR) – 1, a counter overflow event is generated.
It then counts down from the auto-reload value (TIMx_AR) to 1 and generates a counter underflow event. Then the counter
resets to 0 and starts counting up again.

In this mode, the TIMx_CTRL1.DIR direction bits have no effect and the count direction is updated and specified by
hardware. Center-aligned mode is valid when the TIMx_CTRL1. CAMSEL bit is not equal to "00".

The update events can be generated each time the counter overflows and each time the counter underflows. Alternatively,
an update event can also be generated by setting the TIMx_EVTGEN. UDGN bit (either by software or using a slave
mode controller). In this case, the counter restarts from 0, as does the prescaler's counter.

> **Note: if the update source is a counter overflow, auto-reload update before reloading the counter.**

*Figure 10-6 Timing Diagram of the Center-aligned, internal clock divided factor =2/N*


Internal clock                      CK_PSC
divided by 2
CNTEN

Timer clock = CK_CNT

Counter register        0003         0002   0001   0000   0001        0002   0003

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Internal clock divided
by N          CK_PSC

Timer clock = CK_CNT

Counter register       20           1F            01                 00

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

*Figure 10-7 A Center-aligned (ARPEN = 1)*


Counter
underflow                              CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register        06        05 04 03 02 01 00 01 02 03 04 05 06 07

Counter underflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register             FD                   36

Write a new value in TIMx_AR
Auto-reload active register          FD                           3
6

Counter                                CNTEN
overflow
CK_PSC

Counter register       F7        F8 F9 FA FB FC 36 35 34 33 32 31 30 2F

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register             FD                   36

Write a new value in TIMx_AR

Auto-reload active register           FD                           36

#### 10.3.3. Clock Selection

•    The internal clock of timers ：CK_INT

•    Two kinds of external clock mode :

−    external input pin

−    external trigger input ETR

•    Internal trigger input (ITRx): one timer is used as a prescaler for another timer.

##### 10.3.3.1. Internal clock source (CK_INT)

When the TIMx_SMCTRL.SMSEL is equal to “000”, the slave mode controller is disabled. The three
control bits (TIMx_CTRL1.CNTEN、TIMx_CTRL1. DIR、 TIMx_EVTGEN. UDGN) can only be changed
by software (except TIMx_EVTGEN. UDGN, which remains cleared automatically ). It is provided that the
TIMx_CTRL1.CNTEN bit is soft- written as' 1 ', the clock source of the prescaler is provided by the internal clock
CK_INT.

*Figure 10-8 Control Circuit in Normal Mode, Internal clock divided by 1*


CEN=CNTEN

Internal clock

UDGN

CNT_INIT

Counter clock = CK_CNT=CK_PSC
61
Counter register                 62 63 64 65 66 00 01 02 03 04 05 06 07

##### 10.3.3.2. External clock source mode 1

*Figure 10-9 TI2 External Clock Connection Example*


TI2
Filter
(TIMx_CCMOD1.ICF[3:0])

Edge Detector

TIMx_SMCTRL.     CK_INT rising
TSEL[2:0]
Polarity Selection                                                           Internal clock mode
（TIMx_CCEN.CC2P）                         ITRx
TI1_ED          TRGI rising             External clock mode 1
TI1FP1
TI2FP2                                                          CK_PSC
ETRF
ETRF rising
External clock mode 2
TI2F rising or falling
or
Encoder mode
TI1F rising or falling
TIMx_SMCTRL.S
MSEL[2:0]
TIMx_SMCTRL.
EXCEN

This mode is selected by configuring TIMx_SMCTRL.SMSEL=111. The counter can be configured to count on the
rising or falling edge of the clock at the selected input.

For example, to configure up-counting mode to count on the rising edge of the clock at the TI2 input, the
configuration steps are as follows:

•    Configure TIMx_CCMOD1.CC2SEL equal to ‘01’, CC2 channel is configured as input, IC2 is mapped to TI2

•    Configure TIMx_CCEN.CC2P equal to ‘0’, select clock rising edge polarity

•    To select input filter bandwidth by configuring TIMx_CCMOD1.IC2F[3:0] (if filter is not needed, keep
IC2F bit at ‘0000’)

•    Configure TIMx_SMCTRL.SMSEL equal to ‘111’, select timer external clock mode 1

•    Configure TIMx_SMCTRL.TSEL equal to ‘110’, select TI2 as the trigger input source

•    Configure TIMx_CTRL1.CNTEN equal to ‘1’ to start the counter

> **Note: The capture prescaler is not used for triggering, so it does not need to be configured**

When the rising edge of the timer clock occurs at TI2=1, the counter counts once and the TIMx_STS .TITF flag is
pulled high.

The delay between the rising edge of TI2 and the actual clock of the counter depends on the resynchronization
circuit at the input of TI2.

*Figure 10-10 Control Circuit in External Clock Mode 1*


CNTEN

TI2

Timer clock = CK_CNT=CK_PSC

Counter register                 64                                 65                   66

TITF

Write TITF=0

#### 10.3.4. Capture/Compare Channels

Capture/compare channels include capture/compare registers and shadow registers. The input section consists of digital
filters, multiplexers and prescalers. The output section includes comparators and output controls.

The input signal TIx is sampled and filtered to generate the signal TIxF. A signal (TIxF_rising or TIxF_falling) is then
generated by the edge detector of the polarity select function, the polarity of which is selected by the TIMx_CCEN.CCxP
bits. This signal can be used as a trigger input for the slave mode controller. At the same time, the signal ICx is sent to the
capture register after frequency division. The following figure shows a block diagram of a capture/compare channel.

*Figure 10-11 Capture/Compare Channel (example: channel 1 input stage)*


From slave mode controller    TRC

TI2FP1                                  Divider
TI2F_Rising                                                       /1,/2,/4,/8
From channel 2                                                          IC1                           IC1PSC
TI1FP1
TI2F_Falling                                                      TIMx_CCMOD1.
Polarity Selection                                                   IC1PSC[1:0]
TIMx_CCEN.CC2P             TIMx_CCMOD1.CC1SEL[3:0]              TIMx_CCEN.CC1EN
Filter Down
counter
TIMx_CCMOD1.IC1F[
3:0]                           Edge Detector
TI1F_Rising
TI1                     TI1F                                                  To the slave
TI1F_Falling
fDTS                                              Polarity Selection
mode controller
TIMx_CCEN.CC1P

TI1F_ED

The output part generates an intermediate waveform OCxRef (active high) as reference. The polarity acts at the end
of the chain.

*Figure 10-12 Capture/Compare Channel 1 Main Circuit*


CC1SEL[1] CC1SEL[0]                              IC1PSC CC1EN
Input
mode

Read CCDAT1H
S                       TIM1_EVTGEN.CC1GN

Read CCDAT1L       R

Read in
APB Bus                                         progress

MCU Peripheral interface

16 bit
High 8-bits        Capture/                              Capture/
transfer
compare                               compare                          Counter
preload register                       shadow register
Low 8-bits

Output                             Comparator

Write CCDAT1H
mode
S     Write in
progress
CNT>CCDAT1
Write CCDAT1L                                                                                                 CNT=CCDAT1
R

CC1SEL[0] CC1SEL[1]                                 TIM1_CCMOD1.OC1PEN
UEV
From time base unit

*Figure 10-13 Output Part of Channelx (x = 1,2,3,4;take channel 4 as an example)*


To the master mode controller

Output enable
Polarity        circuit
selection
TIM1_CCEN.
CC4P
OC4
ETRF          Ocref_clr                                                               TIM1_CCEN.CC4EN
OC4 REF                               0
CNT=CCDAT4
CNT>CCDAT4                                                1
Output mode
controller
TIM1_CCMOD2.OC2M
D[2:0]

Reads and writes always access preloaded registers when capture/compare. The two specific operating processes are as
follows:

In capture mode, the capture is actually done in the shadow register, and then the value in the shadow register is copied

into the preload register.

In compare mode, as opposed to capture mode, the value of the preload register is copied into the shadow register, which is
compared with the counter.

#### 10.3.5. Input Capture Mode

In capture mode, the TIMx_CCDATx registers are used to latch the counter value after the ICx signal detects.

There is a capture interrupt flag TIMx_STS.CCxITF, which can issue an interrupt or DMA request if the corresponding
interrupt enable is pulled high.

The TIMx_STS. CCxITF bit is set by hardware when a capture event occurs and is cleared by software or by reading the
TIMx_CCDATx register.

The over capture flag TIMx_STS.CCxOCF is set equal to 1 when the counter value is captured in the TIMx_CCDATx
register and TIMx_STS.CC1ITF is already pulled high. Unlike the former, TIMx_STS.CCxOCF is cleared by writing 0 to it.

To achieve a rising edge of the TI1 input to capture the counter value into the TIMx_CCDAT1 register, the configuration
flow is as follows:

•    To select a valid input:

Configure TIMx_CCMOD1.CC1SEL to ‘01’. At this time, the input is the CC1 channel, and IC1 is mapped to TI1.

•    Program the desired input filter duration:

Define the sampling frequency of the TI1 input and the length of the digital filter by configuring the
TIMx_CCMODx.ICxF bits. Example: If the input signal jitters up to 5 internal clock cycles, we must choose a filter
duration longer than these 5 clock cycles. When 8 consecutive samples (sampled at fDTS frequency) with the new level
are detected, we can validate the transition on TI1. Then configure TIMx_CCMOD1. IC1F to ‘0011’.

•    By configuring TIMx_CCEN .CC1P=0, select the rising edge as the valid transition polarity on the TI1 channel.

•    Configure the input prescaler. In this example, configure TIMx_CCMOD1.IC1PSC= ‘00’ to disable the prescaler
because we want to capture every valid transition.

•    Enable capture by configuring TIMx_CCEN. CC1EN = ‘1’.

If you want to enable DMA request, you can configure TIMx_DINTEN.CC1DEN=1.If you want enable related interrupt
request, you can configureTIMx_DINTEN.CC1IEN bit=1

#### 10.3.6. PWM Input Mode

There are some differences between PWM input mode and normal input capture mode, including:

•    Two ICx signals are mapped to the same TIx input.

•    The two ICx signals are active on edges of opposite polarity.

•    Select one of two TIxFP signals as trigger input.

•    The slave mode controller is configured in reset mode.

For example, the following configuration flow can be used to know the period and duty cycle of the PWM signal on TI1 (It
depends on the frequency of CK_INT and the value of the prescaler).

•     Configure TIMx_CCMOD1.CC1SEL equal to ‘01’ to select TI1 as valid input for TIMx_CCDAT1
•     Configure TIMx_CCEN.CC1P equal to ‘0’ to select the active polarity of filtered timer input 1(TI1FP1), valid on the
rising edge.

•     Configure TIMx_CCMOD1.CC2SEL equal to ‘10’ select TI1 as valid input for TIMx_CCDAT2.

•     Configure TIMx_CCEN.CC2P equal to 1 to select the valid polarity of filtered timer input 2(TI1FP2), valid on the
falling edge.

•     Configure TIMx_SMCTRL.TSEL=101 to select Filtered timer input 1 (TI1FP1) as valid trigger input.

•     Configure TIMx_SMCTRL.SMSEL=100 to configure the slave mode controller to reset mode.

•     Configure TIMx_CCEN. CC1EN=1 and TIMx_CCEN.CC2EN=1 to enable capture.

*Figure 10-14 PWM Input Mode Timing*


TI1

TIMx_CNT          0004       0000        0001      0002         0003        0004     0000

TIMx_CCDAT1
0004

TIMx_CCDAT2                                              0002

IC1 capture                         IC2 capture           IC1 capture
IC2 capture                         Pulse width                Period
Reset counter                       measurement           measurement

Because of only filter timer input 1 (TI1FP1) and filter timer input 2 (TI2FP2) are connected to the slave mode
controller, the PWM input mode can only be used with the TIMx_CH1/TIMx_CH2 signals.

#### 10.3.7. Forced Output Mode

Software can force output compare signals to active or inactive level directly, in output mode
(TIMx_CCMODx.CCxSEL=00).

User can set TIMx_CCMODx. OCxMD=101 to force the output compare signal to active level. And the OCxREF will
be forced high, OCx get opposite value to CCxP polarity bit. On the other hand, user can set TIMx_CCMODx.
OCxMD=100 to force the output compare signal to inactive level.

The values of the TIMx_CCDATx shadow register and the counter still comparing with each other in this mode. And
the flag still can be set. Therefore, the interrupt and DMA requests still can be sent.
The comparison between the output compare register TIMx_CCDATx and the counter TIMx_CNT has no
effect on OCxREF. And the flag still can be set. Therefore, the interrupt and DMA requests still can be sent.

#### 10.3.8. Output Compare Mode

User can use this mode to control the output waveform, or to indicate that a period of time has elapsed.

When the capture/compare register and the counter have the same value, the output compare function’s
operations are as follow:

•    TIMx_CCMODx.OCxMD is for output compare mode, and TIMx_CCEN.CCxP is for output polarity.
When the compare matches, if set TIMx_CCMODx.OCxMD=000, the output pin will keep its level;if
set TIMx_CCMODx.OCxMD=001, the output pin will be set active;if set TIMx_CCMODx.OCxMD=010,
the output pin will be set inactive;if set TIMx_CCMODx.OCxMD=011, the output pin will be set to toggle.

•    Set TIMx_STS.CCxITF.

•    If user set TIMx_DINTEN.CCxIEN, a corresponding interrupt will be generated.

•    If user set TIMx_DINTEN.CCxDEN and set TIMx_CTRL2.CCDSEL to select DMA request, and DMA
request will be sent.

User can set TIMx_CCMODx.OCxPEN to choose capture/compare shawdow regisete using capture/compare
preload registers(TIMx_CCDATx) or not.

The time resolution is one count of the counter.

In one pulse mode, the output compare mode can also be used to output a single

pulse. Here are the configuration steps for output compare mode:

•    First of all, user should select the counter clock.

•    Secondly, set TIMx_AR and TIMx_CCDATx with desired data.

•    If user need to generate an interrupt, set TIMx_DINTEN.CCxIEN.

•    Then select the output mode by set TIMx_CCEN.CCxP, TIMx_CCMODx.OCxMD, TIMx_CCEN.CCxEN, etc.

•    At last, set TIMx_CTRL1.CNTEN to enable the counter.

User can update the output waveform by setting TIMx_CCDATx at any time, as long as the preload register is not
enabled. Otherwise the TIMx_CCDATx shadow register will be updated at the next update event.

Here is an example.

*Figure 10-15 Output Compare Mode, Toggle on OC1*


TIM1_CNT         0069       006A       006B                               8800      8801

TIM1_CCDAT1                            006A                                    8801

Write 8801h in CCDAT1
register

OC1REF=OC1

Match detected on CCDAT1
Interrupt generated if enabled

#### 10.3.9. PWM Mode

User can use PWM mode to generate a signal whose duty cycle is determined by the value of the TIMx_CCDATx
register and whose frequency is determined by the value of the TIMx_AR register. And depends on the
value of TIMx_CTRL1.CAMSEL, the TIM can generate PWM signal in edge-aligned mode or center-aligned
mode.

User can set PWM mode 1 or PWM mode 2 by setting TIMx_CCMODx. OCxMD=110 or setting
TIMx_CCMODx.          OCxMD=111.        To     enable   preload    register,   user     must    set    corresponding
TIMx_CCMODx.OCxPEN. And then set TIMx_CTRL1.ARPEN to auto-reload preload register eventually.

User can set polarity of OCx by setting TIMx_CCEN.CCxP. To enable the output of OCx, user need to set the
combination of the value of CCxEN.

The values of TIMx_CNT and TIMx_CCDATx are always compared with each other when the TIM is under PWM mode.

Only if an update event occurs, the preload register will transfer to the shadow register. Therefore user must reset
all the registers by setting TIMx_EVTGEN.UDGN before the counter starts counting.

##### 10.3.9.1. PWM center-aligned mode

If user set TIMx_CTRL1.CAMSEL equal 01, 10 or 11, the PWM center-aligned mode will be active. The setting
of the compare flag depends on the value of TIMx_CTRL1.CAMSEL. There are three kinds of situation that the
compare flag is set, only when the counter counts up, only when the counter counts down, or when the counter
counts up and counts down. User should not modified TIMx_CTRL1.DIR by software, it is updated by hardware.

Examples of center-aligned PWM waveforms is as follow, and the setting of the waveform are: TIMx_AR=8,
PWM mode 1, the compare flag is set when the counter counts down corresponding to TIMx_CTRL1.
CAMSEL=01.

*Figure 10-16 Center-aligned PWM Waveform (AR=8)*


Notes that user should know when using center-aligned mode are as follow:

•    It depends on the value of TIMx_CTRL1.DIR that the counter counts up or down. Cautions that the DIR and
CAMSEL bits should not be changed at the same time.

•    User should not write the counter while running in center-aligned mode, otherwise it will cause unexpected
results. Here are some example:

−   If the value written into the counter is 0 or is the value of TIMx_AR, the direction will be updated but the
update event will not be generated.

−   If the value written into the counter is greater than the value of auto-reload, the direction will not be updated.

•    To be on the safe side, user is suggested setting TIMx_EVTGEN.UDGN to generate an update by software
before starting the counter, and not writing the counter while it is running.

##### 10.3.9.2. PWM edge-aligned mode

There are two kinds of configuration in edge-aligned mode, up-counting and down-counting.

•    Up-counting

User can set TIMx_CTRL1.DIR=0 to make counter counts up. Here is an example for PWM mode1.

When TIMx_CNT < TIMx_CCDATx, the reference PWM signal OCxREF is high. Otherwise it will be low. If the compare
value in TIMx_CCDATx is greater than the auto-reload value, the OCxREF will remains 1. Conversely, if the compare
value is 0, the OCxREF will remains 0.

When TIMx_AR=8, the PWM waveforms are as follow.

*Figure 10-17 Edge-aligned PWM Waveform (APR=8)*


Counter register                0    1     2   3     4   5   6   7   8   0   1

1
CCDATx>8 OCXREF
CCxIF

CCDATx=8 OCXREF

CCxIF

OCXREF
CCDATx=4

CCxIF

OCXREF 0
CCDATx=0
CCxIF

•    Down-counting

User can set TIMx_CTRL1.DIR=1 to make counter counts down. Here is an example for PWM mode1.

When TIMx_CNT > TIMx_CCDATx, the reference PWM signal OCxREF is low. Otherwise it will be high. If the compare
value in TIMx_CCDATx is greater than the auto-reload value, the OCxREF will remains 1.

> **Note: If the nth PWM cycle CCDATx shadow register >= AR value, the shadow register value of CCDATx in the (n+1)th**
PWM cycle is 0. At the moment when the counter is 0 in the (n+1)th PWM cycle, although the value of the counter =
CCDATx shadow register = 0 and OCxREF = '0', no compare event will be generated.

#### 10.3.10. One-pulse Mode

In the one-pulse mode (ONEPM), a trigger signal is received, and a pulse tPULSE with a controllable pulse width is generated
after a controllable delay tDELAY. The output mode needs to be configured as output compare mode or PWM mode. After
selecting one-pulse mode, the counter will stop counting after the update event UEV is generated.

The following is an example of a one-pulse mode:

A rising edge trigger is detected from the TI2 input, and a pulse with a width of tPULSE is generated on OC1 after a
delay of tDELAY.

•   Counter configuration: count up, counter TIMx_CNT < TIMx_CCDAT1 ≤ TIMx_AR;

•   Counting Down Method: CNT > CCDATx
*Figure 10-18 Example of One-pulse Mode*


TI2

TIMx_AR

TIMx_CCDAT1

Counter

0
t

tDELAY    tPULSE

OCxREF

OC1

For example, it is necessary to detect a rising edge at the TI2 input pin to begin, then after a delay of

tDELAY, generate a positive pulse of length tPULSE on OC1.

Assume TI2FP2 as trigger 1。

•   Set CC2SEL='01' in the TIMx_CCMOD1 register, mapping TI2FP2 to TI2.

•   Set CC2P='0' in the TIMx_CCEN register to enable detection of rising edges on TI2FP2.

•   Set TSEL='110' in the TIMx_SMCTRL register, with TI2FP2 as the trigger (TRGI) for the slave mode controller.

•   Set SMSEL='110' in the TIMx_SMCTRL register (trigger mode), using TI2FP2 to start the counter.

•   The waveform of ONEPM is determined by the value written to the comparison register (considering clock
frequency and counter pre-scaler).

•   tDELAY is defined by the value written to the TIMx_CCDAT1 register.

•   tPULSE is defined by the difference between the auto-reload value and the comparison value
(TIMx_AR - TIMx_CCDAT1).

•   Assuming a waveform from 0 to 1 is generated when a comparison match occurs, and from 1 to 0 when the counter

reaches the preload value; first set OC1M='111' in the TIMx_CCMOD1 register to enter PWM mode 2;
selectively enable the preload register as needed: set OC1PEN='1' in TIMx_CCMOD1 and ARPEN in the
TIMx_CTRL1 register; then fill in the comparison value in the TIMx_CCDAT1 register, fill in the auto-reload
value in the TIMx_AR register, modify the UDGN bit to generate an update event, and then wait for an
external trigger event on TI2. In this example, CC1P='0'.

•   In this example, the DIR and CAMSEL bits in the TIMx_CTRL1 register should be set low.

•   Because only one pulse is needed, set ONEPM='1' in the TIMx_CTRL1 register to stop counting at the next
update event (when the counter flips from the auto-reload value to 0).

##### 10.3.10.1. Special Case: OCx Fast Enable

In one-pulse mode, an edge is detected through the TIx input, and triggers the start of the counter to count to the
comparison value and then output a pulse. These operations limit the minimum delay tDELAY that can be achieved.

You can set TIMx_CCMODx.OCxFEN=1 to turn on OCx fast enable, after triggering the rising edge, the OCxREF
signal will be forced to be converted to the same level as the comparison match occurs immediately, regardless of
the comparison result. OCxFEN fast enable only takes effect when the channel mode is configured for PWM1 and
PWM2 modes.

#### 10.3.11. Encoder Interface Mode

The encoder uses two inputs TI1 and TI2 as an interface and the counter counts on every edge change on TI1FP1 or
TI2FP2. The count direction is automatically controlled by hardware TIMx_CTRL1.DIR. There are three types
of encoder count modes:

•   The counter only counts on the edge of TI1, TIMx_SMCTRL.SMSEL = ‘001’;

•   The counter only counts on the edge of TI2, TIMx_SMCTRL.SMSEL = ‘010’;

•   The counter counts on the edges of TI1 and TI2 at the same time, TIMx_SMCTRL.SMSEL = ‘011’;

The encoder interface is equivalent to using an external clock with direction selection, and the counter only
counts continuously between 0 and the auto-reload value (TIMx_AR.AR [15:0]). Therefore, it is necessary to
configure the auto- reload register TIMx_AR in advance.

> **Note: Encoder mode and external clock mode 2 are not compatible and must not be selected together.**

The relationship between the counting direction and the encoder signal is shown in Error! Reference source not

found.: Table 10-1 Relationship Between Counting Direction And Encoder Signals

Level on Opposite              TI1FP1 Signal               TI2FP2 Signal
Signals
Active Edge
Rising         Falling        Rising         Falling
(TI1FP1 forTI2,

TI2FP2 for TI1)

Counting only at TI1           High              Counting      Counting up    Don't count    Don't count
down

Low             Counting up       Counting     Don't count    Don't count
down

Counting only at TI2            High             Don't count      Don't count        Counting up    Counting
down

Low              Don't count      Don't count         Counting     Counting up
down

Counting on                 High                  Counting    Counting up        Counting up    Counting
down                                           down
TI1 and TI2

Low              Counting up       Counting           Counting     Counting up
down               down

Here is an example of an encoder with dual edge triggering selected to suppress input jitter:

•   IC1FP1 is mapped to TI1 (TIMx_CCMOD1.CC1SEL= ‘01’), IC1FP1 is not inverted (TIMx_CCEN.CC1P= ‘0’);

•   IC1FP2 is mapped to TI2 (TIMx_CCMOD2.CC2SEL= ‘01’), IC2FP2 is not inverted (TIMx_CCEN.CC2P= ‘0’);

•   The input is valid on both rising and falling edges (TIMx_SMCTRL.SMSEL = ‘011’);

•   Enable counter TIMx_CTRL1.CNTEN= ‘1’;

*Figure 10-19 Example of Counter Operation in Encoder Interface Mode*


Forward           Jitter         Backward        Jitter

TI1

TI2

Counter

Up               Down

The following figure shows the example of counter behavior when IC1FP1 polarity is inverted (CC1P=
'1', other configurations are the same as above)

*Figure 9-43 Encoder Interface Mode Example with IC1FP1 Polarity Inverted*


forward           jitter         backward     jitter

TI1

TI2

Counter

down                                up

#### 10.3.12. Timer Input XOR Functionality

By setting the TI1SEL bit in the TIMx_CTRL2 register, it allows the input filter of channel 1 to be connected to the
output of an XOR gate, with three inputs to the XOR gate being TIMx_CH1, TIMx_CH2, and TIMx_CH3.

The XOR output can be used for all timer input functionalities, such as triggering or input capture. An example
of using this feature to connect a Hall sensor was provided in the previous chapter in section 9.3.18."

#### 10.3.13. TIMx and External Trigger Synchronization

Same with advanced-control timer, See Section 9.3.16

#### 10.3.14. Timer Synchronization

All TIMx timers are internally connected to each other. This implementation allows any master timer to provide
trigger to reset, start, stop or provide a clock for the other slave timers. The master clock is used for internal
counter and can be prescaled. Below figure shows a Block diagram of timer interconnection.

The synchronization function does not support dynamic change of the connection. User should configure and
enable the slave timer before enable the master timer’s trigger or clock.

Clock                                                                      TIMx （Slave TIM）
Prescaler

Trigger
Counter
selection

Slave mode control
Master mode control TRGO
TIMx_SMCTRL.SMSEL

TIMx_CTRL2.MMSEL
TIMx_SMCTRL.
TSEL                    CK_PSC

Prescaler

TIMx（Master TIM）                                                              Counter

#### 10.3.15. Debug Mode

When the microcontroller is in debug mode (the Cortex-M0 core halted), depending on the
DBG_CTRL.TIMx_STOP configuration in the PWR module, the TIMx counter can either continue to work
normally or stop. For more details, see Section 4.3.8.

### 10.4. TIMx Register Description(x=3)

For abbreviations used in registers, see section 1.1

These peripheral registers can be operated as half word (16-bits) or one word (32-bits).

#### 10.4.1. Register Overview

*Table 10-2 Register Overview*


Of
Registe
fs
r
et
CAMSEL[1:0]
CLKD[1:0]

TIMx_
CTRL1
Reserved

00
0h
Reset
0           0   0   0                 0      0   0   0    0         0
Value
MMSEL[2:0]
Reserved

Reserved

00 TIMx_
4h CTRL2

Of
Registe
fs
r
et

Reset
0       0               0      0   0
Value

SMSELEL[2:0]
TSEL[2:0]
TIMx_S
MCTRL

Reserved
00
8h
Reset
0   0               0      0                     0     0                   0
Value

TIMx_
DINTE
Reserved

00     N
Ch
Reset
0                      0   0                 0   0                 0                  0                      0   0                 0     0                   0
Value

TIMx_S
TS
Reserved

Reserved
01
0h
Reset
0   0                 0   0                                    0                      0   0                 0     0                   0
Value

TIMx_E
VTGEN
Reserved

01
4h
Reset
0                      0   0                 0     0                   0
Value

TIMx_
CCMO
CC2SEL[1:0]

CC1SEL[1:0]
OC2MD[2:0]

OC1MD[2:0]

D1
Output
Reserved

compar

01     e

8h
Reset
0   0           0          0   0                 0   0                 0          0       0               0      0   0                 0     0                   0
Value
CC2SEL[1:0]

CC1SEL[1:0]
IC2PSC[1:0]

IC1PSC[1:0]

TIMx_
Reserved

IC2F[3:0]

IC1F[3:0]

CCMO
D1

Of
Registe
fs
r
et

Input
capture

Reset
0   0           0          0   0                 0   0                 0   0          0               0      0   0                 0   0                 0
Value

TIMx_
CCMO

CC4SEL[1:0]

CC3SEL[1:0]
OC4MD[2:0]

OC3MD[2:0]
D2
Output
Reserved

01
compar
Ch
e

Reset
0   0           0          0   0                 0   0                 0   0          0               0      0   0                 0   0                 0
Value

TIMx_
CCMO

CC4SEL[1:0]

CC3SEL[1:0]
IC4PSC[1:0]

IC3PSC[1:0]
IC4F[3:0]

IC3F[3:0]
D2
Reserved

01 Input
Ch capture

Reset
0   0           0          0   0                 0   0                 0   0          0               0      0   0                 0   0                 0
Value

TIMx_
CCEN
Reserved

Reserved

Reserved

Reserved

02
0h
Reset
0          0                         0                 0                              0      0                         0                 0
Value

TIMx_
CNT[15:0]
CNT
02
Reserved
4h
Reset
0   0           0          0   0                 0   0                 0   0          0               0      0   0                 0   0                 0
Value

TIMx_P
PSC[15:0]
SC
02
Reserved
8h
Reset
0   0           0          0   0                 0   0                 0   0          0               0      0   0                 0   0                 0
Value

Of
Registe
fs
r
et

TIMx_A
02                                                               AR[15:0]
R
Ch             Reserved
Reset
1   1   1   1   1   1   1    1   1     1   1   1   1   1   1   1
Value

03
Reserved
0h

TIMx_
CCDAT                                                   CCDAT1[15:0]
03     1
Reserved
4h
Reset
0   0   0   0   0   0   0    0   0     0   0   0   0   0   0   0
Value

TIMx_
CCDAT                                                   CCDAT2[15:0]
03     2
Reserved
8h
Reset
0   0   0   0   0   0   0    0   0     0   0   0   0   0   0   0
Value

TIMx_
CCDAT                                                   CCDAT3[15:0]
03     3
Reserved
Ch
Reset
0   0   0   0   0   0   0    0   0     0   0   0   0   0   0   0
Value

TIMx_
CCDAT                                                   CCDAT4[15:0]
04     4
Reserved
0h
Reset
0   0   0   0   0   0   0    0   0     0   0   0   0   0   0   0
Value

04
Reserved
4h

Of
Registe
fs
r
et

TIMx_
DBLEN[4:0]                                DBADDR[4:0]
DCTRL

Reserved

Reserved
04
8h
Reset
0   0   0   0      0                      0    0   0   0    0
Value

TIMx_
DADD                                                                                  BURST[15:0]
04     R
Reserved
Ch
Reset
0   0   0   0   0   0   0      0   0    0         0   0    0   0   0    0
Value

#### 10.4.2. Control Register 1 (TIMx_CTRL1)

Offset address: 0x00

Reset value: 0x0000 0000

Bit Field     Name          Description

31:10         Reserved      Reserved, the reset value must be maintained

9:8           CLKD[1:0]     Clock division

CLKD[1:0] indicates the division ratio between CK_INT (timer clock) and tDTS
(clock used for dead-time generator and digital filters (ETR, TIx))

00: tDTS = tCK_INT

01: tDTS = 2 × tCK_INT

10: tDTS = 4 × tCK_INT

11: Reserved, do not use this configuration

7             ARPEN         ARPEN: Auto-reload preload enable

Bit Field   Name          Description

0: Shadow register disable for TIMx_AR register

1: Shadow register enable for TIMx_AR register

6:5         CAMSEL[1:0]   Center-aligned mode selection

00: Edge-aligned mode. TIMx_CTRL1.DIR specifies up-counting or down-counting.

01: Center-aligned mode 1. The counter counts in center-aligned mode, and the
output compare interrupt flag bit is set to 1 when down-counting.

10: Center-aligned mode 2. The counter counts in center-aligned mode, and the
output compare interrupt flag bit is set to 1 when up-counting.

11: Center-aligned mode 3. The counter counts in center-aligned mode, and the
output compare interrupt flag bit is set to 1 when up-counting or down-counting.

> **Note: Switching from edge-aligned mode to center-aligned mode is not allowed when**
the counter is still enabled (TIMx_CTRL1.CNTEN = 1).

4           DIR           Direction

0: Up-counting

1: Down-counting

> **Note: This bit is read-only when the counter is configured in center-aligned mode or**
encoder mode.

3           ONEPM         One-pulse mode

0: Disable one-pulse mode, the counter counts are not affected when an update event
occurs.

1: Enable one-pulse mode, the counter stops counting when the next update event
occurs (clearing TIMx_CTRL1.CNTEN bit)

2           UPRS          Update request source

This bit is used to select the UEV event sources by software.

0: If update interrupt or DMA request is enabled, any of the following events will
generate an update interrupt or DMA request:

–    Counter overflow/underflow

–    The TIMx_EVTGEN.UDGN bit is set

–    Update generation from the slave mode controller

1: If update interrupt or DMA request is enabled, only counter overflow/underflow
will generate update interrupt or DMA request

Bit Field     Name          Description

1             UPDIS         Update disable

This bit is used to enable/disable the Update event (UEV) events generation by
software.

0: Enable UEV. And UEV will be generated if one of following condition is
fulfilled:

–     Counter overflow/underflow

–     The TIMx_EVTGEN.UDGN bit is set

–     Update generation from the slave mode controller

Shadow registers will update with preload value.

1: UEV disabled. No update event is generated, and the shadow registers (AR, PSC,
and CCDATx) keep their values. If the TIMx_EVTGEN.UDGN bit is set or a
hardware reset is issued by the slave mode controller, the counter and prescaler are
reinitialized.

0             CNTEN         Counter Enable

0: Disable counter

1: Enable counter

> **Note: external clock, gated mode and encoder mode can only operate after**
TIMx_CTRL1.CNTEN bit is set in the software. Trigger mode can automatically set
TIMx_CTRL1.CNTEN bit by hardware.

#### 10.4.3. Control Register 2 (TIMx_CTRL2)

Offset address: 0x04

Reset value: 0x0000

Bit Field     Name         Description

15:8          Reserved     Reserved, the reset value must be maintained

7             TI1SEL       TI1 selection

0: TIMx_CH1 pin connected to TI1 input.

1: TIMx_CH1, TIMx_CH2, and TIMx_CH3 pins are XOR connected to the TI1 input.

6:4           MMSEL[2:0]   Master Mode Selection

Bit Field     Name       Description

These 3 bits (TIMx_CTRL2. MMSEL [2:0]) are used to select the synchronization
information (TRGO) sent to the slave timer in the master mode. Possible
combiNSING are as follows:

000: Reset –When the TIMx_EVTGEN.UDGN is set or a reset is generated by the
slave mode controller, a TRGO pulse occurs. And in the latter case, the signal on
TRGO is delayed compared to the actual reset.

001: Enable - The TIMx_CTRL1.CNTEN bit is used as the trigger output (TRGO).
Sometimes you need to start multiple timers at the same time or enable slave timer for
a period of time.

The counter enable signal is set when TIMx_CTRL1.CNTEN bit is set or the trigger
input in gated mode is high.

When the counter enable signal is controlled by the trigger input, there is a delay on
TRGO except if the master/slave mode is selected (see the description of the
TIMx_SMCTRL.MSMD bit).

010: Update - The update event is selected as the trigger output (TRGO). For example,
a master timer clock can be used as a slave timer prescaler.

011: Compare pulse - Triggers the output to send a positive pulse (TRGO) when the
TIMx_STS.CC1ITF is to be set (even if it is already high), when a capture or a
comparison succeeds.

100: Compare - OC1REF signal is used as the trigger output (TRGO).

101: Compare - OC2REF signal is used as the trigger output (TRGO).

110: Compare - OC3REF signal is used as the trigger output (TRGO).

111: Compare - OC4REF signal is used as the trigger output (TRGO).

3             CCDSEL     Capture/compare DMA selection

0: When a CCx event occurs, a DMA request for CCx is sent.

1: When an update event occurs, a DMA request for CCx is sent.

2:0           Reserved   Reserved, the reset value must be maintained

#### 10.4.4. Slave Mode Control Register (TIMx_SMCTRL)

Offset address: 0x08

Reset value: 0x0000

Bit Field   Name         Description

15:8        Reserved     Reserved, the reset value must be maintained

7           MSMD         Master/ Slave mode

0: No action

1: Events on the trigger input (TRGI) are delayed to allow a perfect synchronization
between the current timer (via TRGO) and its slaves. This is useful when several timers
are required to be synchronized to a single external event.

6:4         TSEL[2:0]    Trigger selection

These 3 bits are used to select the trigger input of the synchronous counter.

000: Internal trigger 0 (ITR0) 100: TI1 edge detector (TI1F_ED)

001: Internal trigger 1 (ITR1) 101: Filtered timer input 1(TI1FP1)

010: Internal trigger 2 (ITR2) 110: Filtered timer input 2 (TI2FP2)

011: Internal trigger 3 (ITR3) 111: External triggered Input (ETRF)

For more details on ITRx, see Table 10-3 below.

> **Note: These bits must be changed only when not in use (e. g.**
TIMx_SMCTRL.SMSEL=000) to avoid false edge detection at the transition.

3           Reserved     Reserved, the reset value must be maintained

2:0         SMSEL[2:0]   Slave mode selection

When an external signal is selected, the active edge of the trigger signal (TRGI) is
linked to the selected external input polarity (see input control register and control
register description)

000: Disable slave mode. If TIMx_CTRL1.CNTEN = 1, the prescaler is driven directly
by the internal clock.

001: Encoder mode 1. According to the level of TI2FP2, the counter up-counting or
down-counting on the edge of TI1FP1.

010: Encoder mode 2. According to the level of TI1FP1, the counter up-counting or
down-counting on the edge of TI2FP2.

011: Encoder mode 3. According to the input level of another signal, the counter up-
counting or down-counting on the edges of TI2FP1 and TI2FP2.

100: Reset mode. On the rising edge of the selected trigger input (TRGI), the counter is
reinitialized and the shadow register is updated.

Bit Field   Name   Description

101: Gated mode. When the trigger input (TRGI) is high, the clock of the counter is
enabled. Once the trigger input becomes low, the counter stops counting, but is not
reset. In this mode, the start and stop of the counter are controlled.

110: Trigger mode. When a rising edge occurs on the trigger input (TRGI), the counter
is started but not reset. In this mode, only the start of the counter is controlled.

111: External clock mode 1. The counter is clocked by the rising edge of the selected
trigger input (TRGI).

> **Note: Do not use gated mode if TI1F_ED is selected as the trigger input**
(TIMx_SMCTRL.TSEL=100). This is because TI1F_ED outputs a pulse for each TI1F
transition, whereas gated mode checks the level of the triggered input.

*Table 10-3 TIMx Internal Trigger Connection*


Slave timer       ITR0 (TSEL = 000)     ITR1 (TSEL = 001)        ITR2 (TSEL = 010)   ITR3 (TSEL = 011)

TIM3                TIM1                     NA                     NA                  NA

#### 10.4.5. DMA/Interrupt Enable Registers (TIMx_DINTEN)

Offset address: 0x0C

Reset value: 0x0000

Bit Field   Name           Description

15          Reserved       Reserved, the reset value must be maintained

14          TDEN           Trigger DMA request enable

0: Disable trigger DMA request

1: Enable trigger DMA request

13          Reserved       Reserved, the reset value must be maintained

12          CC4DEN         Capture/Compare 4 DMA request enable

0: Disable capture/compare 4 DMA request

1: Enable capture/compare 4 DMA request

11          CC3DEN         Capture/Compare 3 DMA request enable

0: Disable capture/compare 3 DMA request

1: Enable capture/compare 3 DMA request

10          CC2DEN         Capture/Compare 2 DMA request enable

0: Disable capture/compare 2 DMA request

1: Enable capture/compare 2 DMA request

9           CC1DEN         Capture/Compare 1 DMA request enable

0: Disable capture/compare 1 DMA request

1: Enable capture/compare 1 DMA request

Bit Field   Name        Description

8           UDEN        Update DMA request enable

0: Disable update DMA request

1: Enable update DMA request

7           Reserved    Reserved, the reset value must be maintained

6           TIEN        Trigger interrupt enable

0: Disable trigger interrupt

1: Enable trigger interrupt

5           Reserved    Reserved, the reset value must be maintained

4           CC4IEN      Capture/Compare 4 interrupt enable

0: Disable capture/compare 4 interrupt

1: Enable capture/compare 4 interrupt

3           CC3IEN      Capture/Compare 3 interrupt enable

0: Disable capture/compare 3 interrupt

1: Enable capture/compare 3 interrupts

2           CC2IEN      Capture/Compare 2 interrupt enable

0: Disable capture/compare 2 interrupt

1: Enables capture/compare 2 interrupts

1           CC1IEN      Capture/Compare 1 interrupt enable

0: Disable capture/compare 1 interrupt

1: Enables capture/comparing 1 interrupt

0           UIEN        Update interrupt enable

0: Disable update interrupt

1: Enables update interrupt

#### 10.4.6. Status Registers (TIMx_STS)

Offset address: 0x10

Reset value: 0x0000

Bit Field   Name       Description

15:13       Reserved   Reserved, the reset value must be maintained

12          CC4OCF     Capture/Compare 4 over capture flag

See TIMx_STS.CC1OCF description.

11          CC3OCF     Capture/Compare 3 over capture flag

See TIMx_STS.CC1OCF description.

10          CC2OCF     Capture/Compare 2 over capture flags

See TIMx_STS.CC1OCF description.

9           CC1OCF     Capture/Compare 1 over capture flag

This bit is set by hardware only when the corresponding channel is configured in input
capture mode. Cleared by software writing 0.

0: No over capture occurred

1: TIMx_STS.CC1ITF was already set when the value of the counter has been
captured in the TIMx_CCDAT1 register.

8:7         Reserved   Reserved, the reset value must be maintained

6           TITF       Trigger interrupt flag

This bit is set by hardware when an active edge is detected on the TRGI input when
the slave mode controller is in a mode other than gated. This bit is set by hardware
when any edge in gated mode is detected. This bit is cleared by software.

0: No trigger event occurred

1: Trigger interrupt occurred

5           Reserved   Reserved, the reset value must be maintained

4           CC4ITF     Capture/Compare 4 interrupt flag

See TIMx_STS.CC1ITF description.

3           CC3ITF     Capture/Compare 3 interrupt flag

See TIMx_STS.CC1ITF description.

2           CC2ITF     Capture/Compare 2 interrupt flag

See TIMx_STS.CC1ITF description.

1           CC1ITF     Capture/Compare 1 interrupt flag

Bit Field   Name        Description

When the corresponding channel of CC1 is in output mode:

Except in center-aligned mode, this bit is set by hardware when the counter value is
the same as the compare value (see TIMx_CTRL1.CAMSEL bit description). This bit
is cleared by software.

0: No match occurred.

1: The value of TIMx_CNT is the same as the value of TIMx_CCDAT1.

When the value of TIMx_CCDAT1 is greater than the value of TIMx_AR, the
TIMx_STS.CC1ITF bit will go high if the counter overflows (in up-counting and
up/down-counting modes) and underflows in down-counting mode.

When the corresponding channel of CC1 is in input mode:

This bit is set by hardware when the capture event occurs. This bit is cleared by
software or by reading TIMx_CCDAT1.

0: No input capture occurred.

1: Input capture occurred. Counter value has captured in the TIMx_CCDAT1. An edge
with the same polarity as selected has been detected on IC1.

0           UDITF       Update interrupt flag

This bit is set by hardware when an update event occurs under the following
conditions:

–    When TIMx_CTRL1.UPDIS = 0, overflow or underflow (An update event is
generated).

–    When TIMx_CTRL1.UPRS = 0, TIMx_CTRL1.UPDIS = 0, and set the
TIMx_EVTGEN.UDGN bit by software to reinitialize the CNT.

–    When TIMx_CTRL1.UPRS = 0, TIMx_CTRL1.UPDIS = 0, and the counter
CNT is reinitialized by the trigger event. (See TIMx_SMCTRL Register
description)

This bit is cleared by software.

0: No update event occurred

1: Update interrupt occurred

#### 10.4.7. Event Generation Registers (TIMx_EVTGEN)

Offset address: 0x14

Reset values: 0 x0000

Bit Field   Name       Description

15：7        Reserved   Reserved, the reset value must be maintained.

6           TGN        Trigger generation

This bit can generate a trigger event when set by software. And at this time
TIMx_STS.TITF = 1, if the corresponding interrupt and DMA are enabled, the
corresponding interrupt and DMA will be generated. This bit is automatically cleared
by hardware.

0: No action

1: Generated a trigger event

5           Reserved   Reserved, the reset value must be maintained

4           CC4GN      Capture/Compare 4 generation

See TIMx_EVTGEN.CC1GN description.

3           CC3GN      Capture/Compare 3 generation

See TIMx_EVTGEN.CC1GN description.

2           CC2GN      Capture/Compare 2 generation

See TIMx_EVTGEN.CC1GN description.

1           CC1GN      Capture/Compare 1 generation

This bit can generate a capture/compare event when set by software. This bit is
automatically cleared by hardware.

When the corresponding channel of CC1 is in output mode:

The TIMx_STS.CC1ITF flag will be pulled high, if the corresponding interrupt and
DMA are enabled, the corresponding interrupt and DMA will be generated.

When the corresponding channel of CC1 is in input mode:

TIMx_CCDAT1 will capture the current counter value, and the TIMx_STS.CC1ITF
flag will be pulled high, if the corresponding interrupt and DMA are enabled, the
corresponding interrupt and DMA will be generated. If The IMx_STS.CC1ITF is
already pulled high, pull TIMx_STS.CC1OCF high.

0: No action

1: Generated a CC1 capture/compare event

0           UDGN       Update generation

This bit can generate an update event when set by software. And at this time the counter
will be reinitialized, the prescaler counter will be cleared, the counter will be cleared in center-

Bit Field   Name                Description

aligned or up-counting mode, but take TIMx_AR in down-counting mode the value of the
register. This bit is automatically cleared by hardware.

0: No action

1: Generated an update event

#### 10.4.8. Capture/Compare Mode Register 1 (TIMx_CCMOD1)

Offset address: 0x18

Reset value: 0x0000

Channels can be used for input (capture mode) or output (compare mode), and the direction of the channel is
defined by the corresponding CCxSEL bit. The other bits of the register act differently in input and output modes.
OCx describes the function of a channel in output mode, ICx describes the function of a channel in input mode.
Hence, please note that the same bit can have different meanings for output mode and for input mode.

Output compare mode:

Bit Field     Name               Description

15            OC2CEN             Output Compare 2 clear enable

14:12         OC2MD[2:0]         Output Compare 2 mode

11            OC2PEN             Output Compare 2 preload enable

10            OC2FEN             Output Compare 2 fast enable

9:8           CC2SEL[1:0]        Capture/compare 2 selection

These bits are used to select the input/output and input mapping of the channel

00: CC2 channel is configured as output

01: CC2 channel is configured as input, IC2 is mapped on TI2

10: CC2 channel is configured as input, IC2 is mapped on TI1

11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC2SEL is writable only when the channel is off (TIMx_CCEN.CC2EN = 0).**

7             OC1CEN             Output Compare 1 clear enable

0: OC1REF is not affected by ETRF input level

Bit Field   Name         Description

1: OC1REF is cleared immediately when the ETRF input level is detected as high

6:4         OC1MD[2:0]   Output Compare 1 mode

These bits are used to manage the output reference signal OC1REF, which
determines the values of OC1 and OC1N, and is valid at high levels, while the active
levels of OC1 and OC1N depend on the TIMx_CCEN.CC1P and
TIMx_CCEN.CC1NP bits.

000: Frozen. Comparison between TIMx_CCDAT1 register and counter TIMx_CNT
has no effect on OC1REF signal.

001: Set channel 1 to the active level on match. When TIMx_CCDAT1 =
TIMx_CNT, OC1REF signal will be forced high.

010: Set channel 1 as inactive level on match. When TIMx_CCDAT1 = TIMx_CNT,
OC1REF signal will be forced low.

011: Toggle. When TIMx_CCDAT1 = TIMx_CNT, OC1REF signal will be toggled.

100: Force to inactive level. OC1REF signal is forced low.

101: Force to active level. OC1REF signal is forced high.

110: PWM mode 1 - In up-counting mode, if TIMx_CNT < TIMx_CCDAT1,
OC1REF signal of channel 1 is high, otherwise it is low. In down-counting mode, if
TIMx_CNT > TIMx_CCDAT1, OC1REF signal of channel 1 is low, otherwise it is
high.

111: PWM mode 2 - In up-counting mode, if TIMx_CNT < TIMx_CCDAT1,
OC1REF signal of channel 1 is low, otherwise it is high. In down-counting mode, if
TIMx_CNT > TIMx_CCDAT1, OC1REF signal of channel 1 is high, otherwise it is
low.

Note 1: In PWM mode 1 or PWM mode 2, the OC1REF level changes only when the
comparison result changes or when the output compare mode is switched from
frozen mode to PWM mode.

3           OC1PEN       Output Compare 1 preload enable

0: Disable preload function of TIMx_CCDAT1 register. Supports write operations to
TIMx_CCDAT1 register at any time, and the written value is effective immediately.

1: Enable preload function of TIMx_CCDAT1 register. Only read and write
operations to preload registers. When an update event occurs, the value of
TIMx_CCDAT1 is loaded into the active register.

Note 1: Only when TIMx_CTRL1.ONEPM = 1(In one-pulse mode), PWM mode can
be used without verifying the preload register, otherwise no other behavior can be

Bit Field      Name             Description

predicted.

2              OC1FEN           Output Compare 1 fast enable

This bit is used to speed up the response of the CC output to the trigger input event.

0: CC1 behaves normally depending on the counter and CCDAT1 values, even if the
trigger is ON. The minimum delay for activating CC1 output when an edge occurs
on the trigger input is 5 clock cycles.

1: An active edge of the trigger input acts like a comparison match on CC1 output.
Therefore, OC is set to the comparison level regardless of the comparison result. The
delay time for sampling the trigger input and activating the CC1 output is reduced to
3 clock cycles.

OCxFEN only works if the channel is configured in PWM1 or PWM2 mode.

1：0            CC1SEL[1:0]      Capture/compare 1 selection

These bits are used to select the input/output and input mapping of the channel

00: CC1 channel is configured as output

01: CC1 channel is configured as input, IC1 is mapped on TI1

10: CC1 channel is configured as input, IC1 is mapped on TI2

11: CC1 channels are configured as inputs and IC1 is mapped to TRC. This mode is
only active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC1SEL is writable only when the channel is off (TIMx_CCEN.CC1EN = 0).**

Input capture mode:

Bit field    Name             Description

15:12        IC2F[3:0]        Input Capture 2 Filter

Bit field   Name          Description

11:10       IC2PSC[1:0]   Input Capture 2 Prescaler

9:8         CC2SEL[1:0]   Capture/Compare 2 selection

These bits are used to select the input/output and input mapping of the channel

00: CC2 channel is configured as output

01: CC2 channel is configured as input, IC2 is mapped on TI2

10: CC2 channel is configured as input, IC2 is mapped on TI1

11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC2SEL is writable only when the channel is off (TIMx_CCEN.CC2EN = 0).**

7:4         IC1F[3:0]     Input Capture 1 filter

These bits are used to define sampling frequency of TI1 input and the length of digital
filter. The digital filter is an event counter that generates an output transition after N
events are recorded.

0000: No filter, sampling at fDTS frequency

0001: fSAMPLING = fCK_INT, N = 2

0010: fSAMPLING = fCK_INT, N = 4

0011: fSAMPLING = fCK_INT, N = 8

0100: fSAMPLING = fDTS/2, N = 6

0101: fSAMPLING = fDTS/2, N = 8

0110: fSAMPLING = fDTS/4, N = 6

0111: fSAMPLING = fDTS/4, N = 8

1000: fSAMPLING = fDTS/8, N = 6

1001: fSAMPLING = fDTS/8, N = 8

1010: fSAMPLING = fDTS/16, N = 5

1011: fSAMPLING = fDTS/16, N = 6

1100: fSAMPLING = fDTS/16, N = 8

1101: fSAMPLING = fDTS/32, N = 5

1110: fSAMPLING = fDTS/32, N = 6

1111: fSAMPLING = fDTS/32, N = 8

3:2         IC1PSC[1:0]   Input Capture 1 prescaler

Bit field    Name               Description

These bits are used to select the ratio of the prescaler for IC1 (CC1 input).

When TIMx_CCEN.CC1EN = 0, the prescaler will be reset.

00: No prescaler, capture is done each time an edge is detected on the capture input

01: Capture is done once every 2 events

10: Capture is done once every 4 events

11: Capture is done once every 8 events

1:0          CC1SEL[1:0]        Capture/Compare 1 selection

These bits are used to select the input/output and input mapping of the channel

00: CC1 channel is configured as output

01: CC1 channel is configured as input, IC1 is mapped on TI1

10: CC1 channel is configured as input, IC1 is mapped on TI2

11: CC1 channel is configured as input, IC1 is mapped to TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC1SEL is writable only when the channel is off (TIMx_CCEN.CC1EN = 0).**

#### 10.4.9. Capture/Compare Mode Register 2 (TIMx_CCMOD2)

Offset address: 0x1C

Reset value: 0x0000

See the description of the CCMOD1 register

above Output comparison mode:

Bit Field      Name             Description

15             OC4CEN           Output compare 4 clear enable

14:12          OC4MD[2:0]       Output compare 4 mode

11             OC4PEN           Output compare 4 preload enable

10             OC4FEN           Output compare 4 fast enable

9:8            CC4SEL[1:0]      Capture/Compare 4 selection

These bits are used to select the input/output and input mapping of the channel

Bit Field      Name           Description

00: CC4 channel is configured as output

01: CC4 channel is configured as input, IC4 is mapped on TI4

10: CC4 channel is configured as input, IC4 is mapped on TI3

11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC4SEL is writable only when the channel is off (TIMx_CCEN.CC4EN = 0).**

7              OC3CEN         Output compare 3 clear enable

6:4            OC3MD[2:0]     Output compare 3 mode

3              OC3PEN         Output compare 3 preload enable

2              OC3FEN         Output compare 3 fast enable

1:0            CC3SEL[1:0]    Capture/Compare 3 selection

These bits are used to select the input/output and input mapping of the channel

00: CC3 channel is configured as output

01: CC3 channel is configured as input, IC3 is mapped to TI3

10: CC3 channel is configured as input, IC3 is mapped on TI4

11: CC3 channel is configured as input, IC3 is mapped to TRC. This mode is only
active when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC3SEL is writable only when the channel is off (TIMx_CCEN.CC3EN = 0).**

Input capture mode:

Bit Field Name                Description

15:12        IC4F[3:0]        Input Capture 4 filter

11:10        IC4PSC[1:0]      Input Capture 4 Prescaler

9:8          CC4SEL[1:0]      Capture/Compare 4 selection

These bits are used to select the input/output and input mapping of the channel

00: CC4 channel is configured as output

Bit Field Name             Description

01: CC4 channel is configured as input, IC4 is mapped on TI4

10: CC4 channel is configured as input, IC4 is mapped on TI3

11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC4SEL is writable only when the channel is off (TIMx_CCEN.CC4EN = 0).**

7:4          IC3F[3:0]     Input Capture 3 filter

3:2          IC3PSC[1:0]   Input Capture 3 Prescaler

1:0          CC3SEL[1:0]   Capture/compare 3 selection

These bits are used to select the input/output and input mapping of the channel

00: CC3 channel is configured as output

01: CC3 channel is configured as input, IC3 is mapped to TI3

10: CC3 channel is configured as input, IC3 is mapped on TI4

11: CC3 channel is configured as input, IC3 is mapped to TRC. This mode is only active
when the internal trigger input is selected by TIMx_SMCTRL.TSEL.

> **Note: CC3SEL is writable only when the channel is off (TIMx_CCEN.CC3EN = 0).**

#### 10.4.10. Capture/Compare Enable Registers (TIMx_CCEN)

Offset address: 0x20

Reset value: 0x0000

Bit Field    Name         Description

15:14        Reserved     Reserved, the reset value must be maintained.

13           CC4P         Capture/Compare 4 output polarity

See TIMx_CCEN.CC1P description.

12           CC4EN        Capture/Compare 4 output enable

See TIMx_CCEN.CC1EN description.

11:10        Reserved     Reserved, the reset value must be maintained

Bit Field   Name       Description

9           CC3P       Capture/Compare 3 output polarity

See TIMx_CCEN.CC1P description.

8           CC3EN      Capture/Compare 3 output enable

See TIMx_CCEN.CC1EN description.

7:6         Reserved   Reserved, the reset value must be maintained

5           CC2P       Capture/Compare 2 output polarity

See TIMx_CCEN.CC1P description.

4           CC2EN      Capture/Compare 2 output enable

See TIMx_CCEN.CC1EN description.

3:2         Reserved   Reserved, the reset value must be maintained

1           CC1P       Capture/Compare 1 output polarity

When the corresponding channel of CC1 is in output mode:

0: OC1 active high

1: OC1 active low

When the corresponding channel of CC1 is in input mode:

At this time, this bit is used to select whether IC1 or the inverse signal of IC1 is used as
the trigger or capture signal.

0: non-inverted: Capture action occurs when IC1 generates a rising edge. When used as
external trigger, IC1 is non-inverted.

1: inverted: Capture action occurs when IC1 generates a falling edge. When used as
external trigger, IC1 is inverted.

> **Note: If TIMx_BKDT.LCKCFG = 3 or 2, these bits cannot be modified.**

0           CC1EN      Capture/Compare 1 output enable

When the corresponding channel of CC1 is in output mode:

0: Disable - Disable output OC1 signal.

1: Enable - Enable output OC1 signal.

When the corresponding channel of CC1 is in input mode:

At this time, this bit is used to disable/enable the capture function.

0: Disable capture

Bit Field   Name             Description

1: Enable capture

*Table 10-4 Output control bits of standard OCx channel*


CCxEN                        OCx output status

0          Disable output (OCx=0)

1          OCx = OCxREF + polarity

> **Note: The state of external I/O pins connected to standard OCx channels depends on the OCx channel state and**
GPIO and AFIO registers.

#### 10.4.11. Counters (TIMx_CNT)

Offset address: 0x24

Reset value: 0x0000

Bit Field   Name               Description

15:0        CNT[15:0]          Counter value

#### 10.4.12. Prescaler (TIMx_PSC)

Offset address: 0x28

Reset value: 0x0000

Bit Field   Name               Description

15:0        PSC[15:0]          Prescaler value

Counter clock fCK_CNT = fCK_PSC/ (PSC [15:0] +1).

Each time an update event occurs, the PSC value is loaded into the active prescaler
register.

#### 10.4.13. Auto-reload Register (TIMx_AR)

Offset address: 0x2C Reset

values: 0xFFFF

Bit Field    Name            Description

15:0         AR[15:0]        Auto-reload value

These bits define the value that will be loaded into the actual auto-reload register.

When the TIMx_AR.AR [15:0] value is null, the counter does not work.

#### 10.4.14. Capture/Compare Register 1 (TIMx_CCDAT1)

Offset address: 0x34

Reset value: 0x0000

Bit         Name            Description
Field

15:0        CCDAT1[15:0]    Capture/Compare 1 value

1) CC1 channel is configured as output:

CCDAT1 contains the value to be compared to the counter TIMx_CNT, signaling on the
OC1 output.

If the preload feature is not selected in TIMx_CCMOD1.OC1PEN bit, the written value
is immediately transferred to the active register. Otherwise, this preloaded value is
transferred to the active register only when an update event occurs.

2) CC1 channel is configured as input:

CCDAT1 contains the counter value transferred by the last input capture 1 event (IC1).

When configured as input mode, register CCDAT1 is only readable.

When configured as output mode, register CCDAT1 is readable and writable.

#### 10.4.15. Capture/Compare Register 2 (TIMx_CCDAT2)

Offset address: 0x38

Reset value: 0x0000

Bit Field   Name           Description

15:0        CCDAT2[15:0]   Capture/Compare 2 values

3) CC2 channel is configured as output:

CCDAT2 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC2 output.

If the preload feature is not selected in TIMx_CCMOD1.OC2PEN bit, the written
value is immediately transferred to the active register. Otherwise, this preloaded value
is transferred to the active register only when an update event occurs.

4) CC2 channel is configured as input:

CCDAT2 contains the counter value transferred by the last input capture 2 event (IC2).

When configured as input mode, register CCDAT2 is only readable.

When configured as output mode, register CCDAT2 is readable and writable.

#### 10.4.16. Capture/Compare Register 3 (TIMx_CCDAT3)

Offset address: 0x3C

Reset value: 0x0000

Bit Field   Name           Description

15:0        CCDAT3[15:0]   Capture/Compare 3 value

5) CC3 channel is configured as output:

CCDAT3 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC3 output.

If the preload feature is not selected in TIMx_CCMOD2.OC3PEN bit, the written
value is immediately transferred to the active register. Otherwise, this preloaded value
is transferred to the active register only when an update event occurs.

6) CC3 channel is configured as input:

CCDAT3 contains the counter value transferred by the last input capture 3 event
(IC3).

Bit Field   Name           Description

When configured as input mode, register CCDAT3 is only readable.

When configured as output mode, register CCDAT3 is readable and writable.

#### 10.4.17. Capture/Compare Register 4 (TIMx_CCDAT4)

Offset address: 0x40

Reset value: 0x0000

Bit Field   Name           Description

15:0        CCDAT4[15:0]   Capture/Compare 4 value

7) CC4 channel is configured as output:

CCDAT4 contains the value to be compared to the counter TIMx_CNT, signaling on
the OC4 output.

If the preload feature is not selected in TIMx_CCMOD2.OC4PEN bit, the written
value is immediately transferred to the active register. Otherwise, this preloaded value
is transferred to the active register only when an update event occurs.

8) CC4 channel is configured as input:

CCDAT4 contains the counter value transferred by the last input capture 4 event
(IC4).

When configured as input mode, register CCDAT4 is only readable.

When configured as output mode, register CCDAT4 is readable and writable.

#### 10.4.18. DMA Control Register (TIMx_DCTRL)

Offset address: 0x48

Reset value: 0x0000

Bit Field    Name          Description

15:13        Reserved      Reserved, the reset value must be maintained

Bit Field    Name          Description

12:8         DBLEN[4:0]    DMA Burst Length

This bit field defines the number DMA will accesses (write/read) TIMx_DADDR
register.

00000:1 time transfer

00001: 2 times transfers

00010: 3 times transfers

…

10001: 18 times transfers

7:5          Reserved      Reserved, the reset value must be maintained.

4:0          DBADDR[4:0]   DMA Base Address

This bit field defines the first address where the DMA accesses the TIMx_DADDR
register.

When access is done through the TIMx_DADDR first time, this bit-field specifies the
address you just access. And then the second access to the TIMx_DADDR, you will
access the address of “DMA Base Address + 4”

00000: TIMx_CTRL1,

00001: TIMx_CTRL2,

00010: TIMx_SMCTRL,

…

#### 10.4.19. DMA Transfer Buffer Register (TIMx_DADDR)

Offset address: 0x4C

Reset value: 0x0000

Bit Field    Name          Description

15:0         BURST[15:0]   DMA access buffer.

When a read or write operation is assigned to this register, the register located at the address
range (DMA base address + DMA burst length × 4) will be accessed.

Bit Field     Name              Description

DMA base address = The address of TIM_CTRL1 + TIMx_DCTRL. DBADDR * 4；

DMA burst len = TIMx_DCTRL.DBLEN + 1.

Example:

If TIMx_DCTRL.DBLEN = 0x3(4 transfers), TIMx_DCTRL.DBADDR = 0xD
(TIMx_CCDAT1), DMA data length = half word, DMA memory address = buffer address in
SRAM, DMA peripheral address = TIMx_DADDR address.

When an event occurs, TIMx will send requests to the DMA, and transfer data 4 times.

For the first time, DMA access to the TIMx_ DADDR register will be mapped to access
TIMx_CCDAT1 register;

For the second time, DMA access to the TIMx_ DADDR register will be mapped to access
TIMx_CCDAT2 register;

……

For the fourth time, DMA access to the TIMx_ DADDR register will be mapped to access
TIMx_CCDAT4 register;

## 11. Basic Timers (TIM6)


### 11.1. Introduction

The basic timer (TIM6) contains a 16-bit auto-reload counter, driver by a programmable prescaler. It can provide
a time base for general timers. The basic timers are independent form each other do not share any resources.

### 11.2. Main Features

•       A 16-bit auto-reload up-counting counters.

•       A 16-bit programmable prescaler. (The frequency division factor can be configured with any value
between 1 and 65536)

•       Generation of an interrupt/DMA request on an update event (counter overflow)

*Figure 11-1 Block Diagram of TIMx（x = 6）*


PSC
Trigger controller                                       prescaler       CK_CNT

Internal clock(CK_INT)
TIMxCLK
from RCC

Reset, enable, count
CK_PSC                                                        counter(up-counting)       UDIT

Controller                                                                      Stop, clear or up

Update event
Update event
auto-reload register

The event                  Interrupt and DMA

### 11.3. Basic Timers Description


#### 11.3.1. Time-base Unit

The time-base unit mainly includes: prescaler, counter and auto-reload. When the time base unit is operating, the
software can read and write the corresponding registers (TIMx_PSC, TIMx_CNT and TIMx_AR) at any time.

Depending on the setting of the auto-reload preload enable bit (TIMx_CTRL1.ARPEN), the value of the preload
register is transferred to the shadow register immediately or at each update event UEV. An update event is
generated when the counter reaches the overflow condition and it can be generated by software when
TIMx_CTRL1.UPDIS=0. The counter CK_CNT is valid only when the TIMx_CTRL1.CNTEN bit is set. The
counter starts counting one clock cycle after the TIMx_CTRL1.CNTEN bit is set.

##### 11.3.1.1. Prescaler description

The TIMx_PSC register consists of a 16-bit counter that can be used to divide the counter clock frequency by any
factor between 1 and 65536. It can be changed dynamically at the runtime as it is buffered. The prescaler value is
only taken into account at the next update event.

*Figure 11-2 Counter Timing Diagram With Prescaler Division Change From 1 to 4*


CNTEN

CK_PSC

Timer Clock = CK_CNT

Counter register    87       88 89 8A 8B 8C               00                   01

Update event（UEV）

Prescaler controller register     0                               3

Write a new value in TIMx_PSC

Prescaler counter                     0           0    1    2   3   0       1    2   3

Prescaler buffer                     0                                 3

#### 11.3.2. Counter Mode


##### 11.3.2.1. Up-counting mode

In up-counting mode, the counter will count from 0 to the value of the register TIMx_AR, then it resets to 0. And a counter
overflow event is generated.

If the TIMx_CTRL1.UPRS bit (select update request) and the TIMx_EVTGEN.UDGN bit are set, an update event (UEV)
will generate, and TIMx_STS.UDITF will not be set by hardware. Therefore, no update interrupts or update DMA requests
are generated. This setting is used in scenarios where you want to clear the counter but do not want to generate an update
interrupt.

Depending on the update request source is configured in the TIMx_CTRL1.UPRS, When an update event occurs, all
registers are updated and the TIMx_STS.UDITF is set:

•    Update auto-reload shadow registers with preload value(TIMx_AR), when TIMx_CTRL1.ARPEN = 1.

•    The prescaler shadow register is reloaded with the preload value(TIMx_PSC)

To avoid updating the shadow registers when new values are written to the preload registers, you can disable the update by
setting TIMx_CTRL1.UPDIS=1.

When an update event occurs, the counter will still be cleared and the prescaler counter will also be set to 0 (but the
prescaler value will remain unchanged).

The figure below shows some examples of the counter behavior and the update flags for different division factors in the
up-counting mode.

*Figure 11-3 Timing Diagram of Up-counting. The internal clock divider factor = 2/N*


Internal clock
divided by 2                           CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register        0034    0035   0036   0000   0001        0002   0003

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

Internal clock
divided by N                       CK_PSC

Timer clock = CK_CNT

Counter register       1F          20                           00

Counter overflow

Update interrupt flag(UDITF)

Update event(UEV)

*Figure 11-4 Timing Diagram of the up-counting, update event when ARPEN=0/1*


ARPEN = 0                  CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    31      32 33 34 35 36 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register     FF                     36

Change AR value
Write a new value in TIMx_AR

ARPEN = 1
CNTEN

CK_PSC

Timer clock = CK_CNT

Counter register    F0      F1 F2 F3 F4 F5 00 01 02 03 04 05 06 07

Counter overflow

Update event（UEV）

Update interrupt flag(UDITF)

Auto-reload preload register      F5                           36

Write a new value in TIMx_AR

Auto-reload shadow register      F5                            36

#### 11.3.3. Clock Selection

The internal clock of timers: CK_INT

##### 11.3.3.1. Internal clock source (CK_INT)

It is provided that the TIMx_CTRL1.CNTEN bit is written as' 1 ' by software, the clock source of the prescaler is
provided by the internal clock CK_INT.

*Figure 11-5 Control Circuit in Normal Mode, Internal Clock Divided by 1*


CEN=CNTEN

Internal clock

UDGN

CNT_INIT

Counter clock = CK_CNT=CK_PSC
61
Counter register          62 63 64 65 66 00 01 02 03 04 05 06 07

#### 11.3.4. Debug Mode

When the microcontroller is in debug mode (the Cortex-M0 core halted), depending on the
DBG_CTRL.TIMx_STOP configuration in the PWR module, the TIMx counter can either continue to work
normally or stop. For more details, see Section 4.3.18.

### 11.4. TIMx Register Description(x=6)

For abbreviations used in registers, see section 1.1

These peripheral registers can be operated as half word (16-bits) or one word (32-bits).

#### 11.4.1. Register Overview

*Table 11-1 Register Overview*


Offs
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12

10
11

9
8
7
6
5
4
3
2
1
0
et

TIMx_CT

ONEPM
ARPEN

CNTEN
UPDIS
UPRS
000      RL1

Reserved

Reserved
h      Reset
0                                   0       0              0         0
Value

MMSEL[2:0]
TIMx_CT

004     RL2

Reserved

Reserved
h
Reset
0      0             0
Value

008
Reserved

h

UDEN
00C TIMx_DINTEN

UIEN
Reserved

Reserved
h
Reset Value                                                                                                                                          0                                                                         0

UDITF
010     TIMx_STS
Reserved

h
Reset Value                                                                                                                                                                                                                    0

UDGN
014 TIMx_EVTGEN
Reserved

h
Reset Value                                                                                                                                                                                                                    0

018
Reserved
h

01C
Reserved
h

020
Reserved
h

024     TIMx_CNT                                                                                                                                            CNT[15:0]
Reserved
h      Reset Value                                                                                                        0    0    0    0    0   0    0    0      0      0      0            0       0       0          0            0

TIMx_PSC                                         Reserved                                                                                           PSC[15:0]

Offs
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12

10
11

9
8
7
6
5
4
3
2
1
0
et

028
Reset Value                                                                                     0    0    0    0    0   0    0   0   0 0     0   0   0 0     0   0
h

02C       TIMx_AR                                                                                                                         AR[15:0]
Reserved
h       Reset Value                                                                                     0    0    0    0    0 0      0   0   0   0   0   0   0   0   0   0

#### 11.4.2. Control Register 1 (TIMx_CTRL1)

Offset address: 0x00

Reset value: 0x0000

Bit Field                  Name                      Description

15:8                       Reserved                  Reserved, the reset value must be maintained

7                          ARPEN                     ARPEN: Auto-reload preload enable

0: Shadow register disable for TIMx_AR register

1: Shadow register enable for TIMx_AR register

6:4                        Reserved                  Reserved, the reset value must be maintained

3                          ONEPM                     One-pulse mode

0: Disable one-pulse mode, the counter counts are not affected when an update event occurs.

1: Enable one-pulse mode, the counter stops counting when the next update event occurs
(clearing TIMx_CTRL1.CNTEN bit)

2                          UPRS                      Update request source

This bit is used to select the UEV event sources by software.

0: If update interrupt or DMA request is enabled, any of the following events will generate an
update interrupt or DMA request:

Counter overflow

The TIMx_EVTGEN.UDGN bit is set

1: If update interrupt or DMA request is enabled, only counter overflow will generate update
interrupt or DMA request

Bit Field      Name            Description

1              UPDIS           Update disable

This bit is used to enable/disable the Update event (UEV) events generation by software.

0: Enable UEV. UEV will be generated if one of following condition been fulfilled:

Counter overflow

The TIMx_EVTGEN.UDGN bit is set

Shadow registers will update with preload value.

1: UEV disabled. No update event is generated, and the shadow registers (AR, PSC) keep their
values. If the TIMx_EVTGEN.UDGN bit is set, the counter and prescaler are reinitialized.

0              CNTEN           Counter Enable

0: Disable counter

1: Enable counter

#### 11.4.3. Control Register 2 (TIMx_CTRL2)

Offset address: 0x04

Reset value: 0x0000

Bit Field     Name         Description

15:7          Reserved     Reserved, the reset value must be maintained

6:4           MMSEL[2:0]   Master mode selection

These bits are used to select the synchronization signal (TRGO) sent to the slave timer in master
mode, and there are the following combiNSING:

000: Reset – use the UDGN bit of the TIMx_EVTGEN register as trigger output (TRGO). If a
reset is generated by the trigger input (the slave mode controller is configured in reset mode), the
signal on TRGO is delayed with respect to the actual reset signal.

001: Enable – The counter enable signal CNT_EN is used as trigger output (TRGO). It can be
used to start multiple timers at the same time, or to control the timing of enabling slave timers.
The counter enable signal is generated by the 'logic OR' of the CNTEN control bit and the trigger
input when configured in gated mode.

When the counter enable signal is controlled by the trigger input, there is some delay on the
TRGO output, unless master/slave mode is selected (see MSMD bit of TIMx_SMCTRL register).

Bit Field     Name       Description

010: Update – The update event is used as a trigger output (TRGO). For example a master timer
can be used as a prescaler for the slave timer.

3:0           Reserved   Reserved, the reset value must be maintained

#### 11.4.4. DMA/Interrupt Enable Registers (TIMx_DINTEN)

Offset address: 0x0C

Reset value: 0x0000

Bit Field     Name       Description

15:9          Reserved   Reserved, the reset value must be maintained

8             UDEN       Update DMA Request enable

0: Disable update DMA request

1: Enable update DMA request

7:1           Reserved   Reserved, the reset value must be maintained

0             UIEN       Update interrupt enable

0: Disable update interrupt

1: Enables update interrupt

#### 11.4.5. Status Registers (TIMx_STS)

Offset address: 0x10

Reset value: 0x0000

Bit Field     Name         Description

15:1          Reserved     Reserved, the reset value must be maintained

Bit Field     Name        Description

0             UDITF       Update interrupt flag

This bit is set by hardware when an update event occurs under the following conditions:

When TIMx_CTRL1.UPDIS = 0, and counter value overflow.

When TIMx_CTRL1.UPRS = 0, TIMx_CTRL1.UPDIS = 0, and set the
TIMx_EVTGEN.UDGN bit by software to reinitialize the CNT.

This bit is cleared by software.

0: No update event occurred

1: Update interrupt occurred

#### 11.4.6. Event Generation registers (TIMx_EVTGEN)

Offset address: 0x14

Reset values: 0 x0000

Bit Field     Name        Description

15：1          Reserved    Reserved, the reset value must be maintained.

0             UDGN        UDGN: Update generation

Software can set this bit to update configuration register value and hardware will clear it
automatically.

0: No effect.

1: Timer counter will restart and all shadow register will be updated. It will restart prescaler
counter also.

#### 11.4.7. Counters (TIMx_CNT)

Offset address: 0x24

Reset value: 0x0000

Bit Field     Name        Description

15:0          CNT[15:0]   Counter value

#### 11.4.8. Prescaler (TIMx_PSC)

Offset address: 0x28

Reset value: 0x0000

Bit Field        Name           Description

15:0             PSC[15:0]      Prescaler value

PSC register value will be updated to prescaler register at update event. Counter clock frequency
is input clock frequency divide PSC + 1.

#### 11.4.9. Automatic reload register (TIMx_AR)

Offset address: 0x2C

Reset values: 0xFFFF

Bit Field        Name           Description

15:0             AR[15:0]       Auto-reload value

These bits define the value that will be loaded into the actual auto-reload register.

See 11.3.1 for more details.

When the TIMx_AR.AR [15:0] value is null, the counter does not work.

## 12. Independent Watchdog (IWDG)


### 12.1. Introduction

The N32WB03x has built-in independent watchdog (IWDG) and window watchdog (WWDG) timers to solve the problems
caused by software errors. Watchdog timer is very flexible to use, which improves the safety of the system and the accuracy of
timing control.

Independent Watchdog (IWDG) is driving by Low-speed internal clock (LSI clock) running at 30 KHz, which will still
running event dead loop or MCU stuck is happening. This can provide higher safety level, timing accuracy and flexibility of
watchdog. It can reset and resolve system malfunctions due to software failure. The IWDG is best suited for applications that
require the watchdog to run as a totally independent process outside the main application, but have lower timing
accuracy constraints.

When the power control register PWR_CTRL2.IWDGRSTEN bit is '1' and the IWDG counter reaches 0, a system reset
will be generated (if this bit is '0', the IWDG will count but not reset).

> **Note: This chapter is based on the system default value IWDGRSTEN = 1.**

### 12.2. Main Features

•    A 12-bit down-counter that runs independently

•    RC oscillator provides an independent clock source and can work normally in STOP mode

•    Reset can be matched.

•    A system reset occurs when the down counter reaches 0x0000(if watchdog activated)

### 12.3. Function Description

*Figure 12-1 Functional Block Diagram Of The Independent Watchdog Module*


LSI
User Program                     IWDG_PREDIV.PD
30KHz
4/8/16/32/64/128/256

IWDG_KEY.KEYV == 0x5555

IWDG_STS.PVU

IWDG_RELV.REL                            12 Bit            Counter == 0
IWDG Reset
12-bit reload value                  Down Counter

IWDG_STS.CRVU

Reload              Enable

IWDG_KEY.KEYV

To enable IWDG, we need to write 0xCCCC to IWDG_KEY.KEYV[15:0] bits. Counter starts counting down from reset
value (0xFFF). When counter count to 0x000, it generates a reset signal (IWDG_RESET) to MCU. Other than that, as long
as 0xAAAA (reload request) is write to IWDG_KEY.KEYV[15:0] bits before reset, the counter value is set to the reload
value in the IWDG_RELV.REL[11:0] bits and prevents the watchdog from resetting the entire device.

If the "hardware watchdog timer" function is enabled through the selection byte, the watchdog will automatically start
running after the system is powered on and will generate a system reset, unless the software reloads the counter before it
reaches ‘0’.

#### 12.3.1. Register Access Protection

IWDG_PREDIV and IWDG_RELV register are write protected. To modify the value of those two register, user
needs to write 0x5555 to IWDG_KEY.KEYV[15:0] bits. Writing other value enables write protections again.
IWDG_STS.PVU indicates whether the pre-scaler value update is on going. IWDG_STS.CRVU indicates whether
the IWDG is updating the reload value. The hardware sets the IWDG_STS.PVU bit and/or IWDG_STS.CRVU bit
when the pre-scaler value and/or reload value is updating. After the pre-scaler value and/or reload value
update is complete, the hardware clears the IWDG_STS.PVU bit and/or IWDG_STS.CRVU bit.

The reload operation (IWDG_KEY.KEYV[15:0] configured with value of 0xAAAA) will also cause the
registers to become write protected again.

12.3.2    Debug mode

In debug mode (Cortex-M0 core stops), IWDG counter will either continue to work normally or stops,
depending on DBG_CTRL.IWDG_STOP bit in debug module. If this bit is set to ‘1’, the counter stops. The
counter works normally when the bit is ‘0’.

### 12.4. User Interface

IWDG module user interface contains 4 registers: Key Register (IWDG_KEY), Pre-scale Register
(IWDG_PREDIV), Reload Register (IWDG_RELV) and Status Register (IWDG_STS).

#### 12.4.1. Operate Flow

When IWDG is enable from reset from software (write 0xAAAA to IWDG_KEY.KEYV[15:0] bits) or
hardware (clear WDG_SW bit). It starts counting down from 0xFFF. Down counting gap is determined by pre-
scale LSI clock. Once the counter is reloaded, each new round will start from the value in
IWDG_RELV.REL[11:0] instead of 0xFFF.

When program is running normally, software needs to feed IWDG before counter reaches 0 and start a new round
of down counting. When counter reach 0, this indicates program malfunction. IWDG generates reset signal under
this circumstance.

If user wants to configure IWDG pre-scale and reload value register, it needs to write 0x5555 to
IWDG_KEY.KEYV[15:0]           first.   Then   confirm     IWDG_STS.CRVU          bit   and    IWDG_STS.PVU   bit.
IWDG_STS.CRVU bit indicates reload value update is ongoing, IWDG_STS.PVU indicates Pre-scale divider
ratio is updating. Only when those two bit are 0 then user can update corresponding value. When update is
on-going, hardware sets corresponding bit to 1. At this time, reading IWDG_PREDIV.PD[2:0] or
IWDG_RELV.REL[11:0] is invalid since data needs sync to LSI clock domain. The value read from
IWDG_PREDIV.PD[2:0] or IWDG_RELV.REL[11:0] will be valid after hardware clears the IWDG_STS.PVU bit
or IWDG_STS.CRVU bit.

If the application uses more than one reload value or pre-scaler value, it must wait until the IWDG_STS.CRVU bit
is reset before changing the reload value, the same as changing the pre-scaler value. However, after updating the
pre-scale and/or the reload value, it is not necessary to wait until IWDG_STS.CRVU bit or IWDG_STS.PVU bit
are reset before continuing code execution.

Pre-scale register and reload register controls the time that generates reset, as shown in Table 12-1.

*Table 12-1 IWDG Counting Maximum And Minimum Reset Time*


Min Timeout (ms)                                       Max Timeout (ms)
Pre-Scale Factor                        PD[2:0]
REL [11:0]=0x000                                       REL [11:0]=0xFFF

/4                                  000                                           0.1                                                      409.6

/8                                  001                                           0.2                                                      819.2

/ 16                                 010                                           0.4                                                     1638.4

/ 32                                 011                                           0.8                                                     3276.8

/ 64                                 100                                           1.6                                                     6553.6

/ 128                                 101                                           3.2                                                     13107.2

/ 256                                 11x                                           6.4                                                     26214.4

### 12.5. IWDG Registers

#### 12.5.1. IWDG Register Overview

*Table 12-2 IWDG Register Overview*

Offs
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10

et
9
8
7
6
5
4
3
2
1
0
IWDG_KEY                                                                                                                         KEYV[15:0]
0x00                                                    Reserved
Reset value                                                                                     0    0    0    0    0    0   0     0   0      0     0   0   0   0      0      0

IWDG_PRE
PD[2:0]
DIV
0x04                                                                                   Reserved

Reset value                                                                                                                                                     0      0      0

IWDG_REL
REL[11:0]
V
0x08                                                              Reserved
Reset value                                                                                                         1    1   1     1   1      1     1   1   1   1      1      1
CRVU

IWDG_STS
PVU

0x0C                                                                                        Reserved
Reset value                                                                                                                                                            0      0

#### 12.5.2. IWDG key register (IWDG_KEY)

Address offset: 0x00

Reset value: 0x00000000

Bit Field   Name         Description

31:16       Reserved     Reserved, the reset value must be maintained.

15:0        KEYV[15:0]   Key value register: only certain value will serve particular function

0xCCCC: Start watch dog counter, does not have any effect if hardware watchdog is enable, (if
hardware watchdog is selected, it is not limited by this command word)

0xAAAA: Reload counter with REL value in IWDG_RELV register to prevent reset.

0x5555: Disable write protection of IWDG_PREDIV and IWDG_RELV register

#### 12.5.3. IWDG Pre-scaler Register (IWDG_PREDIV)

Address offset: 0x04

Reset value: 0x00000000

Bit Field   Name         Description

31:3        Reserved     Reserved, the reset value must be maintained.

Bit Field   Name         Description

2:0         PD[2:0]      Pre-frequency division factor
Pre-scaler divider: with write access protection when IWDG_KEY.KEYV[15:0] is not 0x5555. The
IWDG_STS.PVU bit must be 0 otherwise PD [2:0] value cannot be changed. Divide number is as
follow:
000: divider /4

001: divider /8

010: divider /16

011: divider /32

100: divider /64

101: divider /128

Other : divider /256
> **Note: Reading this register will return the pre-divided value from the VDD voltage domain. If a write**
operation is in progress, the read-back value may be invalid. Therefore, the read value is valid only
when the IWDG_STS.PVU bit is ‘0’.

#### 12.5.4. IWDG Reload Register (IWDG_RELV)

Address offset: 0x08

Reset value: 0x00000FFF

Bit Field   Name         Description

31:12       Reserved     Reserved, the reset value must be maintained.

11:0        REL[11:0]    Watchdog counter reload value.
With write protection. Defines the reload value of the watchdog counter, which is loaded to the
counter every time 0xAAAA is written to IWDG_KEY.KEYV[15:0] bits. The counter then starts to
count down from this value. The watchdog timeout period can be calculated from this reloading value
and the clock pre-scaler value, refer to Table 12-1.

This register can only be modified when the IWDG_STS.CRVU bit is ‘0’.
> **Note: Reading this register will return the reload value from the VDD voltage domain. If a write**
operation is in progress, the read-back value may be invalid. Therefore, the read value is valid only
when the IWDG_STS.CRVU bit is ‘0’.

#### 12.5.5. IWDG Status Register (IWDG_STS)

Address offset: 0x0C

Reset value: 0x00000000

Bit Field     Name            Description

31:2          Reserved        Reserved, the reset value must be maintained.

1             CRVU            Watchdog reload value update
Reload Value Update: this bit indicates an update of reload value is ongoing. Set by hardware and
clear by hardware. Software can only try to change IWDG_RELV.REL[11:0] value when
IWDG_KEY.KEYV[15:0] bits’ value is 0x5555 and this bit is 0.

0             PVU             Watchdog pre-scaler value update
Pre-scaler Value Update: this bit indicates an update of pre-scaler value is ongoing. Set by hardware
and clear by hardware. Software can only try to change IWDG_PREDIV.PD[2:0] value when
IWDG_KEY.KEYV[15:0] bits’ value is 0x5555 and this bit is 0.

## 13. Window Watchdog (WWDG)


### 13.1. Introduction

The clock of the window watchdog (WWDG) is obtained by dividing the APB1 clock frequency by 4096, and whether the
program operation is abnormal is detected through the configuration of the time window. Therefore, WWDG is suitable
for precise timing, and is often used to monitor software failures caused by external disturbances or unforeseen logic
conditions that cause an application to deviate from its normal operating sequence. A system reset occurs when the WWDG
down counter is refreshed before reaching the window register value or after the WWDG_CTRL.T6 bit becomes 0.

### 13.2. Main Features

R7-bit independent down counter programmable

•       After WWDG is enabled, a reset occurs under the following conditions

−       The value of the decremented counter is less than 0x40.

−       When the decremented counter value is greater than the value of the window register, it is reloaded.

•    Early wake-up interrupt: If the watchdog is started and the interrupt is enabled, wake-up
interrupt (WWDG_CFG.EWINT) will be generated when the count value reaches 0x40.

### 13.3. Function Description

If the watchdog is activated (the WWDG_CTRL.ACTB bit), when the 7-bit (WWDG_CTRL.T[6:0]) down-counter
reaches 0x3F(WWDG_CTRL.T6 bit is cleared), or the software reloads the counter when the counter value is
greater than the value of the window register, a system reset will be generated. In order to avoid system reset,
the software must periodically refresh the counter value in the window during normal operation.

*Figure 13-1 Watchdog Block Diagram*


PCLK1

WWDG_CFG.TIMERB[1:0]
1/2/4/8

WWDG Control Register(WWDG_CTRL)
T[6:0] 7-bit Down-counter value                  T[6:0] > W[6:0]
COMP

Comparing results = 1

WWDG Config Register(WWDG_CFG)
W[7:0] 7-bit window value
Reset

Write WWDG_CTRL

WWDG_CTRL.T6

WWDG_CTRL.ACTB = 1

Set the WWDG_CTRL.ACTB bit to enable the watchdog, and thereafter, the WWDG will remain on until reset
occurs. The 7-bit down-counter runs independently, and the counter keeps counting down whether WWDG is
enabled or not. Therefore, before enabling the watchdog, you need to set WWDG_CTRL.T [6] bit to 1, preventing
reset right after enable. The pre-scaler value set by the clock APB1 and WWDG_CFG.TIMERB[1:0] bits
determine the decrement speed of the counter. WWDG_CFG.W[6:0] bits set the upper limit of the window.

When the down-counter is refreshed before reaching the window register value or after WWDG_CTRL.T6 bit
becomes 0, a system reset will be generated. Figure 13-2 describes the working process of the window register.

Set the WWDG_CFG.EWINT bit to enable early wake-up interrupt. When the count-down counter reaches
0x40, an interrupt will be generated. You can analyze the cause of software failure or save important data in
the corresponding interrupt service routine (ISR), and reload the counter to prevent WWDG from resetting.
Write '0' to the WWDG_STS.EWINTF bit to clear the interrupt.

### 13.4. Timing For Refresh Watchdog And Interrupt Generation

*Figure 13-2 Refresh Window And Interrupt Timing Of WWDG*


CNT DownCounter
Refresh not allowed        Refresh allowed

T[6:0]

W[6:0]

0x3F

Time

TPCLK × 4096 × 2TIMERB

0x41
0x40

0x3F

WWDG_EWINT

EWINTF = 0
Reset
T[6] value

Watchdog refreshing window is between WWDG_CFG.W[6:0] value (maximum value 0x7F) and 0x3F, refresh
outside this window will generates reset request to MCU. Counter count down from 0x7F to 0x3F using scaled
APB1 clock, the maximum counting time and minimum counting time is shown in Table 13-1 (assuming
APB1 clock 48 MHz) with calculate equation:

T𝑊𝑊𝐷𝐺=𝑇𝑃𝐶𝐿𝐾1×4096×2𝑇𝐼𝑀𝐸𝑅𝐵×(𝑇[5:0]+1)
In which:

TWWDG: WWDG timeout TPCLK1:APB1 clock interval in ms

Minimum-maximum timeout value at PCLK1 = 48MHz

*Table 13-1 Maximum And Minimum Counting Time Of WWDG*


Min Timeout value(μs)                                        Max Timeout Value(ms)
TIMERB
T[5:0] = 0x00                                                  T[5:0] = 0x3F

0                                                     85.33                                                         5.46

1                                                     170.67                                                        10.92

2                                                     341.33                                                        21.85

3                                                     682.67                                                        43.68

### 13.5. Debug Mode

In debug mode (Cortex-M0 core stops), WWDG counter will either continue to work normally or stops,
depending on DBG_CTRL.WWDG_STOP bit in debug module. If this bit is set to ‘1’, the counter stops. The
counter works normally when the bit is ‘0’. For details, see Peripheral Debug Support.

### 13.6. User Interface


#### 13.6.1. 13.6.1 WWDG Configuration Flow

•      Configure RCC_APB1PCLKEN.WWDGEN[11] bit to enable the clock of WWDG module;

•      Software setting WWDG_CFG.TIMERB[8:7] bits to configure pre-scale factor for WWDG.

•      Software configure WWDG_CTRL.T[6:0] bits, setting starting value of counter. Need to set
WWDG_CTRL.T[6] bit to 1, preventing reset right after enable.

•      Configure WWDG_CFG.W[6:0] bits to configure upper boundary window value;

•      Setting WWDG_CTRL.ACTB[7] bit to enable WWDG;

•      Software operates WWDG_STS.EWINTF[0] bit to clear wake-up interrupt flag;

•      Configure WWDG_CFG.EWINT[9] bit to enable early wake-up interrupt.

### 13.7. WWDG Registers


#### 13.7.1. WWDG Register Overview

*Table 13-2 WWDG Register Overview*

Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10

set
9
8
7
6
5
4
3

2
1
0

WWDG_
ACTB

T[6:0 ]
000 CTRL
d
Reserve
h
Reset
0      1    1   1     1       1   1   1
Value

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
set

9
8
7
6
5
4
3
2
1
0
WWDG_                                                                                                                            TIMERB

EWINT
W[6:0]
004 CFG                                                                                                                                 [1:0]
Reserved
h    Reset
0       0    0   1   1   1     1      1   1   1
Value

WWDG_

EWINTF
008 STS
Reserved
h    Reset
0
Value

#### 13.7.2. WWDG Control Register (WWDG_CTRL)

Address offset : 0x00

Reset value : 0x0000007F

Bit Field          Name                Description

31:8               Reserved            Reserved, the reset value must be maintained.

7                  ACTB                Activation bit
When ACTB=1, the watchdog can generate a reset. This bit is set by software and only cleared by
hardware after a reset. When ACTB = 1, the watchdog can generate a reset.
0: Disable watchdog
1: Enable watchdog

6:0                T[13:0]             These bits contain the value of the watchdog counter. It is decremented every (4096x2TIMERB) PCLK1
cycles. A reset is produced when it rolls over from 0x40 to 0x3F (T6 becomes cleared).

#### 13.7.3. WWDG Config Register (WWDG_CFG)

Address offset: 0x04

Reset value : 0x0000007F

Bit Field   Name          Description

31:10       Reserved      Reserved, the reset value must be maintained.

9           EWINT         Early wake-up interrupt
When set, an interrupt occurs whenever the counter reaches the value 0x40. This interrupt is only
cleared by hardware after a reset.

8:7         TIMERB[1:0]   Timer base.

The time base of the pre-scaler can be modified as follows:
00: CK Counter Clock (PCLK1 div 4096) div 1

01: CK Counter Clock (PCLK1 div 4096) div 2

10: CK Counter Clock (PCLK1 div 4096) div 4

11: CK Counter Clock (PCLK1 div 4096) div 8

6:0         W[6:0]        7-bit window value
These bits contain the window value to be compared to the down counter.

#### 13.7.4. WWDG Status Register (WWDG_STS)

Address offset: 0x08

Reset value : 0x0000

Bit Field   Name          Description

31:1        Reserved      Reserved, the reset value must be maintained.

0           EWINTF        Early wake-up interrupt flag

This bit is set by hardware when the counter has reached the value 0x40. It must be cleared by
software by writing ‘0’. A write of ‘1’ has no effect. This bit is also set if the interrupt is not enabled.

## 14. BLE Subsystem


### 14.1. Introduction To BLE Subsystem

The BLE subsystem includes 3 components, BLE Baseband, RF Control and Modem.

#### 14.1.1. Introduction to Baseband

Specify the basic radio frequency parameters of BLE communication, including signal frequency, modulation scheme, and
frequency modulation mechanism;

Control the state of the device, including Standby, Adverting, Scan, Initating and Connected; Provide a standard interface for

communication between Host and Controller;

Provide data encapsulation service for the upper layer; Provide pair and key distribution for secure data exchange;

#### 14.1.2. Introduction to RF Control

The commands of SETMODE1M, SETMODE2M, SETMODELR, PRETX, POSTTX, PRERX, and POSTRX can be pre-
configured, and the Modem’s working mode can be controlled according to the output state of Baseband.

#### 14.1.3. Introduction to Modem

It mainly implement the conversion of baseband signal and radio frequency signal.

Transmit precedure: The baseband signal is modulated, divided into two signals in the same direction (I signal) and
quadrature (Q signal), and then pass through DA conversion and low-pass filter again, and then use the frequency
synthesizer to perform frequency conversion. After the two signal components are synthesized, they are pushed to the
antenna through the PA amplifier.

Receive precedure: The radio frequency signal pass through a low-noise amplifier (LNA), enter two phase components
(I/Q), pass through a band-pass filter, amplify using VGA, and finally convert it into a digital signal and transmit it to the
baseband.

### 14.2. Main Features Of The BLE Subsystem


#### 14.2.1. Main Features of BLE Baseband

•    Conform to Bluetooth5.1 specification

•    Support AHB bus

•    Support data packet types: including broadcast packets, advertisement packets, data packets, control packets and LR
packets

•    Advertising Extension

•    Support synchronous channel operation

•    Encryption/Decryption (AES-CCM)

•    Bitstream processing (CRC, whitening)

•    Frequency hopping calculation

•    FDMA/TDMA/data formatting and synchronization

•    Device class support includes: broadcast, central, observer, peripheral

•    Support low power mode

#### 14.2.2. Modem Main Features

•    Compliance with Bluetooth Core Specification V5.1

•    -96dBm receiving sensitivity at 1Mbps

•    -93dBm receiving sensitivity at 2Mbps

•    Soc embedded integration for various applications

•    Support data rates include 125Kbps, 250Kbps, 1Mbps, 2Mbps

•    Integrated receive filter auto-tuning

•    Integrated Receive Skew Compensation

•    Integrated VCO auto-tuning

•    Integrated PLL bandwidth calibration

•    Integrated DC offset compensation

•    Integrated matching network and antenna switch

•    Efficient Power Management

## 15. Voice Control and ADC


### 15.1. Introduction

It supports 10 bit 1.33Msps ADC (16 bit 16Ksps can be configured), single-end or differential AMIC, and built-in
PGA, with the maximum gain up to 42dB. In addition it supplies power to MIC by MICBIAS with adjustable
voltage, and the optional output voltage falls within the range of 1.6V~2.3V.

There are up to 8 channels, 5 external single ends, 1 differential MIC and 2 internal channels. The internal channel
includes VCC detection channel and temperature sensor channel. For the five external channels, channel 1 (PB10)
and 2 (PB9) has the detection range of 0V-1V, while channel 3 (PB8), 4 (PB7), and 5 (PB6) has the detection
range of 0V-3.6V (when VCC=3.3V).

Built-in programmable gain amplifier (PGA) and microphone bias is used in the voice mode. MIC signals are
amplified through PGA, and then converted into digital signals through analog ADC. After voice input control
(low-pass decimation filterand optional energy and zero crossing detection), voice data is stored in the system
RAM through DMA, and finally the output in 16bit 16kHz voice signal format is available.

### 15.2. Main Features

•   Support AMIC input, with microphone offset adjustable

•   PGA supports single-end or differential input, with adjustable gain

•   Support 1 ADC, measure 5 external single ends, 1 differential MIC and 2 internal channels, with optional input
channel

•   Internal channel supports TempSensor, VCC, PGA differential input

•   10 bit 1.33Msps ADC (16 bit 16Ksps configurable)

•   Support digital filtering to 16 bits and noise filtering

•   Support single and continuous conversion mode

•   Support DMA request generation during channel conversion

•   The analog watchdog feature allows the application to detect whether the input PB10 voltage exceeds the user-
defined high/low threshold

•   Generate interrupts when conversion ends, PGA interrupts and analog watchdog events occur

•   The filter output data is stored in the 16 bit data register in voice mode, and the right alignment of data is
stored in the 16 bit data register in general mode

•   Output is made in the format of 16bit 16Ksps signed PCM in voice mode and 10bit 4Msps unsigned output in
general mode

•   ADCCTRL clock includes working clock

−    ADC_CLK: for ADC working clock, HSE_DIV8 (4MHz) or AUDIOPLLCLK (4.096MHz) can be
configured as the source of working clock

•   The specific internal signal connection of ADC is shown below:

*Figure 15-1 Architecture Diagram of Voice Control and ADC*


### 15.3. AMIC Input Channel

It supports one single-end or differential AMIC, and provides voltage-adjustable (in the range of 1.6~2.3V) MICBIAS.

MICBIAS provides a low-noise bias voltage to the microphone outside the chip. There are two working modes: normal
mode and turn-off mode.

Output VDD_ MIC power supply, drive capacity of 0.9mA, and support external power supply and 4.7uF off-chip capacitor.

•   Configure PGA_ CFG. MICBIAS_ EN=1, and enable MICBIAS

•   Configure PGA_ CFG.MICBIAS[2:0]

•   Obtain the corresponding VDD_ MIC output

### 15.4. PGA Control

Programmable mono PGA (programmable gain amplifier) with adjustable gain. The AMIC input channel can implement
signal amplification through PGA.

•   Configure PGA_ CFG. PGA_ EN=1, and enable PGA;

•   Configure PGA_CFG.PGA_INIT_ENA as 1 first, then as 0 after 5ms. Enable off-chip capacitor fast charging, and use
PGA at startup time.

•   Configure PGA_ CFG. PGA_GAIN, with corresponding gain of 0~42dB and step size of 6dB, select appropriate gain
configuration, manually adjust the input volume, wait 10ms for stabilization after switching gain;

•   For input channel, set ADC_CTRL.ADC_CH_SEL to 0 (by default), select PGA path, and input impedance 10K Ω
corresponding to PB11/PB13

### 15.5. Digital Voice Control

Digital voice input processing mainly includes low-pass filter (LPF), energy and zero crossing rate detection module, which
can be enabled or disabled by configuration.

It supports digital filtering to 16 bits and noise filtering. In the voice mode, the filter output data is stored in the 16 bit data
register, providing output in the format of 16 bit 16Ksps signed mono PCM.

#### 15.5.1. Low-pass Decimation Filter LPF

LPF is a data filter. LPF performs fixed ratio 256 samples after receiving analog ADC output, and filters high-frequency
noise mixed in ADC samples, so as to obtain the effective received data.

In voice mode, VOICE_DET_CR .VAD_FILTER_BYP has to be configured as 0 (by default). Finally LFP has 16 bit

16Ksps signed data output.

#### 15.5.2. Energy Detection and Zero Crossing Rate Detection

For sound input, first take the information of the former three frames of signals as the initial mean noise parameter,
and the sum of which and the constant C is the threshold M0. Voiced sound is determined when the value is
above M0, and zero crossing detection is conducted when the value is lower than M0. Those sounds with zero
crossing rate ZC between 30 and 70 (can be set by software) are considered clear voice, and the others are mute.

Format of energy detection (ED) and zero crossing rate detection (ZCRD): four bytes at the end of each frame.

Byte 1                    Byte 2                     Byte 3                              Byte 4

ZCRD                       ED                          ED                   EDx6+Silent, Clear and Voice x2

Frame format of Sound: 248 16KHz 16 bit voice sampling data+1 result word of 32-bit energy detection and zero crossing
detection

Format of the words of energy detection and zero crossing rate detection:

Word [31:24]: Zero crossing detection result

Word [23:2]: average energy compressed to 24bit

Word [1:0]: sound detection result

00: Silent: mute

01: Clear: soft tone

10: Voice: voiced sound

### 15.6. Function Description Of ADC


#### 15.6.1. Input Channel and Range of Input Voltage

ADC is a high-speed successive approximation type analog-to-digital converter, including up to 8 channels, 5 external
single ends, 1 differential MIC and 2 internal channels. The internal channel includes VCC detection channel and
temperature sensor channel. The A/D conversion of channels can be performed in single and continuous mode.

Configure ADC_CTRL.ADC_CH_SEL for ADC input channel selection.

000 (by default) differential MIC channel, corresponding to PB11/PB13, is selected.

001-010 is used for off-chip pre-voltage division input, corresponding to PB9/PB10, without resistive load, and with
detection range of 0-1V.

011~101 is used for direct detection, corresponding to PB6~PB8, with input impedance of 360KΩ and detection range of
0~3.6V.

110 is used for VCC detection.

111 is used for the voltage detection of on-chip temperature sensor.

#### 15.6.2. ADC Switch Control

ADC can be enabled by setting the ADC_EN bit of ADC_ CTRL register. When the ADC_ENC is set for the first time,
ADC needs a stable time tSTAB 64 cycle before starting precise conversion, and then once conversion is performed each
cycle.

In single mode, the hardware automatically turns off ADC_EN after conversion. If the interrupt is enabled, the
corresponding conversion termination interrupt can be generated. Users confirm whether the conversion is completed by
querying the ADC_DONE_F in ADC_SR.

the conversion can be stopped in continuous mode by clearing the ADC_EN. ADC_EN bit has to be disabled before

switching input channels.

#### 15.6.3. Conversion Mode


##### 15.6.3.1. Single Conversion Mode

Each conversion relates to two phases: sample phase and conversion phase.

In single conversion mode, ADC only performs one conversion. After setting ADC_CTRL.ADC_CH_SEL and selecting
the input channel, users can enable conversion by setting the ADC_EN bit of ADC_CTRL.

Once the selected channel is successfully converted:
•    Conversion data is stored in 16 bit ADC_ DAT register

•    ADC_ DONE_ F (end of conversion) flag is set

•    An interrupt is generated when ADC_DONE_IE is set

The conversion is completed as fast as only one adcclk clock cycle. However, in the single mode, switching circuit of the
input channel requires 64 adcclk stabilization periods when ADCEN or ADCSEL is modified.

##### 15.6.3.2. Continuous conversion mode


By setting ADC_CTRL.ADC_MODE to "1", users can use ADC in continuous mode. In continuous conversion
mode, another ADC conversion will be started as soon as the previous one ends. After setting
ADC_CTRL.ADC_CH_SEL and selecting the input channel, users can trigger the first ADC conversion by
setting the ADC _ EN bit of CTRL register, but afterwards, new conversion data will be automatically generated
for each adcclk cycle.

Support setting oversample rate and configured value ADC_OVR_SAMP_CNT. OS_CNT_LD_CNT should
be >=2, sample one data from OS_ CNT_ LD_ CNT+1 data.

After each conversion:

•    Conversion data is stored in 16 bit ADC_ DAT register

•    DMA mode is enabled, and DMA request will be generated after each conversion

#### 15.6.4. Analog Watchdog

If the analog PB10 voltage converted by ADC is lower than the low threshold or higher than the high threshold, the
AWDG analog watchdog status bit is set. Threshold is located at the lowest 10 significant bits of ADC_
WDHIGH and ADC_WDLOW register. The generation of corresponding interrupt can be allowed by
setting the AWD_IE bit of ADC_CTRL register.

*Figure 15-2 Analog Watchdog Warning Area*


Analog voltage

High
HT
threshold

Warning area

Low
threshold                                                   LT

### 15.7. Data Alignment

The format of right alignment of data is presented in the following Figure.

*Figure 15-3 Right Alignment of Data*


0        0      0       0       0     0      D9      D8     D7      D6      D5     D4      D3      D2      D1      D0

### 15.8. DMA Request

Because the channel conversion value is stored in a single data register, DMA has to be used when we convert multiple
channels, thus avoiding the loss of the data already stored in ADC_DAT register.

DMA request is generated only when the channel conversion ends, and the converted data is transferred from the register
ADC_DAT to the user-specified destination address.

### 15.9. Temperature Sensor

Temperature sensor can be used to measure the temperature (TA) around a device.

Temperature sensor is internally connected to the VINP7/VINN7 input channel which converts the sensor’s output voltage
into a digital value. TS_EN bit must be set to activate the internal channel. When not in use, the sensor can be kept in the
power-off mode to reduce power consumption.

The output voltage of temperature sensor changes linearly with the temperature. Due to the change in the production process,
the deviation of temperature change curve will vary on different chips (with the maximum difference of 4 °C).

The internal temperature sensor is more suitable for the detection of temperature change than the measurement of absolute
temperature. An external temperature sensor should be used as long as accurate temperature measurement is required.

#### 15.9.1. Read Temperature

Complete the configuration as follows to read the temperature by using a temperature sensor:

•       Select VINP7/VINN7 input channel, ADC_ CH_ SEL=3’b111

•       Set TS_EN bit of ADC control register (ADC_CTRL) to wake up the temperature sensor in power off mode

•       Activate ADC conversion by setting ADC_ EN bit

•       Read VSENSE data result on ADC data register

•       Obtain the temperature through the following formula Temperature (°C)={(V25 - VSENSE) / Avg_Slope} + 25 Where:

V25=VSENSE value at 25 °C

Avg_ Slope= average slope of temperature versus VSENSE curve (in mV/°C or μV/°C）

Refer to the actual value of V25 and Avg_Slope in the Electrical Characteristics section of the Data Manual.

4h
01
0h
01
00
8h
00
4h
00
0h
00

Ch
Off
set
15.10.

15.10.1.

ES
P_CNT
Register

ADC_SR

ADC_DAT

PGA_CFG
ADC_CTRL

Reset Value
Reset Value
Reset Value
Reset Value
Reset Value

ADC_WDT_THR
ADC_OVR_SAM
31
30
29

Reserved
28

1
HT[9]                                                                                                                                                                                    27

1
HT[8]                                                                                                                                                                                    26

1
HT[7]                                                                                                                                                                                    25

1
HT[6]                                                                                                                                                                                    24

Reserved
1
HT[5]                                                                                                                                                                                    23

Reserved

1
and TS_ EN bit should be set at the same time.

15.10 ADC Register

HT[4]                                                                                                                                                                                    22

1
ADC Register Overview

HT[3]                                                                                                                                                                                    21

1
Reserved
HT[2]                                                                                                                                                                                    20

1
HT[1]                                                                                                                                                                                    19

1
HT[0]                                                                                                                                                                                    18

Reserved
PGA_GAIN[3]                                                                                                                                                                                                                    17

Reserved
PGA_GAIN[2]                                                                                                                                                                                                                    16

0
PGA_GAIN[1]                                                                     ADC_DATA[15]                                                                                                                                   15

0
PGA_GAIN[0]                                                                     ADC_DATA[14]                                                                                                                                   14

0
*Table 15-1 ADC Register Overview*


PGA_EN                                                                          ADC_DATA[13]                                                                                                                                   13

Reserved
0
AUDIOPGA_DRIVE[1]                                                               ADC_DATA[12]                                                                                                                                   12

0
AUDIOPGA_DRIVE[0]                                                               ADC_DATA[11]                                                                                                                                   11

0
0

AUFIOPGA_PEAK[1]                                                                ADC_DATA[10]                                                                                                        PGARDY_IE                  10

0
0
0

AUFIOPGA_PEAK[0]                      LT[9]                                     ADC_DATA[9]                                                                                                         AWD_IE                      9

0
0
0

AUDIOPGA_RESERVE                      LT[8]                                     ADC_DATA[8]                                                                                                         ADC_DONE_IE                 8

0
0
0

AUDIOPGA_RESERVE                      LT[7]                                     ADC_DATA[7]                                                                                                         ADC_MODE                    7

0
0
0

AUDIOPGA_RESERVE                      LT[6]                                     ADC_DATA[6]                                                                                                         ADC_CH_SEL[2]               6

0
0
0

AUDIOPGA_RESERVE                      LT[5]                                     ADC_DATA[5]                                                                                                         ADC_CH_SEL[1]               5

0
0
1
0

AUDIOPGA_INIT_ENA                     LT[4]                                     ADC_DATA[4]                          OS_CNT_LD_CNT[                                                                 ADC_CH_SEL[0]               4

0
0
1
0

MICBIAS[2]                            LT[3]                                     ADC_DATA[3]                          OS_CNT_LD_CNT[                                                                 DMA_MODE_EN                 3

0
0
1
0
0
> **Note: There is a setup time from the time when the sensor is waken up in the power off mode to the time when the**
VSENSE of correct level can be output, and so does ADC after power on. In order to shorten the delay, ADC_ EN

MICBIAS[1]                            LT[2]                                     ADC_DATA[2]                          OS_CNT_LD_CNT[                             PGARDY_F                            AWD_EN                      2

0
0
1
0
0

MICBIAS[0]                            LT[1]                                     ADC_DATA[1]                          OS_CNT_LD_CNT[                             AWD_F                               TS_EN                       1

0
0
1
0
0

MICBIAS_EN                            LT[0]                                     ADC_DATA[0]                          OS_CNT_LD_CNT[                             ADC_DONE_F                          ADC_EN                      0

#### 15.10.2. ADC Control Register (ADC_CTRL)

Address offset: 0x00
Reset value: 0x0000 0000

Bit Field     Name           Description

31:11         Reserved       Must be reserved as 0.

10            PGARDY_IE      PGA abnormal operation interrupt enable

0: Disable

1: Enable

9             AWD_IE         Analog watchdog interrupt enable

0: Disable

1: Enable

8             ADC_DONE_IE ADC single conversion completion interrupt enable

0: Disable

1: Enable

7             ADC_MODE       ADC working mode selection

0: Single conversion

1: Continuous conversion

6:4           ADC_CH_SEL ADC input channel selection

000: MIC input

001: Input of PB10

010: Input of PB9

011: Input of PB8

100: Input of PB7

101: Input of PB6

110: External voltage input

111: TS input

3              DMA_MODE_E DMA mode enable control
N
0: Do not use DMA

1: Use DMA

2              AWD_EN       Analog watchdog enable

0: Disable

1: Enable

1              TS_EN        Temperature monitor enable

0: Disable

1: Enable

0              ADC_EN       ADC work enable

0: Disable

1: Enable

#### 15.10.3. ADC Status Register(ADC_SR)

Address offset: 0x04

Reset value: 0x0000 0000

Bit Field      Name          Description

31:3           Reserved      Must be reserved as 0.

2              PGARDY_F      PGA abnormal state indication

0: PGA works normally

1: PGA works abnormally

1              AWD_F         Analog watchdog working status indication

0: without analog watchdog event

1: with analog watchdog event

0              ADC_DONE_F     ADC single conversion completion status indication

0: Single conversion not completed

1: Single conversion completed

#### 15.10.4. ADC Oversample Control Register (ADC_OVR_SAMP_CNT)

Address offset: 0x08

Reset value: 0x0000 001F

Bit Field      Name                Description

31:5           Reserved            Must be reserved as 0.

4:0            OS_CNT_LD_CNT       Oversample rate setting

> **Note: Configuration value should be>=2**

OS_ CNT_ LD_ CNT+1 data sample for each data

#### 15.10.5. ADC Data Register (ADC_DAT)

Address offset: 0x0C

Reset value: 0x0000 0000

Bit Field      Name        Description

31:16          Reserved    Must be reserved as 0.

15:0           ADC_DATA Store ADC conversion data

When VAD_FILTER_BYP is 1, store 10bit unsigned number

When VAD_FILTER_BYP is 0 and the filter works, store 16 bit signed numbers

#### 15.10.6. ADC Watchdog Threshold Control Register (ADC_WDT_THRES)

Address offset: 0x10

Reset value: 0x0FFC 0000

Bit Field      Name        Description

31:28          Reserved    Must be reserved as 0.

27:18          HT          Analog watchdog high threshold

17:10          Reserved    Must be reserved as 0.

9:0            LT          Analog watchdog low threshold

#### 15.10.7. PGA&BIAS Control Register (PGA_CFG)

Address offset: 0x14

Reset value: 0x0002 0A08

Bit Field      Name          Description

31:18          Reserved      Must be reserved as 0.

17             Reserved      It is 1 by default

16:14   PGA_GAIN         PGA gain configuration

000 : 0dB

001 : 6dB

010 : 12dB

011 : 18dB

100 : 24dB

101 : 30dB

110 : 36dB

111 : 42dB

13      PGA_EN           PGA enable

0: Not enabled

1: Enable

12:11   Reserved         Default: 01

10:9    Reserved         Default: 01

8:5     Reserved         Must be reserved as 0

4       PGA_INIT_ENA PGA initialization enable, off-chip capacitor fast charging enable, and activate during PGA start

0: Disable

1: Enable

> **Note: provide pulses greater than 5ms (first 1, then 0)**

3:1     MICBIAS          Setting the MIC BIAS output voltage range, step=0.1V

000 : 1.6V

001 : 1.7V

010 : 1.8V

011 : 1.9V

100 : 2.0V (default);

101 : 2.1V

110 : 2.2V

111 : 2.3V

0              MICBIAS_EN    MICBIAS enable control

0: Disable

1: Enable

#### 15.10.8. Voice Detection Control Register (VOICE_DET_CR)

Address offset: 0x18

Reset value: 0x0000 0000

Bit Field      Name               Description

31:3           Reserved           Must be reserved as 0.

2              VAD_FILTER_BYP     FILTER BYPASS enable

0: Disable

1: Enable

1              VAD_ED_EN          Energy detection enable

0: Disable

1: Enable

0              VAD_ZCRD_EN        Zero crossing rate detection enable

0: Disable

1: Enable

#### 15.10.9. Voice Zero Crossing Rate Detection Threshold Register

(VOICE_ZCR_THRES)
Address offset: 0x1C

Reset value: 0x0000 6D2E

Bit Field      Name                         Description

31:16          Reserved                     Must be reserved as 0.

15:8           VAD_ZCRD_HIGH_THRES Zero crossing detection high threshold value

7:0            VAD_ZCRD_LOW_THRES           Zero crossing detection low threshold value

#### 15.10.10. Voice Energy Detection Threshold Register (VOICE_ED_THRES)

Address offset: 0x20

Reset value: 0x0000 0000

Bit Field      Name         Description

31:23          Reserved     Must be reserved as 0.

22:0           MOFFSET      Environmental noise energy threshold

#### 15.10.11. Voice Energy Detection Underflow Register (VOICE_ED_DWN_THRES)

Address offset: 0x24

Reset value: 0x0000 0000

Bit Field      Name         Description

31:23          Reserved     Must be reserved as 0.

22:0           GATE_MIN     Adaptive threshold attenuation limits the lower limit to prevent underflow.

#### 15.10.12. Voice Energy Detection Overflow Register (VOICE_ED_UP_THRES)

Address offset: 0x28

Reset value: 0x0000 0000

Bit Field      Name          Description

31:23          Reserved      Must be reserved as 0.

22:0           GATE_MAX Adaptive threshold attenuation limits the upper limit to prevent overflow.

## 16. I2C Interface


### 16.1. Introduction

I2C bus is a widely used bus structure, it has only two bidirectional lines, namely data bus SDA and clock bus SCL. All
devices compatible with I2C bus can communicate directly with each other through I2C bus with these two lines.

I2C interface connects microcontroller and serial I2C bus, and can be used for communication between MCU and external
I2C devices. It supports standard speed mode and fast mode, it supports CRC calculation and verification, SMBus (System
Management Bus) and PMBus (Power Management Bus), it also provides multi-host function to control all I2C bus specific
timing, protocol, arbitration. I2C interface module also supports DMA mode, which can effectively reduce the CPU
overload.

### 16.2. Main Features

•    Parallel-bus to I2C protocol converter

•    Multi host function: the module can be used as both master and slave equipment

•    As I2C master, it can generate clock, start and stop signal

•    As I2C slave, it supports address detection, stop bit detection function

•    Supports 7-bit/10-bit address mode and broadcast addressing

•    Support staFndard speed mode(up to 100 kHz) and fast mode(up to 400 kHz,1MHz)

•    Support interrupt vector, Event interrupt and error interrupt share one interrupt vector

•   Optional clock extending function

•   Support DMA mode

•   Optional PEC (Packet Error Check) generation and verification

•   Compatible with SMBus 2.0 and PMBus

> **Note: not all of the above features are included in all products. Please refer to the relevant data manual to confirm the I2C**
functions supported by the product.

### 16.3. Function Description

I2C module receives and transmits data, and converts data from serial to parallel or parallel to serial, It support interrupt
mode, users can enable or disable interrupt according to their needs. I2C interface is connected to I2C bus through data pin
(SDA) and clock pin (SCL) to communicate with external devices. It can be connected to standard (up to 100kHz) or fast
(up to 400kHz,1MHz) I2C bus.

#### 16.3.1. Mode Selection

The interface supports four operation modes:

•    Slave transmitter mode

•    Slave receiver mode

•    Master transmitter mode

•    Master receiver mode

I2C works in slave mode by default. The I2C interface is configured by software to send a start bit on the bus, and
then the interface automatically switches from the slave mode to the master mode. When arbitration is lost or
a stop signal is generated, the interface will switched to the slave mode from the receive mode. Allow multi host
functionality.

##### 16.3.1.1. Communication flow

In master mode, I2C host device starts data transmission and generates SCL clock.Serial data transmission
always starts with a start condition and ends with a stop condition. Both the start condition and stop
condition are generated by software control in the master mode.

In slave mode, I2C interface can identify its own address (7 or 10 bits) and broadcast call address. The software
can control the identification of broadcast call addres.

During the 9th clock period after 8 clocks of one byte transmission, the receiver must send back an ACK to the
transmitter. Refer to the figure below.

*Figure 16-1 Bus Protocol*


SDA
MSB                                                       ACK

SCL
1                2                       8                9

Start                                                                                          Stop
condition                                                                                      condition

The software can enable or disable answering (ACK), and can set the address of I2C interface (7-bit, 10 bit
address or broadcast call address). The functional block diagram of I2C interface is shown in the figure below.

*Figure 16-2 I2C Functional Block Diagram*


SDA                           Data
Shift register              Data register
GPIO                         control

PEC                         Own address register
Comparator
calculation                    Dual address register
PEC register

SCL                           Clock
Clock Control Register
GPIO                         control

Control Register

SMBALERT                            Control
logic
Status Register

Interrupts DMA requests

> **Note: in SMBus mode, SMBALERT is an optional signal. If SMBus is disabled, the signal cannot be used.**

#### 16.3.2. I2C Slave Mode

By default the I2C interface always works in slave mode. To switch from slave mode to master mode, a start
condition needs to be generated. In order to generate correct timing, the input clock to the module must be
programmed in the I2C_CTRL2 register. The frequency of the input clock must be at least:

•    grammeIn standard mode: 2MHz (100KHz communication rate)

•    In fast mode: 3MHz (1MHz communication rate) and 10MHz (400KHz communication rate).

Once a START condition is detected, the address received on the SDA line is sent to the shift register. Then with
the chip's own address OADDR1 and OADDR2 (when DUALEN=1 or wide compared with the broadcast call
address (if GCEN=1). Note: In 10-bit address mode, the comparison includes the header sequence (11110xx0),
where xx are the two most significant bits of the address.

Header or address mismatch: The I2C interface ignores it and waits for another START condition.

Header match (10-bit mode only): If the ACK bit is set to '1', the I2C interface generates an acknowledge pulse and
waits for the 8-bit slave address.

Address Match: The I2C interface generates the following timing:

•    If ACK is set to '1', an acknowledge pulse is generated

•    Hardware sets ADDRF bit; generates an interrupt if EVTINTEN bit is set

•    If DUALEN=1, software must read the DUALFLAG bit to confirm which slave address responded.

In 10-bit mode, the slave is always in receiver mode after receiving an address sequence. Transmitter mode will
be entered when a repeated START condition is received after receiving a header sequence matching the address
with the lowest bit being '1' (i.e. 11110xx1).

In slave mode the TRF bit indicates whether it is currently in receiver or transmitter mode.

##### 16.3.2.1. Slave transmission mode

After receiving the address and clearing the ADDRF bit, the slave transmitter sends the byte from the DAT register
to the SDA line via the internal shift register.

The slave keeps SCL low until the ADDRF bit is cleared and the data to be transmitted has been written to the
DAT register. (see EV1 and EV3 in the figure below).

When an acknowledged pulse is received:

•   The TXDATE bit is set by hardware, and an interrupt is generated if the EVTINTEN and BUFINTEN bits are set.

If the TXDATE bit is set, but no new data is written to the I2C_DAT register before the end of the next data
transmission, the BSF bit is set, and the I2C interface will keep SCL low before clearing BSF; write after reading
I2C_STS1 Writing to the I2C_DAT register will clear the BSF bit

*Figure 16-1 Slave Transmitter Transfer Sequence Diagram*


7-bit address

Master        Master           Slave        Slave     Master     Slave     Master               Slave   Master   Master
Tx            Tx               Tx           Tx        Tx         Tx        Tx                   Tx      Tx       Tx
Start     Address(R) ACK                    Data1     ACK        Data2      ACK              DataN NACK Stop

EV1 EV3-1             EV3                    EV3                  EV3                                   EV3-2

10-bit address

Master    Master        Slave       Master     Slave     Master           Master      Slave      Slave   Master             Slave   Master   Master
Tx        Tx            Tx          Tx         Tx        Tx               Tx          Tx         Tx      Tx                 Tx      Tx       Tx
Start Header(W) ACK Address ACK                             Start     Header(R) ACK                Data1   ACK               DataN NACK Stop

EV1                           EV1 EV3-1        EV3                 EV3                                 EV3-2

Instructions：

•    Start (start condition), Stop (stop condition), ACK=response, Evx=event.

•    EV1: I2C_STS1.ADDRF = 1, read STS1 and then STS2 to clear the event.

•    EV3-1: I2C_STS1.TXDATE=1, shift register is empty, data register is empty, write DAT.

•    EV3: I2C_STS1.TXDATE=1, shift register is not empty, data register is empty, write DAT will clear the event.

•    EV3-2: I2C_STS1.ACKFAIL=1, ACKFAIL bit of STS1 register write "0" to clear the event.

Notes
(1) EV1 and EV3_1 event prolongs the low SCL time until the end of the corresponding software sequence.

(2) The software sequence of EV3 must be completed before the end of the current byte transfer.

##### 16.3.2.2. Slave receiving mode

After receiving the address and clearing ADDRF, the slave receiver stores the byte received from the SDA line
through the internal shift register into the DAT register. The I2C interface performs th`e following actions after
each byte received:

•      Generate an acknowledge pulse if the ACK bit is set

•      Hardware setting RXDATNE=1. An interrupt is generated if the EVTINTEN and BUFINTEN bits are set.

If RXDATN is set, and the DAT register is not read before the end of receiving new data, the BSF bit is set, the I2C
interface will keep SCL low before clearing BSF; write to the I2C_DAT register after reading I2C_STS1 The BTF
bit will be cleared. (See below).

*Figure 16-4 Slave Receiver Transfer Sequence Diagram*


7-bit address

Master      Master      Slave        Master      Slave     Master     Slave             Master      Slave      Master
Tx          Tx          Tx           Tx          Tx        Tx         Tx                Tx          Tx         Tx
Start    Address(W) ACK             Data1        ACK       Data2      ACK              DataN        ACK Stop

EV1                               EV2                  EV2                   EV2                EV4

10-bit address

Master    Master       Slave     Master   Slave           Master     Slave            Master      Slave      Master
Tx        Tx           Tx        Tx       Tx              Tx         Tx               Tx          Tx         Tx
Start Header(W) ACK Address ACK                           Data1      ACK              DataN       ACK Stop

EV1                                  EV2                EV2                 EV4

Instructions:

•      Start (start condition), Stop (stop condition), ACK=response, Evx=event.

•      EV1: I2C_STS1.ADDRF = 1, read STS1 and then STS2 to clear the event.

•      EV2: I2C_STS1.RXDATNE =1, reading DAT will clear this event.

•      EV4: I2C_STS1.STOPF=1, reading STS1 and then writing the CTRL1 register will clear this event.

Notes:

(1) EV1 event prolongs the time when SCL is low until the end of the corresponding software sequence.

(2) The software sequence of EV2 must be completed before the end of the current byte transmission.

##### 16.3.2.3. Close slave communication

When the master generates a STOP condition after the last data byte has been transmitted, the I2C interface detects this

condition:

•    Set STOPF=1, and generate an interrupt if EVTINTEN bit is set.

Then the I2C interface waits to read the STS1 register before writing the CTRL1 register. (See EV4 in Figure 16-4).

#### 16.3.3. I2C master mode

In master mode, the I2C interface initiates data transfers and generates clock signals. Serial data transfers always begin
with a START condition and end with a STOP condition. The device enters master mode when a START condition is
generated on the bus via the START bit. The following is the sequence of operations required by master mode:

•    Set the input clock to the module in the I2C_CTRL2 register to generate the correct timing

•    Configure clock control register

•    Configure rise time register

•    Program the I2C_CTRL1 register to enable the peripheral

•    Set the STARTGEN bit in the I2C_CTRL1 register to 1 to generate a start condition.

•    The input clock frequency of the I2C module must be at least:

•    In standard mode: 2MHz (100KHz communication rate)

•    In fast mode: 3MHz (1MHz communication rate) and 10MHz (400KHz communication rate).

##### 16.3.3.1. Initial condition

When BUSY=0, set STARTGEN=1, the I2C interface will generate a start condition and switch to master mode (MSMODE bit
is set).

> **Note: In master mode, setting the STARTGEN bit will generate a restart condition by hardware after the current byte has**
been transferred.

Once the start condition is issued:

•    The STARTBF bit is set by hardware, and an interrupt will be generated if the EVTINTEN bit is set. The master then
waits to read the STS1 register, followed by writing the slave address to the DAT register (see EV5 in Figure 16-5 and
*Figure 16-6).*


16.3.3.2. end slave address
The slave address is sent to the SDA line through the internal shift register.

•    In 10-bit address mode, sending a header sequence generates the following events:

−     The ADDR10F bit is set by hardware, and an interrupt is generated if the EVTINTEN bit is set. The master then
waits to read the STS1 register before writing the second address byte to the DAT register (see Figure 16-5 and
*Figure 16-6).*


−     The ADDRF bit is set by hardware, and an interrupt is generated if the EVTINTEN bit is set. The master then
waits for a read of the STS1 register followed by a read of the STS2 register (see Figure 16-5 and Figure 16-6)

•    In 7-bit address mode, only one address byte needs to be sent out. Once the address byte is sent,

−     The ADDRF bit is set by hardware, and an interrupt is generated if the EVTINTEN bit is set. The

master then waits for a read of the STS1 register followed by a read of the STS2 register (see Figure 16-
5 and Figure 16-6). According to sending the lowest bit of the slave address, the master decides whether
to enter the transmitter mode or the receiver mode.

•   In 7-bit address mode,

−    To enter the transmitter mode, the master device sets the lowest bit to '0' when sending the slave address.

−    To enter receiver mode, the master device sets the lowest bit to '1' when sending the slave address.

•   In 10-bit address mode

−    To enter the transmitter mode, the master device first sends the header byte (11110xx0), and then sends
the slave address with the lowest bit being '0'. (here xx represents the highest 2 bits of the 10-bit address)

−    To enter receiver mode, the master device first sends the header byte (11110xx0), and then sends the
slave address with the lowest bit being '1'. Then resend a START condition followed by the header
byte (11110xx1) (where xx represents the most significant 2 bits of the 10-bit address).

The TRF bit indicates whether the master is in receiver or transmitter mode.

##### 16.3.3.3. Master transmission mode

After sending the address and clearing the ADDRF bit, the master sends the byte from the DAT register to the
SDA line through the internal shift register.

The master waits until TXDATE is cleared, (see EV8 in Figure 16-5).

When an acknowledge pulse is received:

•   The TXDATE bit is set by hardware, and an interrupt is generated if the EVTINTEN and BUFINTEN bits are
set. If TXDATE is set and no new data byte is written to the DAT register before the end of the last data
transmission, BSF is set by hardware, and the I2C interface will keep SCL low before clearing BSF; read
I2C_STS1 and then Writing to the I2C_DAT register will clear the BSF bit.

##### 16.3.3.4. Close communication

After writing the last byte in the DAT register, generate a stop condition by setting the STOPGEN bit (see EV8_2 in
*Figure 16-5), then the I2C interface will automatically return to slave mode (MSMODE bit cleared). Note:*

When TXDATE or BSF bit is set, stop condition should be scheduled on EV8_2 event.

*Figure 16-2 Master Transmitter Transfer Sequence Diagram*


7-bit address

Master        Master      Slave         Master       Slave     Master     Slave             Master      Slave      Master
Tx            Tx          Tx            Tx           Tx        Tx         Tx                Tx          Tx         Tx
Start      Address(W) ACK              Data1     ACK           Data2      ACK              DataN        ACK        Stop

EV5                EV6 EV8-1        EV8                      EV8                  EV8                  EV8-2

10-bit address

Master      Master      Slave       Master    Slave           Master     Slave            Master      Slave      Master
Tx          Tx          Tx          Tx        Tx              Tx         Tx               Tx          Tx         Tx
Start     Header(W) ACK            Address ACK                Data1      ACK              DataN       ACK        Stop

EV5                     EV9          EV6 EV8-1            EV8                      EV8                EV8-2

Instructions:

•    Start (start condition), Stop (stop condition), ACK=response, Evx=event (Interrupt generated when EVTINTEN=1).

•    EV5: SB= 1, reading STS1 and writing the address to the DAT register will clear the event.

•    EV6: ADDRF = 1, read STS1 and then STS2 to clear the event.

•    EV8_1: TXDATE = 1, shift register is empty, data register is empty, write DAT register.

•    EV8: TXDATE = 1, shift register is not empty, data register is empty, write to DAT register will clear the event.

•    EV8_2: TXDATE = 1, BSF = 1, request to set stop bit. These two events are cleared by the hardware when
a stop condition is generated.

•    EV9: .ADDR10F = 1, read STS1 and then write to DAT register to clear the event.

Notes:

(1) EV5, EV6, EV9, EV8_1 and EV8_2 event prolonged the low SCL time until the end of the corresponding
software sequence.

(2) The software sequence of EV8 must be completed before the end of the current byte transfer.

##### 16.3.3.5. Master receiving mode

After sending the address and clearing ADDRF, the I2C interface enters master receiver mode. In this mode,
the I2C interface receives data bytes from the SDA line and sends them to the DAT register through the internal
shift register. After each byte, the I2C interface performs the following operations in sequence:

•   If the ACKEN bit is set, send an acknowledge pulse.

•   Hardware setting RXDATNE=1, if EVTINTEN and BUFINTEN bits are set, an interrupt will be generated
(see EV7 in Figure 16-6).

If the RXDATNE bit is set, and the data in the DAT register has not been read before the end of receiving new
data, the hardware will set BSF=1, The I2C interface will hold SCL low until BSF is cleared; reading the
I2C_DAT register after reading I2C_STS1 will clear the BSF bit.

##### 16.3.3.6. Close communication

The master sends a NACK after receiving the last byte from the slave. After receiving a NACK, the slave releases control of
the SCL and SDA lines; the master can then send a stop/restart condition.

■ In order to generate a NACK pulse after the last byte is received, the ACKEN bit must be cleared after reading the
second-to-last data byte (after the second-to-last RXDATNE event).

■ To generate a stop/restart condition, software must set the STOPGEN / STARTGEN bit after reading the second-to-last
data byte (after the second-to-last RXDATNE event).

■ When only one byte is received, just after EV6 (when EV6_1, after ADDR is cleared), the generation bit of acknowledge
and stop condition should be turned off.

After a stop condition is generated, the I2C interface automatically returns to slave mode (MSMODE bit is cleared).

*Figure 16-3 Master Receiver Transfer Sequence Diagram*


7-bit address

Master             Master       Slave        Slave     Master          Slave    Master                Slave    Master   Master
Tx                 Tx           Tx           Tx        Tx              Tx       Tx                    Tx       Tx       Tx
(1)
Start       Address(R) ACK                   Data1     ACK            Data2     ACK                  DataN NACK Stop

EV5                     EV6          EV6-1                        EV7                 EV7       EV7-1                 EV7

10-bit address

Master        Master        Slave       Master     Slave     Master             Master       Slave       Slave    Master     Slave   Master            Slave   Master   Master
Tx            Tx            Tx          Tx         Tx        Tx                 Tx           Tx          Tx       Tx         Tx      Tx                Tx      Tx       Tx
Start     Header(W) ACK               Address ACK             Start         Header(R) ACK               Data1     ACK(1)    Data2    ACK              DataN NACK Stop

EV5                         EV9                    EV6         EV5                       EV6           EV6-1                EV7               EV7     EV7-1                      EV7

1. Instructions:


2. Start (start condition),Stop (stop condition), ACK(response), NACK(non-response), Evx=event (interrupt generated

when EVTINTEN=1)

3. EV5: STARTBF=1, reading STS1 and then writing the address into the DAT register will clear this event.


4. EV6: ADDRF=1, reading STS1 and STS2 in sequence will clear this event. In the 10-bits master receiving mode, the

STARTGEN should be set to 1 after this event.

5. EV6_1: There is no corresponding event flag, only suitable for receiving 1 byte. Just after EV6 (that is after clearing

ADDRF), the generation bits for acknowledge and stop condition should be cleared.

6. EV7: RXDATNE=1, read the DAT register to clear this event.


7. EV7_1: RXDATNE =1, read the DAT register to clear this event. Set ACKEN=0 and STOPGEN=1.


8. EV9: ADDR10F=1, reading STS1 and then writing to the DAT register will clear this event.


> **Note:**

a) If a single byte is received, it is NA EV5, EV6, and EV9 events extend the low level of SCL until the corresponding
software sequence ends.

b) The EV7 software sequence shall be completed before the end of the current byte transmission.

c)   The software sequence of EV6_1 or EV7_1 shall be completed before the ACK pulse of the current transmission byte.

#### 16.3.4. Error Conditions Description

The following conditions may cause communication failure.

##### 16.3.4.1. Bus Error(BUSERR)

when address or data is transmissing,I2C interface receive external stop or start condition,it will happen a bus error,

•    BUSERR bit is set. An interrupt occurs, when.ERRINTEN bit is set to 1.

•    I2C device as slave, when data is discarded in transmission and the bus releases by hardware, it will have two situation:

−       If an error start condition is detected, the slave device considers a restart condition and waits for an
address or a stop condition.

−       If an error stop condition is detected, the slave device operates as a normal stop condition and the
hardware releases the bus.

•    I2C device as master, the hardware does not release bus, as the same time it done not affect the current
status of transfer,The current transfer will determined by software whether suspend.

##### 16.3.4.2. Acknowledge Failure (ACKFAIL)

The interface have a acknowledge bit is detected that does not match the expectation, it will occurs acknowledge fail error,

•    ACKFAIL bit is set. An interrupt occurs, when ERRINTEN bit is set to 1.

•    When transmitter receives a NACK, The communication must be reset:

−       Device in slave mode, hardware release the bus;

−       Device in master mode, it must generate a stop condition from software.

##### 16.3.4.3. Arbitration Lost(ARLOST)

The interface have arbitration lost is detected, hardware release the bus, it will occurs arbitration lost error,

•    ARLOST bit is set. An interrupt occurs, when I2C_CTRL2.ERRINTEN bit is set to 1.

•    I2C interface will go to slave mode automatically(I2C_STS2.MSMODE bit is cleared). When the I2C
interface lost the arbitration, in the same communication, it can not respond to its slave address, but it can
respond when master win the bus retransmits a start signal.

•    Hardware releases the bus.

##### 16.3.4.4. Overrun/Underrun Error(OVERRUN)

In slave mode, disable clock extend,When I2C interface is receiving data (RXDATNE=1, data have received in
register), and DAT register still have previous byte has not been read, it will occurs an overrun error. In this
situation:

•    the last received data is discarded.
•   software should clear RXDATNE bit, transmitter retransmit last byte.

In slave mode, disable clock extend, When I2C interface is sending data (TXDATE=1, new data have not sending to
register), and DAT register still empty, it will occurs an underrun error. In this situation:

•   the previous byte in the DAT register is sending repeatedly.

•   And User make sure that in the event of an underrun error, the receiver discard repeatedly byte, and transmitter should
update the DAT register at the specified time according to the I2C bus standard.

In sending the first byte, DAT register must be written after ADDRF bit is cleared and the before the first SCL rising edge. If
cannot make sure do that, the first byte should be discard by receiver.

#### 16.3.5. SDA/SCL line control

The I2C module has two interface lines: a serial data line (SDA) and a serial clock line (SCL). Devices connected to the
bus communicate with each other over these two wires. Both SDA and SCL are bidirectional lines, connected to the positive
pole of the power supply through a current source or a pull-up resistor. When the bus is free, both lines are high. The output of
the device connected to the bus must have an open drain or open collector to provide a wired-AND function. Data on the
I2C bus can reach 100 kbit/s in standard mode and 1Mbit/s in fast mode. Since devices of different processes may be
connected to the I2C bus, the levels of logic '0' and logic '1' are not fixed and depend on the actual level of VDD.

Allowing clock stretching, i.e. pulling the SCL line low, avoids overrun errors on receive and underrun errors on transmit.

•   If clock stretching is allowed:

−    Transmitter mode: If TXDATE=1 and BSF=1: I2C interface keeps the clock line low before transmission to wait
for the software to read STS1, then write the data into the data register (buffer and shift register are both empty ).

−    Receiver mode: If RXDATNE =1 and BSF=1: I2C interface keeps the clock line low after receiving the data byte to
wait for the software to read STS1, then read the data register DAT (both buffer and shift register are full of).

•   If clock stretching is disabled in slave mode:

−    If RXDATNE=1, DAT has not been read before receiving the next byte, an overload error occurs. The last byte
received was lost.

−    If TXDATE=1, an underrun error occurs if no new data is written to DAT before the next byte must be sent. The
same bytes will be emitted repeatedly.

−    Does not control duplicate write conflicts

#### 16.3.6. SMBus


##### 16.3.6.1. Introduction

The System Management Bus (SMBus) is a two-wire interface. It allows devices to communicate with each other and with
other parts of the system. It is based on the I2C operating principle. SMBus provides a control bus for system and power
management related tasks. A system can use SMBus to exchange information with multiple devices without using separate
control lines. The System Management Bus (SMBus) standard addresses three classes of devices. Slave device: A device
that receives or responds to commands. Master device: used to send commands, A device that generates clocks and
terminates transmissions. Host: A dedicated master device that provides the main interface to the system CPU. The host

must have master-slave functionality and must support the SMBus alert protocol. Only one host is allowed in a system

##### 16.3.6.2. Similarities between SMBus and I2C

•    Both bus protocols contain of 2 wires (a clock wire SCL and a data wire SDA), with an optional SMBus alert wire.

•    Both are master-slave communication modes, and the master device provides the clock.

•    Both support multi master

•    The data format is similar. SMBus data format is similar to 7-bit address format of I2C.

##### 16.3.6.3. Differences between SMBus and I2C

Table 16--1 Comparison between SMBus and I2C

SMBus                                                I2C

Maximum transmission speed 100kHz                         Maximum transmission speed 1MHz

Minimum transmission speed 10kHz                          No minimum transmission speed

Low clock timeout 35ms                                    No clock timeout

Fixed logic level                                         VDD determined logic level

Different address types (reserved, dynamic, etc.)         7-bit, 10-bit, and broadcast call slave
address types

Different bus protocols (quick command, call              No bus protocol
handling, etc.)

##### 16.3.6.4. SMBus usage

Using the system management bus, a device can provide manufacturer information, tell the system its model/part number,
save status on halt events, report various types of errors, receive control parameters, and return its status. SMBus provides a
control bus for system and power management related tasks.

##### 16.3.6.5. Device identification

On the SMBus, as a slave have a only address for any device, named slave address.

Please refer to the SMBus specification version 2.0 (http://smbus.org/specs/) for the reserved slave address table

##### 16.3.6.6. Bus protocol

SMBus specification include 9 bus protocols. If want browse the details on protocols or SMBus address types,it can refer to
the SMBus specification v2.0(http://smbus.org/specs/). User’s software can device what protocols are implemented..

##### 16.3.6.7. Address resolution protocol (ARP)

The SMBus resolves address conflicts by dynamically assigning a new unique address to each slave device. This is the
address resolution protocol(ARP) .

Address Resolution Protocol (ARP) has the following characteristics:

•    Use the standard SMBus physical layer arbitration mechanism to assign addresses;

•    While the device is powered on, the assigned address remains unchanged, also allowing the device to retain its
address after a power outage.

•    After address allocation, there is no additional SMBus packaging overhead (that is to say, it takes the same time to
access a device with an assigned address and a device with a fixed address);

•    Any SMBus master can traverse the bus.

##### 16.3.6.8. Unique Device Identifier (UDID)

In order to assign addresses, a mechanism is required to distinguish each device, and each device must possess a unique
device identifier. For details on the 128-bit UDID over ARP, refer to version 2.0 of the SMBus specification
(http://smbus.org/specs/).

##### 16.3.6.9. SMBus alter mode

SMBus offer a optional interrupt signal SMBALERT(like SCL and SDA,is a wired-and signal) that devices uses to extend
their control capabilities at expense of a pin. SMBus broadcast call address often combine with SMBALERT. There is 2
bytes message about SMBus.

A device which only has slave function can set SMBALERT bit to indicate it want to communicate with host. The host
handles the interrupt and accesses all SMBALERT devices through the ARA (Alert Response Address, address value
0001100x). Only those devices that pull SMBALERT low can respond to ARA. This state is identified by the SMBALERT.
The 7-bit device address provided from the sending device is placed on the 7 most significant bits of the byte, the eighth bit
can be either '0' or '1'.

When more than one device’s SMBALERT is low, the highest priority(The smaller the address, the higher the priority) can
win bus communication through the standard arbitration during address transmission. If confirming the slave address,
device’s SMBALERT is no longer pulled low. If message transmitted completely,device’s SMBALERT still is low,it mean
host will read ARA again.

The host can periodically access the ARA when the SMBALERT signal is not used.

For more details on the SMBus alert mode, please refer to the SMBus specification version 2.0 (http://smbus.org/specs/).

##### 16.3.6.10. Timeout function

There are many differences between I2C and SMBus in terms of timing specifications.

SMBus defines a clock low timeout, 35ms timeout. SMBus defines TLOW:SEXT as the accumulated clock low extension
time of the slave device. SMBus specifies that TLOW:MEXT is the accumulated clock low extension time for the master
device. Please refer to the SMBus specification version 2.0 (http://smbus.org/specs/) for more timeout details.

The status flag TIMOUT error in I2C_STS1 indicates the status of this feature.

##### 16.3.6.11. How to use the interface in SMBus mode

In order to switch from I2C mode to SMBus mode, the following steps should be performed:

•    Set the SMBMODE bit in the I2C_CTRL1 register;

•    Configure the SMBTYPE and ARPEN bits in the I2C_CTRL1 register as required by the application. If the device is to
be configured as a master, the steps for generating a START condition are described in Section 16.3.3 I2C Master

Mode. Otherwise, see Section 16.3.2 I2C Slave Mode.

A software program must handle multiple SMBus protocols.

•   If ARPEN=1 and SMBTYPE=0, use SMB device default address.

•   If ARPEN=1 and SMBTYPE=1, use SMB master header field.

•   If SMBALERT=1, use SMB alert response address.

#### 16.3.7. DMA requests

DMA requests (when enabled) are used for data transfers only. A DMA request is generated when the data register becomes
empty during transmission or when the data register becomes full during reception.Before transfer current byte at the end
DMA requests must be answered. If set the DMA channel transfer data is done, DMA will send EOT(End Of Transmission) to
I2C, and ocurrs a interrupt when enable interrupt bit.

•   In the master transmit mode, in EOT interrupt handler need to disbale DMA request , and set stop condition after
waiting for BSF event.

•   In the master receive mode, the data of received is great than or equal to 2, DMA will send a hardware signal EOT_1 in
DMA transmission(byte number-1). If set .DMALAST bit, when hardware have send the EOT_1 next byte it will send
a NACK automatically. The user can set a stop condition in the interrupt handler after the DMA transfer is
completed if interrupt enable.

##### 16.3.7.1. Transmit process

If use the DMA mode need set the DMAEN bit. When TXDATE bit is set, the data will send to I2C_DAT from storage
area by the DMA. DMA assign a channle for I2C transmission, (x is the channel number) the following step must be opreate:

•   In the DMA_PADDRx register set the I2C_DAT register address. Data will be send to address in every TXDATE event.

•   In the DMA_MADDRx register set the memory address. Data will send to I2C_DAT address in every TXDATE event.

•   In the DMA_TXNUMx register set the number of need to be transferred.In every TXDATE event this number-1 until 0.

•   In the DMA_CHCFGx register set PRIOLVL[1:0] bit to configure the priority of channel.

•   In the DMA_CHCFGx register set DIR bit to configure when ocurrs an interrupt whether send a half data or all
completed.

•   In the DMA_CHCFGx register set CHEN bit to enable transfer channel.

When DMA transfer data is done, DMA need send a EOT/EOT_1 signal to I2C indicate this transfer is done. If interrupt is
enable, DMA ocurrs a interrrupt.

> **Note: if DMA is used for transmission, do not set BUFINTEN bit.**

##### 16.3.7.2. Receive process

If use DMA mode need set.DMAEN bit. When data byte is received,DMA will send I2C data to storage area, set DMA
channel for I2C reception. The following steps must be opreate:

•   In DMA_PADDRx register set the address of the I2C_DAT register. In every RXDATEN event, data will send from
address to storage area.
•   In DMA_MADDRx register set the memory area address. In every RXDATEN event,data will send from I2C_DAT
register to storage area.

•   In DMA_TXNUMx register set the number of need to be transferred. In every RXDATEN event the number-1 until 0.

•   In DMA_CHCFGx register set PRIOLVL[0:1] to configure the priority of channel.

•   In DMA_CHCFGx register clear DIR to configure when ocurrs a interrupt request whether received half data or all
data is received.

•   In the DMA_CHCFGx register set CHEN bit to activate the channel.

When DMA tansfer data is done, DMA need to send EOT/EOT_1 signal to I2C indicate this transfer is done, if
interrupt is enbale, DMA ocurrs a interrupt.

> **Note: If DMA is used for receiving, do not set I2C_CTRL2.BUFINTEN bit.**

#### 16.3.8. Packet error check(PEC)

The Packet Error Check (PEC) calculator is used to improve the reliability of communication. This calculator
uses the following CRC-8 polynomial to calculate each bit of serial data:

C(x) = x8 + x2 + x + 1

•   The PEC calculation is activated by the PECEN bit of the I2C_CTRL1 register. The PEC is calculated using a
CRC-8 algorithm for all information bytes, including address and read/write bits.

−    When sending: Set the PEC transmission bit of the I2C_CTRL1 register at the last TXDATE event, and
the PEC will be sent after the last byte.

−    On receive: After the last RXDATNE event the PEC bit of the I2C_CTRL1 register is set, and if the next
received byte is not equal to the internally calculated PEC, the receiver sends a NACK. If it is the
primary receiver, a NACK will be sent after the PEC regardless of the result of the collation. The PEC bit
must be set before receiving the ACK pulse for the current byte.

•   PECERR error flag/interrupt available in I2C_STS1 register.

•   If both DMA and PEC calculators are activated:

−    On transmission: When the I2C interface receives the EOT signal from the DMA controller, it
automatically sends the PEC after the last byte.

−    On Receive: When the I2C interface receives an EOT_1 signal from the DMA, it will automatically take
the next byte as PEC and will check it. A DMA request is generated after receiving the PEC.

•   In order to allow intermediate PEC transfers, there is a control bit (DMALAST bit) in the I2C_CTRL2
register to determine whether it is really the last DMA transfer. If it is indeed the last master receiver's DMA
request, a NACK is automatically sent after the last byte is received.

•   The PEC calculation is invalid when arbitration is lost.

### 16.4. Interrupt Request

All I2C interrupt requests are listed in the following table.

*Table 16-2 I2C Interrupt Request*


Interrupt Event                Event Flag             Set Control Bit

Start bit sent (master)              STARTBF
EVTINTEN
Address sent (master) or             ADDRF

address matched (slave)

10-bit header sent (master)          ADDR10F

Received stop (slave)                STOPF

Data byte transfer completed.        BSF

Receive buffer is not empty.         RXDATNE
EVTINTEN and
BUFINTEN
Send buffer is empty.                TXDATE

Bus error                            BUSERR

Lost arbitration (master)            ARLOST

Acknowledge fail                     ACKFAIL

Overrun/underrun                     OVERRUN             ERRINTEN

PEC error                            PECERR

Timeout /Tlow error                  TIMOUT

SMBus Alert                          SMBALERT

> **Note: 1. STARTBF, ADDRF, ADDR10F, STOPF, BSF, RXDATNE and TXDATE are merged into a interrupt**
channel through logical OR.

1. BUSERR, ARLOST, ACKFAIL, OVERRUN, PECERR, TIMEOUT and SMBALERT are merged into a

interrupt channel through logical OR.

2. Event interrupts and error interrupts are logically ORed into the global interrupt channel.


*Figure 16-7 I2C Interrupt Map*


### 16.5. I2C Debug Mode

When the microcontroller enters debug mode (Cortex®-M0 core in halt state), the SMBUS timeout control either
continues to work normally or can be stopped depending on the debug control register (DBG_CTRL)
configuration bits I2C1TIMEOUT and I2C2TIMEOUT in the PWR module. See Section 4.3.18 Register for
details.

### 16.6. I2C Registers Desciption

For the abbreviations used in the register description, see Section 1.1

These peripheral registers can be operated by half word (16 bits) or word (32 bits)

#### 16.6.1. I2C Register Overview

*Table 16-16-3 I2C Register Overview*


Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set
NOEXTEND
STARTGEN
SMBALERT

SMBMODE
SWRESET

STOPGEN

I2C_CT
SMBTYPE
ACKPOS
ACKEN

PECEN
ARPEN
GCEN
PEC

EN

00      RL1
Reserved

Reserved

Reserved
0h Reset
0                     0           0        0         0          0             0          0           0      0       0        0                    0        0
Value
ERRINTEN
BUFINTEN
EVTINTEN

I2C_CT
DMALAST
DMAEN

CLKFREQ[5:0]
00      RL2
Reserved

Reserved
4h Reset
0        0         0          0             0                             0       0        0         0          0        0
Value
Rese
ADDRMODE

rved

I2C_OA
ADDR[9:8]

ADDR0

ADDR[7:1]
00 DDR1
Reserved                                                                       Reserved

8h Reset
0                                                            0        0         0          0          0           0          0         0     0        0
Value

Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

I2C_OA

DUALEN
ADDR2[7:1]
00 DDR2
Reserved
Ch Reset
0          0          0           0          0         0     0        0
Value

I2C_DA
DATA[7:0]
01       T
Reserved
0h Reset
0          0          0           0          0         0     0        0
Value

SMBALERT

OVERRUN
I2C_STS

RXDATNE

ADDR10F

STARTBF
PECERR

BUSERR
ACKFAIL
ARLOST

TXDATE
TIMOUT

ADDRF
STOPF

BSF
01       1

Reserved

Reserved
Reserved
4h Reset
0          0                    0         0         0        0        0         0          0                      0          0         0     0        0
Value

SMBHADDR
SMBDADDR
GCALLADD
DUALFLAG
I2C_STS

MSMODE
BUSY
PECVAL[7:0]

TRF
01       2

Reserved
Reserved
8h
Reset
0          0        0           0         0         0        0        0         0          0          0           0                    0     0        0
Value

I2C_CLK
FSMODE
DUTY

CLKCTRL[11:0]
01 CTRL
Reserved

Reserved
Ch Reset
0          0                              0         0        0        0         0          0          0           0          0         0     0        0
Value

I2C_TM
TMRISE[5:0]
02 RISE
Reserved
0h Reset
0           0          0         0     0        0
Value

#### 16.6.2. I2C Control Register 1 (I2C_CTRL1)

Address offset: 0x00

Reset value: 0x0000

Bit Field    Name       Description

15           SWRESET    Software reset

When asserted, I2C is in reset. Make sure the I2C pins are released and the bus is empty before
resetting this bit.

0:The I2C module is not in reset state;;

1:The I2C module in reset state.

> **Note: This bit can be used when the I2C_STS2.BUSY bit is set to 1 and no stop condition is**
detected on the bus.

14           Reserved   Reserved, the reset value must be maintained

SMBus alert

It can be set or cleared by software. When I2C_CTRL1.EN=0, it will be cleared by hardware.
13           SMBALERT
0: SMBAlert pin go high. The response address header is followed by the NACK signal;

1: SMBAlert pin go low. The response address header is followed by the ACK signal.

Packet error checking

It can be set or cleared by software. It will be cleared by hardware when PEC has been transferred,
or by start or stop condition, or when I2C_CTRL1.EN=0.
12           PEC
0: No PEC transfer

1: PEC transfer.

> **Note: When arbitration is lost, the calculation of PEC is invalid.**

Acknowledge/PEC Position（for data reception）

It can be set or cleared by software. Or when I2C_CTRL1.EN=0, it will be cleared by hardware.

0: I2C_CTRL1.ACKEN bit determines whether to send an ACK to the byte currently being
received; I2C_CTRL1.PEC bit indicates that the byte in the current shift register is PEC.

1: I2C_CTRL1.ACKEN bit etermines whether to send an ACK to the next received byte;
I2C_CTRL1.PEC bit indicates that the next byte received in the shift register is PEC.

11   ACKPOS    Note：

ACKPOS bit can only be used in 2-byte receiving configuration and must be configured before
receiving data.

For the second byte of NACK, the I2C_CTRL1.ACKEN bit must be cleared after the
I2C_STS1.ADDRF bit is cleared.

To detect the PEC of the second byte, the I2C_CTRL1.PEC bit must be set after the ACKPOS bit is
configured and when the ADDR event is extended.

10   ACKEN     Acknowledge enable

It can be set or cleared by software. Or when EN equals to 0, it will be cleared by hardware.

0：No acknowledge send;

1：Send an acknowledge after receiving a byte

Stop generation

It can be set or cleared by software. Or it will be cleared by hardware when a stop condition is
detected. Or it will be set by hardware when SMBus timeout error is detected,.

In the master mode:

0：No stop condition generates;

1：Generate a stop condition.
9    STOPGEN
In the slave mode:

0：No stop condition generates;

1：Release SCL and SDA lines after the current byte.

Note：When the STOPGEN, STARTGEN or PEC bit is set, the software should not take any write
operation to I2C_CTRL1 until this bit is cleared by hardware. Otherwise, the STOPGEN,
STARTGEN or PEC bits may be set twice.

Start generation

It can be set or cleared by software. Or it will be cleared by hardware when the start condition is
transferred or I2C_CTRL1.EN=0.

In the master mode:

8   STARTGEN   0: No start condition generates;

1: repeated generation of start conditions

In the slave mode:

0: No start condition generates;

1: A START condition is generated when the bus is free.

Clock extending disable (Slave mode)

This bit determines whether to pull SCL low when the data is not ready(I2C_STS1.ADDRF or

7   NOEXTEND   I2C_STS1.BSF flag is set) in slave mode, and is cleared by software reset

0: Enable Clock extending.

1: Disable Clock extending.

General call enable
6   GCEN
0：Disable General call. not respond(NACK) to the address 00h;
1：Enable General call. respond(ACK) the address 00h.

PEC enable

5   PECEN      0：Disable PEC module;

1：Enable PEC module.

ARP enable

0: Disable ARP；

4   ARPEN      1: Enable ARP.

If I2C_CTRL1.SMBTYPE=0, the default address of SMBus device is used.

If I2C_CTRL1.SMBTYPE=1, the host address of SMBus is used.

SMBus type

3   SMBTYPE    0: Device

1: Host

2   Reserved   Reserved, the reset value must be maintained.

SMBus mode

1            SMBMODE      0：I2C mode；

1：SMBus mode.

I2C Peripheral enable 0

：Disable I2C module;

1：Enable I2C module
0            EN
> **Note: If this bit is cleared when communication is in progress, the I2C module is disabled and**
returns to the idle state after the current communication ends,all bits will be cleared.

In master mode, this bit must never be cleared until the communication has ended.

#### 16.6.3. I2C Control Register 2 (I2C_CTRL2)

Address offset: 0x04

Reset value: 0x0000

Bit Field      Name            Description

15:13          Reserved        Reserved, the reset value must be maintained.

12             DMALAST         DMA last transfer

0: Next DMA EOT is not the last transfer

1: Next DMA EOT is the last transfer

> **Note: This bit is used in the master receiving mode, so that a NACK can be generated when**
data is received for the last time.

11             DMAEN           DMA requests enable

0: Disable DMA

1: Enable DMA

10             BUFINTEN        Buffer interrupt enable

0: When I2C_STS1.TXDATE=1 or I2C_STS1.RXDATNE=1, any interrupt is not generated.

1: If I2C_CTRL2.EVTINTEN= 1,When I2C_STS1.TXDATE=1 or I2C_STS1.RXDATNE=
1, interrupt will be generated.

9     EVTINTEN    Event interrupt enable 0：

Disable event interrupt； 1

：Enable event interrupt

This interrupt is generated when:

I2C_STS1.STARTBF = 1 (Master)

I2C_STS1.ADDR F = 1 (Master/Slave)

I2C_STS1.ADD10F = 1 (Master)

I2C_STS1.STOPF = 1 (Slave)

I2C_STS1.BSF = 1 with no I2C_STS1.TXDATE or I2C_STS1.RXDATNE event

I2C_STS1.TXDATE = 1 if I2C_CTRL2.BUFINTEN = 1

I2C_STS1.RXDATNE = 1 if I2C_CTRL2.BUFINTEN = 1

8     ERRINTEN    Error interrupt enable 0：

Disable error interrupt； 1

：Enable error interrupt.

This interrupt is generated when:

I2C_STS1.BUSERR = 1；

I2C_STS1.ARLOST = 1；

I2C_STS1.ACKFAIL = 1；

I2C_STS1.OVERRUN = 1；

I2C_STS1.PECERR = 1；

I2C_STS1.TIMOUT = 1；

I2C_STS1.SMBALERT = 1.

7:6   Reserved    Reserved, the reset value must be maintained.

5:0            CLKFREQ[5:0]     I2C Peripheral clock frequency

CLKFREQ[5:0] should be the APB1 clock frequency to generate the correct timming.

000000：Disable

000001：Disable

000010：2MHz

000011：3MHz

... 110000：

48MHz

110001~111111：Disable.

#### 16.6.4. I2C Own Address Register 1 (I2C_OADDR1)

Address offset: 0x08

Reset value: 0x0000

Bit Field   Name             Description

15          ADDRMODE         Addressing mode (slave mode)

0: 7-bit slave address

1: 10-bit slave address

14          Reserved         Must always be kept as' 1' by the software.

13:10       Reserved         Reserved, the reset value must be maintained.

9:8         ADDR[9:8]        Interface address

Bit Field    Name          Description

9~8 bits of the address.

> **Note: don't care these bits in 7-bit address mode**

7:1          ADDR[7:1]     Interface address

7~1 bits of the address.

0            ADDR0         Interface address

0 bit of the address.

> **Note: don't care these bits in 7-bit address mode**

#### 16.6.5. I2C Own address register 2 (I2C_OADDR2)

Address offset: 0x0C

Reset value: 0x0000

Bit Field    Name         Description

15:8         Reserved     Reserved, the reset value must be maintained.

7:1          ADDR2[7:1]   Interface address

7~1 bits of address in dual address mode.

0            DUALEN       Dual addressing mode enable

0: Disable dual address mode, only OADDR1 is recognized;

1: Enable dual address mode, both OADDR1 and OADDR2 are recognized.

> **Note: Valid only for 7-bit address mode**

#### 16.6.6. I2C Data Register (I2C_DAT)

Address offset: 0x10

Reset value: 0x0000

Bit Field    Name         Description

15:8         Reserved     Reserved, the reset value must be maintained.

Bit Field    Name        Description

7:0          DATA[7:0]   8-bit data register

Used to store received data or place data for sending to the bus Transmitter mode: Data
transmission is automatically initiated when a byte is written to the DAT register. Once the
transmission starts (TXDATE=1), if the next data to be transmitted can be written into the DAT
register in time, the I2C module will maintain a continuous data flow.

Receiver mode: Received bytes are copied to the DAT register (RXDATNE=1). Continuous data
transmission can be realized by reading the data register before receiving the next byte
(RXDATNE=1).

> **Note: In the slave mode, the address will not be copied into the DAT register;**

> **Note:Hardware does not manage write conflicts( if I2C_STS1.TXDATE =0, data can still be**
written into the data register);

> **Note: If the ARLOST event occurs when processing the ACK pulse, the received byte will not be**
copied into the data register, so it cannot be read.

#### 16.6.7. I2C Status Register 1 (I2C_STS1)

Address offset: 0x14

Reset value: 0x0000

Bit Field   Name         Description

15          SMBALERT     SMBus alert

0: No SMBus alert(master mode) or no SMB alert response address header sequence(slave
mode);

1: SMBus alert event is generated on the pin(master mode) or receive SMBAlert response
address(slave mode)

14          TIMOUT       Timeout or Tlow error

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No Timeout error；

1: A timeout error occurred

Error in the following cases:

1) SCL has kept low for 25ms (Timeout).

Bit Field   Name       Description

2) Master cumulative clock low extend time more than 10 ms (Tlow:mext).

3) Slave cumulative clock low extend time more than 25 ms (Tlow:sext).

Timeout in slave mode: slave device resets the communication and hardware frees the bus.

Timeout in master mode: hardware sends the stop condition.

13          Reserved   Reserved, the reset value must be maintained.

12          PECERR     PEC Error in reception

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No PEC error;receiver returns ACK after receiving PEC (if ACKEN=1);

1: PEC error: receiver will returns NACK Whether the I2C_CTRL1.ACKEN bit is enabled

11          OVERRUN    Overrun/Underrun

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No Overrun/Underrun

1: Overrun/Underrun

Set by hardware in slave mode when I2C_CTRL1.NOEXTEND=1, and when receiving a new
byte in receiving mode, if the data within DAT register has not been read yet, over-run occurs,the
new received byte will be lost.When transferring a new byte in transfer mode, but there is not
new data that has not been written in DAT register, under-run occurs which leads that the same
byte will be send twice.

10          ACKFAIL    Acknowledge failure

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No acknowledge failed;

1: Acknowledge failed.

9           ARLOST     Arbitration lost (master mode)

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No arbitration lost;

1: Arbitration lost.

Bit Field   Name       Description

When the interface loses control of the bus to another host, the hardware will set this bit to '1',
and the I2C interface will automatically switch back to slave mode (I2C_STS2.MSMODE=0).

> **Note: In SMBUS mode, the arbitration of data in slave mode only occurs in the data stage or the**
acknowledge transfer interval (excluding the address acknowledge).

8           BUSERR     Bus error

Writing ‘0’ to this bit by software can clear it, or it is cleared by hardware when
I2C_CTRL1.EN=0.

0: No start or stop condition error

1: Start or stop condition error

7           TXDATE     Data register empty (transmitters)

Writing data to DAT register by software can clear this bit; Or after a start or stop condition
occurs, or automatically cleared by hardware when I2C_CTRL1.EN=0.

0: Data register is not empty;

1: Data register is empty.

When sending data, this bit is set to' 1' when the data register is empty, and it is not set at the
address sending stage.

If a NACK is received, or the next byte to be sent is PEC(I2C_CTRL1.PEC=1), this bit will not
be set.

> **Note: After the first data to be sent is written, or data is written when BSF is set, the TXDATE bit**
cannot be cleared, because the data register is still empty.

6           RXDATNE    Data register not empty(receivers)

This bit is cleared by software reading and writing to the data register, or cleared by hardware
when I2C_CTRL1.EN=0.

0: Data register is empty;

1: Data register is not empty.

During receiving data, this bit is set to' 1' when the data register is not empty, and it is not set at
the address receiving stage.

RXDATNE is not set when the ARLOST event occurs.

> **Note: When BSF is set, the RXDATNE bit cannot be cleared when reading data, because the data**
register is still full.

5           Reserved   Reserved, the reset value must be maintained.

4           STOPF      Stop detection (slave mode)

Bit Field   Name      Description

After the software reads the STS1 register, the operation of writing to the CTRL1 register will
clear this bit, or when I2C_CTRL1.EN=0, the hardware will clear this bit.

0: No stop condition is detected;

1: Stop condition is detected.

After a ACK, the hardware sets this bit to' 1' when the slave device detects a stop condition on
the bus.

> **Note: I2C_STS1.STOPF bit is not set after receiving NACK.**

3           ADDR10F   10-bit header sent (Master mode)

After the software reads the STS1 register, the operation of writing to the CTRL1 register will
clear this bit, or when I2C_CTRL1.EN=0, the hardware will clear this bit.

0: No ADD10F event;

1: Received has sent the first address byte.

In 10-bit address mode, when the master device has sent the first byte, the hardware sets this bit
to' 1'.

> **Note: After receiving a NACK, the I2C_STS1.ADDR10F bit is not set.**

2           BSF       Byte transfer finished

After the software reads the STS1 register, reading or writing the data register will clear this bit;
Or after sending a start or stop condition in sending mode, or when I2C_CTRL1.EN=0, this bit
is cleared by hardware.

0：Byte transfer does not finish.

1：Byte transfer finished.

When I2C_CTRL1.NOEXTEND =0, the hardware sets this bit to' 1' in the following cases:

In receiving mode, when a new byte (including ACK pulse) is received and the data register has
not been read (I2C_STS1.RXDATNE=1).In sending mode, when a new data is to be transmitted
and the data register has not been written with the new data (I2C_STS1.TXDATE=1).

> **Note: After receiving a NACK, the BSF bit will not be set.**

If the next byte to be transferred is PEC (I2C _STS2.TRF is' 1' and I2C_CTRL1.PEC is' 1'), the
BSF bit will not be set.

1           ADDRF     Address sent (master mode) / matched (slave mode)

After the STS1 register is read by software, reading the STS2 register will clear this bit, or when
I2C_CTRL1 .EN=0, it will be cleared by hardware.

0: Address mismatch or no address received(slave mode) or Address sending did not end(master
mode);

Bit Field   Name          Description

1: Received addresses matched(slave mode) or Address sending ends(master mode)

In master mode:

In 7-bit address mode, this bit is set to' 1' after receiving the ACK of the address.In 10-bit address
mode, this bit is set to' 1' after receiving the ACK of the second byte of the address.

In slave mode:

Hardware sets this bit to' 1' (when the corresponding setting is enabled) when the received slave
address matches the content in the OADDR register, or a general call or SMBus device default
address or SMBus host or SMBus alter is recognized.

> **Note: After receiving NACK, the I2C_STS1.ADDRF bit will not be set.**

0           STARTBF       Start bit (Master mode)

After the STS1 register is read by software, writing to the data register will clear this bit, or when
I2C_CTRL1.EN=0, the hardware will clear this bit.

0: Start condition was not sent;

1: Start condition has been sent.

This bit is set to' 1' when the start condition is sent.

#### 16.6.8. I2C Status register 2 (I2C_STS2)

Address offset: 0x18

Reset value: 0x0000

Bit Field   Name            Description

15:8        PECVAL[7:0]     Packet error checking register

PECVAL [7:0] Stores the internal PEC value When I2C_CTRL1.PECEN =1.

7           DUALFLAG        Dual flag(Slave mode)

Hardware clears this bit when a stop condition or a repeated start condition is generated, or
when I2C_CTRL1.EN=0.

0: Received address matches the content in OADDR1;

1: Received address matches the content in OADDR2.

6           SMBHADDR        SMBus host header（Slave mode）

Hardware clears this bit when a stop condition or a repeated start condition is generated, or

Bit Field   Name        Description

when I2C_CTRL1.EN=0.

0: SMBus host address was not received;

1: when I2C_CTRL1.SMBTYPE=1 and I2C_CTRL1.ARPEN=1, the SMBus host address is
received.

5           SMBDADDR    SMBus device default address（Slave mode）

Hardware clears this bit when a stop condition or a repeated start condition is generated, or
when I2C_CTRL1.EN=0.

0: The default address of SMBus device has not been received;

1: when I2C_CTRL1.ARPEN=1, the default address of SMBus device is received.

4           GCALLADDR   General call address(Slave mode)

Hardware clears this bit when a stop condition or a repeated start condition is generated, or
when I2C_CTRL1.EN=0.

0: No general call address was received;

1: when I2C_CTRL1.GCEN=1, general call address was received.

3           Reserved    Reserved, the reset value must be maintained.

2           TRF         Transmitter/receiver

After detecting the stop condition (I2C_STS1.STOPF=1), repeated start condition or bus
arbitration loss (I2C_STS1.ARLOST=1), or when I2C_CTRL1.EN=0, the hardware clears it.

0: Data receiving mode

1: Data transmission mode;

At the end of the whole address transmission stage, this bit is set according to the R/W bit of
the address byte.

1           BUSY        Bus busy

Hardware clears this bit when a stop condition is detected.

0: No data communication on the bus;

1: Data communication on the bus.

When detecting that SDA or SCL is low level, the hardware sets this bit to' 1';

> **Note:This bit indicates the bus communication currently in progress, and this information is**
still updated when the interface is disabled (I2C_CTRL1.EN=0).

0           MSMODE      Master/slave mode

Hardware clears this bit when a stop condition is detected on the bus, arbitration is lost

Bit Field   Name           Description

(I2C_STS1.ARLOST=1), or when I2C_CTRL1.EN=0.

0: In slave mode;

1: In master mode.

When the interface is in the master mode (I2C_STS1.STARTBF=1), the hardware sets this bit;

#### 16.6.9. I2C Clock control register (I2C_CLKCTRL)

Address offset: 0x1c

Reset value: 0x0000

Bit Field   Name          Description

15:14       DUTY          SCL duty cycle

00: Tlow/Thigh = 1；

01: Tlow/Thigh = 1；

10: Tlow/Thigh = 2；

11: Tlow/Thigh = 16/9；

> **Note: 00 or 01 configuration is recommended for SCL 100K or 1M.**

13:12       Reserved      Reserved, the reset value must be maintained.

11:0        CLKCTRL[11:   Clock control register in Fast/Standard mode (Master mode)
0]
This division factor is used to set the SCL clock in the master mode.

■    If DUTY = 00 or 01:

Tlow = CLKCTRL ×TPCLK1

Thigh = CLKCTRL ×TPCLK1

■    If DUTY = 10:

Tlow = 2 ×CLKCTRL×TPCLK1

Thigh = CLKCTRL×TPCLK1

■    If DUTY = 11:

Tlow = 16 ×CLKCTRL×TPCLK1

Thigh = 9 ×CLKCTRL×TPCLK1

Bit Field   Name            Description

For example, if DUTY = 00 Generates SCL frequency of 100kHz

if CLKFREQ = 08 , TPCLK1 = 125ns Then CLKCTRL must be written to 0x28 (40×125ns = 5000
ns).

> **Note: 1.When DUTY = 00 or 01, the allowable minimum value is 0x04, and the other allowable**
minimum value is 0x01;

The minimum setting value is 0x04 in standard mode and 0x01 in fast mode;

2. Thigh= Tr(SCL)+Tw(SCLH). See the definitions of these parameters in the data sheet for details.


3. Tlow= Tf(SCL)+Tw(SCLL), see the definitions of these parameters in the data sheet for details;


4. These delays have no filters;


5.The CLKCTRL register can only be set when I2C is turned off (EN = 0);

#### 16.6.10. I2C Rise time register (I2C_TMRISE)

Address offset: 0x20

Reset value: 0x0002

Bit Field      Name          Description

15:6           Reserved      Reserved, the reset value must be maintained.

5:0            TMRISE[5:0]   Maximum rise time in fast/standard mode (master mode).

These bits must be set to the maximum SCL rising time given in the I2C bus specification, and
incremented step is 1.

For example, the maximum allowable SCL rise time in standard mode is 1000ns. if the value in
I2C_CTRL2.CLKFREQ [5:0] is equal to 0x08(8MHz) and TPCLK1=125ns ,09h(1000ns/125 ns +
1) must be written in TMRISE[5:0] .

If the result is not an integer, write the integer part to TMRISE[5:0] to ensure the tHIGH parameter.

> **Note: TMRISE[5:0] can only be set when I2C is disabled (EN=0).**

## 17. Universal Synchronous Asynchronous Receiver Transmitter (USART)


### 17.1. Introduction

The Universal Synchronous Asynchronous Receiver Receiver (USART) is a full-duplex or half-duplex,
synchronous or asynchronous serial data exchange interface. USART mention A programmable baud rate
generator is provided to divide the system clock to generate the specific frequency required for USART
transmission and reception.

USART supports standard asynchronous transceiver mode, IrDA, SIR, smart card protocol, LIN, synchronous
single- duplex mode, multiprocessor communication and Modem flow control operation (CTS/RTS), and also
uses multi-buffer configuration for high-speed data communication DMA mode..

### 17.2. Main Features

•    Full-duplex asynchronous communication

•    Single-wire half-duplex communication

•    NRZ standard format (Mark/Space)

•    Programmable baud rate generator

−   Shared programmable baud rate for both transmission and reception, up to 4Mbit/s

•    Support serial data frame structure with 8 or 9 data bits, 1 or 2 stop bits

•    The LIN master has the ability to send break characters and the LIN slave has the ability to detect break characters.

−   When the USART hardware is configured as LIN, it generates a 13-bit break; detects 10/11-bit breaks

•    The sender provides the clock for synchronous transfers

•    IrDA SIR encoder-decoder supporting 3/16 bit duration in normal mode

•    Asynchronous smart card protocol defined in the ISO7816-3 standard , smart card mode supports 0.5 or 1.5 stop bits

•    Configurable DMA multi-buffer communication; receive/send data with DMA buffer

•    Support data overflow error detection, frame error detection, noise error detection, parity error detection

•    Interrupt requests include: transmit data register empty, CTS flag, transmit complete, receive data ready to
read, data overflow detected, idle line detected, parity error, LIN break frame detection, noise flag/overflow
error/frame error in multi-buffer communication

•    Support multi-processor communication: if the address does not match, it will enter silent mode, and can be
woken up by idle bus detection or address identification

•    Two ways to wake up the receiver: address bit (MSB, bit 9), bus free.

### 17.3. Functional Block Diagram

*Figure 17-1 USART Block Diagram*


CPU/DMA

Transmit Data                Receive
Register(TDR)               Data(RDR)
TX
IrDA
RX                 SIR
ENDEC
Transmit Shift             Receive Shift
SW_RX                   BLOCK
Register                  Register

Hardware
nRTS
flow
TX control                RX control
nCTS                 controller

Tx clock
Rx clock
Buadrate
PCLK                                                    CTRL register
generate

BRCF
Interrupt                   Wake up
register

GTP                       STS                       USART
register                  register                    address

CK

### 17.4. Function Description

As shown in Figure 17-1, any USART two-way communication requires at least two pins: receive data input (RX) and
transmit data output (TX). RX: Serial data input terminal. In order to distinguish data from noise, oversampling technique
is used. TX: Serial data output terminal. When the transmission is enabled, the pin defaults to a high level. The TX port
can also be used for sending and receiving data at the same time, such as single In line or smart card mode, when the
transmission is not enabled, the TX port is configured as the original I/O port.

•    The bus should be idle when not sending or receiving

•    a start bit

•    1 data word (8 or 9 bits), least significant bit first

•    0.5, 1, 1.5, 2 stop bits, indicating the end of the data frame

•    Using the Fractional Baud Rate Generator - 12-bit Integer and 4-Decimal Representation

•    1 status register (USART_STS)

•    1 Data register (USART_DAT)

•    1 baud rate configuration register (USART_BRCF), 12-bit integer and 4 decimal

•    1 guard time register (GTV) in smart card mode For the specific definition of each bit in the above registers,
please refer to the register description section 17.8: USART registers. The following pins are required in
synchronous mode:

•    CK: This pin outputs the clock for synchronous transmission, (there is no clock pulse on the Start bit and
Stop bit, USART_CTRL2 registers The LBCLK bit in the register controls whether the clock pulse is output
on the CK pin at the corresponding moment of the last bit of data). Data can be synchronized on RX is
received. This can be used to control external devices with shift registers (such as LCD drivers). CK clock
phase and polarity can be The clock polarity and phase can be modified by CLKPOL/CLKPHA in the
USART_CTRL2 register. In smart card mode, the CK can also Clock provided. The following pins are
required in hardware flow control mode:

•    CTS: Receive clear, if it is low level, it means that the next data transmission can be continued at the end of the
current data transmission; if it is high level, Block the next data transmission when the current data
transmission ends.

•    RTS: Request to send, if it is low level, it indicates that the USART is ready to receive data.

### 17.5. USART Frame Format

The word length is selected as 8 or 9 bits by programming the WL bits in the USART_CTRL1 register (see
*Figure 17-2 and ). The TX pin is low during start bits and high during stop bits.*


Idle frame: A complete data frame consisting of '1', also includes the stop bit of the data. For example: if WL=0,
the idle frame consists of 10 '1'; if WL=1, the idle frame consists of 11 '1',

Disconnected frame: It is considered to have received all '0' within a frame period (including the stop bit period,
which is also '0'). At the end of the disconnection frame, the transmitter inserts 1 or 2 stop bits ('1') to
acknowledge the start bit, when WL=0, 10 low levels, followed by a stop bit; or when WL=1, 11 Bit low,
followed by a stop bit, the length cannot be greater than 10 or 11 bits.

Both transmit and receive are driven by a common baud clock generator, which generates baud clocks for the
transmitter and receiver respectively when their respective enable bits are set.

*Figure 17-2 Word Length = 8 Setting*


8-bit word length ,1 stop bit

Clock                                                                                   **

Data frame                   bit7 can be the parity bit                         Data frame
Stat                                                                                      Start
bit0      bit1      bit2      bit3     bit4      bit5      bit6    bit7    Stop     bit
bit
bit
Start
Idle frame                                                           bit
Start
Stop
bit
Break frame                                                       bit

9-bit word length , 1 stop bit

Clock                                                                                       **

Data frame                         bit8 can be the parity bit     Start           Data frame
Stat
bit    bit0      bit1     bit2     bit3      bit4     bit5      bit6   bit7 bit8 Stop            bit
bit
Start
Idle frame                                                              bit

Start
Stop
bit
bit
Break frame

> **Note: In this chapter, unless otherwise specified, setting means that a certain register is set to a state of '1', and**
resetting or clearing means that a certain register is set to a state of '0'; either hardware or program may set a
bit Or clear a certain register, please refer to the specific content in this chapter.

#### 17.5.1. Transmitter

When the transmit enable bit (TXEN) is set and there is data in the buffer, the transmitter transmits an 8-bit or 9-
bit data word depending on the state of the WL bit. The data in the transmit shift register is output on the
TX pin, and the corresponding clock pulse is output on the CK pin.

##### 17.5.1.1. Character send

When the USART transmits data, the TX pin shifts out the least significant bit of the data first. In character transmit
mode, the USART_DAT register contains a buffer between the internal bus and the transmit shift register (see
*Figure 17-1). Each character is preceded by a low-level start bit; followed by a configurable number of stop*

bits. The USART supports configurations of 0.5, 1, 1.5, and 2 stop bits.

> **Note: 1. The TXEN bit cannot be reset during data transmission, otherwise the data on the TX pin will be destroyed,**
because the baud rate counter stops counting. Current data being transferred will be lost. 2. When the TXEN bit
is activated, the USART will automatically transmit an idle frame.

##### 17.5.1.2. Stop bit


The characters are followed by stop bits, the number of which can be configured by setting USART_CTRL2. STPB[1:0].

*Table 17-1 Stop Bit Configuration*


Stop       Bit    Length
USART_CTRL2.STPB[1:0]                                                Functional Description
(bits)

00                                    1                              default

01                                    0.5                            Receiving in Smartcard mode

General USART mode, single-wire mode and
10                                    2
modem mode.

11                                    1.5                            Transmitting and receiving in Smartcard mode

*Figure 17-3 Configuration Stop Bit*


8-bit Word length (WL bit is reset)

CLOCK                                                                         **

Data frame                  bit7 can be the parity bit
Stat                                                                         Start
bit    bit0   bit1  bit2   bit3         bit4     bit5    bit6   bit7         bit              Data frame
0.5 Stop bit                                                         0.5Stop bit

Data frame                bit7 can be the parity bit                      Data frame
Stat                                                                              Start
bit    bit0   bit1  bit2   bit3         bit4   bit5   bit6    bit7 Stop            bit
bit
1 Stop bit
Data frame              bit7 can be the parity bit         Start        Data frame
Stat                                                                              bit
bit   bit0   bit1    bit2       bit3   bit4    bit5   bit6   bit7 1.5Stop bits

1.5 Stop bits

Data frame                bit7 can be the parity bit                Start Data frame
Stat
bit    bit0   bit1   bit2    bit3       bit4     bit5   bit6    bit7     2Sto p bits      bit

2 Stop bits

##### 17.5.1.3. Single byte communication

A write to the USART_DAT register clears the USART_STS.TXDE bit.

The USART_STS.TXDE bit is set by hardware when the data in the TDR register is transferred to the transmit shift register

(indicating that data is being transmitted). An interrupt will be generated if USART_CTRL1.TXDEIEN is set. At
this point, the next data can be sent to the USART_DAT register because the TDR register has been cleared and
will not overwrite the previous data.

Write operation to USART_DAT register:

•    When the transmit shift register is not sending data and is in an idle state, the data is directly put into the shift
register for transmission, and the USART_STS.TXDE bit is set by hardware;

•    When the transmit shift register is sending data, the data is stored in the TDR register, and after the current
transmission is completed, the data is put into the shift register.

When a frame containing data is sent and USART_STS.TXDE=1, the USART_STS.TXC bit is set to '1' by hardware. An
interrupt is generated if USART_CTRL1.TXCIEN is '1'.USART_STS.TXC bit is cleared by a software sequence (read
USART_STS register first, then write USART_DAT register).

> **Note: The TXC bit can also be cleared by writing '0' to it by software. This clearing method is only recommended to be**
used in DMA multi-buffer communication mode.

*Figure 17-4 TXC/TXDE Changes During Transmission*


set by hardware           set by hardware
TXDE flag                           cleared by software       cleared by software
set by hardware

USART
enable
Write data1       Write data2             Write data2
into USART_DAT    into USART_DAT          into USART_DAT

USART_DAT                data1              data2                       data3

Because TXDE=1
TXC is set
TXC flag                                                                                                           Set by
hardware

Idle                Frame 1                    Frame 2                    Frame 3
TX line

##### 17.5.1.4. Break frame

Use USART_CTRL1.SDBRK to send the break character. When there is 8-bit data, the break frame consists of 10 bits of
low level, followed by a stop bit; when there is 9-bit data, the break frame consists of 11 bits of low level, followed by a
stop bit.

After the break frame is sent, USART_CTRL1.SDBRK is cleared by hardware, and the stop bit of the break frame is being
sent. Therefore, to send a second break frame, USART_CTRL1.SDBRK should be set after the stop bit of the previous
break frame has been sent.

If software resets the USART_CTRL1.SDBRK bit before starting to send the break frame, the break frame will not be
sent.If two consecutive break frames are to be sent, the SDBRK bit should be set after the stop bit of the previous break
frame.

##### 17.5.1.5. Idle frame

Setting USART_CTRL1.TXEN will cause the USART to transmit an idle frame before the first data frame.

#### 17.5.2. Receiver


The WL bit of USART_CTRL1 determines whether to receive an 8-bit or 9-bit data word.

##### 17.5.2.1. Start bit detection

When the received sampling sequence is: 1 1 1 0 X 0 X 0 X 0 0 0 0, it is considered that a start bit is detected.

> **Note: If the sequence is incomplete, the receiver will exit start bit detection and return to idle state (without**
setting any flags) and wait for a falling edge.

The samples at the 3rd, 5th, and 7th bits, and the samples at the 8th, 9th, and 10th bits are all '0' (that is, 6 '0'), then
confirm     the    receipt     of   the       start       bit,        the             USART_STS.RXDNE                                  flag   bit   is   set,   and   if
USART_CTRL1.RXDNEIEN=1, an interruption occurs and will not Set the NEF noise flag.

•     The samples of the 3rd, 5th, and 7th bits have two '0' points, and at the same time, the samples of the 8th,
9th, and 10th bits have three '0' points, then the start bit is confirmed, but it will be set NEF noise flag.

•     The samples of the 3rd, 5th, and 7th bits have three '0' points, and at the same time, the samples of the
8th, 9th, and 10th bits have two '0' points, then the start bit is confirmed, but it will be set NEF noise flag.

•     The samples of the 3rd, 5th, and 7th bits have two '0' points, and at the same time, the samples of the 8th,
9th, and 10th bits have two '0' points, then it is confirmed that the start bit is received, but it will be set bit
NEF noise flag.

•     If the sampling values in the 3rd, 5th, 7th, 8th, 9th and 10th bits cannot meet the above four
requirements, the USART receiver thinks that it has not received the correct start bit, and will exit the
start bit detection and Return to idle state and wait for falling edge.

*Figure 17-5 Start Bit Detection*


RX line

sample
clock                               1       2       3       4       5       6       7        8       9       10 11 12   13 14 15    16

One-bit time
Conditions
1     1    1      0       x       0       x       0       x       0        0       0       0    x x x      x    x x
to validate
the start bit
Falling edge                      At least 2 bits At least 2 bits
detection                        out of 3 at 0    out of 3 at 0

##### 17.5.2.2. Character reception

During USART reception, the least significant bit of the data is first shifted in from the RX pin. In this mode, the

buffer contained in the USART_DAT register is located between the internal APB bus and the receive shift
register. The USART receiver is enabled using the following steps:

•   Set the UEN bit in the USART_CTRL1 register to enable USART;

•   Write the WL bit in the USART_CTRL1 register to set the word length;

•   Write the STPB[1:0] bits in the USART_CTRL2 register to set the length of the stop bits;

•   If multi-buffer communication mode is selected, DMA should be enabled in the USART_CTRL3 register (DMARXEN
bit);

•   Set the baud rate in the USART_BRCF register;

•   Set the RXEN bit in USART_CTRL1. When a data frame is received:

•   The RXDNE bit is set, and the content of the shift register is transferred to the RDR (Receiver Data Register). At this
point, the data has been received and can be read out (including the associated error flags).

•   If the RXDNEIEN bit is set, an interrupt is generated.

•   During the reception process, frame errors, noise, or overflow errors are detected, and the error flags will be set.

•   In multi-buffer communication mode, the RXDNE flag bit is set after each byte is received and cleared by the DMA
read operation of the data register.

•   In single buffer mode, the software can clear the RXDNE bit by reading the USART_DAT register or writing 0 can
also clear the RXDNE bit. The RXDNE bit must be cleared before the end of the next frame data reception to avoid
overflow errors.

During the reception process, RXNE must be enabled; otherwise, the current data frame will be lost.

##### 17.5.2.3. Break frame

When a break frame is received, the USART frame error flag will be set.

> **Note: In LIN mode, when a break frame is received, it will be treated as a break frame, and LINBDF will be set.**

##### 17.5.2.4. Idle symbol

When an idle frame is detected, the processing steps are the same as receiving a normal data frame, but if the IDLEIEN bit
is set, an interrupt will be generated.

##### 17.5.2.5. Overrun error

If the received data is not read in time, causing RXDNE to not be reset in time, and another character is received, an
overflow error occurs. The data can only be transferred from the shift register to the RDR register when the RXDNE bit is
cleared. The RXDNE flag is set by hardware after each byte is received. If the next data has been received or the previous
DMA request has not been serviced, the RXDNE flag remains set, and an overflow error is generated.

When an overflow error occurs:

•   The OREF bit is set.

•   The content of RDR will not be lost. Reading the USART_DAT register can still obtain the previous data.

•   The content of the shift register will be overwritten. Subsequently received data will be lost.

•   If the RXDNEIEN bit is set or both the ERRIEN and DMARXEN bits are set, an interrupt is generated.

•   Sequentially performing read operations on the USART_STS and USART_DAT registers can reset OREF.

> **Note: When OREF is set, it indicates that at least 1 data has been lost. There are two possibilities:**

•   If RXDNE=1, the previous valid data is still in the receive register RDR and can be read.
•   If RXDNE=0, it means that the previous valid data has been read, and there is nothing to read in RDR. This situation
may occur when new (i.e., lost) data is received while the previous valid data is being read in RDR. This situation may
also occur when new data is received during the read sequence (between the read access of the USART_STS register
and the read access of the USART_DAT).

Noise errors use oversampling techniques (except in synchronous mode) to perform data recovery by distinguishing
between valid input data and noise.

*Table 17-2 Data Sampling For Noise Detection*


Sample Value         NE Status             Received Bits        Data Validity

000                  0                     0                    Effective

001                  1                     0                    be invalid

010                  1                     0                    be invalid

011                  1                     1                    be invalid

100                  1                     0                    be invalid

101                  1                     1                    be invalid

110                  1                     1                    be invalid

111                  0                     1                    Effective

When noise is detected in the received frame, the following actions can be taken:

•   The NEF flag is set on the rising edge of the RDNE bit.

•   Invalid data is transferred from the shift register to the USART_DAT register.

•   In the case of single-byte communication, no noise interrupt is generated. However, because the NEF flag and the
RXDNE flag are set at the same time, RXDNE will generate an interrupt. In the case of multi-buffer communication, if
the ERRIEN bit in the USART_CTRL3 register has been set, an interrupt will be generated.

Sequentially reading the USART_STS register and then reading the USART_DAT register will clear the NEF flag.

##### 17.5.2.6. Framing error

A frame error is detected when the following situation occurs: the stop bit is not received and recognized at the expected
time due to data not being synchronized or due to a large amount of noise.

When a frame error is detected, the hardware sets the FEF bit, and invalid data will be transferred from the shift register to
the USART_DAT register.

In single-byte communication, no frame error interrupt is generated. However, this bit and the RXDNE bit are set
simultaneously, and the latter will generate an interrupt. In multi-buffer communication, if the ERRIEN bit in the
USART_CTRL3 register is set, an interrupt will be generated.

Sequentially performing read operations on the USART_STS and USART_DAT registers can reset the FEF bit.

##### 17.5.2.7. Configurable stop bits during reception

During data reception, the number of data stop bits can be configured by the USART_CTRL2.STPB[1:0]. In normal mode, 1

or 2 stop bits can be selected. In Smartcard mode, 0.5 or 1.5 stop bits can be selected.

•   0.5 stop bits (receive in smartcard mode): 0.5 stop bits are not sampled. Therefore, if 0.5 stop bits is selected, framing
errors and broken frames cannot be detected.

•   1 stop bit: the sampling of one stop bit is carried out through three points, and the 8th, 9th and 10th sampling bits are
selected.

•   1.5 stop bit (Smartcard mode): when sending in Smartcard mode, the device must check whether the data is sent
correctly. So the receiver function block must be activated (USART_CTRL1.RXEN=1) and sample the signal on the
data line during the transmission of the stop bit. If a parity error occurs, the smartcard will pull down the data line
when the transmitter samples the NACK signal, that is, within the time corresponding to the stop bit on the bus,
indicating that a framing error has occurred. The USART_STS.FEF is set together with the USART_STS.RXDNE at
the end of the 1.5th stop bit. The 1.5 stop bits were sampled at points 16, 17 and 18. The 1.5 stop bits can be divided
into two parts: one is 0.5 clock cycles, during which nothing is done. This is followed by the stop bit of 1 clock cycle,
which is sampled at the midpoint of this period of time. For details, see Section 17.5.10 : Smartcard mode.

•   2 stop bits: the sampling of the 2 stop bits is completed at the 8th, 9th and 10th sampling points of the first stop position. If
a frame error is detected during the first stop bit, the frame error flag is set. The second stop bit does not detect
framing error. The USART_STS.RXNE flag will be set at the end of the first stop bit.

#### 17.5.3. Fractional baud rate generation

The baud rate of the USART can be configured in the USART_BRCF register. This register defines the integer and
fractional parts of the baud rate divider. The baud rate of the transmitter and receiver should be configured to the same
value. Be careful not to change the value of the USART_BRCF register during communication, because the baud rate
counter will be replaced by the new value of the baud rate register.

TX / RX baud rate = 𝑓PCLK /(16 ∗USARTDIV)

where 𝑓PCLK is the clock provided to the peripheral:
•   PCLK1 is used for USART2, up to 48MHz;

•    PCLK2 is used for USART1, up to 48 MHz. USARTDIV is an unsigned fixed-point number.

##### 17.5.3.1. USARTDIV and USART_BRCF register configuration

Example 1:

If DIV_Integer = 27, DIV_Decimal = 12 (USART_BRCF = 0x1BC), then DIV_Integer(USARTDIV) = 27

DIV_Decimal(USARTDIV) = 12/16 = 0.75 So USARTDIV = 27.75

Example 2:

Requirements USARTDIV = 25.62, there are:

DIV_Decimal = 16*0.62 = 9.92 Closest integer is: 10 = 0x0A

DIV_Integer = DIV_Integer(25.620) = 25 = 0x19 So, USART_BRCF = 0x19A

Example 3:

Requirements USARTDIV = 50.99, there are: DIV_Decimal = 16*0.99 = 15.84

Closest integer: 16 = 0x10 => DIV_Decimal[3:0] overrun => carry must be added to the fractional part DIV_Integer =

DIV_Integer (0d50.990+carry) = 51 = 0x33

So: USART_BRCF= 0x330, USARTDIV = 0d51.00

*Table 17-3 Error Calculation When Setting Baud Rate*


Baud Rate                       fPCLK = 36MHz                                       fPCLK = 48MHz

serial
Kbps    Reality      Set value in register    Error(%)      Reality       Set value in register   Error(%)
number

1       2.4        2.4               937.5               0%            2.4                1250              0%

2       9.6        9.6              234.375              0%            9.6               312.5              0%

3      19.2       19.2             117.1875              0%            19.2              156.25             0%

4      57.6       57.6              39.0625              0%           57.623            52.0625            0.04%

5      115.2    115.384               19.5              0.15%         115.1             26.0625            0.08%

6      230.4    230.769               9.75              0.16%        230.769                 13            0.16%

7      460.8    461.538              4.875              0.16%        461.538                 6.5           0.16%

8      921.6    923.076              2.4375             0.16%        923.076              3.25             0.16%

9      2250       2250                  1                0%         2285.714             1.3125            1.58%

10      3000    impossible          impossible         impossible      3000                   1              0%

Notes: The lower the clock frequency of the CPU, the lower the error for a particular baud rate.

#### 17.5.4. Receiver’s tolerance clock deviation

The USART asynchronous receiver requires that the overall clock system variation is within the tolerance range
of the USART asynchronous receiver. Factors affecting these variations include:

Variations caused by transmitter errors (including variations in the transmitter-side oscillator)

•   Errors caused by rounding the baud rate on the receiver side

•   Variations in the receiver-side oscillator

•   Variations caused by the transmission line (usually due to inconsistencies between the transition timing from
low to high and the transition timing from high to low in the transceiver).

The errors caused by the above four points need to

satisfy: 1+2+3+4 < USART receiver's tolerance

For normal data reception, the USART receiver's tolerance is equal to the maximum tolerable variation, which
depends on the following selections:

•   10 or 11-bit character length defined by the WL bit in the USART_CTRL1 register

•   Whether fractional baud rate generation is used

*Table 17-4 When DIV_Decimal = 0. Tolerance Of USART Receiver*


WL bit                                 NF is an error                          NF is don’t care

0                                      3.75%                                   4.375%

1                                      3.41%                                   3.97%

*Table 17-5 When DIV_Decimal != 0. Tolerance Of USART Receiver*


WL bit                                 NF is an error                          NF is don’t care

0                                      3.33%                                   3.88%

1                                      3.03%                                   3.53%

> **Note: In special cases, when the received frame contains some idle frames that are exactly 10**
bits at WL=0 (11 bits at WL=1),

There may be slight discrepancies between the data in the 2 tables above.

#### 17.5.5. Parity control

Parity can be enabled by configuring the USART_CTRL1.PCEN bit.

When the parity bit is enabled for transmission, A parity bit is generated, parity check is performed on reception.

*Table 17-6 Frame Format*


WL bit           PCEN bit        USART Frame

0                0               | Start bit | 8-bit data | Stop bit |

0                1               | Start bit | 7 bits of data | Parity bit | Stop bit |

1                0                  | Start bit | 9-bit data | Stop bit |

1                1                  | start bit | 8-bit data | parity bit | stop bit |

Even parity:

Configure USART_CTRL1.PSEL to 0, and even parity can be selected.

Make the number of '1' in the transmitted data (including parity bit) be an even number. That is: if
Data=11000101, there are 4 '1's, then the parity bit will be '0' (4 '1' in total). After the data and check digit are
sent to the receiver, the receiver calculates the number of 1s in the data again. If it is an even number, the check is
passed, indicating that no errors occurred during the transmission process. If it is not even, it means that an error
has occurred, the USART_STS.PEF flag is set to '1', and if USART_CTRL1.PEIEN is enabled, an interrupt is
generated.

Odd parity:

Configure USART_CTRL1.PSEL to 1, you can choose odd parity.

Make the number of '1' in the transmitted data (including parity bit) be an odd number. That is: if
Data=11000101, there are 4 '1's, then the parity bit will be '1' (5 '1' in total). After the data and check digit are
sent to the receiver, the receiver calculates the number of 1s in the data again. If it is an odd number, the check is
passed, indicating that no errors occurred during the transmission process. If it is not an odd number, it means that
an error has occurred, the USART_STS.PEF flag is set to '1', and if USART_CTRL1.PEIEN is enabled, an
interrupt is generated.

#### 17.5.6. Multiprocessor communication

Multiprocessor communication refers to multiple USARTs connected to a common network. For example, a USART
device can be the master, and its TX output is connected to the RX input of other USART slave devices; the
respective TX outputs of the USART slave devices are logically connected together and connected to the RX
input of the master device. In a multiprocessor configuration, it is a large processor overhead for a device to
monitor all RX pins, and the mute mode can be turned on, which can reduce redundant USART Service Burden.
In mute mode.

•      None of the receive status bits will be set.

•      All receive interrupts are disabled.

•      The RCVWU bit in the USART_CTRL1 register is set to 1. RCVWU can be automatically controlled by
hardware or written by software under certain conditions.

Depending on the state of the WUM bit in the USART_CTRL1 register, the USART can enter or exit silent mode
in two ways.

•      If the WUM bit is reset: Perform idle bus detection.

•      If the WUM bit is set: Perform address mark detection.

USART allows multiprocessor communication. The principle is: multiple processors communicate through
USART, and it is necessary to determine who is the master device, and the remaining processors are all slave

devices. The TX output of the master device is directly connected to the RX port of all slave device. The TX
outputs of the slaves are logically AND together and connected to the RX inputs of the master.

When multi-processor communication is performed, the slave devices are all in mute mode, and the host uses a
specific method to wake up a slave device to be communicated when needed, so that the slave device is in an
active state and transmits data with the master device.

The USART can wake up from mute mode by idle line detection or address mark detection.

##### 17.5.6.1. Idle line detection (WUM=0)

When the RCVWU bit is set, the USART enters mute mode. When the RX pin detects an idle frame, RCVWU is
cleared by hardware, but the IDLEF bit in the USART_STS register will not be set to 1. RCVWU can also be
written to 0 by software. The RCVWU state when an idle frame is detected is shown in Figure 17-7 below.

*Figure 17-8 Mute Mode Using Idle Line Detection*


RXDNE = 1   RXDNE = 1

RX          Data1        Data2    Data3      Data4            IDLE   Data5     Data6

RCVWU                              Mute Mode                             Normal Mode

RCVWU written to 1                            Idle frame detected

##### 17.5.6.2. Address mark detection (WUM=1)

In this mode, the highest bit is the address flag, if it is 1, the byte is the address, otherwise it is the data. In an
address byte, the address of the intended receiver is placed in the lower 4 bits. If this 4-bit address is
different in ADDR[3:0] of USART_CTRL2 register, that is, when the received byte does not match its
programmed address, the USART will enter mute mode and set the RCVWU bit. In this case, RXDNE will not be
set either.

When the received byte is the same as the programmed address in the receiver, receive the data frame that wakes
up the USART, then the hardware clears the RCVWU bit, the USART exits the mute mode, and the subsequent
bytes are received normally, and the matching address word is received The RXDNE bit will be set during the
festival because the receive The wake-up bit RCVWU bit has been cleared.

When the receive buffer does not contain data (RXDNE=0 in USART_STS), the receive wake-up bit RCVWU
can be written as 0 or 1. Otherwise, the write operation is ignored.

The figure below shows an example of using address mark detection to wake up and enter mute mode.

*Figure 17-4 Mute Mode Detected Using Address Mark*


In this example, the current address of the receiver is 1                 RXDNE=1 RXDNE=1         RXDNE=1

ADDR                                            ADDR                      Addr
RX         IDLE
=0
Data1       Data2        IDLE         =1    Data3   Data4       =2           Data5

RCVWU                                     Mute Mode                               Normal Mode                 Mute Mode

Error address                       Current address
RCVWU written to 1                                                                              Error address
（RXDNE was cleared）

#### 17.5.7. LIN mode

LIN mode is selected by setting the LINMEN bit of the USART_CTRL2 register. In LIN mode, the following
registers must be cleared to 0:

•   CLKEN bit and STPB[1:0] of USART_CTRL2 register

•   SCMEN, HDMEN and IRDAMEN of USART_CTRL3 register.

##### 17.5.7.1. LIN receiving and transmitting

There are some differences between the sending steps of LIN and the normal operation process of USART, When
sending ordinary data frames, the LIN sending process is the same as the ordinary sending process, but the data
length can only be 8, so clear the WL bit to configure the 8-bit word length, and set the SDBRK in USART_CTRL1
to send 13 '0' as Disconnect sign. Then send a stop bit. When the LIN mode is enabled, the break symbol detection
function is completely independent of the USART receiver, and the break detection can be used when the bus is
free and between sending a certain data frame.

When the receiver is activated (RXDEN=1 of USART_CTRL1), the circuit starts to monitor the start signal on RX.
When the start bit is detected, it samples the 8th, 9th, and 10th oversampling clock points of each bit. . If 10 or
11 consecutive bits are '0' followed by a fixed delimiter, the LINBDF flag of USART_STS is set. If LINBDIEN
bit=1, an interrupt will be generated. Check the delimiter before acknowledging the break symbol because it means
the RX line has returned to high.

If a '1' is sampled before the 10th or 11th sampling point, the detection circuit searches for the start bit again and
cancels the current detection. If LIN mode is disabled, the receiver does not need to detect break symbols.

If the LIN mode is enabled (LINMEN=1), a framing error occurs (that is, the stop bit detects '0', which occurs in a
break frame), and the receiver will be stopped until the break symbol detection circuit receives a A '1' (this happens
when a break symbol has not been emitted completely), or a delimiter (this happens when a full break symbol has
been detected).

*Figure 17-9 shows the behavior of the broken symbol detector state machine in relation to the broken symbol flag.*

*Figure 17-10 shows an example of a disconnected frame*


*Figure 17-9 Break Detection in LIN Mode (11-bit break length-the LINBDL bit is set)*


Case 1: break signal not long enough => break discarded, LINBDF is not set

Break frame

Idle     Bit0     Bit1       Bit2          Bit3       Bit4        Bit5      Bit6        Bit7       Bit8       Bit9       Bit10         Idle
Break frame

Sample

Sample data                      0           0            0          0          0           0           0          0          0          0       1

LINBDF

Case 2: break signal just long enough => break detected

Break frame

delimiter is immediate

Break frame      Idle     Bit0        Bit1       Bit2         Bit3       Bit4        Bit5       Bit6        Bit7       Bit8       Bit9   B10            Idle

Sample
Sample data                      0          0            0          0           0          0           0          0          0          0       0

LINBDF

Case 3: break signal long enough => break detected

Break frame
RX line

Break frame    Idle     Bit0    Bit1    Bit2      Bit3       Bit4   Bit5       Bit6     Bit7    Bit8       Bit9 Bit10        wait delimiter           Idle

Sample
Sample data                      0          0            0          0           0          0           0          0          0          0       0

LINBDF

*Figure 17-10 Break Detection And Framing Error Detection in LIN Mode*


In these examples, we suppose that LINBDL=1(11-bit break length),WL=0(8-bit data)

Break occurring after an Idle：

RX line          frame1        Idle                                            frame2        frame3

1 frame time               1 frame time

RXDNE/FEF

LINBDF

Break occurring while a data is being received：

RX line          frame1        frame2                                          frame2        frame3

1 frame time                             1 frame time

RXDNE/FEF

LINBDF

#### 17.5.8. Usart Synchronous mode

Synchronous mode is selected by writing the CLKEN bit in the USART_CTRL2 register, the LINMEN bit
in the USART_CTRL2 register, the SCMEN, HDMEN and IRRAMEN bits in the USART_CTRL3 register must
be zero.

USART only supports master mode to control bidirectional synchronization serial communication, and cannot use
the input clock from other devices to receive or send data (CK is always an output). The CK pin is used as the
output of the USART transmitter clock. During start bit and stop bit, CK pin will not output clock pulse.

The external CK clock is not active during the bus idle state, before the actual data arrives and when the break
symbol is sent. The CK pin works jointly with the TX pin. Thus, the clock is only provided when the transmitter is
enabled (TXEN=1) and data is being transmitted (writing data to the USART_DAT register). A USART receiver
in synchronous mode works differently than in asynchronous mode. If RXEN=1, data is sampled on CK
without oversampling. But setup time and duration (depending on baud rate, 1/16 bit time) must be considered.

Notes:

1. Synchronous mode does not send data and cannot receive synchronous data.


2. LBCLK, CLKPOL and CLKPHA bits can only be configured when synchronous mode is not working.


3. The same code sets TXEN and RXEN, which can reduce the setup time and hold time of the receiver.


*Figure 17-11 USART Synchronous Transmission Example*


RX                        MISO
TX                        MOSI

USART                                 SPI（slave）

CLK

*Figure 17-12 USART Data Clock Timing Example (WL=0)*


Clock（CLKPOL=0，CLKPHA=0）

Clock（CLKPOL=0，CLKPHA=1）

Clock（CLKPOL=1，CLKPHA=0）

Clock（CLKPOL=1，CLKPHA=1）

Data on TX                                                    6
01       2   3     4   5        7
(from master)
Start LSB                            MSB Stop

Data on RX
01       2   3     4   5   6    7
(from slave)
LSB                             MSB

*Figure 17-13 USART Data Clock Timing Example (WL=1)*


Clock（CLKPOL=0，CLKPHA=0）

Clock（CLKPOL=0，CLKPHA=1）

Clock（CLKPOL=1，CLKPHA=0）

Clock（CLKPOL=1，CLKPHA=1）

Data on TX                           0     1    2      3     4    5      6   7    8
(from master)
Start LSB                                         MSB Stop

Data on RX
0     1    2      3     4    5      6   7    8
(from slave)
LSB                                          MSB

*Figure 17-14 RX Data Sampling / Holding Time*


SCLK(capture strobe on SCLK
rising edge in this example)

Data on RX
valid DATA bit
(from slave)

tSETUP         tHOLD
tSETUP = tHOLD 1/16bit time

> **Note: the function of CK is different in Smartcard mode, please refer to the Smartcard mode section for details.**

#### 17.5.9. Single-wire half-duplex mode

Single-wire half-duplex mode is selected by setting the HDMEN bit of the USART_CTRL3 register. The
following bits must be set to zero, the LINMEN and CLKEN bits of the USART_CTRL2 register, and the
SCMEN and IRDAMEN bits of the USART_CTRL3 register.

In single-wire half-duplex mode, the TX and RX pins are interconnected inside the chip, and the RX pin is not used anymore.

The HDMEN bit in USART_CTRL3 selects half-duplex and full-duplex communication. TX is always released when there is
no data transfer. Therefore, when it is in the idle state or the receive state Appears as a standard I/O port. Therefore, when the
TX port is not enabled by the USART, it must be configured as a floating input or an open-drain output high.
Communication collisions are managed by software (eg by using a central arbiter).

#### 17.5.10. Smartcard mode (ISO7816)

Smart card mode is an asynchronous communication mode that supports the ISO7816-3 protocol.

Set the SCMEN bit of the USART_CTRL3 register to select the smart card mode. In smart card mode, the following
registers must be cleared: LINMEN bit of USART_CTRL2 register, HDMEN bit and IRDAMEN bit of USART_CTRL3
register

If the CLKEN bit is set, the USART provides the clock to the smart card through the CK pin. This interface supports the
smart card asynchronous protocol. USART should be set to:

•   8 data bits plus parity bit: At this time, WL=1, PCEN=1 in the USART_CTRL1 register

•   1.5 stop bits when sending and receiving: ie STPB=11 of USART_CTRL2 register

> **Note: 1/2 stop bits can also be selected on receive, but to avoid switching between the 2 configurations, it is recommended to**
use 1.5 stop bits on both transmit and receive.

The figure below shows two situations with and without check digit.

*Figure 17-15 ISO7816-3 Asynchronous Protocol*


Without Parity error
Guard time

S     0     1     2      3     4     5       6       7   P
Start
bit

With Parity error
Guard time

S     0     1     2      3     4     5       6       7   P              Line pulled low by
receiver during stop
Start                                                                     In case of parity error
bit

When connecting with a smart card, the TX pin needs to be set to open-drain mode, and an external pull-up resistor is
connected. This pin will drive the same bidirectional connection with the smart card. During the data byte, it is released
(weak pull-up) during the transmission of the stop bit, so the receiver can pull the data line low in case of parity error. If
TXEN is not enabled, TX will be pulled high during the stop bit.

•   To send data from the transmit shift register, the delay must be greater than or equal to half a baud clock. During
normal operation, a full transmit shift The bit register will begin shifting out data on the next baud clock edge. In smart
card mode, the delay is half a baud clock.

•   If a parity error is detected during reception of a data frame with 0.5 or 1.5 stop bits set, at the end of the stop bits, The
transmit line will be pulled low for one baud clock cycle (NACK), notifying the smart card that the USART did not
receive the data correctly. This NACK signal A framing error will be generated on the sender, meaning the sender is
configured for 1.5 stop bits. Programs can resend data. if set If the SCNACK control bit is set, the receiver will give a
NACK signal when a parity error occurs; otherwise, it will not send a NACK.
•   The setting of TXC flag can be delayed by programming the protection time register. When the transmit shift register
is cleared and no new transmits please When requested, TXC is set high. In smart card mode, an empty transmit shift
register will trigger the guard time counter to start counting up. Before the value reaches, TXC is forced to be low, and
after the count value is reached, TXC is set high.

•   Smart card mode does not affect the deactivation of the TXC flag. 298 / 400

•   If the transmitter detects a NACK signal received by the receiver, the receiver function module of the transmitter will
not treat NACK as a start bit detection. The duration of a received NACK can be 1 or 2 baud clock periods according
to the ISO protocol.

•   On the receiver side, if a parity error is detected, NACK is also sent, and the receiver will not detect NACK as a start
bit.

Notes:

(1) The break symbol has no meaning in smart card mode. A 00h data with framing error will be treated
as data instead of break symbol.

(2) When the TXEN bit is toggled back and forth, no IDLE frame is sent. The ISO protocol does not define IDLE frames.

*Figure 17-16 samples the NACK signal for the USART. In the figure the USART is sending data and is configured*

for 1.5 stop bits. to check Data integrity and NACK signals enable the receive function block of the USART.

*Figure 17-16 Use 1.5 Stop Bits to Detect Parity Errors*


Bit 7       Parity Bit                 1.5 Stop Bit

1 bit time                      1.5 bit time

sampling at                           sampling at
8th, 9th, 10th                       16th, 17th, 18th

0.5 bit time       1 bit time

sampling at                            sampling at
8th, 9th, 10th                         8th, 9th, 10th

USART can provide clock for smart card through CK output. In the smart card mode, CK is not directly related to
communication, but first drives the clock of the smart card with the internal peripheral input clock through a 5-bit

prescaler. The division ratio can be configured in the prescaler register USART_GTP. The frequency of CK division is from f
CK /2 to f CK /62, where f CK is the peripheral input clock.

#### 17.5.11. IrDA Infrared decoding function

The IrDA mode is selected by setting the IRDAMEN bit in the USART_CTRL3 register. In this mode, the LINMEN, STPB,
CLKEN, SCMEN, and HDMEN bits need to be cleared.

The IrDA SIR physical layer uses a Return-to-Zero Inverted (RZI) modulation scheme, where the infrared light source
pulse (RTZ signal) represents a logical 0 (see Figure 17-17). The pulse width is specified as 3/16 of a bit period.

In IrDA mode, the USART data frame is modulated by the SIR transmit encoder. The modulated signal is sent via an
infrared LED and, after demodulation, the data is sent to the USART receiver. For the encoder, the baud rate should be less
than 115200.

The Return-to-Zero (RTZ) bit stream from the infrared receiver is demodulated by the SIR receive decoder, and the received
Non-Return-to-Zero (NRZ) serial bit stream is then output to the USART.

In the idle state, the decoder input is usually high, and the polarity of the transmit encoder output is opposite to that of the
decoder input. When the decoder input is low, a start bit is detected.

•   The receiver can communicate with a low-power transmitter.

•   The SIR transmit logic sends '0' as a high pulse and '1' as a low level. The pulse width is specified as 3/16 of the bit
period in normal mode (see Figure 17-18).

•   The SIR receive logic interprets the high level state as '1' and the low pulse as '0'.

•   The polarity of the transmit encoder output is opposite to that of the decoder input. When idle, the SIR output is in a
low state.

•   The SIR decoder converts the IrDA-compatible received signal into a bit stream for the USART.

•   The pulse width is programmable. The IrDA specification requires the pulse to be wider than 1.41us. (PSCV is the
prescaler value programmed in the IrDA low-power baud rate register USART_GTP). Those with a width greater than 2
cycles are considered a valid pulse. When PSCV=0, the IrDA encoder/decoder does not work.

•   In IrDA mode, the STPB bit in the USART_CTRL2 register must be configured for 1 stop bit.

##### 17.5.11.1. IrDA low power mode transmitter and receiver

In the low-power mode of the transmitter, the pulse width no longer lasts for 3/16 of a bit period. Instead, the pulse width is
3 times the low-power baud rate, which can be as low as 1.42MHz.

The reception in the low-power mode of the receiver is similar to the reception in normal mode. To eliminate other power
supply interference, low-level signals of the IrDA low-power baud rate clock with a duration greater than 2 cycles are
considered valid signals.

Notes:

(1) Pulses with a width less than 1 PSCV cycle must be filtered out, but those with a width greater than 1 but less than 2
PSCV cycles may be received or filtered out.

(2) Software manages the control of the receiver's setup time. The IrDA Physical Layer Technical Specification
protocol specifies a minimum delay of 10ms between transmission and reception.

*Figure 17-17 IrDA SIR ENDEC-Block Diagram*


TX
0
TX pin
Transmit         1
Encoder
NORMAL                  IRDAMEN
= 0 or 1
USART
MODE                                            Receive
1       Decoder                      RX pin
RX
0

*Figure 17-18 IrDA Data Modulation (3/16)-Normal Mode*


TX normal                Start
1         0       0       1      0        0         1      0    Stop
frame                   bit                                                                        bit

TX pin
frame

3/16

RX pin
frame

RX normal                Start                                                                       Stop
bit       1         0       0       1      0        0         1     0
frame                                                                                               bit

#### 17.5.12. DMA communication mode

The USART can use DMA to access the send buffer and receive buffer respectively.

##### 17.5.12.1. DMA transmitting

The steps to allocate a DMA channel for USART transmission are as follows (x represents the channel number):

•   The USART_DAT register address is configured as the destination address of the DMA transfer, and the
memory address is configured as the source address of the DMA transfer.

•   Configure the total number of bytes to transfer.

•   Configure the channel priority.

•   Configure the DMA interrupt to be generated when the transfer is half or fully completed.

•   Activate the channel.
When a DMA transfer is complete, an interrupt is generated on the corresponding DMA channel. In the sending

mode, when the DMA transmits all the data to be sent, the DMA controller sets the TXCFx flag of the
DMA_INTSTS register, and the TXC flag is set high by the hardware to indicate that the transmission is
complete. The software needs to wait for TXDE=1 first, and then wait for TXC= 1.

*Figure 17-19 Transmission Using DMA*


TXDE flag                                 set by hardware         cleared by DMA

DMA writes Data0   DMA writes Data1                  DMA writes DataN
into USART_DAT     into USART_DAT                    into USART_DAT

DMA request

Data 0                               Data 1         Data N
TX line
Software waits
TXC=1

TXC flag                                                                                     set by hardware

DMA transfer
is complete

DMA TXCF Flag
set by hardware      cleared by software

##### 17.5.12.2. DMA reception

The steps to allocate a DMA channel for USART reception are as follows (x represents the channel number):

•   Configure the USART_DAT register address as the source address of the transfer through the DMA control
register, and set the destination address of the transfer with the memory address.

•   Configure the number of DMA bytes to transfer.

•   Configure the channel priority on the DMA registers, configure the transfer.

•   Configure the DMA interrupt to be generated when the transfer is half or fully completed.

•   Activate the channel.

When receiving the transfer amount specified by the DMA controller, the DMA controller generates an interrupt
on the interrupt vector of the DMA channel..

*Figure 17-20 Reception Using DMA*


Data 0                           Data 1                      Data N
RX line

RXDNE flag                              set by hardware        cleared by DMA

DMA reads Data0             DMA reads Data1                 DMA reads DataN
from USART_DAT              from USART_DAT                  from USART_DAT

DMA request

DMA transfer
is complete

DMA TXCF flag
set by hardware    clear by software

##### 17.5.12.3. Error flags and interrupt generation in multi-buffer communication

When an error occurs in the multi buffer communication, an error flag will be set when the current byte transmission is
completed. If the interrupt enable bit is set, an error interrupt will be generated. When a single byte is received, the frame
error, overflow error and noise flags set together with RXDNE will cause an interrupt when the current byte transmission
ends if a separate error flag interrupt enable bit is set.

17.5.12 Hardware flow control
Hardware flow control functions are implemented through nCTS input and nRTS output. The figure below shows
how to connect two devices in this mode.

*Figure 17-20 Hardware Flow Control Between Two USART*


DEVICE 1                                      DEVICE 2
TX                   RX

TX control                                    RX control
CTS              RTS

RX                   TX

RX control                                    TX control
RTS               CTS

RTS and CTS flow control can be enabled independently by setting RTSEN and CTSEN in USART_CTRL3 respectively.

##### 17.5.12.4. RTS flow control

If RTS flow control is enabled (RTSEN=1), when a frame of data is received, nRTS is high, otherwise it is low. The figure
below is an example of communication with RTS flow control enabled.

*Figure 17-21 RTS Flow Control*


Waiting to read data                   Read data register
register,RTS high                      end,RTS low
RTS line

Start                               Stop           Start
RX line                             Data 1                     Idle                      Data 2             Stop
Bit                                 Bit            Bit
Bit

##### 17.5.12.5. CTS flow control

If CTS flow control is enabled (CTSEN=1), the transmitter checks the nCTS pin to decide whether to send data
before sending the next frame. If nCTS is pulled low (active), the transmitter sends data (assuming that data is
ready to be sent, that is, TXDE=0). If nCTS is pulled high during transmission, the transmission of the current data
frame will stop after the transmission is completed. If the CTS flow control is enabled (CTSEN=1), the nCTS
pin signal bit changes, the CTSF status bit is set to 1, and if the CTSIEN bit of the USART_CTRL3 register is
set, an interrupt will be generated, as shown in Figure 17-23 to enable CTS flow control.

*Figure 17-23 CTS Flow Controls*


CTSF = 1                        CTSF = 1

CTS line

Writing Data 3 in
Data register

Data register       Data 2             empty                        Data 3                          empty

CTS = 1,                            CTS = 0,
Transmit delay                      Transmit Data 3
Start                                               Start
TX line         Data 1    Stop    Bit            Data 2             Stop     Idle                Data 3
Bit              Bit
Bit

### 17.6. Interrupt Request

The various interrupt events of USART are logical OR relations, if the corresponding enable control bit is set, these
events can generate their own interrupts, but only one interrupt request can be generated at the same time.

*Table 17-7 USART Interrupt Request*


Interrupt Function          Interrupt Event                                 Event Flag                Enable Bit

Transmission data register is empty.            TXDE                      TXDEIEN

CTS flag                                        CTSF                      CTSIEN

Transmission complete                           TXC                       TXCIEN

Receive data ready to be read                   RXDNE
RXDNEIEN
Data overrun error detected.                    ORERR
USART global interrupt

Idle line detected                              IDLEF                     IDLEIEN

Parity error                                    PEF                       PEIEN

Disconnect flag                                 LINBDF                    LINBDIEN

Noise, overrun error and framing error in
NEF/OREF/FEF              ERRIEN(1)
multi-buffer communication

> **Note: This flag is only used when receiving data using DMA. The various interrupt events of the USART are in a**
logical OR relationship (see Figure 17-24 below). If the corresponding enable control bit is set, these events can
generate their own interrupts, but only one interrupt request can be generated at the same time.

### 17.7. Usart Mode Configuration


*Table 17-8 USART Mode Setting (1)*


Communication Mode                           USART1              USART2

Asynchronous mode                                  Y                   Y

Hardware           flow       control
Y                   Y
mode

DMA                communication
Y                   Y
mode

Multiprocessor                                     Y                   Y

Synchronous mode                                   Y                   Y

Smartcard mode                                     Y                   Y

Single-wire          half duplex
Y                   Y
mode

IrDA infrared mode                                 Y                   Y

LIN                                                Y                   Y

> **Note:**
(1)
Y = support this mode, N = do not support this mode

### 17.8. USART Register


#### 17.8.1. USART Register Overview

*Table 17-9 USART Register Overview And Reset Value*


Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0

set

USART_
LINBDF

RXDNE
IDLEF
OREF
TXDE
CTSF

TXC

NEF

PEF
FEF

00     STS
Reserved
0h Reset
0      0        1      1     0       0       0      0     0     0
Value

USART_
DATV[8:0]
00     DAT
Reserved
4h Reset
0        0      0     0       0       0      0     0     0
Value

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

DIV_Decimal
USART_
DIV_Integer[11:0]
00 BRCF
Reserved                                                                                                                                                       [3:0]
8h
Reset
0    0        0      0   0        0        0       0        0          0          0          0         0       0         0        0
Value

RXDNEIEN
USART_

TXDEIEN

IDLEIEN

RCVWU
TXCIEN

SDBRK
PEIEN
PCEN

RXEN
TXEN
WUM

PSEL
UEN
WL
00 CTRL1
Reserved
Ch
Reset
0      0   0        0        0       0        0          0          0          0         0       0         0        0
Value

STPB
USART_

LINBDIEN
CLKPHA
CLKPOL
LINMEN

LINBDL
CLKEN

LBCLK
ADDR[3:0]
01 CTRL2

Reserved

Reserved
Reserved                                                   [1:0]
0h
Reset
0        0      0   0        0        0       0                   0          0                    0       0         0        0
Value

DMARXEN
DMATXEN
USART_

IRDAMEN
SCNACK

ERRIEN
CTSIEN

HDMEN
SCMEN

IRDALP
CTSEN
RTSEN
01 CTRL3
Reserved
4h Reset
0        0       0        0          0          0          0         0       0         0        0
Value

USART_
GTV[7:0]                                                            PSCV[7:0]
01     GTP
Reserved
8h Reset
0    0        0      0   0        0        0       0        0          0          0          0         0       0         0        0
Value

#### 17.8.2. USART Status Register (USART_STS)

Address offset : 0x00

Reset value : 0x0000 00C0

Bit Field   Name       Description

31:10       Reserved   Reserved, the reset value must be maintained

9           CTSF       CTS flag

If USART_CTRL3.CTSEN bit is set, this bit is set by hardware when the nCTS input
changes. If USART_CTRL3.CTSIEN bit is set, an interrupt will be generated.

This bit is cleared by software.

0:nCTS status line has not changed.

1:nCTS status line changes.

8           LINBDF     LIN break detection flag.

If USART_CTRL2.LINMEN bit is set, this bit is set by hardware when LIN
disconnection is detected. If USART_CTRL2.LINBDIEN bit is set, an interrupt will be
generated.

This bit is cleared by software.

0: LIN break character not detected.

1: LIN break character detected.

7           TXDE       The Transmit data register empty.

Set to 1 after power-on reset or data to be sent has been sent to the shift register. Setting
USART_CTRL1.TXDEIEN will generate an interrupt.

This bit is cleared to 0 when the software writes the data to be sent into USART_DAT.

0: Send data buffer is not empty.

1: The transmitting data buffer is empty.

6           TXC        Transmission complete.

This bit is set to 1 after power-on reset. If USART_STS.TXDE is set, this bit is set when
the current data transmission is completed.

Setting USART_CTRL1.TXCIEN bit will generate an interrupt.

This bit is cleared by software.

0: Transmitting did not complete.

1: Send completed.

5           RXDNE      The Read data register not empty.

This bit is set when the read data buffer receives data from the shift register. When
USART_CTRL1.RXDNEIEN bit is set, an interrupt will be generated.

Bit Field   Name    Description

Software can clear this bit by writing 0 to it or reading the USART_DAT register.

0: The read data buffer is empty.

1: The read data buffer is not empty.

4           IDLEF   IDLE line detected flag.

Within one frame time, the idle state is detected at the RX pin, and this bit is set to 1.
When USART_CTRL1.IDLEIEN bit is set, an interrupt will be generated.

The software can clear this bit by reading USART_STS first and then reading
USART_DAT.

0: No idle frame detected.

1: idle frame detected.

> **Note: IDLEF bit will not be set high again until USART_STS.RXDNE bit is set (that is,**
an idle line is detected again).

3           OREF    Overrun error

With RXDNE set, this bit is set if the USART_DAT register receives data from the shift
register. When USART_CTRL3.ERRIEN bit is set, an interrupt will be generated.

The software can clear this bit by reading USART_STS first and then reading
USART_DAT.

0: No overrun error was detected.

1: Overflow error detected.

2           NEF     Noise error flag.

When noise is detected in the received frame, this bit is set by hardware. It is cleared by
the software sequence (read first USART_STS, read USART_DAT again).

0: No noise error detected.

1: Noise error detected.

> **Note: this bit will not generate an interrupt because it appears with**
USART_STS.RXDNE, and the hardware will generate an interrupt when setting the
USART_STS.RXDNE flag. In the multi-buffer communication mode, if the
USART_CTRL3.ERRIEN bit is set, an interrupt will be generated when the NEF flag is
set.

1           FEF     Framing error.

When the data is not synchronized or a large amount of noise is detected, and the stop
bit is not received and recognized at the expected time, it will be judged that a framing
error has been detected, and this bit will be set to 1. First read USART_STS, then read

Bit Field              Name                Description

USART_DAT can cleared this bit.

0: No framing errors were detected.

1: A framing error or a Break Character is detected.

> **Note: this bit will not generate an interrupt because it appears with**
USART_STS.RXDNE, and the hardware will generate an interrupt when setting the
USART_STS.RXDNE flag. If the currently transmitted data has both framing errors and
overload errors, the hardware will continue to transmit the data and only set the
USART_STS.OREF flag bit.

In the multi-buffer communication mode, if the USART_CTRL3.ERRIEN bit is set, an
interrupt will be generated when the FEF flag is set.

0                      PEF                 Parity error.

This bit is set when the parity bit of the received data frame is different from the
expected check value.

The software can clear this bit by reading USART_STS first and then reading
USART_DAT.

0: No parity error was detected.

1: Parity error detected.

#### 17.8.3. USART Data Register (USART_DAT)

Address offset : 0x04

Reset value : undefined (uncertain value)

Bit Field              Name                Description

31:9                   Reserved            Reserved, the reset value must be maintained

8:0                    DATV[8:0]           Data value

Contains the data sent or received; Software can change the transmitted data by writing
these bits, or read the values of these bits to obtain the received data.

If parity is enabled, when the transmitted data is written into the register, the highest bit
of the data (the 7th or 8th bit depends on USART_CTRL1.WL bit) will be replaced by

Bit Field              Name         Description

the parity bit.

#### 17.8.4. USART Baud Rate Register (USART_BRCF)

Address offset : 0x08

Reset value : 0x0000 0000

> **Note: When USART_CTRL1.UEN=1, this register cannot be written; The baud counter stops**
counting if USART_CTRL1.TXEN or USART_CTRL1.RXEN are disabled respectively.

Bit field      Name                     Description

31:16          Reserved                 Reserved, the reset value must be maintained

15:4           DIV_Integer [11:0]       Integer part of baud rate divider.

3:0            DIV_Decimal[3:0]         Fractional part of baud rate divider.

#### 17.8.5. USART Control Register 1 (USART_CTRL1)

Address offset : 0x0C

Reset value : 0x0000 0000

Bit field              Name         Description

31:14                  Reserved     Reserved, the reset value must be maintained

13                     UEN          USART enable

Bit field   Name      Description

When this bit is cleared, the divider and output of USART stop working after the current
byte transmission is completed to reduce power consumption. Software can set or clear
this bit.

0:USART is disabled.

1:USART is enabled.

12          WL        Word length.

0:8 data bits.

1:9 data bits.

> **Note: If data is in transfer, this bit cannot be configured.**

11          WUM       Wake up mode from mute mode.

0: Idle frame wake up.

1: Address identifier wake up.

10          PCEN      Parity control enable

0: Parity control is disabled.

1: Parity control is enabled.

9           PSEL      Parity selection.

0: even check.

1: odd check.

8           PEIEN     PE interrupt enable

If this bit is set to 1, an interrupt is generated when USART_STS.PEF bit is set.

0: Parity error interrupt is disabled.

1: Parity error interrupt is enabled.

7           TXDEIEN   TXDE interrupt enable

If this bit is set to 1, an interrupt is generated when USART_STS.TXDE bit is set.

0: Send buffer empty interrupt is disabled.

1: Send buffer empty interrupt is enabled.

6           TXCIEN    Transmit complete interrupt enable.

If this bit is set to 1, an interrupt is generated when USART_STS.TXC is set.

0: Transmission completion interrupt is disabled.

Bit field   Name       Description

1: Transmission completion interrupt is enabled.

5           RXDNEIEN   RXDNE interrupt enable

If this bit is set to 1, an interrupt is generated when USART_STS.RXDNE or
USART_STS.OREF is set.

0: Data buffer non-empty interrupt o and overrun error interrupt are disabled.

1: Data buffer non-empty interrupt o and overrun error interrupt are enabled.

4           IDLEIEN    IDLE interrupt enable.

If this bit is set to 1, an interrupt is generated when USART_STS.IDLEF is set.

0:IDLE line detection interrupt is disabled.

1: IDLE line detection interrupt is enabled.

3           TXEN       Transmitter enable.

0: The transmitter is disabled.

1: the transmitter is enabled.

2           RXEN       Receiver enable

0: The receiver is disabled.

1: the receiver is enabled.

1           RCVWU      The receiver wakes up

Software can set this bit to 1 to make USART enter mute mode, and clear this bit to 0
to wake up USART.

In idle frame wake-up mode (USART_CTRL1.WUM=0), this bit is cleared by hardware
when an idle frame is detected. In address wake-up mode (USART_CTRL1.WUM=1),
when an address matching frame is received, this bit is cleared by hardware. Or when
an address mismatch frame is received, it is set to 1 by hardware.

0: The receiver is in normal operation mode.

1: The receiver is in mute mode.

0           SDBRK      Send Break Character.

The software transmits a break character by setting this bit to 1.

This bit is cleared by hardware during stop bit of the break frame transmission.

0: No break character was sent.

1: Send a break character.

Bit Field   Name        Description
31:15       Reserved    Reserved, the reset value must be maintained
14          LINMEN      LIN mode enable

0:LIN mode is disabled

1:LIN mode enabled
13:12       STPB[1:0]   STOP bits.

00:1 stop bit.

01:0.5 stop bit.

10:2 stop bit.

11:1.5 stop bit.

11          CLKEN       Clock enable

0:CK pin is disabled

1:CK pin enabled

10          CLKPOL      Clock polarity.

This bit is used to set the polarity of CK pin in synchronous mode.

0: CK pin remains low when it is not transmitted to the outside.

1: CK pin remains high when it is not sent to the outside.

9           CLKPHA      Clock phase.
This bit is used to set the phase of CK pin in synchronous mode.

0: Sample the first data at the first clock edge.

1: Sample the first data at the second clock edge

8           LBCLK       The Last bit clock pulse.
This bit is used to set whether the clock pulse corresponding to the last transmitted data byte
(MSB) is output on CK pin in synchronous mode.

0: The clock pulse of the last bit of data is not output from CK.

1: The clock pulse of the last bit of data will be output from CK.

7           Reserved    Reserved, the reset value must be maintained
6           LINBDIEN    LIN break detection interrupt enable.

If this bit is set to 1, an interrupt will be generated when USART_STS.LINBDF bit is set.

0: Disconnect signal detection interrupt is disabled.

1: Turn-off signal detection interrupt enabled

5           LINBDL      LIN break detection length.

This bit is used to set the length of the break frame.

0:10 bit break detection

1:11 bit break detection

> **Note: LINBDL can be used to control the detection length of Break Characters in LIN mode**
and other modes, and the detection length is the same as that in LIN mode.

4                 Reserved           Reserved, the reset value must be maintained

3:0               ADDR[3:0]          USART address.

Used in the mute mode of multiprocessor communication, using address identification to
wake up a USART device.

In address wake-up mode (USART_CTRL1.WUM=1), if the lower four bits of the received
data frame are not equal to the ADDR[3:0] value, USART will enter the mute mode; If the
lower four bits of the received data frame are equal to the ADDR[3:0] value, USART will be
awakened.

#### 17.8.6. USART Control Register 2 (USART_CTRL2)

Address offset : 0x10

Reset value : 0x0000 0000

> **Note: These three bits (USART_CTRL2.CLKPOL, USART_CTRL2.CLKPHA, USART_CTRL2.LBCLK)**
cannot be overwritten after enabling transmission.

#### 17.8.7. USART control register 3 (USART_CTRL3)

Address offset : 0x14

Reset value : 0x0000 0000

Bit Field   Name       Description

31:11       Reserved   Reserved, the reset value must be maintained

10          CTSIEN     CTS interrupt enable.

If this bit is set to 1, an interrupt will be generated when USART_STS.CTSF bit is set.

0:CTS interrupt is disabled.

1:CTS interrupt is enabled.

9           CTSEN      CTS enable.

This bit is used to enable the CTS hardware flow control function.

0:CTS hardware flow control is disabled.

1:CTS hardware flow control is enabled.

8           RTSEN      RTS enable.

This bit is used to enable RTS hardware flow control function.

0:RTS hardware flow control is disabled.

1:RTS hardware flow control is enabled.

7           DMATXEN    DMA transmitter enable.

0:DMA transmission mode is disabled.

1:DMA transmission mode is enabled.

6           DMARXEN    DMA receiver enable.

0:DMA receive mode is disabled.

1:DMA receive mode is enabled.

5           SCMEN      Smartcard mode enable.

This bit is used to enable Smartcard mode.

0: Smartcard mode is disabled.

1: Smartcard mode is enabled.

4           SCNACK     Smartcard NACK enable.

This bit is used for Smartcard mode to enable transmitting NACK when parity error
occurs.

0: Do not send NACK when there is a parity error.

1: send NACK when there is a parity error.

Bit Field              Name       Description

3                      HDMEN      Half-duplex mode enable.

This bit is used to enable half-duplex mode.

0: Half-duplex mode is disabled.

1: Half-duplex mode is enabled.

2                      IRDALP     IrDA low-power mode.

This bit is used to select the low power consumption mode for IrDA mode.

0: Normal mode.

1: Low power mode.

1                      IRDAMEN    IrDA mode enable.

0:IrDA is disabled.

1:IrDA is enabled.

0                      ERRIEN     Error interrupt enable.

When DMA receive mode (USART_CTRL3.DMARXEN=1) is enabled, an interrupt
will be generated when USART_STS.FEF, USART_STS. OREF or USART_STS. NEF
bit is set.

0: Error interrupt is disabled.

1: Error interrupt enabled.

#### 17.8.8. USART Guard Time and Prescaler Register (USART_GTP)

Address offset : 0x18

Reset value : 0x0000 0000

Bit Field              Name       Description

31:16                  Reserved   Reserved, the reset value must be maintained

15:8                   GTV[7:0]   Guard time value in Smartcard mode.

This bit field specifies the guard time in baud clock. In Smartcard mode, this function
is required. The setting time of USART_STS.TXC flag is delayed by GTV[7:0] baud

Bit Field   Name        Description

clock cycles.

7:0         PSCV[7:0]   Prescaler value.

In IrDA low power consumption mode:

these bits are used to set the frequency division coefficient for dividing the peripheral
clock (PCLK1/PCLK2) to generate low power consumption frequency.

00000000: reserved-do not write this value.

00000001: divide the source clock by 1.

…

11111111: divide the source clock by 255.

In IrDA normal mode:

PSCV can only be set to 00000001.

In Smartcard mode:

PSCV[4:0] is used to set the frequency division of Smartcard clock generated by
peripheral clock (PCLK1/ PCLK2).

Coefficient. The actual frequency division coefficient of is twice the set value of
PSCV[4:0].

0000: reserved-do not write this value.

0001: Divide the source clock by 2.

0010: Divide the source clock by 4.

…

1111: Divide the source clock by 62.

In Smartcard mode, PSCV[7:5] is reserved.

## 18. Low Power Universal Asynchronous Receiver Transmitter (LPUART)

### 18.1. Introduction

Low power universal asynchronous receiver transmitter (LPUART) is a low power, full duplex, asynchronous
serial communication interface. The LPUART can be clock provided by HSI, HSE, LSI, LSE, SYSCLK and
PCLK1. When 32.768khz LSE is selected as the clock source, the LPUART can work in STOP low-power
mode with a maximum communications up to 9600bps. LPUART supports receiving data wake-up. By
configuring wake-up events, the CPU in STOP2 mode can be woken up.

At the same time, when MCU works in RUN mode, LPUART can also be used as a common asynchronous
serial port. Users can switch the clock source to HSI, HSE, SYSCLK and PCLK1 to obtain higher communication
speed.

### 18.2. Main Features

•    Full duplex asynchronous communication

•    Selectable clock source of HSI, HSE, LSI, LSE, SYSCLK, or PCLK1

•    Fractional baud rate generator system: Programmable baud rate shared by sending and receiving up to
1Mbits/s, baud rates from 300bps to 9600bps when using 32.768 kHz clock source (LSE)

•    Fixed 8-bit data word length, 1 stop bit and optional 1 parity bit

•    Support DMA data transfer

•    Support hardware flow control

•    Transfer detection flag: Receive buffer full, Receive buffer half full, Receive buffer not empty, Receive buffer
overrun, Transmission complete

•    Parity control: Odd and even parity selection, Parity can be disable

•    Error detection flag: Parity error, Overrun error, Noise error

•    32 byte receive buffer

•    Baud rate error correction at low frequencies

•    Configurable sampling method of 1 or 3 samples

•    Noise detection

•    Configurable flow control RTS threshold

•    Support STOP mode Configurable source mode

−    Start bit detection

−    Receive buffer non-empty detection

−    A configurable receive byte

−    A programmable 4-byte frame

### 18.3. Functional Block Diagram

*Figure 18-1 LPUART Block Diagram*


CPU/DMA

Transmission data            Receive data register(RDR)
register(TDR)                    Receive buffer

TX              Transmission shift register          Receive shift register

RX

RTS           Hardware                                      CTRL register
data flow
CTS           control
Wake up
Tx control                                Rx control
controller

STS register

Interrupt control
INTEN register

Baud rate generator

Transmitter
Receiver baud
baud rate
rate control
control

BRCFG1 register

BRCFG2 register

### 18.4. Function Description

As shown in Figure 18-1, LPUART bidirectional communication requires at least two pins: receiving data input
(RX) and sending data output (TX).

RX: Serial data input. When the number of samples is 3, data and noise can be distinguished.

TX: Serial data output. When sending is enabled, the pin defaults to be high level.

LPUART has the following characteristics:

•      Idle status without sending or receiving

•      A start bit

•      A data word (8 bits) with the least significant bits first

•      A stop bit, indicating the end of a data frame

•      A status register (LPUART_STS)

•      Data register (LPUART_DAT)

•      Two baud rate configuration registers (LPUART_BRCFG1 and LPUART_BRCFG2) using fractional
baud rate generators:16-bit integer and 8-bit decimal representations

•      The following pins are required in hardware flow control mode:

•      CTS (Clear To Send): When transmitter detects that CTS is valid (low level), the next data is sent

•      RTS (Request To Send): When receiver is ready to receive new data, pull the RTS pin low.

#### 18.4.1. LPUART frame format

The LPUART data word length is fixed at 8 bits (see Figure 18-2). During the start bit, TX pin is at a low level and
during the stop bit it is at a high level. The parity bit follows the data word when enabled.

Both     sending     and     receiving       are   driven      by   two      different       baud       clock      generators.        When        the
LPUART_CTRL.TXEN of transmitter is set, the corresponding baud clock generator generates baud clock. When
the start bit is received, the receiver's corresponding baud clock generator generates the clock.

*Figure 18-2 Frame Format*


8-bit word length, 1 stop bit
Data frame                                             Possible                      Next data frame
parity bit
Start                                                                                                          start
Bit 0   Bit 1     Bit 2   Bit 3    Bit 4     Bit 5   Bit 6      Bit 7                Stop               bit
bit
bit

> **Note:If there is no parity bit, the stop bit is after bit 7**

> **Note: in this chapter, unless special instruction, setting means that a register is set to state '1', and resetting or**
clearing means that a register is set to state '0'. Hardware or programs may set or clear a register. Please refer
to this chapter for details.

#### 18.4.2. Transmitter

When the Transmit Enable bit (LPUART_CTRL.TXEN) is set and there is data in the buffer, the transmitter
sends 8-bit data words. The data in the shift register is output on the TX pin.

##### 18.4.2.1. Transmit process

During an LPUART transmission, the least significant bit of the data is shifted out on TX pin. In this mode, the

LPUART_DAT register contains a buffer between the internal bus and the transmitter shift register (see Figure 18-1).

Each character is preceded by a low level starting bit; and is terminated by a stop bit.

> **Note: You cannot reset the LPUART_CTRL.TXEN bit during data transfer, otherwise the data on the TX pin**
will be corrupted because the baud rate counter stops counting. The current data being transferred will be lost.

The LPUART sends data as follows:

•    Configure baud rate, parity check, DMA, flow control, etc.

•    Set the LPUART_CTRL.TXEN bit to enable data transmission.

•    Write data to the LPUART_DAT register.

•    Check if the LPUART_STS.TXC flag is set, it means the transmission is over. If the flag is set, write 1
to the LPUART_STS.TXC bit to clear the flag.

•    Check the LPUART_STS.PEF bit to confirm whether the parity is wrong.

•    Otherwise, go to Step 3 and send the next data.

> **Note: Be sure to initialize the LPUART module before using the transmitter.**

LPUART initialization as follows:

•    Set all flag bits in the LPUART_STS register to clear the interrupt flag.

•    To enable the interrupt function, configure LPUART_INTEN.

•    Set LPUART_CTRL.FLUSH clear the

RX buffer. When send data:

•    After configuring the baud rate and setting LPUART_CTRL.TXEN, the CPU can write directly to the
LPUART_DAT register to send data.

•    When a frame transmission is completed (after the stop bit is sent), the LPUART_STS.TXC bit is set.
If the LPUART_INTEN.TXCIEN bit is set, an interrupt occurs immediately.

•    After the last data byte is written to the LPUART_DAT register, you must wait for LPUART_STS.TXC=1
before shutting down the LPUART module or setting the microcontroller into low-power mode.

##### 18.4.2.2. Single byte communication

After configuring the baud rate and setting TXEN, the CPU can directly write the LPUART_DAT register to
send data. When a frame transmission is completed (after the stop bit is sent), the TXC bit is set, and if the
TXCIEN bit in the LPUART_INTEN register is set, an interrupt will be generated immediately. After
writing the last data word in the LPUART_DAT register, you must wait for TXC=1 before turning off the
LPUART module or setting the microcontroller into a low-power mode. The TXC bit can be cleared by writing
the TXC bit 1 of the LPUART_STS register by software.

*Figure 18-3 TXC Changes During Transmission*


Software
Write Data0 directly                              Software clears TXC and                       Software clears TXC and
enable TXEN             in LPUART_DAT                                         writes Data1 in                               writes Data2 in
without waiting                                     LPUART_DAT                                    LPUART_DAT

LPUART_DAT                             Data0                                              Data1                                    Data2

TXC flag                                       Set by hardware                          Set by hardware                              Set by hardware

Send stop bit                            Send stop bit                              Send stop bit
and set TXC                              and set TXC                                and set TXC

Data 0 frame           Stop bit            Data 1 frame         Stop bit                 Data 2 frame
TX line

18.4.3 Receiver

##### 18.4.2.3. Start bit detection

If the LPUART_CTRL.SMPCNT bit is 0, that is, the number of samples is 3, when there are at least two 0s in
the three sample numbers, the start bit is valid. Otherwise it will be invalid.

Sampling values                  NF state                Received bit value               Start bit validity

000                            0                            0                              effective

001                            1                            0                              effective

010                            1                            0                              effective

011                            1                            1                              invalid

100                            1                            0                              effective

101                            1                            1                              invalid

110                            1                            1                              invalid

111                            0                            1                              invalid

##### 18.4.2.4. Receive process

During LPUART reception, the least significant bits of data are first moved in from the RX pin. In this
mode, the LPUART_DAT register contains a buffer between the internal APB bus and the receive shift register.

The steps for LPUART to receive data are as follows:

•    Configure baud rate, parity check, wake up event/enable, sampling mode, DMA, flow control, etc.

•    Check the interrupt flags of the LPUART_STS register: buffer is not empty, buffer is half full, buffer is full,
buffer overrun;

•    Read the data by reading the LPUART_DAT register.

•    Return to Step 2 and continue receiving data.

> **Note: Please be sure to initialize the LPUART module before using the receiver.**

When receiving a data frame:

•    The LPUART_STS.FIFO_NE bit is set, and the contents of the shift Register are transferred to the RDR
(Receiver Data Register). In other words, the data has been received and can be read (including its
associated error flags).

•    If the LPUART_INTEN.FIFO_NEIEN bit is set, an interrupt is generated.

•    Frame errors (parity detection errors), noise or overrun errors are detected during reception, so the error flag
will be set.

•    In multi-buffer communication mode, the LPUART_STS.FIFO_NE flag bit is placed after each byte
received and cleared by DMA's read operation on the data register.

•    In single buffer mode, the software can clear LPUART_STS.FIFO_NE bits by reading the LPUART_DAT
register or by writing 0.The LPUART_STS.FIFO_NE bit must be cleared before the end of the next frame
of data reception to avoid overrun errors.

##### 18.4.2.5. Overrun error

The LPUART receiving data buffer has a total of 32 bytes. The LPUART_STS.FIFO_FU flag will be set after
receiving 32 bytes of data. When the buffer data is not read out and causes LPUART_STS.FIFO_FU to be not
reset in time, if next character is received, an overrun error occurs. This character will be discarded by the
hardware. Data can only be transferred from the shift register to the receiving data buffer if the
LPUART_STS.FIFO_FU bit is cleared. If the next data has been received or the previous DMA request has not
been served, the LPUART_STS.FIFO_FU flag is still set and an overrun error occurs.

When an overrun error occurs:

•    The LPUART_STS.FIFO_OV bit is set.

•    The receiving data buffer content will not be lost. Reading the LPUART_DAT register still returns the previous data.

•    The contents of the shift register will be overwritten. Any subsequent data received will be lost.

•    If the LPUART_INTEN.FIFO_OVIE bit is set, an interrupt is generated.

•    LPUART_DAT register read operation, reset LPUART_STS.FIFO_OV.

##### 18.4.2.6. Noise error

Noise errors use an over-sampling technique (if the LPUART_CTRL.SMPCNT bit is 0, that is, the number of samples is
3) to recover data by distinguishing valid input data from noise.

*Figure 18-4 Data Sampling for Noise Detection*


Receiving
signal line

Sampling
clock                              1                     2                     3

The length of a bit

*Table 18-1 Data Sampling for Noise Detection*


Received bit
Sampling values             NF state
value

000                      0                       0

001                      1                       0

010                      1                       0

011                      1                       1

100                      1                       0

101                      1                       1

110                      1                       1

111                      0                       1

When noise is detected in a receiving frame, you can do the following:

•    If three sample values are inconsistent, set the LPUART_STS.NF flag immediately.

•    The received data is transferred from the shift register to the buffer.

•    Software write 1 clears the LPUART_STS.NF flag bit.

#### 18.4.3. Fractional baud rate generation

Baud rate frequency division coefficient is divided into 16-bit integer part and 8-bit decimal part. The baud rate
generator uses the value of the combination of these two parts to determine the baud rate. The fractional baud rate
divider will enable the LPUART to generate all standard baud rates.

Baud rate frequency division coefficient (LPUARTDIV) has the following relationship with system clock (PCLK) :

TX/RX baud rate = 𝑓CLK /(LPUARTDIV)

Here the 𝑓CLK is the clock for LPUART (the clock source of LPUART can be HSI, HSE, LSI, LSE, SYSCLK, or PCLK1).
The value of LPUARTDIV is set in the baud rate configuration registers LPUART_BRCFG1 and LPUART_BRCFG2

> **Note: After writing LPART_BRCFG1 and LPUART_BRCFG2, the baud rate counter is replaced with the new**
value of the baud rate register. Therefore, do not change the value of the baud rate register during communication.

Configure baud rates through LPUART_BRCFG1 and LPUART_BRRCFG2

For example, baud rate = 4800bps, clock frequency = 32768Hz.

LPUARTDIV = 32768/4800 = 6.82667.LPUART_BRCFG1 = 6 and the value of LPUART_BRCFG2 is calculated
by adding fractions in the table below (the value of LPUART_BRCFG2 is 0xEFh).

Decimal Addition                   Carry To The Next Integer            Bit Field       Value

0.82667 + 0.82667 = 1.65333                          YES                    DECIMAL0           1

1.65333 + 0.82667 = 2.48000                          YES                    DECIMAL1           1

2.48000 + 0.82667 = 3.30667                          YES                    DECIMAL2           1

3.30667 + 0.82667 = 4.13333                          YES                    DECIMAL3           1

4.13333 + 0.82667 = 4.96000                          NO                     DECIMAL4           0

4.96000 + 0.82667 = 5.78667                          YES                    DECIMAL5           1

5.78667 + 0.82667 = 6.61333                          YES                    DECIMAL6           1

6.61333 + 0.82667 = 7.44000                          YES                    DECIMAL7           1

When LSE clock (32.768KHz) is used, the values of baud rate configuration registers LPUART_BRCFG1 and
LPUART_BRCFG2 with different baud rate Settings are as follows:

Baud Rate      Divisor     LPUART_BRCFG1          LPUART_BRCFG2

300        109.2267            6Dh                    88h

600        54.6133             36h                    ADh

1200         27.3067                 1Bh                              24h

2400         13.6533                0Dh                               6Dh

4800         6.8267                  06h                              EFh

9600         3.4133                  03h                              4Ah

> **Note: The lower the clock frequency of the CPU, the lower the accuracy of a particular baud rate.**

#### 18.4.4. Parity control

Reset the LPUART_CTRL.PCDIS bit, enable parity control (generate a parity bit when sending, parity check
when receiving), set or reset the LPUART_CTRL.PSEL bit selection to use odd or even check. LPUART frame
formats are listed in the table below.

*Table 18-2 Parity Frame Format*


PCDIS Bit                                   LPUART Frame

0                        | Start bit | 8-bit data | parity bit | stop bit |

1                              | Start bit | 8 bits data | stop bit |

Transfer mode: Parity is enabled by resetting the LPUART_CTRL.PCDIS bit. If parity fails, the
LPUART_STS.PEF flag is set to '1', and an interrupt occurs if LPUART_INTEN.PEIE is set.

Odd parity: LPUART_CTRL.PSEL＝1.

Make the number of '1' in one frame data (including parity bit) be an odd number. That is: if Data=11000101,
there are 4 '1's, then the parity bit will be '1' (5 '1' in total).
Even parity: LPUART_CTRL.PSEL＝0.

Make the number of '1' in one frame data (including parity bit) be an even number. That is: if Data=11000101,
there are 4 '1's, then the parity bit will be '0' (4 '1' in total).

#### 18.4.5. DMA application

LPUART can access the transmit data register (TDR) and receive buffer respectively through DMA.

##### 18.4.5.1. DMA transmission

The steps for assigning a DMA channel to the LPUART transmissions are as follows (x indicates the channel number) :

•     Configure the LPUART_DAT register address as the destination address for DMA transfer, and the memory
address as the source address for DMA transfer.

•     Set the total number of bytes to be transmitted.

•     Set the channel priority.

•    Configure to generate DMA interrupts when the transfer is half or all complete.

•    Activate the channel.
Completing a DMA transfer will generate an interrupt on the corresponding DMA channel. In transmission
mode, when the DMA has finished the data transfer, the DMA controller sets the DMA_INTSTS.TXCFx flag. The
LPUART_STS.TXC flag bit is asserted by the hardware to indicate that the transfer is completed. The
software needs wait for LPUART_STS.TXC=1.

*Figure 18-5 Sending Using DMA*


Data frame 0         Stop bit         Data frame 1      Stop bit         Data frame 2     Stop bit
TX line

Software enable                                                                                                  Transfer is over, DMA
to send data                                                                                                controller ignores this request
DMA writes Data0                           DMA writes Data1                    DMA writes Data2
in LPUART_DAT                              in LPUART_DAT                       in LPUART_DAT
DMA request

LPUART_DAT                              Data0                                          Data1                         Data2

TXC flag                                   Set by hardware

Send stop bit                TXC has been set, if the
and set TXC                  software does not clear

##### 18.4.5.2. DMA Reception

The steps for assigning a DMA channel to the LPUART receiving are as follows (x indicates the channel number) :

•    Configure the LPUART_DAT register address as the source address for transmission and the memory address
as the destination address for transmission through the DMA configuration register.

•    Configure the number of DMA bytes to be transferred.

•    Configure the channel priority on the DMA register for data transfer.

•    Configure interrupts to generate DMA interrupts when the transfer is half or all complete.

•    Activate the channel.

When completing the transfer specified by the DMA controller, the DMA controller generates an interrupt on
the DMA channel's interrupt vector.

*Figure 18-6 Receiving With DMA*


Data frame 0     Stop bit            Data frame 1      Stop bit              Data frame 2   Stop bit
RX line

Cleared by DMA
FIFO_NE                     Set by hardware         detection operation

DMA reads Data0                           DMA reads Data1                   DMA reads Data2
in LPUART_DAT                             in LPUART_DAT                     in LPUART_DAT
DMA request

LPUART_DAT                                                     Data0                                  Data1                        Data2

#### 18.4.6. Hardware Flow Control

Hardware flow control functions through CTS input and RTS output. The following figure shows how two
devices are connected in this mode.

*Figure 18-7 Hardware Flow Control Between Two LPUART*


DEVICE 1                                                                 DEVICE 2
TX                           RX

TX control                                                               RX control
CTS                         RTS

RX                           TX

RX control                                                               TX control
RTS                          CTS

RTS   and    CTS    flow   control    can     be     independently              enabled        by    setting     LPUART_CTRL.RTSEN                      and
LPUART_CTRL.CTSEN.

##### 18.4.6.1. RTS flow control

If RTS flow control is enabled (LPUART_CTRL.RTSEN=1), the RTS will be driven high (active) when the RTS threshold
condition is achieved, otherwise it will be driven low.How is the RTS valid can be selected by the
LPUART_CTRL.RTS_THSEL[1:0] bits. The RTS threshold can be selected to be effective when the FIFO is half full, 3/4

full, or full. Below is an example of communication with RTS flow control enabled.

*Figure 18-8 RTS Flow Control*


Waiting to read data                   Read data register
register,RTS high                      end,RTS low
RTS line

Start                                 Stop           Start
RX line                             Data 1                      Idle                      Data 2             Stop
Bit                                   Bit            Bit
Bit

##### 18.4.6.2. CTS flow control

If CTS flow control is enabled (LPUART_CTRL.CTSEN=1), the sender will check the CTS pin to decide whether
or not send data before sending the next frame. If the CTS is pulled low (valid), the sender sends data (assuming
that data is ready to be sent). If the CTS is pulled up during transmission, the transmission of the current
data frame is stopped after transmission.

If CTS flow control is enabled (LPUART_CTRL.CTSEN=1), the signal of CTS pin will be changed. See Figure
18-9 for enabling CTS flow control.

*Figure 18-9 CTS Flow Control*


CTSF = 1                        CTSF = 1

CTS line

Writing Data 3 in
Data register

Data register       Data 2              empty                        Data 3                          empty

CTS = 1,                            CTS = 0,
Transmit delay                      Transmit Data 3
Start                                                Start
TX line        Data 1     Stop    Bit            Data 2              Stop     Idle                Data 3
Bit              Bit
Bit

#### 18.4.7. Low power wake up

LPUART can work in STOP mode, if the LPUART_CTRL.WUSTP is set, it can wake up the system on EXTI line

22 when a specific waking up event occurs.
The LPUART waking up event can be handled in the following ways (through the LPUART_CTRL.WUSEL[1:0]) :

•       A waking up event is generated when a start bit is detected

•       A waking up event is generated when the receive buffer non-empty flag is set

•       A waking up event is generated when data is received and the first byte matches LPUART_WUDAT[7:0]

•       A waking up event is generated when data is received and four bytes match

LPUART_WUDAT[31:0] When waking up event occurs, the LPUART_STS.WUF bit will be set.

### 18.5. Interrupt Request

*Table 18-3 LPUART Interrupt Requests*


Interrupt Function                                       Interrupt Event                                   Event Flag                                 Enable Bit

Parity check error                                     PEF                                             PEIE

TX complete                                       TXC                                             TXCIE

Receive buffer overrun                                FIFO_OV                                  FIFO_OVIE

LPUART global interrupt                                       Receive buffer full                               FIFO_FU                                   FIFO_FUIE

Receive buffer half full                              FIFO_HF                                   FIFO_HFIE

Receive buffer not empty                               FIFO_NE                                  FIFO_NEIE

Wake up in STOP mode                                        WUF                                         WUFIE

LPUART interrupt events are logical OR. If the corresponding enable control bit is set, these events can generate
their own interrupt, but only one interrupt request can be generated at the same time.

### 18.6. LPUART Registers


#### 18.6.1. LPUART register overview

*Table 18-4 LPUART Register Overview*


Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0

set

LPUART_S
FIFO_OV
FIFO_NE

FIFO_FU
FIFO_HF
WUF
CTS

TXC
PEF
NF

00       TS
Reserved
0h      Reset
0   0     0        0           0           0           0          0       0
Value
FIFO_OVIE
FIFO_NEIE
FIFO_HFIE
FIFO_FUIE

00 LPUART_I
WUFIE

TXCIE

Reserved
PEIE

4h      NTEN

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

Reset
0          0          0         0       0       0      0
Value

WUSEL

DMA_RXEN

LOOPBACK
DMA_TXEN
LPUART_

RTS_THS
SMPCNT

WUSTP
EL [1:0]
RTSEN
CTSEN

FLUSH
PCDIS

TXEN
PSEL
00      CTRL                                                                                                       [1:0]
Reserved
8h
Reset
0        0    0    0       0        1        0          0        0          0          0         0       0       0      0
Value

LPUART_B
INTEGER[15:0]
00 RCFG1
Reserved
Ch      Reset
0    0        0    0    0       0        0        1          0        1          1          1         0       1       0      0
Value

LPUART_
DAT[7:0]
01      DAT
Reserved
0h      Reset
0        0          0          0         0       0       0      0
Value

LPUART_B
DECIMAL[7:0]
01 RCFG2
Reserved
4h      Reset
0        0          0          0         0       0       0      0
Value

LPUART_
WUDAT[31:0]
01 WUDAT
8h      Reset
0    0    0    0    0    0    0     0    0    0    0     0    0    0    0    0    0    0        0    0    0       0        0        0          0        0          0          0         0       0       0      0
Value

#### 18.6.2. LPUART Status Register (LPUART_STS)

Address offset: 0x00

Reset value: 0x0000 0000

Bit Field                           Name                       Description

31:9                                Reserved                   Reserved, the reset value must be maintained.

Bit Field   Name      Description

8           NF        Noise detected flag.

When noise is detected in the received frame, this bit is set by hardware.This bit is
cleared by the software.

0: No noise is detected.

1: Noise is detected.

7           WUF       Wakeup from STOP mode Flag.

0: No wake up event is detected.

1: A wake up event is detected.

6           CTS       CTS signal (hardware flow control) flag.

Once the sender requests to send data, it is ready to receive it.

0: CTS line is reset.

1: CTS line is set.

5           FIFO_NE   FIFO non-empty flag.

0: Buffer is empty.

1: Buffer is not empty.RX data is ready to be read

4           FIFO_HF   FIFO half full flag.

0: Buffer is not half full.

1: Buffer is half full.RX data should be read before the buffer is full

3           FIFO_FU   FIFO full flag.

0: Buffer is not full.

1: Buffers is full.RX data should be read out in preparation for receiving new data

2           FIFO_OV   FIFO overrun flag.

0: Buffer did not overrun

1: Buffer overrun.

1           TXC       TX complete flag.

0: TX is disabled or not complete.

1: TX transmission is complete.

0           PEF       Parity check error flag.

Bit Field             Name        Description

0: No parity error detected.

1: Parity error detected

#### 18.6.3. LPUART Interrupt Enable Register (LPUART_INTEN)

Address offset: 0x04

Reset value: 0x0000 0000

Bit Field             Name         Description

31:7                  Reserved     Reserved, the reset value must be maintained.

6                     WUFIE        Wake up interrupt enable

0: Disable wake up interrupt

1: Enable wake up interrupt

5                     FIFO_NEIE    Receive buffer not empty interrupt enable

0: Disable buffer non-empty interrupt

1: Enable buffer non-empty interrupt

4                     FOFO_HFIE    Receive buffer half-full interrupt enable

0: Disables buffer half-full interrupt

1: Enables buffer half-full interrupt

3                     FOFO_FUIE    Receive buffer full interrupt enable

0: Disables buffer full interrupt

1: Enable buffer full interrupt

2                     FIFO_OVIE    Receive buffer overrun interrupt enable

0: Disables buffer overrun interrupt

1: Enable buffer overrun interrupt

1                     TXCIE        TX complete interrupt enable

0: Disable TX complete interrupt

Bit Field              Name   Description

1: Enable TX complete interrupt

0                      PEIE   Parity check error interrupt enable

0: Disable parity error interrupt

1: Enable parity error interrupt

#### 18.6.4. LPUART control register (LPUART_CTRL)

Address offset: 0x08

Reset value: 0x0000 0200

Bit Field       Name              Description

31:15           Reserved          Reserved, the reset value must be maintained.

14              SMPCNT            Specify sampling method

0: 3 sample bits, noise detection is allowed (LPUARTDIV should be large
enough, such as greater than 10)

1: 1 sample bits, closed noise detection

13:12           WUSEL[1:0]        Wake up event selection.

00: Start bit detection

01: Non-empty detection of receive buffer

10: A configurable receive byte

11: A programmable 4-byte frame

11              RTSEN             RTS hardware flow control enable

0: Disables RTS hardware flow control

1: Enables RTS hardware flow control

10              CTSEN             CTS hardware flow control enable

0: Disables CTS hardware flow control

1: Enables CTS hardware flow control

Bit Field       Name             Description

9:8             RTS_THSEL[1:0]   RTS threshold selection

00: When FIFO is half full, RTS is effective (pull up)

x1: When FIFO is 3/4 full, RTS effective (pull up)

10: When FIFO is full, RTS effective (pull up)

7               WUSTP            LPUART STOP mode wakeup enabled

0: Cannot wake up STOP mode

1: Can wake up the STOP mode

6               DMA_RXEN         DMA RX request enable

5               DMA_TXEN         DMA TX request enable

4               LOOKBACK         Loopback self-test

0: Normal mode

1: Loopback self-test mode

3               PCDIS            Parity control

0: Enables parity bit

1: Disables parity bit

2               FLUSH            Clear receive buffer

0: Disables buffer clear

1: Clear buffer content

1               TXEN             TX enable

0: Disables TX

1: Enables TX

0               PSEL             Odd parity enable

0: Even parity

1: Odd parity

#### 18.6.5. LPUART Baud Rate Configuration Register 1 (LPUART_BRCFG1)

Address offset: 0x0C

Reset value: 0x0000 0174

Bit Field             Name            Description

31:16                 Reserved        Reserved, the reset value must be maintained.

15:0                  INTEGER[15:0]   Baud rate configuration register 1.

The calculation of baud rate configuration register 1 is as follows:

If the baud rate is 9600bps and the clock frequency is 32768Hz.

LPUARTDIV = 32768/9600 = 3.4133

In this case, the integer part of the LPUARTDIV is 3 and the decimal part is
0.4133.LPUART_BRCFG1 = 3.LPUART_BRCFG2 will be used for baud rate
error correction.For the 3-bit sampling method with noise detection characteristics,
LPUARTDIV is not large enough at this time, so 1-bit sampling method should be
adopted to avoid sampling error.

#### 18.6.6. LPUART data register (LPUART_DAT)

Address offset: 0x10

Reset value: 0x0000 0000

Bit Field             Name            Description

31:8                  Reserved        Reserved, the reset value must be maintained.

7:0                   DAT[7:0]        Write to the data register when sending

Read the data register when receiving

#### 18.6.7. LPUART Baud Rate Configuration Register 2 (LPUART_BRCFG2)

Address offset: 0x14

Reset value: 0x0000 0000

Bit Field             Name           Description

31:8                  Reserved       Reserved, the reset value must be maintained.

7:0                   DECIMAL[7:0]   Baud rate configuration register 2 is used for baud rate error correction at low
frequencies.For example,

If the baud rate is 4800bps and the clock frequency is 32768Hz.

LPUARTDIV = 32768/4800 = 6.8266

LPUART_BRCFG1 = 6.In this case, to correct the baud rate error, you should
configure register 2 with baud rate. For details on how to configure register 2, refer
to the section "18.4.4 Fractional baud rate generation".

#### 18.6.8. LPUART Wake-up Data Register (LPUART_WUDAT)

Address offset: 0x18

Reset value: 0x0000 0000

Bit Field             Name           Description

31:0                  WUDAT[31:0]    When LPUART_CTRL.WUSEL[1:0] = 1x, WUDAT[31:0] is used to check
whether the conditions for wake up from STOP mode is matched (byte match or
frame match):

LPUART_CTRL.WUSEL[1:0] = 10 is used to wake up byte matching. In this case,
the first byte is valid

LPUART_CTRL.WUSEL[1:0] = 11 is used to wake up frame matching. In this
case, all 4 bytes are valid

## 19. Serial Peripheral Interface/Inter-IC Sound (SPI/ I2S)


### 19.1. Introduction

This module is about SPI/I2S. It works in SPI mode by default and users can choose to use I2S by setting the
value of registers.

Serial peripheral interface (SPI) is able to work in master or slave mode, support full-duplex and simplex
high-speed communication mode, and have hardware CRC calculation and configurable multi-master mode.

On-chip audio interface (I2S) is able to work in master and slave modes in simplex communication, and
supports four audio standards: Philips I2S standard, MSB alignment standard, LSB alignment standard and PCM
standard.

### 19.2. SPI and I2S Main Features


#### 19.2.1. SPI Features

•   Full duplex mode and simplex synchronous mode.

•   Support master mode, slave mode and multi-master mode.

•   Supports 8-bit or 16-bit data frame format.

•   Data bit sequence programmable.

•   NSS management by hardware or software.

•   Clock polarity and phase programmable.

•   Sending and receiving support hardware CRC calculation and check.

•   Supports DMA.

#### 19.2.2. I2S Features

•   Simplex synchronous mode.

•   Supports master mode and slave mode operation.

•   Four audio standards are supported: Philips I2S standard, MSB alignment standard, LSB alignment standard
and PCM standard.

•   The audio sampling frequency from 8kHz to 96kHz can be configured.

•   Supports 16-bit, 24-bit or 32-bit data length and data frame format (configured according to requirements).

•   Steady state clock polarity programmable.

•   The data direction is always MSB first.

•   Supports DMA

### 19.3. SPI Function Description


#### 19.3.1. General description

*Figure 19-1 SPI Block Diagram*


address and data bus

Read

Receive buffer

LSBFF                                    MOSI
SPI_CTRL2
control bit
TE       RNE            ERR
-          -      SSOEN     TDMAEN      RDMAEN
Shift register
INTEN     INTEN          INTEN                                                                                                            MISO

SPI_STS
Send buffer
CRC
BUSY      OVER          MODERR              UNDER      CHSIDE         TE       RNE
ERR
Write

0
Communication circuit
1

CLK      CLK
LSBFF   SPIEN              BR[2:0]              MSEL
POL      PHA

Baud rate generator                  SCLK
SPI_CTRL1
BIDIRM     BIDIRO            CRC
CRCEN             DATFF     RONLY      SSMEN       SSEL
ODE         EN              NEXT

Main controller

NSS

A total of 4 pins of SPI are connected to external devices::

•    SCLK: serial clock pin. Serial clock signal is output from the SCLK pin of master device and input to SCLK
pin of slave device.

•    MISO: master input/slave output pin. Data is received from the MISO pin of master device and send by the
MISO pin of slave device.

•    MOSI: master output/slave input pin. Data is send by the MOSI pin of master device and received from the
MOSI pin of slave device.

•    NSS: chip select pin. There are two types of NSS pin, internal pin and external pin. If the internal pin detects
a high level, SPI works in the master mode. Conversely, SPI works in the slave mode. Users can use a
standard I/O pin of the master device to control the NSS pin of the slave device.

Software NSS mode:

SSMEN=1 in the SPI_CTRL1 register enables software slave management (see Figure 19 2).

In this mode, the NSS pin is not used, and the internal NSS signal level is driven by writing the SSEL bit of
SPI_CTRL1 (master mode SSEL=1, slave mode SSEL=0).

Hardware NSS mode:

SSMEN=0 in SPI_CTRL1 register disables software slave management.

Input mode: configured as master mode, NSS output disabled (MSEL=1, SSOEN=0), allowing operation in multi-
master mode. The master device (slave device) should connect the NSS pin to a high level (low level) during the
entire data frame transmission.

Output mode: configured as master mode, NSS output enable (MSEL=1, SSOEN=1), SPI as the master device
must pull NSS low, all SPI devices that are set to NSS hardware mode and connected to it will detect Low level,
automatically enter the slave state. If the master device cannot pull down NSS, it will enter the slave mode and
generate a master mode failure error MODERR bit '1'.

> **Note: The choice of software mode or hardware mode depends on whether NSS control is required in the**
communication protocol. If not needed, you can select the software mode to release a GPIO pin for other uses.

*Figure 19-2 Selective Management of Hardware/Software*


SSMEN

Internal NSS     0         External NSS

1         SSEL

The following figure is an example of the interconnection of single master and single slave devices.

*Figure 19-3 Master and Save Applications*


Master                                               Slave

MSBit                   LSBit                     MSBit                     LSBit

MISO
8-bit shift register                                8-bit shift register

MOSI

SPI clock                           SCK
generator
NSS(1)          NSS(1)
VDD

Not used if NSS is managed
by software

> **Note: NSS pin is set as input**

SPI is a ring bus structure. The master device outputs a synchronous clock signal through the SCLK pin, and the
MOSI and MISO pins are connected accordingly. Data is transmitted serially between the master and the slave,
and the data is sent to the slave device through the MOSI pin with the lowest bit. At the same time, the highest
bit of the slave device is transmitted to the lowest bit of the master device through the MISO pin. When the
second bit of data is sent, The data in the lowest bit will be shifted one bit to the left and the new data will be
stored in the lowest bit.

SPI timing mode:

The combination of CLKPOL clock polarity and CLKPHA clock phase selects the clock edge for data capture.
Configuring the CLKPOL and CLKPHA bits of the SPI_CTRL1 register has the following four timing
relationships.

•   CLKPOL=0, CLKPHA=0: The SCLK pin remains low in the idle state, and the data is sampled on the first
edge, that is, the rising edge;

•   CLKPOL=0, CLKPHA=1: The SCLK pin remains low in the idle state, and the data is sampled on the
second edge, that is, the falling edge;

•   CLKPOL=1, CLKPHA=0: The SCLK pin remains high in the idle state, and the data is sampled on the first
edge, that is, the falling edge;

•   CLKPOL=1, CLKPHA=1: The SCLK pin remains at a high level in the idle state, and the data is sampled
on the second edge, that is, the rising edge.

*Figure 19-4 is when the SPI_CTRL1 register LSBFF=0, the four CLKPHA and CLKPOL bit combination timings*

for SPI transmission..

*Figure 19-4 Data Clock Timing Diagram*


CLKPHA=1

CLKPOL=1

CLKPOL=0

MISO                       MSB                                                                   LSB

SPI_CTRL1 decided data is 8bits or 16bits

MOSI                       MSB                                                                    LSB

NSS

Sampling time

CLKPHA=0

CLKPOL=1

CLKPOL=0

MISO                       MSB                                                                   LSB

SPI_CTRL1 decided data is 8bits or 16bits

MISO                       MSB                                                                    LSB

NSS

Sampling time

Data format:

User can selects the data order by setting the SPI_CTRL1.LSBFF bit. When SPI_CTRL1.LSBFF = 0, SPI will
send the high-order data (MSB) first; When SPI_CTRL1.LSBFF = 1, SPI will send low-order data (LSB) first.

User can selects the data frame by setting the SPI_CTRL1.DATFF bit.

#### 19.3.2. SPI work mode


Master full duplex mode(SEL=1，BIDIRMODE=0，RONLY=0):

The transfer process starts after data is written to the register SPI_DAT (transmit buffer). While sending the first bit
of data, the data is transferred from the send buffer to the 8-bit shift register in parallel, and the SPI shifts the
data serially to the MOSI pin in sequence according to the configuration of LSBFF; at the same time, in The data
received on the MISO pin is serially shifted into the 8-bit shift register in the same order, and then transferred to the
SPI_DAT register (receive buffer) in parallel. The software operation process is as follows (see Figure 19-5, the
schematic diagram of TE/RNE/BUSY changes during continuous transmission in 5 host full-duplex mode):

•    Set SPI_CTRL1.SPIEN = 1, Enable SPI module.

•    Write the first data to be sent to the SPI_DAT register (this operation will clear SPI_STS.TE bit).

•    Wait for SPI_STS.TE bit to be set to '1', and write the second data to be sent into SPI_DAT. Wait for
SPI_STS.RNE bit to be set to '1', read SPI_DAT to get the first received data, and the SPI_STS.RNE bit will
be cleared by hardware while reading SPI_DAT. Repeat the above operation, sending subsequent data and
receiving n-1 data at the same time;

•    Wait for SPI_STS.RNE bit to be set to '1' to receive the last data;

•    Wait for SPI_STS.TE to be set to '1', then wait for SPI_STS.BUSY bit to be cleared and turn off SPI module.

The process of data sending and receiving can also be realized in the interrupt handler generated by the rising edge
of RNE or TE flag.

*Figure 19-5                      TE/RNE/BUSY Changing During Continuous Transmission in Full-duplex Mode*


Master mode：CLKPOL=1.CLKPHA=1

SCK

DATA1=0x11                                                DATA2=0x22                                        DATA3=0x33
MISO/MOSI
b0     b1   b2    b3     b4     b5       b6    b7      b0      b1   b2       b3     b4     b5    b6     b7    b0   b1   b2    b3   b4     b5   b6   b7
(out)

TE flag                                                     Set by hardware                                 Set by software

Tx buffer               0x11                           0x22                                                                     0x33
（write SPI_DAT）

BUSY flag                                                                                      Set\clear by hardware

DATA1=0xAA                                                DATA2=0xBB                                        DATA3=0xCC
MISO/MOSI
b0     b1   b2    b3     b4     b5       b6    b7      b0      b1   b2       b3     b4     b5    b6     b7    b0   b1   b2    b3   b4     b5   b6   b7
（in）

RNE flag                                       Set by hardware                                   Set by software

Rx buffer
0xAA                                               0xBB                     0xCC
（read SPI_DAT）

Write 0x11 into SPI_DAT                                                            Wait TE=1，write 0x33 into SPI_DAT

Wait TE=1，write 0x22 into SPI_DAT                                          Wait RNE=1，read 0xBB from SPI_DAT

Wait RNE=1，read 0xAA from SPI_DAT                                          Wait RNE=1，read 0xCC from SPI_DAT

Master one-way send-only mode (MSEL=1，BIDIRMODE=0，RONLY=0):

The transmission principle of the one-way send-only mode is the same as that of the full-duplex mode. The
difference is that this mode will not read the received data, so the OVER bit in the SPI_STS register will be set to
'1', and the software does not need to care about it. The software operation process is as follows (see Figure 19-
6, the schematic diagram of TE/BUSY changes when the host transmits continuously in one-way only mode):

•    Enable SPI module, set SPIEN = 1.

•    Write the first data to be sent into SPI_DAT register (this operation will clear TE bit).

•    Wait for TE bit to be set to '1', and write the second data to be sent into SPI_DAT. Repeat this operation
to send subsequent data;

•    After writing the last data to SPI_DAT, wait for SPI_STS.TE bit to set '1'; then wait for SPI_STS.BUSY
bit to be cleared to complete the transmission of all data.

The process of data sending can also be implemented in the interrupt handler generated by the rising edge of the TE flag.

*Figure 19-6 TE/BUSY Changing During Transmits Continuously in Half-duplex Mode*


Master mode：CLKPOL=1.CLKPHA=1

SCK

DATA1=0x11                                                           DATA2=0x22                                                            DATA3=0x33
MISO/MOSI                                                                                                                                                                                                               b7
b0     b1    b2     b3     b4       b5         b6        b7        b0        b1    b2     b3    b4       b5        b6        b7        b0        b1      b2     b3    b4        b5        b6
(out)

TE flag                                                 Set by hardware                                                   Clear by software

Tx buffer
0x11                              0x22                                                                                  0x33
（write SPI_DAT）

BUSY flag                                                                                             The flag set/clear by hardware

Write 0x11 into SPI_DAT
Wait until TE=1,Write 0x33 into SPI_DAT                                                                      Wait BUSY=0
Wait until TE=1,Write 0x22 into SPI_DAT                                                           Wait TX=1

Master one-way receive-only mode(MSEL=1，BIDIRMODE=0，RONLY=1):

When SPIEN=1, the receiving process starts. The data received on the MISO pin is serially shifted into the 8-
bit shift register in sequence, and then transferred to the SPI_DAT register (receive buffer) in parallel. The
software operation process is as follows (see Figure 19-7, the schematic diagram of RNE changes during
continuous transmission in only receive mode (BIDIRMODE=0 and RONLY=1)):

•   Enable the receive-only mode (RONLY = 1).

•   Enable SPI module, set SPIEN = 1: in master mode, SCLK clock signal is generated immediately, and serial
data is continuously received before SPI is turned off (SPIEN = 0); in slave mode, serial data is continuously
received when the SPI master device pulls low the NSS signal and generates SCLK clock.

•   Wait for RNE bit to be set to '1', read the SPI_DAT register to get the received data, and the RNE bit will be
cleared by hardware while reading SPI_DAT register. Repeat this operation to receive all data.

The process of data receiving can also be implemented in the interrupt handler generated by the rising edge of
the RNE flag.

*Figure 19-7 RNE Changing During Continuous Transmission Occurs in Receive-only Mode*


Configure：CLKPOL=1.CLKPHA=1,RONLY=1

SCK

DATA1=0xAA                                                     DATA2=0xBB                                                     DATA3=0xCC
MISO/MOSI
b0    b1    b2     b3      b4     b5        b6        b7        b0    b1   b2     b3    b4     b5     b6        b7        b0        b1    b2     b3     b4   b5    b6        b7
（in）

RNE flag                                   Set by hardware                                    Clear by software

Rx buffer
0xAA                                                             0xBB                             0xCC
（read fromSPI_DAT）

Wait until RNE=1,read 0xAA from SPI_DAT                     Wait until RNE=1,read 0XBB from SPI_DAT                            Wait until RNE=1,read 0xCC from SPI_DAT

Master bidirectional send mode (MSEL=1，BIDIRMODE=1，BIDIROEN=1，RONLY=0):

After data is written to register SPI_DAT (transmit buffer), the transmission process starts. This mode does not
receive data. While sending the first bit of data, the data is transferred from the send buffer to the 8-bit shift
register in parallel, and the SPI shifts the data to the MOSI transistor in sequence according to the configuration of
LSBFF. on feet.

The software operation flow of the master bidirectional send mode is the same as that of the send-only

mode. Master bidirectional receive mode (MSEL=1，BIDIRMODE=1，BIDIROEN=0，RONLY=0):

When SPIEN=1, BIDIROEN=0, the receiving process starts. In this mode, the MISO pin has no data output,
and the received data is serially shifted into the 8-bit shift register in sequence, and then transferred to the SPI_DAT
register (receive buffer) in parallel.

The software operation process of the host two-way receiving mode is the same as the receiving-only

mode. Slave full duplex mode (MSEL=0，BIDIRMODE=0 and RONLY=0):

The transmission process begins when the slave device receives the first edge of the clock signal. The software
must ensure that the data to be sent has been written in the register SPI_DAT before the SPI master device
starts the data transfer.

*Figure 19-8 TE/RNE/BUSY Changing During Slave Continuously TX in Full-duplex Mode*


Slave mode：CLKPOL=1.CLKPHA=1

SCK

DATA1=0x11                                          DATA2=0x22                                     DATA3=0x33
MISO/MOSI                                                             b7    b0                                         b7   b0                                    b7
b0     b1   b2     b3      b4      b5     b6               b1   b2     b3    b4     b5   b6               b1   b2     b3   b4    b5   b6
(out)

TE flag                                            Set by hardware                             Clear by software

Tx buffer
（write to SPI_DAT）          0x11                              0x22                                                                     0x33

BUSY flag                                                                         The flag set/clear by hardware
DATA1=0xAA                                          DATA2=0xBB                                     DATA3=0xCC
MISO/MOSI
b0     b1   b2     b3      b4      b5     b6   b7    b0    b1   b2     b3    b4     b5   b6     b7   b0   b1   b2     b3   b4    b5   b6   b7
（in）

RNE flag                                    Set by hardware                         Clear by software

Rx buffer
0xAA                                             0xBB                     0xCC
（read from SPI_DAT）

Write 0x11 into SPI_DAT                                                        Wait until TE=1,write 0x33 into SPI_DAT

Wait until TE=1,write 0x22 into SPI_DAT                                     Wait until RNE=1,read 0xBB from SPI_DAT

Wait until RNE=1,read 0xAA from SPI_DAT                         Wait until RNE=1,read 0xCC from SPI_DAT

Slave one-way send-only mode (MSEL=0，BIDIRMODE=0 and RONLY=0):

*Figure 19-9 TE/BUSY Changing During Continuous Slave Transmit-only Mode*


Slave mode：CLKPOL=1.CLKPHA=1

SCK

DATA1=0x11                                      DATA2=0x22                                      DATA3=0x33
MISO/MOSI
b0     b1   b2    b3   b4      b5    b6   b7    b0   b1     b2    b3    b4    b5   b6      b7   b0   b1     b2     b3   b4    b5   b6   b7
(out)

TE flag                                      Set by hardware                            Clear by software

Tx buffer
（write to SPI_DAT）       0x11                          0x22                                                                    0x33

BUSY flag                                                                The flag set/clear by hardware

Write 0x11 into SPI_DAT                  Wait until TE=1,write 0x33 into SPI_DAT                              Wait BUSY=0
Wait until TE=1,write 0x22 into SPI_DAT                           Wait TX=1

Slave receive-only mode (MSEL=0，BIDIRMODE=0 and RONLY=1):

The data reception process begins when the slave device receives the clock signal and the first data bit appears on its MOSI.
The received data is sequentially shifted into the 8-bit shift register, and then transferred to the SPI_DAT register (receive
buffer) in parallel.

Slave bidirectional send mode (MSEL=0，BIDIRMODE=1 and BIDIROEN=1):

The transmission process begins when the slave device receives the first edge of the clock signal. Data is not received in
this mode, and the software must ensure that the data to be sent has been written into the register SPI_DAT before the SPI
master device starts data transmission.

Slave bidirectional receive mode (MSEL=0，BIDIRMODE=1 and BIDIROEN=0):

Data transfer begins when the slave device receives the clock signal and the first data bit is present on its MOSI. There is no
data output in this mode, and the received data is sequentially shifted into the 8-bit shift register, and then transferred to the
SPI_DAT register (receive buffer) in parallel.

> **Note: The software operation process of the slave can refer to the master.**

SPI initialization process:

•       The baud rate of serial clock is defined by the SPI_CTRL1.BR[2:0] bits (this step is ignored if it is working in slave
mode).

•       Select CLKPOL bit and CLKPHA bit to define the phase relationship between data transmission and serial clock (see
*Figure 19-4).*


•       Set DATFF bit to define 8-bit or 16-bit data frame format.

•       Configure the SPI_CTRL1.LSBFF bit to define the frame format.

•       Configure the NSS mode as described above for the NSS function.

•       Run mode is configured by MSEL bit, BIDIRMODE bit, BIDIROEN bit and RONLY bit.

•       Set the SPIEN = 1 to enable SPI. Basic send and receive process:

When SPI sends a data frame, it first loads the data frame from the data buffer into the shift register, and then starts to send
the loaded data. When the data is transferred from the send buffer to the shift register, the send buffer empty flag is set
(SPI_STS.TE = 1), and the next data can be loaded into the send buffer; if the TEINTEN bit is set (SPI_CTRL2.TEINTEN
= 1), an interrupt will be generated; writing data to the SPI_DAT register will clear the SPI_STS.TE bit.

At the last edge of the sampling clock, when the data is transferred from the shift register to the receive buffer, the receive
buffer non-empty flag is set (SPI_STS.RNE = 1), at this time the data is ready and can be read from the SPI_DAT register; if
the receive buffer non-empty interrupt is enabled (SPI_CTRL2.RNEINTEN = 1), an interrupt will be generated; the
SPI_STS.RNE bit can be cleared by reading the SPI_DAT register data.

In master mode, the sending process starts when data is written to the send buffer. If the next data has been written into the
SPI_DAT register before the current data frame sending is completed, the continuous sending function can be achieved.

In slave mode, the NSS pin is low, and when the first clock edge arrives, the transmission process begins. In order to avoid
accidental data transmission, software must write data to the transmit buffer before data transmission (it is recommended to
enable the SPI module before the host sends the clock).

In some configurations, when the last data is sent, the BUSY flag (SPI_STS.BUSY) can be used to wait for the end of the
data sending.

Continuous and discontinuous transmission:

When sending data in master mode, if the software is fast enough to detect each TE (TE) rising edge (or TE interrupt), and
the data is written to the SPI_DAT register immediately before the end of the ongoing transmission. At this time, the SPI
clock remains continuous between the transmission of data items, and the BUSY bit will not be cleared, continuous
communication can be achieved.

If the software is not fast enough, it will result in discontinuous communication; in this case, the SPI_STS.BUSY bit is
cleared between the transmission of each data items (see figure below).

In master receive-only mode (RONLY = 1), communication is always continuous and the BUSY flag (BUSY) is always
high.

In slave mode, the continuity of communication is determined by the SPI master device. However, even if the
communication is continuous, the BUSY flag (BUSY) will be low for at least one SPI clock cycle between each data item
(see Figure 19-9).

*Figure 19-10 TE/BUSY Changing During Transmitted Discontinuously.*


配置：CLKPOL=1.CLKPHA=1

SCK
DATA1=0x11                                           DATA2=0x22                                   DATA3=0x33
MOSI
b0     b1   b2    b3   b4    b5   b6    b7        b0    b1       b2    b3   b4   b5   b6   b7       b0   b1   b2     b3   b4   b5   b6   b7
（out）

TE flag
`

Tx buffer
0x11                                                 0x22                                          0x33
（write to SPI_DAT）

BUSY flag

Wait until TE=1,write 0x22 into SPI_DAT                                              Wait TX=1         Wait BUSY=0
Write 0x11 into SPI_DAT
Wait until TE=1,write 0x33 into SPI_DAT

#### 19.3.3. Status flag

The SPI_STS register has 3 flag bits to monitor the status of the SPI:

19.3.1.1 Send buffer empty flag bit (TE)

When the send buffer is empty, this bit is '1', and new data to be sent can be written into SPI_DATA at this

time. When the transmit buffer is not empty, this bit is cleared to '0'.

19.3.1.2 Receive buffer non-empty flag bit (RNE)

When the receiving buffer is not empty, this bit is '1', indicating that the valid data has been received in the receiving

buffer. When reading the register SPI_DATA, this bit is cleared to '0'.

##### 19.3.3.1. BUSY flag bit (BUSY)

The BUSY flag can detect whether the transmission is over, set and cleared by hardware (software operation is invalid).

When the SPI communication is in progress, the BUSY flag is set to '1', but in the two-way receiving mode of the
master mode (MSEL=1, BIDIRMODE=1, BIDIROEN=0), the BUSY flag is set to '0' during reception. When
the transmission ends or the I2S module is turned off, the flag is set to '0':

●    End of transmission (except for continuous communication in master mode);

●    Turn off the SPI module (SPIEN = 0);

●    The master mode error occurs (MODERR = 1)

When the communication is discontinuous: the BUSY flag is cleared to '0' between the transmission of each data item.

When communication is continuous: in master mode, the BUSY flag remains high during the entire transfer
process; In slave mode, the BUSY flag will be low for 1 SPI clock cycle between each data item transfer. So do not
use the BUSY flag to handle the sending and receiving of each data item.

#### 19.3.4. Turn off the SPI

In order to turn off the SPI module, different operation modes require different operation steps.

##### 19.3.4.1. Master or slave full duplex mode

1. Wait for the RNE flag to be set to 1 and the last byte to be received;


2. Wait for the TE flag to be set to 1;


3. Wait for the BUSY flag to be cleared to 0;


4. Turn off the SPI module (SPIEN = 0).

##### 19.3.4.2. One-way send-only mode or bidirectional send mode for master or slave

1. After writing the last byte to the SPI_DAT register, wait for the TE flag to be set to 1;


2. Wait for the BUSY flag to be cleared to 0;


3. Turn off the SPI module (SPIEN = 0).

##### 19.3.4.3. One-way receive-only mode or bidirectional receive mode for master

1. Wait for the penultimate RNE to be set to 1;


2. Before closing the SPI module (SPIEN = 0), wait for 1 SPI clock cycle (using software delay);


3. Wait for the last RNE to be set before entering shutdown mode (or turning off the SPI module clock).

##### 19.3.4.4. One-way receive-only mode or bidirectional receive mode for slave

1. The SPI module can be turned off at any time (SPIEN = 0), and after the current transfer is over, the SPI

module will be turned off;

2. If you want to enter the shutdown mode, you must wait for the BUSY flag to be set to 0 before entering the

shutdown mode (or turn off the SPI module clock).

#### 19.3.5. SPI communication using DMA

SPI uses DMA to transfer data, which releases the application program from the process of reading and
writing the transceiver buffer, which greatly improves the system efficiency.

When the transmit buffer DMA is enabled (SPI_CTRL2 register TDMAEN=1), a DMA request is issued each
time TE is set to '1', and the DMA automatically writes data to the SPI_DAT register, which clears the TE flag.
When the receive buffer DMA is enabled (SPI_CTRL2 register RDMAEN =1), a DMA request is issued each
time RNE is set to '1', and the DMA automatically reads data from the SPI_DAT register, which clears the RNE
flag.

When only using SPI to send data, only need to enable the send DMA channel of SPI. At this time, because the
received data has not been read, OVER is set to '1', and the software does not need to process this flag at this time.

When only using SPI to receive data, only need to enable the receive DMA channel of SPI.

In the send mode, when the DMA has transmitted all the data to be sent (the TXCF flag of the DMA_INTSTS
register becomes '1'), you can monitor the BUSY flag to confirm the end of the SPI communication, which can
avoid closing the SPI or entering the stop mode , the transmission of the last data is destroyed. So the software
needs to wait for TE=1 first, and then wait for BUSY=0.

*Figure 19-11 Transmission Using DMA*


CLKPOL=1.CLKPHA=1

SCK

DATA1=0x11                                                                     DATA2=0x22                                                 DATA3=0x33
MISO/MOSI
b0    b1        b2        b3        b4        b5        b6        b7        b0         b1     b2     b3    b4    b5    b6      b7    b0         b1        b2        b3        b4        b5      b6      b7
(out)

Clear by DMA
TE flag                                                           Set by hardwa re
write

Tx buffer                    0x1
0x22                                                                               0x33
（write to SPI_DAT）                 1

BUSY flag                                                                                                         The flag set/clear by hardware

DMA request

DMA writes to
SPI_DAT

Set by hardware                                                        Clear by software
DMA flag
（DMA transfer complete）

DMA writes 0x11 into                                         DMA writes 0x33 into
SPI_DAT                                                      SPI_DAT                                         Wait BUSY=0

Configure the DMA                                      DMA writes 0x22 into                                          DMA tansfer is
Tx channel and                                           SPI_DAT                                                     complete
enable the SPI                                                                                                                                  Wait TX=1

*Figure 19-12 Reception using DMA*


CLKPOL=1.CLKPHA=1

SCK

DATA1=0xAA                                                                  DATA2=0xBB                                         DATA3=0xCC
MISO/MOSI             b0      b1   b2     b3       b4        b5        b6        b7        b0        b1        b2     b3     b4     b5   b6    b7   b0     b1     b2     b3        b4        b5        b6        b7
(in)

Clear by DMA
RNE flag                                         Set by hardware
read

Rx buffer
（read from SPI_DAT）                                                                                                         0xAA                                                 0xBB                                             0xCC

DMA request

DMA read from
SPI_DAT

DMA flag
Set by hardware                       Clear by software
（DMA transfer complete）

Configure the DMA                                                    DMA reads 0xAA from
Rx channel and                                                          SPI_DAT                                        DMA reads 0xCC from SPI_DAT
enable the SPI                                                                                                         DMA transfer is complete
DMA reads 0xBB from
SPI_DAT

#### 19.3.6. CRC Calculation

SPI contains two independent hardware CRC calculators for data transmission and data reception to ensure the reliability
of data transmission. CRC adopts different calculation methods according to the format of the data frame sent and/or
received: 8-bit data frame adopts CRC8; 16-bit data frame adopts CRC16. The SPI_CRCPOLY register sets the polynomial
value required for the calculation, and sets the CRCEN bit of the SPI_CTRL1 register to '1' to enable CRC calculation.

In the sending mode, after the last data is written into the sending buffer, set the CRCNEXT bit to '1', instructing the

hardware to send the CRC value (the value of SPI_CRCTDAT) after sending this data. During the transmission of the CRC,
the CRC calculation is stopped.

In receive mode, set the CRCNEXT bit to '1' after the penultimate data frame is received. The subsequent received CRC is
compared with the SPI_CRCRDAT value. If the two are different, the CRCERR flag position of the SPI_STS register is
'1', and when the ERRINTEN of the SPI_CTRL2 register is set to '1', an interrupt is generated.

In order to keep the synchronization of the next CRC calculation result of the master and slave devices, the CRC values at
both ends of the master and slave should be cleared. Setting the CRCEN bit resets both SPI_CRCRDAT and
SPI_CRCTDAT. Execute the steps in sequence: SPIEN=0; CRCEN=0; CRCEN=1; SPIEN=1.

It is worth mentioning that when SPI is configured as slave mode and CRC is enabled, as long as there is a clock pulse on
the SCLK pin, even if the NSS pin is high, the CRC calculation will still be performed. This situation is common when the
master device communicates with multiple slave devices alternately, and attention should be paid to avoid CRC
misoperation.

When the SPI hardware CRC check is enabled (CRCEN=1) and the DMA mode is enabled, at the end of the communication,
the hardware automatically completes the sending and receiving of the CRC byte.

#### 19.3.7. Error flag


##### 19.3.7.1. Master mode failure error (MODERR)

The following two conditions will cause the master mode failure error:

●    NSS pin hardware management mode, the master device NSS pin is pulled low;
●    NSS pin software management mode, the SSEL bit is set to 0.

When a master mode timing error occurs, the MODERR flag will be '1', if ERRINTEN=1, an interrupt will be generated;
the SPIEN bit and the MSEL bit are write-protected, and the hardware will clear them to 0, the SPI is turned off, and it is
forced to enter the slave mode.

The software performs a read or write operation to the SPI_STS register, and then writes the SPI_CTRL1 register to clear
the MODERR bit (in a multi-master configuration, the NSS pin of the master device must be pulled high first).

Under normal configuration, the MODERR bit of the slave device cannot be set to '1'. However, in a multi-master
configuration, a device can be in slave mode with the MODERR bit set; in this case, the MODERR bit indicates that a
multi-master conflict may have occurred. The interrupt routine can perform a reset or return to the default state to recover
from the error condition.

##### 19.3.7.2. Overflow error (OVER)

When RNE is set to '1', if there is still data sent to the receive buffer, an overflow error will occur. At this time, the OVER
flag position is '1', if ERRINTEN=1, an interrupt will be generated. All newly received data will be lost, and the SPI_DAT
register is the previously unread data.

Reading the SPI_DAT register followed by the SPI_STS register clears the OVER bit.

##### 19.3.7.3. CRC error (CRCERR)

The CRC error flag is used to check the validity of the received data. A CRC error is generated when the
received CRC value does not match the value in the SPI_CRCRDAT register. At this time, the CRCERR
flag position is '1', if ERRINTEN=1, an interrupt will be generated.

#### 19.3.8. SPI interrupt

*Table 19-1 SPI Interrupt Request*


Interrupt event                   Event flag bit      Enable control bit

Send buffer empty flag                                  TE                 TEINTEN

Receive buffer non empty flag                          RNE                RNEINTEN

Master mode failure event                            MODERR

Overflow error                                        OVER                ERRINTEN

CRC error flag                                       CRCERR

### 19.4. I2S Function Description

The block diagram of I2S is shown in the figure below:

*Figure 19-13 I2S Block Diagram*


Address and data bus

Tx buffer
SPI_STS
MODER         CRC
BUSY         OVER                               UNDER        CHSIDE        TE           RNE
R           ERR                                                       LSBFF         16-bit                     MOSI/SD
control bit
Shift register
MISO
16-bit
Communication circuit
Rx buffer

SPI_I2SCFG

MODCFG[1:0]          STDSEL[1:0]
CLK
TDATLEN[1:0]
CH                                                             NSS/WS
POL                         BITS

-          -            -       -      -                  MOD
-                 I2SEN
SEL

SPI_CTRL1                                                                                                  Baud rate generator

CLK         CLK
LSBFF   SPIEN                    BR[2:0]                MSEL
POL         PHA

BIDIR           BIDIRO                CRC
CRCEN             DATFF       RONLY        SSMEN     SSEL
MODE              EN                  NEXT

Main controller
CLK
I2S clock generator

SPI_I2SPREDIV
MCLK     ODD_                                                                     MODSEL
OEN      EVEN
LDIV[7:0]
I2Sx CLK
MCLK

The I2S interface uses the same pins, flags and interrupts as the SPI interface. Setting the MODSEL = 1 selects
the I2S audio interface.

I2S has a total of 4 pins, 3 of which are shared with SPI:

■    CLK: Serial clock (shared with SCLK pin), CLK generates a pulse every time 1-bit audio data is sent.

■    SD: Serial data (shared with MOSI pin), used for data send and receive;

■    WS: Channel selection (shared with NSS pin), used as data control signal output in master mode, and used
as input in slave mode;

■    MCLK: master clock (independent mapping, optional), output 256 × Fs clock signal to ensure better
synchronization between systems.

> **Note: FS is the sampling frequency of audio signal**

When set to master mode, I2S uses its own clock generator to generate the clock signal for communication.

#### 19.4.1. Supported Audio Protocols

Four audio standards can be selected by setting the SPI_I2SCFG.STDSEL[1:0] bits:

•    I2S Philips standard

•    MSB alignment standard

•    LSB alignment standard

•    PCM standard

Supports time-division multiplexing of audio data on the left and right channels, and the left channel always
sends data before the right channel. The CHSIDE bit of the register SPI_STS is used to distinguish which
channel the received data belongs to. The CHSIDE bit is meaningless in the PCM protocol.

The TDATLEN bit of the register SPI_I2SCTRL sets the length of the data to be transmitted, and the CHBITS bit
sets the number of data bits of the channel. Support four data formats to send data:

•    16-bit data is packed into 16-bit data frame

•    16-bit data is packed into a 32-bit data frame (the first 16 bits are meaningful data, and the last 16 bits are set
to 0 by hardware)

•    24-bit data is packed into 32-bit data frame (the first 24-bit data is meaningful data, and the latter 8-bit data
is set to 0 by hardware)

•    32-bit data is packed into 32-bit data frame

I2S uses the same SPI_DAT register as SPI to send and receive 16-bit wide data. If I2S needs to send or receive
24-bit or 32-bit wide data, the CPU needs to read or write the SPI_DAT register twice. On the other hand, when I2S
sends or receives 16-bit wide data, the CPU only needs to read or write the SPI_DAT register once.

Regardless of which data format and communication standard is used, I2S always sends the data high-order bit (MSB) first.

##### 19.4.1.1. I2S Philips standard

In the I2S Philips standard, the sender changes data on the falling edge of the clock signal, and the receiver reads
data on the rising edge. The WS signal is valid one clock cycle before sending the first bit of data (MSB), and
starts to change on the falling edge of the clock signal.

*Figure 19-14 I2S Philips Protocol Waveform (16/32-bit full precision, CLKPOL = 0)*


CLK

WS                              Send                    Receive

SD                       MSB                                                  LSB   MSB

Left channel (data format 16-bit or 32-bit)          Right channel

*Figure 19-15 I2S Philips Protocol Standard Waveform (24-bit frame, CLKPOL = 0)*


CLK

WS                             Send                      Recieve

The remaining
SD                     MSB                                   LSB       8 bits are    MSB
forced to 0
24-bit data
Right channel
Left channel 32-bit data

When the 24-bit data is packed into the frame format of the 32-bit data frame, the register SPI_DAT needs to be read or
written twice for each frame of data transmission. Example: Send 24-bit data 0x95AA66, write 0x95AA in SPI_DAT for
the first time, write 0x66XX in SPI_DAT for the second time (only the upper 8 bits are valid, the lower 8 bits are
meaningless, and can be any value); receive 24-bit data 0x95AA66, the first Read SPI_DAT for the first time to get 0x95AA,
and read SPI_DAT for the second time to get 0x6600 (only the upper 8 bits are valid, and the lower 8 bits are always 0).

*Figure 19-16 I2S Philips Protocol Standard Waveform (16-bit to 32-bit, CLKPOL = 0)*


CLK

WS                            Send     Recieve

The remaining 16 bits are
SD                      MSB                    LSB                                     MSB
forced to 0

16-bit data
Right channel
Left channel 32-bit data

When the 16-bit data is packed into the frame format of the 32-bit data frame, only one read and write operation is required
for the register SPI_DAT. The lower 16 bits used to expand to 32 bits are set to 0x0000 by hardware. Example: 16-bit data
to be sent or received is 0x89C1 (extended to 32-bit is 0x89C10000). When sending, the upper 16-bit halfword (0x89C1)
needs to be written into the register SPI_DAT; the flag bit TE is '1', which means that new data can be written, and if the
corresponding interrupt is enabled, an interrupt can be generated. Sending is done by hardware, even if the last 16 bits of
0x0000 have not been sent, TE will be set and a corresponding interrupt will be generated; when receiving, the flag bit
RNE will be set to ' 1', an interrupt can be generated if the corresponding interrupt is enabled. In this way, there is more
time between 2 reads and writes, which can prevent underflow or overflow from happening.

##### 19.4.1.2. MSB alignment standard

In the MSB alignment standard, the sender changes data on the falling edge of the clock signal; the receiver reads
data on the rising edge. The WS signal is generated simultaneously with the first data bit (MSB).

The standard data sending and receiving processing method is the same as the I2S Philips standard.

*Figure 19-17 The MSB is aligned with 16-bit or 32-bit full precision, CLKPOL = 0.*


CLK

WS                        Send                                   Receive

SD           MSB                                                                   LSB   MSB

Left channel (16-bit or 32-bit)                            Right Channel

*Figure 19-18 MSB Aligns 24-bit Data, CLKPOL = 0*


CLK

WS                       Send                                Receive

8-bit
SD            MSB                                                     LSB   remaining 0    MSB
forced
24-bit data
Right channel
Left channel 32-bit

*Figure 19-19 MSB-aligned 16-bit data is extended to 32-bit packet frame, CLKPOL = 0*


CLK

WS                         Send                                  Receive

SD            MSB                                 LSB       16-bit remaining 0 forced    MSB

16-bit data
Right channel
Left channel 32-bit

##### 19.4.1.3. LSB alignment standard

In 16-bit or 32-bit full-precision frame format, LSB alignment standard is the same as MSB alignment standard.

*Figure 19-20 LSB Alignment 16-bit or 32-bit Full Precision, CLKPOL = 0*


CLK

WS                             Send                                    Receive

SD              MSB                                                                             LSB       MSB

Left channel (16-bit or 32-bit)                                           Right channel

*Figure 19-21 LSB Aligns 24-bit data, CLKPOL = 0*


CLK

WS                                                 Send                 Receive

8-bit data
SD                                  MSB                                             LSB   MSB
forced 0

24-bit data
Right channel
Left channel 32-bit

When the 24-bit data is packed into the frame format of the 32-bit data frame, the register SPI_DAT needs to be
read or written twice for each frame of data transmission. Example: To send 24-bit data 0x95AA66, the first
SPI_DAT writes 0xXX95 (only the lower 8 bits are valid, the upper 8 bits are meaningless, and can be any value),
and the second SPI_DAT writes 0xAA66. Receive 24-bit data 0x95AA66, read SPI_DAT for the first time to get
0x0095 (only the lower 8 bits are valid, and the upper 8 bits are always 0), and read SPI_DAT for the second time
to get 0xAA66.

*Figure 19-22 LSB Aligned 16-bit data is extended to 32-bit packet frame, CLKPOL = 0*


CLK

WS                                                       Send            Receive

SD               16-bit data forced 0       MSB                                    LSB     MSB

16-bit data
Right channel
Left channel 32-bit

When the 16-bit data is packed into the frame format of the 32-bit data frame, only one read and write operation is required

for the register SPI_DAT. The upper 16 bits used to expand to 32 bits are set to 0x0000 by hardware. Example: 16-
bit data to be sent or received is 0x89C1 (extended to 32-bit is 0x000089C1). When sending, if TE is '1', the user
needs to write the data to be sent (0x89C1). The upper 16 bits 0x0000 used for extension are first sent out by the
hardware, and once valid data starts to be sent out from the SD pin, the next TE event occurs. When receiving,
once valid data (not 0x0000 part) is received, the RNE event occurs. In this way, there is more time between 2
reads and writes, which can prevent underflow or overflow from happening.

##### 19.4.1.4. PCM standard

In the PCM standard, there are two frame structures, short frame and long frame, which are selected by
setting the PCMFSYNC bit of the register SPI_I2SCFG. The WS signal represents frame synchronization
information. The effective time of the WS signal used for synchronization by the long frame is 13 bits; the length
of the WS signal used by the short frame for synchronization is 1 bit.

The standard data sending and receiving processing method is the same as the I2S Philips standard.

*Figure 19-23 PCM Standard Waveform (16 bits)*


CLK

WS short frame

WS long frame                                 13-bit

SD                    MSB                                          LSB   MSB

16-bit

*Figure 19-24 PCM Standard Waveform (16-bit extended to 32-bit packet frame)*


CLK

WS short frame

WS long frame                       13-bit

SD                     MSB                                LSB              MSB

16-bit

#### 19.4.2. Clock Generator

In the master mode, the linear divider needs to be set correctly in order to obtain the desired audio frequency.

*Figure 19-25 I2S Clock Generator Structure*


MCLK

0                                                                      8-bit Linear
Divider            Divider               Divider +
0
1                               by 2               by 4                 reshaping
CLK                                                                                                                  I2Sx CLK
stage
1

MCLKOEN

MOD           MCLK   ODD_
-      -       -        -      -      -         I2SEN                          LDIV[7:0]
SEL           OEN    EVEN

> **Note: The clock source of I2Sx CLK is HSI, HSE or PLL system clock that drives AHB clock.**

The bit rate of I2S determines the data flow on the I2S data line and the frequency of the I2S clock

signal. I2S bit rate = number of bits per channel × number of channels × audio

sampling frequency

For a signal with left and right channels and 16-bit audio, the I2S bit rate is calculated as:

I2S bit rate = 16 × 2 × FS

If the packet length is 32 bits, there are:

I2S bit rate = 32 × 2 × FS

*Figure 19-26 Audio Sampling Frequency Definition*


16-bit or 32-bit left channel                16-bit or 32-bit right channel

32-bit or 64-bit

Fs：Audio sampling frequency
Sampling point                                                                             Sampling point

Audio can be sampled at 96kHz, 48kHz, 44.1kHz, 32kHz, 22.05kHz, 16kHz, 11.025kHz, or 8kHz (or any value
within this range). Set the linear divider by referring to the following formula:

CHBITS=0，𝐹𝑆 = 𝐼2𝑆𝑥𝐶𝐿𝐾[(16 × 2) × ((2 × 𝐿𝐷𝐼𝑉) + 𝑂𝐷𝐷_𝐸𝑉𝐸𝑁)]
CHBITS=1，𝐹𝑆 = 𝐼2𝑆𝑥𝐶𝐿𝐾[(32 × 2) × ((2 × 𝐿𝐷𝐼𝑉) + 𝑂𝐷𝐷_𝐸𝑉𝐸𝑁)]

The exact audio frequency can be obtained by referring to the clock configuration in the table below.

*Table 19-2 Use The Standard 8MHz HSE Clock To Get Accurate Audio Frequency*


#### 19.4.3. I2S Send and Receive Sequence

I2S initialization process:

•   The LDIV[7:0] and ODD_EVEN bits of the register SPI_I2SPREDIV configure prescaler related parameters,
serial clock baud rate.

•   Set the CLKPOL bit of the register SPI_I2S_CFG to define the polarity of the communication clock when it is
idle; MODSEL position '1' is configured as I2S mode, MODCFG[1:0] selects I2S master-slave mode and
transmission direction (send or receive); set STDSEL[ 1:0] to select the required I2S standard (under the PCM
standard, set the PCMFSYNC bit to select the synchronization mode); set TDATLEN[1:0] to select the number of
data bits, set CHBITS to select the number of data bits for each channel.

•   If you need to use interrupt or DMA, the configuration is the same as SPI.

•   Finally start I2S communication, I2SEN bit '1'. Master mode sending process:

When I2S works in master mode, the pin CLK outputs the serial clock, and the pin WS generates the channel
selection signal.

The sending process begins when data is written into the send buffer. When the data of the current channel is moved
from the send buffer to the shift register in parallel, the flag bit TE is set to '1', at this time, the data of another channel
should be written into SPI_DAT. Confirm the channel corresponding to the current data to be transmitted through the flag
bit CHSIDE.

The value of CHSIDE is updated when TE is set to '1'. A complete data frame includes the left channel and the right channel,
and only part of the data frame cannot be transmitted. When the flag bit TE is set to '1', if the TEINTEN bit of the register
SPI_CTRL2 is '1', an interrupt will be generated.

The operation of writing data depends on the selected I2S standard, see Section 19.4.1 for details.

When closing the I2S function, wait for the flag bit TE=1 and BUSY=0, and then clear the I2SEN bit to '0'. Slave mode

sending process:

The sending process of the slave mode is similar to the master mode, the difference is:

When I2S works in slave mode, there is no need to configure the clock, and the pin CLK and pin WS are connected to the
corresponding pins of the master device. The transmit process starts when the external master sends a clock signal and
when the NSS_WS signal requests data transfer. The slave must be enabled and the I2S data register must be written before
the external master can start communicating.

When the first clock edge representing the next data transmission arrives, the new data is still not written into the register
SPI_DAT, that is, an underflow occurs, and the flag bit UNDER is set to '1'. If the ERRINTEN bit of the register
SPI_CTRL2 is '1', An interrupt is generated, indicating that an error has occurred.

The flag bit CHSIDE indicates which channel the data to be transmitted corresponds to. Compared with the sending process
of the master mode, in the slave mode, CHSIDE depends on the WS signal of the external master I2S (the WS signal is '1'
to send the left channel).

Master mode receiving process:

Audio data is always received in 16-bit packets. According to the configured data and channel length, the received audio
data will need to be sent to the receiving buffer once or twice.

When the data is transferred from the shift register to the receive buffer, the non-empty flag bit RNE of the receive buffer
of the SPI_STS register is set to '1', at this time the data is ready and can be read from the SPI_DAT register; if the
SPI_CTRL2 register RNEINTEN position' 1', an interrupt is generated; reading the SPI_DAT register clears the RNE bit.
If the previously received data has not been read and new data is received, an overflow occurs, the flag bit OVER is set to
'1', and if the ERRINTEN bit of the register SPI_CTRL2 is '1', an interrupt is generated, indicating An error has occurred.

The channel corresponding to the currently transmitted data is confirmed by the flag bit CHSIDE, and the value of CHSIDE
is updated when RNE is set to '1'.

The operation of reading data depends on the selected I2S standard, see Section 19.4.1 for details.

When the I2S function is turned off, different audio standards, data length and channel length adopt different operation
steps:

•    Data length is 16 bits, channel length is 32 bits (TDATTLEN = 00, CHBITS = 1), LSB alignment standard (STDSEL
= 10).

−    Wait for the penultimate RNE flag bit to be set to' 1'.

−    Software delay, waiting for 17 I2S clock cycles.

−    Turn off I2S (I2SEN = 0).

•    The data length is 16 bits, the channel length is 32 bits (TDATLEN = 00 and CHBITS = 1), the MSB alignment

standard (STDSEL = 01), I2S Philips standard (STDSEL = 00) or PCM standard (STDSEL = 11)

−    Wait for the last RNE flag bit to be set to' 1'.

−    Software delay, waiting for 1 I2S clock cycle.

−    Turn off I2S (I2SEN = 0).

•    Other combiNSING of TDATLEN and CHBITS and any audio mode selected by STDSEL:

−     Wait for the penultimate RNE flag bit to be set to' 1'.

−     Software delay, waiting for 1 I2S clock cycle.

−     Turn off I2S (I2SEN = 0). Slave mode receiving process:

The receiving process of the slave mode is similar to the master mode, the difference is:

The flag bit CHSIDE indicates which channel the data to be transmitted corresponds to. Compared with the receive flow of
the master mode, in the slave mode, CHSIDE depends on the WS signal of the external master I2S. When closing the I2S
function, clear the I2SEN bit to '0' when the waiting flag bit RNE=1.

#### 19.4.4. Status Flag

There are the following 4 flag bits in the SPI_STS register for monitoring the status of the I2S bus. TX buffer empty flag (E):

When the send buffer is empty, this flag is set to 1, indicating that new data can be written into the SPI_DAT register. When
the send buffer is not empty, this flag is cleared to 0.

RX buffer not empty flag (RNE):

When the receiving buffer is not empty, this bit is '1', indicating that the valid data has been received in the receiving buffer.

When reading register SPI_DATA, this bit is cleared to '0'..

BUSY flag (BUSY):

The BUSY flag can detect whether the transmission is over, set and cleared by hardware (software operation is invalid).

When the I2S communication is in progress, the BUSY flag is set to '1', but in the master receiving mode (MODCFG=11),
the BUSY flag is set to '0' during reception. When the transmission ends or the I2S module is turned off, the flag is set to '0'.

In slave mode with continuous communication, the BUSY flag goes low for 1 I2S clock cycle between each data item
transfer. Therefore, do not use the BUSY flag to handle the sending and receiving of every data item.

Channel flag (CHSIDE):

CHSIDE is used to indicate the channel where the data currently being sent and received is located. Under the PCM
standard, this flag bit has no meaning.

In transmit mode, this flag is updated when TE is set to '1'; in receive mode, this flag is updated when RNE is set to '1'. In

the process of sending and receiving, if an overflow (OVER) or underflow (UNDER) error occurs, this flag is
meaningless, and I2S needs to be turned off and on again.

#### 19.4.5. Error Flag

The SPI_STS register has 2 error

flag bits. Overflow flag (OVER):

When RNE is set to '1', if there is still data sent to the receive buffer, an overflow error will occur. At this time, the
OVER flag position is '1', if ERRINTEN=1, an interrupt will be generated. All newly received data will be lost,
and the SPI_DAT register is the previously unread data.

Reading the SPI_DAT register followed by the SPI_STS register clears the

OVER bit. Underflow flag (UNDER):

In slave transmit mode, if the transmit buffer is still empty when the first clock edge of data transmission
arrives, the UNDER flag is set to '1'. If ERRINTEN=1, an interrupt will be generated.

Reading register SPI_STS clears the UNDER bit.

#### 19.4.6. I2S interrupt

The following table lists all I2S interrupts.

*Table 19-3 I2S Interrupt Request*


Interrupt Event                               Event Flag Bit                      Enable Control Bit

Send buffer empty flag                                               TE                                TEINTEN

Receive buffer non empty flag                                       RNE                               RNEINTEN

Underflow flag bit                                                UNDER
ERRINTEN
Overflow flag bit                                                  OVER

#### 19.4.7. DMA Function

There is no data transmission protection function in I2S mode, so it does not support CRC function, and other
DMA functions are exactly the same as SPI mode.

### 19.5. SPI and I2S Register Description


#### 19.5.1. SPI Register Overview

*Table 19-4 SPI Register Overview*


Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

BIDIRMODE
SPI_CTR

BIDIROEN

CRCNEXT

CLKPHA
CLKPOL
CRCEN

SSMEN
RONLY
DATFF

LSBFF
SPIEN

MSEL
SSEL
BR[2:0]
00      L1
Reserved
0h     Reset
0           0         0        0        0        0       0        0       0          0          0         0           0         0       0         0
Value

RNEINTEN
ERRINTEN
SPI_CTR

TEINTEN

RDMAEN
TDMAEN
SSOEN
00      L2

Reserved
Reserved
4h     Reset
0          0          0                               0       0         0
Value

MODERR
CRCERR

CHSIDE
UNDER
OVER
BUSY
SPI_STS

RNE
TE
00
Reserved
8h     Reset
0          0          0         0           0         0       1         0
Value

SPI_DAT                                                                                                                                                         DAT[15:0]
00
Reset                                        Reserved
Ch                                                                                                   0           0         0        0        0        0       0        0       0          0          0         0           0         0       0         0
Value

SPI_CRC
CRCPOLY[15:0]
01     POLY
Reserved
0h     Reset
0           0         0        0        0        0       0        0       0          0          0         0           0         1       1         1
Value

SPI_CRC
CRCRDAT[15:0]
01 RDAT
Reserved
4h     Reset
0           0         0        0        0        0       0        0       0          0          0         0           0         0       0         0
Value

SPI_CRC
CRCTDAT[15:0]
01     TDAT
Reserved
8h     Reset
0           0         0        0        0        0       0        0       0          0          0         0           0         0       0         0
Value

MODCF                                                                 TDATLE
PCMFSYNC

01 SPI_I2SC                                                                                                                                                                                         STDSEL
Reserved
MODSEL

CLKPOL

CHBITS
I2SEN

Reserved                                                                                         G                                                                     N [1:0]
Ch      FG                                                                                                                                                                                             [1:0]
[1:0]

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

Reset
0    0     0         0         0       0    0    0     0   0   0
Value

ODD_EVEN
MCLKOEN
SPI_I2SP
LDIV[7:0]
02 REDIV
Reserved
0h     Reset
0         0         0   0   0    0    0     0   1   0
Value

#### 19.5.2. SPI Control Register 1 (SPI_CTRL1) (not used in I2S mode)

Address: 0x00

Reset value: 0x0000

Bit Field            Name                      Description

15                   BIDIRMODE                 Bidirectional data mode enable
0: Select the "two-wire one-way" mode.
1: Select the "one-wire bidirectional " mode.
> **Note: Not used in I2S mode.**

14                   BIDIROEN                  Output enable in bidirectional mode
0: Output disable (receive-only mode).
1: Output enabled (send-only mode).
In master mode, the "one-wire" data line is the MOSI pin, and in slave mode, the "one-wire"
data line is the MISO pin.
> **Note: Not used in I2S mode.**

13                   CRCEN                     Hardware CRC check enable
0: Disable CRC calculation.
1: Enable CRC calculation.
> **Note: This bit can only be written when SPI is disabled (SPI_CTRL1.SPIEN = 0), otherwise an**
error will occur.
This bit can only be used in full duplex mode.
> **Note: Not used in I2S mode.**

12                   CRCNEXT                   Send CRC next
0: The next sent value comes from the send buffer.
1: The next send value comes from the CRC register.
> **Note: This bit should be set immediately after the last data is written in SPI_DAT register.**
> **Note: Not used in I2S mode.**

Bit Field   Name    Description

11          DATFF   Data frame format
0: 8-bit data frame format is used for sending/receiving.
1: 16-bit data frame format is used for sending/receiving.
> **Note: This bit can only be written when SPI is disabled (SPI_CTRL1.SPIEN = 0), otherwise an**
error will occur.
> **Note: Not used in I2S mode.**

10          RONLY   Only receive mode

This bit, together with the SPI_CTRL1.BIDIRMODE bit, determines the transfer direction in
two-wire one-way mode. In the application scenario of multiple slave devices, this bit is only set
to 1 by the accessed slave device, and only the accessed slave device can output, so as to avoid
data line conflicts.
0: Full duplex (sending mode and receiving mode).
1: Disable output (receive-only mode).
> **Note: Not used in I2S mode.**

9           SSMEN   Software slave device management
When the SPI_CTRL1.SSMEN bit is set to 1, the NSS pin level is determined by the value of
the SPI_CTRL1.SSEL bit.
0: Disable software slave device management.
1: Enable software slave device management.
> **Note: Not used in I2S mode.**

8           SSEL    Internal slave device selection
This bit only has meaning when the SPI_CTRL1.SSMEN bit is set. It determines the NSS level,
and I/O operations on the NSS pin have no effect.
> **Note: Not used in I2S mode.**

7           LSBFF   Frame format
0: Send MSB first.
1: Send LSB first.
> **Note: This bit cannot be changed during communication.**
> **Note: Not used in I2S mode.**

6           SPIEN   SPI enable
0: Disable SPI device.
1: Enable the SPI device.
> **Note: Not used in I2S mode.**
> **Note: When turning off the SPI device, please follow paragraph 0 section's procedure**
operation.

Bit Field      Name       Description

5:3            BR[2:0]    Baud rate control
000: fPCLK/2
001: fPCLK/4
010: fPCLK/8
011: fPCLK/16
100: fPCLK/32
101: fPCLK/64
110: fPCLK/128
111: fPCLK/256
> **Note: This bit cannot be changed during communication.**
> **Note: Not used in I2S mode.**

2              MSEL       Master device selection
0: Configure as the slave device.
1: Configure as the master device.
> **Note: This bit cannot be changed during communication.**
> **Note: Not used in I2S mode.**

1              CLKPOL     Clock polarity
0: In idle state, SCLK remains low.
1: In idle state, SCLK remains high.
> **Note: This bit cannot be changed during communication.**
> **Note: Not used in I2S mode.**

0              CLKPHA     Clock phase

0: Data sampling starts from the first clock edge.

1: Data sampling starts at the second clock edge.

> **Note: This bit cannot be modified while communication is in progress.**

> **Note: Not used in I2S mode.**

#### 19.5.3. SPI Control Register 2 (SPI_CTRL2)

Address: 0x04

Reset value: 0x0000

Bit Field      Name       Description

15:8           Reserved   Forced by hardware to 0

Bit Field      Name       Description

7              TEINTEN    Send buffer empty interrupt enable
0: Disable TE interrupt.
1: Enable TE interrupt, and interrupt request is generated when TE flag is set to '1'.

6              RNEINTEN   Receive buffer non-empty interrupt enable
0: Disable RNE interrupt.
1: Enable RNE interrupt, and generate interrupt request when RNE flag is set to ‘1’.

5              ERRINTEN   Error interrupt enable
When an error (CRCERR, OVER, UNDER, MODERR) is generated, this bit controls whether
an interrupt is generated
0: Disable error interrupt.
1: Enable error interrupt.

4:3            Reserved   Forced by hardware to 0

2              SSOEN      NSS output enable
0: Disable NSS output in master mode, the device can work in multi-master mode.
1: When the device is turned on, enable NSS output in the master mode, the device cannot work
in the multi-master device mode.
> **Note: Not used in I2S mode.**

1              TDMAEN     Send buffer DMA enable
When this bit is set, a DMA request is issued as soon as the TE flag is set
0: Disable send buffer DMA.
1: Enable send buffer DMA.

0              RDMAEN     Receive buffer DMA enable
When this bit is set, a DMA request is issued as soon as the RNE flag is set
0: Disable receive buffer DMA.
1: Enable receive buffer DMA.

#### 19.5.4. SPI Status Register (SPI_STS)

Address: 0x08

Reset value: 0x0002

Bit Field      Name       Description

15:8           Reserved   Forced by hardware to 0

Bit Field   Name     Description

7           BUSY     Busy flag
0: SPI is not busy.
1: SPI is busy communicating or the send buffer is not empty.
This bit is set or reset by hardware.
> **Note: Use of this flag requires special attention, see section 19.3.3 and section 19.3.4 for**
details..

6           OVER     Overflow flag
0: No overflow error.
1: An overflow error occurred.
> **Note:This bit is set by hardware and cleared according to the sequence of software operations.**
For more information about software sequences, refer to 19.4.5 for details.

5           MODERR   Mode error
0: No mode error.
1: A mode error occurred.
> **Note:This bit is set by hardware and cleared according to the sequence of software operations.**
For more information about software sequences, refer to 19.3.7 for details.
> **Note: Not used in I2S mode.**

4           CRCERR   CRC error flag
0: The received CRC value matches the value the SPI_CRCRDAT register value.
1: The received CRC value does not match the SPI_CRCRDAT register value.

> **Note: this bit is set by hardware and cleared by software by writing 0.**
> **Note: Not used in I2S mode.**

3           UNDER    Underflow flag
0: No underflow occurred.
1: Underflow occurred.
> **Note:This bit is set by hardware and cleared according to the sequence of software operations.**
For more information about software sequences, refer to 0 for details.
> **Note: not used in SPI mode.**

2           CHSIDE   Channel
0: The left channel needs to be sent or received;

1: The right channel needs to be sent or received.
> **Note: not used in SPI mode. No meaning in PCM mode.**

1           TE       The send buffer is empty
0: The send buffer is not empty.
1: The send buffer is empty.

Bit Field      Name             Description

0              RNE              Receive buffer is not empty
0: The receive buffer is empty.
1: The receive buffer is not empty.

#### 19.5.5. SPI Data Register (SPI_DAT)

Address: 0x0C

Reset value: 0x0000

Bit Field      Name             Description

15:0           DAT[15:0]        Data register
Data to be sent or received
The data register corresponds to two buffers: one for write (send buffer); The other is for read
(receive buffer).Write operation writes data to send buffer; The read operation will return the
data in the receive buffer.

Note on SPI mode: According to the selection of the data frame format by the
SPI_CTRL1.DATFF bit, the data sending and receiving can be 8-bit or 16-bit. To ensure correct
operation, the data frame format needs to be determined before enabling the SPI.
For 8-bit data, the buffer is 8-bit, and only SPI_DAT[7:0] is used when sending and receiving.
When receiving, SPI_DAT[15:8] is forced to 0.
For 16-bit data, the buffer is 16-bit, and the entire data register is used when sending and
receiving, that is, SPI_DAT[15:0].

#### 19.5.6. SPI CRC Polynomial Register (SPI_CRCPOLY) (not used in I2S mode)

Address: 0x10

Reset value: 0x0007

Bit Field      Name               Description

15:0           CRCPOLY [15:0]     CRC polynomial register
This register contains the polynomial used for the CRC calculation.

The reset value is 0x0007, other values can be set according to the application.
> **Note: not used in I2s mode.**

#### 19.5.7. SPI RX CRC Register (SPI_CRCRDAT) (not used in I2S mode)

Address offset: 0x14

Reset value: 0x0000

Bit Field    name       describe

15:0         CRCRDAT    Receive CRC Register

When CRC calculation is enabled, CRCRDAT[15:0] contains the CRC value calculated based
on the received bytes. This register is reset when a '1' is written to the CRCEN bit of
SPI_CTRL1. The CRC calculation uses the polynomial in SPI_CRCPOLY.

When the data frame format is set to 8 bits, only the lower 8 bits participate in the calculation,
and follow the CRC8 method; when the data frame format is 16 bits, all 16 bits in the register
participate in the calculation, and follow the CRC16 standard.

> **Note: Reading this register when the BUSY flag is '1' may result in incorrect values.**

> **Note: Not used in I2S mode..**

#### 19.5.8. SPI TX CRC Register（SPI_ CRCTDAT）

Address offset: 0x18

Reset value: 0x0000

Bit Field    Name       Description

15:0         CRCTDAT    Send CRC register
When CRC calculation is enabled, CRCTDAT[15:0] contains the CRC value calculated based
on the bytes to be sent. This register is reset when a '1' is written to the CRCEN bit in
SPI_CTRL1. The CRC calculation uses the polynomial in SPI_CRCPOLY.

When the data frame format is set to 8 bits, only the lower 8 bits participate in the calculation,
and follow the CRC8 method; when the data frame format is 16 bits, all 16 bits in the register
participate in the calculation, and follow the CRC16 standard .

> **Note: Reading this register when the BUSY flag is '1' may result in incorrect values.**

> **Note: Not used in I2S mode.**

#### 19.5.9. SPI_ I2S Configuration Register（SPI_I2SCFG）

Address offset: 0x1c

Reset value: 0x0000

Bit Field     Name       Description

15:12         Reserved   Forced by hardware to 0

11            MODSEL     I2S mode selection
0: Select SPI mode.
1: Select I2S mode.
> **Note: this bit can only be set when SPI or I2S is turned off.**

10            I2SEN      I2S enable
0: Disable I2S.
1: Enable I2S.
> **Note: not used in SPI mode.**

9:8           MODCFG     I2S mode setting
00: Slave device sends.
01: Slave device receives.
10: Master device sends.
11: Master device receives.
> **Note: This bit can only be set when I2S is turned off.**
> **Note: not used in SPI mode.**

7             PCMFSYNC   PCM frame synchronization
0: Short frame synchronization.
1: Long frame synchronization.
> **Note: This bit is only meaningful when STDSEL = 11 (used by the PCM standard).**
> **Note: not used in SPI mode.**

6             Reserved   Forced by hardware to 0

5:4           STDSEL     Selection of I2S standard
00: I2S Philips standard.
01: High byte alignment standard (left alignment).
10: Low byte alignment standard (right alignment).
11: PCM standard.
See for details of I2S standard on section 19.4.1.

> **Note: For correct operation, this bit can only be set when I2S is turned off.**

Bit Field      Name        Description

> **Note: not used in SPI mode.**

3              CLKPOL      Static clock polarity
0: I2S clock static state is low level.
1: I2S clock static state is high level.
> **Note: For correct operation, this bit can only be set when I2S is turned off.**
> **Note: not used in SPI mode.**

2:1            TDATLEN     Length of data to be transmitted
00: 16-bit data length.
01: 24-bit data length;
10: 32-bit data length;
11: Not allowed.
> **Note: For correct operation, this bit can only be set when I2S is turned off.**
> **Note: not used in SPI mode.**

0              CHBITS      Channel length (number of data bits per audio channel)
0: 16 bits wide;
1: 32 bits wide.
Writing to this bit is meaningful only when SPI_I2SCFG.TDATLEN = 00, otherwise the
channel length is fixed to 32 bits by hardware.
> **Note: For correct operation, this bit can only be set when I2S is turned off.**
> **Note: not used in SPI mode.**

#### 19.5.10. SPI_I2S Prescaler Register (SPI_I2SPREDIV)

Address: 0x20

Reset value: 0x0002

Bit Field      Name        Description

15:10          Reserved    Forced by hardware to 0

8              ODD _EVEN   Odd coefficient prescaler

0: Actual frequency division factor = LDIV ×2.

1: Actual frequency division factor = (LDIV × 2) + 1.
See section 19.4.2 for details.
> **Note: For correct operation, this bit can only be set when I2S is turned off.**

> **Note: not used in SPI mode.**

Bit Field        Name               Description

7:0              LDIV               I2S linear prescaler
Disable setting LDIV [7:0] = 0 or LDIV [7:0] = 1
See Section 19.4.2 for details.
> **Note: For correct operation, this bit can only be set when I2S is turned off.**

> **Note: not used in SPI mode.**

## 20. Real-Time Clock (RTC)


### 20.1. RTC Description

The real-time clock (RTC) has a set of independent BCD timer/counters that continuously count. Under the
corresponding software configuration, the calendar function can be provided. At the same time, the RTC
provides an interrupt with a programmable alarm clock.

Two 32-bit registers contain decimal format (BCD) representation of subseconds, seconds, minutes, hours (12 or
24 hour format), day (day of week), day (number), month, and year.

Subsecond values are provided in binary format as separate 32-bit registers. Additional 32-bit registers
contain programmable seconds, minutes, hours, day, day, month and year.

RTC provides automatic wake-up function in low power mode.

### 20.2. Specification

Real Time Clock (RTC) features include:

•         22-bit programmable prescaler

•         16-bit programmable periodic wake-up timer for automatic wake-up in low-power modes

•         Provides calendar functions in BCD format:

−     Accurate timing of seconds, minutes, hours, days, days, months and years

−     Provide individual sub-second values

−     With automatic leap year compensation function, can also perform daylight saving time compensation

−     A digital calibration function compensates for variations in crystal oscillator accuracy.

•         Provides 1 interrupt with programmable alarm for wake-up from low-power modes

•         The following two RTC clock sources can be selected:

−    LSE oscillator clock;

−    LSI oscillator clock

•         2 dedicated maskable interrupts:

−     alarm interrupt

−     Wake-up Timer Interrupt

•     After reset, all RTC registers (except RTC_CTRL, RTC_INITSTS[10:8]) are protected against
possible accidental write access.

### 20.3. RTC Function Description


#### 20.3.1. RTC Block Diagram

*Figure 20-1 RTC Block Diagram*


RTC includes the following functions:

•     Calendar function in BCD format: accurate timing of sub-seconds, seconds, minutes, hours, day of the
week, day, month and year.

•     An alarm clock: sub-second, second, minute, hour, day of the week, day can be set.

•     16-bit programmable periodic wake-up timer for automatic wake-up in low-power modes

#### 20.3.2. RTC Clock and Prescaler

The RTC clock source (RTCCLK) is selected between the LSE clock and the LSI oscillation clock by the clock controller
(RCC). A programmable prescaler generates a 1Hz clock for updating the calendar. To reduce power consumption, the
prescaler is divided into 2 programmable prescalers. Mainly as follows:

•    7-bit asynchronous prescaler configured via the DIVA bit of the RTC_PRE register

•    15-bit synchronous prescaler configured via the DIVS bits of the RTC_PRE register

To use 32.768 kHz LSE, you need to set the asynchronous frequency division factor to 128, and the synchronous frequency
division factor to 256, and the internal clock frequency that can be obtained is 1 Hz (ck_spre). The minimum division factor is
1 and the maximum is 222. This corresponds to a maximum input frequency of approximately 4MHz.

The formula for fck_apre :

The ck_apre clock is used to clock the binary RTC_SUBS sub-second down counter. When 0 is reached, reload the
RTC_SUBs with the contents of the DIVS.

The formula for fck_spre :

The ck_spre clock can be used to update the calendar or as a time base for the 16-bit wake-up auto-reload timer. For shorter
time-out times, a 16-bit wake-up auto-reload timer can also be run with RTCCLK divided by a
programmable 4-bit asynchronous prescaler.

#### 20.3.3. RTC Calendar

The RTC calendar time and date registers are accessed through shadow registers synchronized to PCLK (APB
clock). They can also be accessed directly to avoid waiting for synchronization time. The register comparison is as
follows:

•    RTC_SUBS set subsecond

•    RTC_TSH set time

•    RTC_DATE set date

Every two RTCCLK cycles, the current calendar value is copied into the shadow register and the RSYF bit
of the RTC_INITSTS register is set. No replication is performed in Standby and Sleep modes. When exiting
these modes, the shadow registers are updated after a maximum of 2 RTCCLK cycles.

When the application reads the calendar register, it accesses the contents of the shadow register. The calendar
registers can be accessed directly by setting the BYPS bit in the RTC_CTRL register. By default, this bit is cleared
and the user accesses the shadow register.

When reading the RTC_SUBS, RTC_TSH or RTC_DATE registers in BYPS=0 mode, the frequency of the
APB clock (fAPB) must be at least 367 / 400 times the frequency of the RTC clock (fRTCCLK). Shadow registers
are reset by a system reset.

#### 20.3.4. Programmable Alarms

The RTC unit provides a programmable alarm: Alarm A. The following is a description of alarm clock A. The
programmable alarm function can be enabled through the ALAEN bit in the RTC_CTRL register. The ALAF flag is set if
the calendar subsecond, second, minute, hour, date matches the value programmed in the alarm registers RTC_ALRMASS
and RTC_ALARMA. Each calendar field can be selected individually via the MASKx bits of the RTC_ALARMA register
and the MASKSSBx bits of the RTC_ALRMASS register. The alarm interrupt can also be enabled through the ALAIEN
bit in the RTC_CTRL register.

#### 20.3.5. Periodic Automatic Wakeup

The periodic wake-up flag is generated by a 16-bit programmable auto-reload down-counter. The range of the wake-up
timer can be extended to 17 bits. The wake-up function is enabled by the WTEN bit in the RTC_CTRL register.

This wake-up input clock is as follows:

•    RTC clock (RTCCLK) divided by 2/ 4/8/16.

Configurable wake-up interrupt period from 122 microseconds to 32 seconds with resolution down to 61
microseconds when RTCCLK is LSE (32.768kHz)

•    ck_spre (typically 1Hz internal clock)

When the ck_spre frequency is 1Hz, the available wake-up time is about 1s to 36h, and the resolution is 1 second.
This larger programmable time range is divided into two parts:

−     1s to 18h when WKUPSEL [2:1] = 10

−     About 18h to 36h when WKUPSEL [2:1] = 11. In the latter case, 216 is added to the 16-bit counter current value.
After completing the initialization sequence, the timer starts counting down. When the wake-up function is
enabled in low-power mode, the down-counting remains active. Also, when the counter reaches 0, the WTF flag of
the RTC_INTSTS register is set and the wakeup register is automatically reloaded with its reload value
(RTC_WKUPT register value). Afterwards the WTF flag must be cleared by software.

The device can exit from low-power modes when the periodic wake-up interrupt is enabled by setting the WTIEN bit in
the RTC_CTRL register.

#### 20.3.6. RTC Register Write Protection

After power-up or reset, all RTC registers except RTC_CTRL, RTC_INITSTS[10:8] are write-protected. Need to write
RTC_CTRL .WRPT=1 first to unlock the write protection.

Writing to the RTC registers is then enabled by writing a key to the write protection register RTC_WRP. The RTC registers
are unlocked by the following steps:

•    Write “0xCA” into RTC_WRP register.

•    Write “0x53” into RTC_WRP register.

Writing the wrong key will reactivate the write protection. However, the FSM only checks writes to this register, while we
can choose to write to other registers. The protection mechanism is not affected by system reset.

#### 20.3.7. Calendar Initialization and Configuration

To program the initial time and date calendar values, including time format and preallocator configuration, the following
sequence is required:

•    Set the INITM bit in the RTC_INITSTS register to enter initialization mode. In this mode, the calendar counter is

stopped and its value can be updated

•    Poll the RTC_INITSTS register bit INITF. The initialization phase mode is entered when INITF is set to

1. Takes approximately 2 RTCCLK clock cycles (due to clock synchronization)


•    To generate a 1 Hz clock reference for the calendar counter, program two prescaler factors in the RTC_PRE register

•    Load initial time and date values in shadow registers (RTC_TSH and RTC_DATE) and configure time format
(12 or 24 hours) via HFMT bits in RTC_CTRL register

•    Exit initialization mode by clearing the INITM bit. It then automatically loads the actual calendar counter
value and restarts counting after 4 RTCCLK clock cycles

After the initialization sequence is complete, the calendar starts counting

#### 20.3.8. Daylight Saving Time Configuration

Daylight saving time management is managed through bits SU1H, AD1H and BAKP of the RTC_CTRL register.
Using the SU1H or AD1H, the software only needs a single operation to decrease or increase an hour in the
calendar without going through the initialization process.

Additionally, software can use the BAKP bit to record whether this operation was ever performed.

#### 20.3.9. Alarm Configuration

Please configure Alarm A in the following order:

•    Clear bit ALxEN in register RTC_CTRL Disable Alarm A

•    Configure Alarm x Registers (RTC_ALRMxSS/RTC_ALARMx)

•    Setting bit ALxEN in register RTC_CTRL to 1 enables alarm x.

> **Note: Due to clock synchronization, each change of the RTC_CTRL register needs to be performed**
approximately 2 RTCCLK clock cycles later.

#### 20.3.10. Wakeup Timer Configuration

The wake-up timer auto-reload values are configured in the following order:

•    Clear register RTC_CTRL bit WTEN to disable wake-up timer

•    Poll bit WTWF in RTC_INITSTS until it is set to 1 to ensure that access to the wake-up auto-reload
timer and WKUPSEL[2:0] bits is allowed. After WTWF is set to 1, it needs to be delayed by about 2
RTCCLK clock cycles (due to clock synchronization)

•    Configure wake-up auto-reload value WKUPT[15:0], wake-up clock selection (WKUPSEL[2:0] bits in
RTC_CTRL). Set bit WTEN in RTC_CTRL to 1 to enable the timer again. The wake-up timer restarts
counting. Due to clock synchronization, the WTWF bit is cleared 2 RTCCLK clock cycles after WTEN is
cleared.

•

#### 20.3.11. Calendar Reading

When the BYPS control bit in register RTC_CTRL is cleared:

For correct reading of the RTC calendar registers (RTC_SUBS, RTC_TSH, and RTC_DATE), the APB clock
frequency (fPCLK) must be equal to or greater than 7 times the RTC clock frequency (fRTCCLK). This ensures
the security of the synchronization mechanism.
If the APB clock frequency is less than 7 times the RTC clock frequency, software must read the calendar time and date
registers twice. If the second read of RTC_TS gives the same result as the first read, you can be sure that the data is correct.
Otherwise a third read access must be performed. In any case, the APB clock frequency cannot be lower than the RTC
clock frequency.

RSYF is set to 1 in the RTC_INITSTS register every time the calendar register data is copied to the RTC_SUBS, RTC_TSH
and RTC_DATE registers. Copying is performed every two RTCCLK cycles. To ensure point-in-time consistency between
the 3 values, reading RTC_SUBS or RTC_TSH will lock the value in the high-order calendar shadow register until
RTC_DATE is read. To prevent software from performing read access to the calendar, the time interval is less than 2
RTCCLK cycles. After reading the calendar for the first time, RSYF must be cleared by software, and software must wait
until RSYF is set to 1 before reading the RTC_SUBS, RTC_TSH and RTC_DATE registers again.

After waking up from low-power mode, RSYF must be cleared by software. Software must then wait until RSYF is set
again before reading the RTC_SUBS, RTC_TSH and RTC_DATE registers. The RSYF bit must be cleared after waking
up and not before entering a low-power mode.

After a system reset, software must wait until RSYF is set to 1 before reading the RTC_SUBS, RTC_TSH, and RTC_DATE
registers. In effect, a system reset resets the shadow registers to their default values.

After initialization, software must wait until RSYF is set before reading the RTC_SUBS, RTC_TSH, and RTC_DATE
registers.

After synchronization, software must wait until RSYF is set before reading the RTC_SUBS, RTC_TSH, and RTC_DATE
registers.

When bit BYPS is set to 1 in the RTC_CTRL register:

Reading the calendar register can get the value directly from the calendar counter, so there is no need to wait for the RSF
bit to be set. This is especially useful after exiting from low-power modes, where shadow registers are not updated. When
the BYPS bit is set to 1, the results for different registers may not be consistent if the RTCCLK edge occurs between two
read accesses to the register. Also, if an RTCCLK edge occurs during a read operation, the value in one of the registers may
be incorrect. Software must read all registers twice, and then compare the two results to confirm that the data is consistent
and correct. In addition, the software can also only compare the lowest bit of the results obtained by reading the calendar
register twice.

> **Note: When BYPS=1, the instruction to read the calendar register requires an extra APB cycle to complete.**

#### 20.3.12. RTC Sub-second Register Shift

The RTC can be synchronized with a high-precision remote clock. After reading the subsecond field (RTC_SUBS or
RTC_TSSS), the precise offset between the remote clock and the time maintained by the RTC can be calculated. The RTC
can then be adjusted to remove this offset by using RTC_SCTRL to shift its clock by a fraction of a second.

RTC_SUBS contains the value of the synchronous prescaler counter. This allows calculation of the precise time maintained
by the RTC with a resolution of 1 / (DIVS + 1) seconds. Therefore, the resolution can be increased by increasing the value
of the synchronous prescaler (DIVS[14:0]). When DIVS is set to 0x7FFF, the maximum resolution (30.52us, clock
frequency is 32768 Hz clock) can be obtained.

However, increasing DIVS means that DIVA must be decreased in order to keep the sync prescaler output at 1 Hz. In this
way, the output frequency of the asynchronous prescaler will increase, which will increase the dynamic power consumption
of the RTC.

The RTC can be fine-tuned using the RTC Shift Control Register (RTC_SCTRL). RTC_SCTRL can be written to at a
resolution of 370 / 400 with a size of 1/(DIVS + 1) seconds, shifting (delaying or advancing) the clock by up to 1 second.
In this shift operation, the SUBF[14:0] value is added to the sub-second register SS[15:0]. This will delay the clock. If the
AD1S bit is set to 1 at the same time, one second will be added and at the same time the time will be subtracted by a fraction
of a second, thus advancing the clock.

Before starting the translation operation, the user must check that SS[15] = 0 to ensure that overflow does not occur.
Whenever a write operation is made to the RTC_SCTRL register, the hardware sets the SHOPF flag, indicating that a
panning operation is pending. This bit is cleared by hardware once the translation operation is complete.

#### 20.3.13. RTC Digital Clock Precision Calibration

The RTC frequency can be digitally calibrated with a resolution of approximately 0.954 ppm and a range of -487.1ppm to
+488.5 ppm. Frequency correction is done through a series of small adjustments (adding and/or subtracting individual
RTCCLK pulses). These trims are very evenly distributed, so the RTC is calibrated reasonably well, even when observed
over a short period of time.

When the input frequency is 32768 Hz, the period of precision digital calibration is about 220 RTCCLK pulses or 32 seconds.
This period is maintained by a 20-bit counter cal_cnt[19:0] clocked by RTCCLK.

The fine calibration register (RTC_CALIB) specifies the number of RTCCLK clock cycles to decrement in a 32-second
period:

•   When CM[0] is set to 1, only one pulse is reduced in a period of 32 seconds

•   When CM[1] is set to 1, 2 cycles will be reduced

•   When CM[2] is set, 4 cycles will be reduced

•   By analogy, until CM[8] is set to 1, 256 cycles will be reduced

> **Note: CM[8:0] (RTC_CALIB) specifies the number of RTCCLK pulses to decrement in a 32-second period. The CM can**
reduce the RTC frequency by up to 487.1ppm while the CP can be used to increase the frequency by 488.5ppm when using
the appropriate resolution. Setting CP to "1" effectively inserts an extra RTCCLK pulse every 211 RTCCLK cycles, which
means an additional 512 clocks per 32-second cycle.

When used with CMs and CPs, an offset of -511 to +512 RTCCLK periods can be added over a 32-second period,
corresponding to a calibration range of -487.1 ppm to +488.5 ppm with a resolution of approximately 0.954 ppm.

If the input frequency (FRTCCLK) is known, the effective calibration frequency (FCAL) can be calculated by the following
formula:

FCAL = FRTCCLK * [1 + (CP * 512 - CM) /(220 + CM – CP * 512)]

When DIVA<3 Calibration:

When the asynchronous prescaler value (DIVA bit in the RTC_PRE register) is less than 3, the CP bit will not be able to
be set. If CP is already set and the value of the DIVA bit is less than 3, then CP is ignored and the calibration operates as if
CP equals 0.

To perform calibration with DIVA less than 3, the synchronous prescaler value (DIVS) should be reduced to speed up 8

RTCCLK clock cycles per second, which means an increase of 256 clock cycles per 32 seconds. Thus, using only the CM
bits, you can effectively add 255 to 256 clock pulses every 32 seconds (corresponding to a calibrated range of 243.3 ppm
to 244.1 ppm).
At a nominal RTCCLK frequency of 32768 Hz, when DIVA is equal to 1 (division factor of 2), DIVS should
be set to 16379 instead of 16383 (4 less). The only other relevant case is when DIVA is equal to 0 and DIVS
should be set to 32759 instead of 32767 (8 less).

If DIVS is reduced in this way, the following formula is used to calculate the effective frequency of the
calibration input clock:

FCAL = FRTCCLK * [1 + (256 - CM) / (220 + CM - 256)]

In this case, if RTCCLK happens to be 32768.00 Hz, then when CM[7:0] equals 0x100 (the middle of the CM
range), the setting is correct.

Verify RTC Calibration:

By measuring the precise frequency of RTCCLK, the correct CM and CP values are calculated to ensure the
accuracy of the RTC.

The precise frequency of the RTC is measured over a finite time interval, which may result in a measurement
error of up to 2 RTCCLK clock periods within the measurement period, depending on how the digital
calibration period is aligned with the measurement period. However, this measurement error can be eliminated if
the measurement period is the same as the calibration period. In this case, the only observed error was that due to
the resolution of the digital calibration.

•   By default, the calibration period is 32 seconds
Using this mode, the accuracy of measuring the 1Hz output within exactly 32 seconds can
guarantee a measurement error value within 0.477 ppm (0.5 RTCCLK period in 32 seconds due
to calibration resolution limitation).
•   Set the CW16 bit of the RTC_CALIB register to force a 16-second calibration period

In this case, the RTC accuracy can be measured within 16 seconds with a maximum error of 0.954 ppm (0.5
RTCCLK period in 16 seconds). However, due to the reduced calibration resolution, the long-term RTC accuracy
is also reduced to 0.954 ppm. When CW16 is set, the CM[0] bit is always 0.

•   Set the CW8 bit of the RTC_CALIB register to force an 8-second calibration period

In this case, the RTC accuracy can be measured within 8 seconds with a maximum error of 1.907 ppm (0.5 RTCCLK
cycles over 8 seconds). Long-term RTC accuracy is also down to 1.907 ppm. When CW8 is set to 1, the CM[1:0]
bits will always remain 00.

Dynamic recalibration:

When bit INITF=0 in register RTC_INITSTS, the calibration register (RTC_CALIB) can be updated
dynamically, using the following procedure:

•   Poll register RTC_INITSTS bit RECPF (recalibration pending flag)

•   If the flag is 0, write new values to RTC_CALIB if necessary. Then set RECPF to 1 automatically

•   The new calibration settings will take effect within three ck_apre cycles after a write to RTC_CALIB

#### 20.3.14. RTC Low Power Mode


Lower Power Mode                                                 Description

No effect
Idle
RTC interrupt can take the chip out of Idle mode

When the RTC clock source is LSE or LSI, the RTC keeps working. RTC alarm, RTC
Standby/Sleep
wake-up will cause the device to exit Standby/Sleep mode.

### 20.4. RTC Registers

> **Note: Due to clock synchronization, changes to RTC registers need to be performed approximately 2 RTCCLK clock**
cycles later.

#### 20.4.1. RTC Register Address Map


*Table 20-1 RTC Register Address Map and Reset Value*

Off
Register
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

RTC_TS

HOT[1:0]
HOU[3:0]                            MIT[2:0]                               MIU[3:0]                                  SCT[2:0]                      SCU[3:0]

APM
00       H

Reserved

Reserved
Reserved
0h     Reset
0     0          0    0     0      0      0                  0           0        0         0          0       0          0                   0        0      0       0        0              0         0
Value

MOT
RTC_DA
YRT[3:0]                    YRU[3:0]                 WDU[2:0]                                          MOU[3:0]                                        DAT[1:0]                DAU[3:0]
00      TE

Reserved
Reserved
4h     Reset
0        0     0          0    0     0      0      0        0         0           1        0         0          0       0          1                            0      0           0 0                 0         1
Value

WKUPSEL[2:0
RTC_CT

ALAIEN

ALAEN
WTIEN

WTEN

WRPT
HFMT
BAKP
SU1H
AD1H

BYPS

]
00      RL                                                                                                                                                                                                                              Reserve

Reserved

Reserved

Reserved

Reserved
Reserved
8h                                                                                                                                                                                                                                           d
Reset
0      0      0                  0                    0                    0                  0       0           0        0                       0              0         0
Value

RTC_INIT

SHOPF

ALAWF
RECPF

INITSF

WTWF
INITM

RSYF
ALAF

INITF
WTF
00      STS
Reserved

Reserved

Reserved
Reserved
Ch
Reset
0                                                            0                  0       0           0        0      0       0        1                        1
Value

RTC_PR
DIVA[6:0]                                                                                                DIVS[14:0]
01       E
Reserved

Reserved
0h     Reset
1     1          1    1     1      1      1                  0           0        0         0          0       0          0       1           1        1      1       1        1              1         1
Value

RTC_WK
WKUPT[15:0]
01      UPT
Reserved
4h     Reset
1         1           1        1         1          1       1          1       1           1        1      1       1        1              1         1
Value

RTC_ALA
WKDSEL
MASK4

MASK3

MASK2

MASK1

DTT[1:0]        DTU[3:0]                      HOT[1:0]              HOU[3:0]                            MIT[2:0]                               MIU[3:0]                                  SET[2:0]                      SEU[3:0]
APM

01      RMA

Ch
Reset
0       0         0    0     0    0    0     0   0        0     0          0    0     0      0      0        0         0           0        0         0          0       0          0       0           0        0      0       0        0              0         0
Value

RTC_WR
PKEY[7:0]
02       P
Reserved
4h     Reset
0           0        0      0       0        0              0         0
Value

02 RTC_SU
Reserved                                                                                                                              SS[15:0]
8h      BS

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set

Reset
0    0     0      0     0    0    0   0   0   0    0     0       0   0   0   0
Value

RTC_SC
AD1S

SUBF[14:0]
02      TRL
Reserved
Ch Reset
0                                                                                        0     0      0     0    0    0   0   0   0    0     0       0   0   0   0
Value

RTC_CA

CW16
CM[8:0]

CW8
CP
03      LIB
Reserved                                                             Reserved
Ch Reset
0   0    0     0       0   0   0   0
Value

RTC_AL
MASKSSB[3:0]                                                                                       SSV[14:0]
04 RMASS
Reserved                                            Reserved
4h     Reset
0    0    0    0                                                  0     0      0     0    0    0   0   0   0    0     0       0   0   0   0
Value

#### 20.4.2. RTC Calendar Time Register (RTC_TSH)

This register is a calendar time shadow register and can only be written in initialization

mode. Address offset: 0x00

Reset value: 0x0000 0000

Bit Field                         Name                    Description

31:23                             Reserved                Always reads as 0.

22                                APM                     AM/PM format。

0：AM format or 24-hour format

1：PM format

21:20                             HOT[1:0]                Describes the hour tens value in BCD format

19:16                             HOU[3:0]                Describes the hour units value in BCD format

Bit Field           Name             Description

15                  Reserved         Always reads as 0.

14:12               MIT[2:0]         Describes the minute tens value in BCD format

11:8                MIU[3:0]         Describes the minute units value in BCD format

7                   Reserved         Always reads as 0.

6:4                 SCT[2:0]         Describes the second tens value in BCD format

3:0                 SCU[3:0]         Describes the second units value in BCD format

#### 20.4.3. RTC Calendar Date Register (RTC_DATE)

This register is a calendar date shadow register and can only be written in initialization

mode. Address offset: 0x04

Reset value: 0x0000 2101

Bit Field               Name              Description

31:24                   Reserved          Always reads as 0.

23:20                   YRT[3:0]          Describes the year tens value in BCD format

19:16                   YRU[3:0]          Describes the year units value in BCD format

15:13                   WDU[2:0]          Describes which Week day

12                      MOT               Describes the month tens value in BCD format

11:8                    MOU[3:0]          Describes the month units value in BCD format

7:6                     Reserved          Always reads as 0.

5:4                     DAT[1:0]          Describes the date tens value in BCD format

3:0                     DAU[3:0]          Describes the date units value in BCD format

#### 20.4.4. RTC Control Register (RTC_CTRL)

> **Note: Due to clock synchronization, each change of the RTC_CTRL register needs to be performed after**
approximately 2 RTCCLK clock cycles.

Address offset: 0x08

Reset value: 0x0000 0000

Bit Field        Name                  Description

31:19            Reserved              Always reads as 0.

18               BAKP                  Daylight saving time record

This bit is written by the user

17               SU1H                  Subtract 1 hour (winter time change)

1 hour will be subtracted to the calendar time when the current hour value is
not 0. This bit is always read as 0.

0: No effect.

1: Subtracts 1 hour to the current time.

16               AD1H                  Add 1 hour (summer time change)

When this bit is set, 1 hour can be added to the calendar time. This bit is
always read as.

0: No effect.

1: Adds 1 hour to the current time.

15               Reserved              Always reads as 0.

14               WTIEN                 Wakeup timer interrupt enable

0: Disable wakeup timer interrupt.

1: Enable wakeup timer interrupt.

13               Reserved              Always reads as 0.

Bit Field   Name           Description

12          ALAIEN         Alarm A interrupt enable

0: Disable Alarm A interrupt

1: Enable Alarm A interrupt

11          Reserved       Always reads as 0.

10          WTEN           Wakeup timer enable

0: Disable wakeup timer

1: Enable wakeup timer

9           Reserved       Always reads as 0.

8           ALAEN          Alarm A enable

0: Disable Alarm A

1: Enable Alarm A

7           WRPT           Unlock the write protection bit.

0: Write protection takes effect

1: Write protection disabled.

6           HFMT           Hour format bit

0: 24 hour format

1: Am/PM format

5           BYPS           Bypass values from the shadow registers

0: Calendar values are copied from the shadow registers, which are refreshed
every two RTCCLK cycles.

1: Calendar values are copied directly from the calendar counters.

> **Note: If the frequency of the APB1 clock falls below seven times the**
frequency of RTCCLK, RTC_CTRL.BYPS bit must be set to ‘1’

4:3         Reserved       Always reads as 0.

2:0         WKUPSEL[2:0]   Wakeup clock selection

000: RTC clock is divided by 16

001: RTC clock is divided by 8

010: RTC clock is divided by 4

Bit Field       Name         Description

011: RTC clock is divided by 2

10x: ck_spre (usually 1Hz) clock is selected

11x: ck_spre (usually 1Hz) clock is selected and 216 is added to the
RTC_WKUPT.WKUPT counter.

#### 20.4.5. RTC Initial Status Register (RTC_INITSTS)

Address offset: 0x0C

Reset value: 0x0000 0005

Bit Field       Name         Description

31:17           Reserved     Always reads as 0.

16              RECPF        Recalibration pending flag

The RECPF status flag is automatically set to ‘1’ when software writes to the
RTC_CALIB register, indicating that the RTC_CALIB register is blocked.
After the new calibration settings are processed, this bit returns to ‘0’.

15:11           Reserved     Always reads as 0.

10              WTF          Wake up timer flag

This flag is set by hardware when the value of wakeup auto-reload counter
reaches 0.

This flag is cleared by software by writing 0.

This flag must be cleared by software at least 1.5 RTCCLK periods before
WTF is set again.

9               Reserved     Always reads as 0.

8               ALAF         Alarm A flag

This flag is set to ‘1’ by hardware when the time/date registers value match
the Alarm A register values.

Bit Field   Name     Description

This flag can be cleared by software writing 0

7           INITM    Enter initialization mode

0: Free running mode

1: Enter initialization mode and set calendar time value, date value, and
prescale value.

6           INITF    Initialization flag

RTC is in initialization state when this bit is ‘1’, and calendar time, date and
prescale value can be updated.

0: Calendar time, date and prescale value can not be updated

1: Calendar time, date and prescale value can be updated

5           RSYF     Register synchronization flag

This flag is set to ‘1’ by hardware when the calendar value are copied into the
shadow registers. This bit is cleared by hardware when in initialization mode,
while a shift operation is pending (SHOPF=1), or when in bypass shadow
register mode (RTC_CTRL.BYPS=1). This bit can also be cleared by software.

It is cleared either by software or by hardware in initialization mode.

0: Calendar shadow register not yet synchronized

1: Calendar shadow register synchronized

4           INITSF   Initialization status flag

This flag is set to ‘1’ by hardware when the calendar year field is different from
0 (which is the RTC domain reset state).

0: Calendar has not been initialized

1: Calendar has been initialized

3           SHOPF    Shift operation pending flag

This flag is set to ‘1’ by hardware as soon as a shift operation is initiated by a
write to the RTC_SCTRL register. It is cleared by hardware when the
corresponding shift operation has been completed, note that writing to the
SHOPF bit has no effect.

0: No shift operation is pending

1: A shift operation is pending

2           WTWF     Wakeup timer write flag

Bit Field        Name         Description

Set the WTEN bit to 0 in RTC_CTRL, after a maximum of 2 RTCCLKs, the
hardware will set this bit to 1. Similarly, after the WTEN bit is set to 1, at
most 2 RTCCLKs, the hardware clears this bit.

When WTEN=0, WTWF=1, the wake-up timer value can be changed.

0: Wakeup timer configuration update is not allowed

1: Wakeup timer configuration update is allowed

1                Reserved     Always reads as 0.

0                ALAWF        Alarm A write flag.

This flag is set to ‘1’ by hardware when Alarm A values can be changed, after
the RTC_CTRL.ALAEN bit has been set to 0.

0: Alarm A update is not allowed

1: Alarm A update is allowed

#### 20.4.6. RTC Prescaler Register (RTC_PRE)

Address offset: 0x10

Reset value: 0x007F 00FF

Bit Field        Name         Description

31:23            Reserved     Always reads as 0.

22:16            DIVA[6:0]    Asynchronous frequency division parameter bit.

The formula is as follows:

ck_apre = RTCCLK/(DIVA+1)

15               Reserved     Always reads as 0.

14:0             DIVS[14:0]   Synchronous prescaler factor

The formula is as follows:

Bit Field        Name          Description

ck_spre = ck_apre/(DIVS+1)

#### 20.4.7. RTC Wakeup Timer Register (RTC_WKUPT)

Address offset: 0x14

Reset value: 0x0000 FFFF

Bit Field        Name          Description

31:16            Reserved      Always reads as 0.

15:0             WKUPT[15:0]   Wake up auto-reload value bit.

When the wake-up timer is enabled (WTEN is set to 1), the WTF flag is set
to 1 every ck_wt period (WKUPT[15:0] + 1).

The ck_wt period is selected by the WKUPSEL[2:0] bits of the RTC_CRTL
register.

> **Note: When WKUPSEL[2] = 1, the wake-up timer becomes 17 bits**
(WKUPSEL[1] is equivalent to WKUPT[16], that is, the most significant bit
to be reloaded into the timer).

The first assertion of WTF occurs (WKUPT +1) ck_wt cycles after the
assertion of WTEN. It is forbidden to set WKUPT [15:0] to 0x0000 when
WKUPSEL [2:0]=011(RTCCLK/2).

#### 20.4.8. RTC Alarm A Register (RTC_ALARMA)

Address offset: 0x1C

Reset value: 0x0000 0000

Bit Field   Name       Description

31          MASK4      Alarm A date mask bits.

0: Alarm A is set if the date/day matches

1: Date/day is irrelevant in alarm A comparison

30          WKDSEL     Week day selection

0: DTU[3:0] represents the date units

1: DTU[3:0] represents week day only. DTT[1:0] is not considered

29:28       DTT[1:0]   Describes the date tens value in BCD format

27:24       DTU[3:0]   Describes the date units value in BCD format

23          MASK3      Alarm hours mask

0: If the hours match, alarm A is set

1: In the alarm clock A comparison, the hour is irrelevant

22          APM        AM/PM notation

0: AM or 24 hours format

1: PM format

21:20       HOT[1:0]   Describes the hour tens value in BCD format

19:16       HOU[3:0]   Describes the hour units value in BCD format

15          MASK2      Alarm minutes mask

0: If the minutes match, alarm A is set

1: Minutes don't matter in alarm clock A comparison

14:12       MIT[2:0]   Describes the minute tens value in BCD format

11:8        MIU[3:0]   Describes the minute units value in BCD format

7           MASK1      Alarm seconds mask

0: If the seconds match, alarm A is set

1: In the alarm A comparison, the seconds are irrelevant

6:4         SET[2:0]   Describes the second tens value in BCD format

3:0         SEU[3:0]   Describes the second units value in BCD format

#### 20.4.9. RTC Write Protection register (RTC_WRP)

Address offset: 0x24

Reset value: 0x0000 0000

Bit Field        Name        Description

31:8             Reserved    Always reads as 0.

7:0              PKEY[7:0]   Write protection key.

This byte can be written to by software.

Reading this byte always returns 0x00.

For detail on how to unlock RTC register write protection, see chapter RTC register
write protection.

#### 20.4.10. RTC Sub-second Register (RTC_SUBS)

Address offset: 0x28

Reset value: 0x0000 0000

Bit Field        Name        Description

31:16            Reserved    Always reads as 0.

15:0             SS[15:0]    Sub-second bit.

SS[15:0] is the synchronous prescaler counter value. This subsecond value can be
derived from the following formula:

Subsecond value = (DIVS - SS) / (DIVS + 1)

> **Note: SS can be greater than DIVS only after performing pan operation. In this case,**
the correct time/date is one second behind the time/date indicated by
RTC_TSH/RTC_DATE.

#### 20.4.11. RTC Shift Control Register (RTC_SCTRL)

Address offset: 0x2C

Reset value: 0x0000 0000

31        30                                                                                                     16

AD1S                                                   Reserved

w
15        14                                                                                                     0

Reserved                                            SUBF[14:0]

w

Bit Field         Name         Description

31                AD1S         Add one second

0: no effect

1: Add one second to the clock/calendar

This bit is write-only and always reads as 0. Writing to this bit has no effect when a
panning operation is pending (SHOPF=1 in RTC_INITSTS).

This function should be used in conjunction with SUBF (see description below) to
efficiently add sub-second values to the clock of the atomic operation mechanism.

30:15             Reserved     Always reads as 0.

14:0              SUBF[14:0]   Subtract a fraction of a second

This bit is write-only and always reads as 0. Writing to this bit has no effect when a
translation operation is pending (SHOPF=1 in RTC_INTSTS).

The value written to SUBF will be added to the synchronous prescaler counter. Since
this counter counts down, this operation effectively subtracts (delays) the following
from the clock:

Delay (seconds) = SUBF / (DIVS + 1)

When the AD1S function is used in conjunction with SUBF, it effectively adds a sub-
second value to the clock (advancing the clock), which advances the clock by:

Advance (seconds) = (1 - (SUBF / (DIVS + 1))).

> **Note: Writing to SUBF will clear RSYF. Software then waits until RSYF=1 to determine**
when the shadow registers have been updated to post-shift.

#### 20.4.12. RTC Calibration Register (RTC_CALIB)

Address offset: 0x3C

Reset value: 0x0000 0000

Bit Field   Name       Description

31:16       Reserved   Always reads as 0.

15          CP         Increase frequency of RTC by 488.5 ppm

This feature should be used in conjunction with CM, which reduces the frequency of
the calendar at high resolutions. If the input frequency is 32768 Hz, the number of
RTCCLK pulses added in the 32-second window is calculated as follows:

(512 * CP) - CM

See RTC Precision Digital Calibration.

0: Do not increase RTCCLK pulse

1: Effectively inserts one RTCCLK pulse every 211 pulses (increases the frequency of
the RTC by 488.5ppm).

14          CW8        Select an 8-second calibration cycle period

0: Not effect.

1: Select an 8-second calibration period.

When CW8 is set to ‘1’, the 8-second calibration cycle period is selected.

> **Note: when CW8 = 1, CM[1:0] will always be' 00'**

13          CW16       To select a 16-second calibration cycle period

0: Not effect.

1: Select a calibration period of 16 seconds. If CW8 = 1, this bit cannot be set to 1.

> **Note: when CW16 = 1, CM[0] will always be '0'**

12:9        Reserved   Always reads as 0.

8:0         CM[8:0]    Negative calibration bits

The frequency of the calendar is reduced by masking CM pulses within 220 RTCCLK
pulses (32 seconds if the input frequency is 32768 Hz). Its resolution is 0.9537 ppm.
To increase the frequency of the calendar, this function should be used in conjunction
with the CP.

#### 20.4.13. RTC Alarm A sub-second register (RTC_ALRMASS)

Address offset: 0x44

Reset value: 0x0000 0000

Bit Field        Name           Description

31:28            Reserved       Always reads as 0.

27:24            MASKSSB[3:0]   Mask the most significant bit from this bits.

0: Subseconds of alarm A are not compared. Set the alarm when the seconds unit is
incremented (assuming the rest of the fields match).

1: In Alarm A comparison, SSV[14:1] are don't care bits. Only SSV[0] is compared.

2: In Alarm A comparison, SSV[14:2] are don't care bits. Only SSV[1:0] are
compared.

3: In Alarm A comparison, SSV[14:3] are don't care bits. Only SSV[2:0] is compared.

...

12: In Alarm A comparison, SSV[14:12] are don't care bits. Compare SSV[11:0].

13: In Alarm A comparison, SSV[14:13] are don't care bits. Compare SSV[12:0].

14: SSV[14] is a don't care bit in Alarm A compare. Compare SSV[13:0].

15: All 15 SSV bits are compared and must all match to activate the alarm.

The overflow bit (bit 15) of the synchronous counter is never compared. This bit is
not 0 only after a translation operation has been performed.

23:15            Reserved       Always reads as 0.

14:0             SSV[14:0]      Sub seconds value

This value is compared with the contents of the synchronous prescaler counter to
determine if Alarm A should be activated. Only bits 0 to MASKSSB-1 are compared.

## 21. Infrared Controller (IRC)


### 21.1. Introduction to IRC

By providing a flexible means, infrared generator supports software configure the infrared protocol signals
commonly used by remote control products.

The IRC module has an effective message queue, in which the user can describe the waveform of a specific
IR protocol with only a few bytes of user-defined commands that are independent of the protocol. See Figure
21-1 Schematic Diagram of IRC Module.

### 21.2. Main Features of IRC

•    Carrier frequency range: 30 kHz~60 kHz

•    Support pulse width coding and pulse space coding

•    Support Manchester coding

•    Support carrier-free mode

•    Support any combination of Mark code and Space code

•    Provide 16 (depth) × 21-bit (width) Code FIFO for storing coded commands

•    Support repeatedly sending commands configured by software

•    Generate interrupt upon the end of transmission

### 21.3. Function Description of IRC

*Figure 21-1 IRC Schematic Diagram of Module*


IR generator is a concept based on describing data in two different ways, and supports any IR protocol:
Irgen_clk
CARRIER GEN

AHB        AHB interface                                       Register file             Carrier frequency
generator (30-60 KHz)

0               IR

1
Digital message
generator
Code fifo (21bits x16 depth)                                                                               From
0                   register file

Code fifo (21bits x16 depth)     Message decoding                                   1
Paint message
generator

INPUT STAGE                                                                                                      MODULATOR       OUTPUT STAGE

•    Digital Message: representing logic 1 or 0. The duration of Mark and Space can be configured.

•    Paint Message: representing a fully customized “paint” waveform. The effective way to describe this is to
encode the symbol type (Mark/Space) and the symbol duration in a codeword so that the hardware can
understand and correctly modulate accordingly.

The combination of commands consists of some control words that contain Digital Message or Paint
Message in an effective way.

An example of NEC command of IR code is presented in the figure below:

*Figure 21-2 Schematic Diagram of Commands Corresponding to NEC Code*


Refer to the following table for the coding of Digital Message and Paint Message:

•    Digital message code:

Bit                       Name                                              Description

[20]                   Message Type             1 = Digital message.

[19:16]                 Message length            Payload effective bits-1

[15:0]                 Message payload           Logic “0” and “1”

•    Paint message code:

Bit                       Name                                              Description

[20]                    Message type            0 = Paint message

[19:15]                     Reserve

1 = Mark
[14]                    Symbol type
0 = Space

[13:0]                     Duration              Mark/Space duration (cycles of carrier clock)

IRC controller consists of input user interface, carrier generator and modulator.

#### 21.3.1. Input User Interface

It consists of APB register interface, two FIFO and message decoding engine.

This sub block is responsible for system configuration, and for the storage and decoding of the coded words to be
resided in the code FIFO.

Repeat FIFO can be used to load special commands. If a key is pressed but not released, for example, the same
command has to be sent repeatedly at a specified interval.

The output of code FIFO is decoded by the message decoding engine and sent to the next sub module
modulator in the form of a command.

This sub module is also responsible for triggering the repetition timer when a key is pressed repeatedly. The
repetition time and the message to be sent vary depending on the protocol.

#### 21.3.2. Carrier Generator

This sub module, which is responsible for generating carrier frequency, has its own gated clock and can generate
carrier clock with a frequency range of 30 - 60khz.

#### 21.3.3. Modulator

The sub module is responsible for generating modulation signal which gates the carrier clock pulse
sequence. The modulator state machine will make selection between Digital or Paint information and control the
gating accordingly. Users can also program the signal reverse output.

### 21.4. IRC Register


#### 21.4.1. 21.4.1 IRC Register Overview


*Table 21-1 IRC Register Overview*

Off
Register

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
set

9
8
7
6
5
4
3
2
1
0
FREQ_CAR
00                                                             Reserved                                                                                                              RREQ_CARRIER_ON
R_ON
0h
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             1

FREQ_CAR
00                                                             Reserved                                                                                                              FREQ_CARRIER_OFF
R_OFF
4h
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             1

LOGIC_ON
00                                                  Reserved                                                           LOGIC_ONE_MARK                                                                    LOGIC_ONE_SPACE
E_TIME
8h
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             1            0             0             0             0           0              0           0             1

LOGIC_ZE
00                                                  Reserved                                                           LOGIC_ZERO_MARK                                                                 LOGIC_ZERO_SPACE
RO_TIME
Ch
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             1            0             0             0             0           0              0           0             1

IR_LOGIC_ON

IR_TX_START
IR_INVERT_O

IR_REO_FIFO
IR_LOGIC_ZE

IR_CODE_FIF
NO_CARR_M

IR_ENABLE
IR_IRQ_EN

Reserved
Reserved

01 IR_CTRL
0h

Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             0
IR_REP_FIFO

IR_CODE_FIF
IR_IRQ_FLG

O_WORDS
IR_BUSY
Reserved

_WRDS

01 IR_STATUS
4h

Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             0

IR_REPAT_
01                                                  Reserved                                                                                           IR_REPEAT_TIME
TIME
8h
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             0

IR_CODE_F
01                                    Reserved                                                                               IR_CODE_FIFO_DATA
IFO
Ch
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             0

IR_REPEAT
02                                    Reserved                                                                               IR_REPEAT_FIFO_DATA
_FIFO
0h
Reset Value 0    0    0    0    0    0    0    0    0    0          0          0    0    0    0    0    0    0     0     0     0           0          0             0            0             0             0             0           0              0           0             0

#### 21.4.2. IRC carrier Clock High-duration Register (FREQ_CARR_ON)

Offset address: 0x00

Reset value: 0x0000 0001

Bit Field       Name               Description

31:11           Reserved           Always read as 0.

10:0            FREQ_CARRIER_ON    Define the high duration of carrier clock (the number of AHB bus clock cycles is not
allowed to be configured as 0)

#### 21.4.3. IRC Carrier Clock Low-duration Register (FREQ_CARR_OFF)

Offset address: 0x04

Reset value: 0x0000 0001

Bit Field       Name                Description

31:11           Reserved            Always read as 0.

10:0            FREQ_CARRIER_OFF    Define the low duration of carrier clock (the number of AHB bus clock cycles is not
allowed to be configured as 0)

#### 21.4.4. IRC Logic 1 Time Configuration Register (LOGIC_ONE_TIME)

Offset address: 0x08

Reset value: 0x0000 0101

31:16           Reserved           Always read as 0.

15:8            LOGIC_ONE_MARK     Logic 1 MARK time (carrier clock cycle)

7:0             LOGIC_ONE_SPACE    Logic 1 SPACE time (carrier clock cycle)

#### 21.4.5. IRC Logic 0 Time Configuration Register (LOGIC_ZERO_TIME)

Offset address: 0x0C

Reset value: 0x0000 0101

Bit Field       Name                Description

31:16           Reserved            Always read as 0.

15:8            LOGIC_ZERO_MARK     Logic 0 MARK time (carrier clock cycle)

7:0             LOGIC_ZERO_SPACE    Logic 0 SPACE time (carrier clock cycle)

#### 21.4.6. IRC Control Register (IR_CTRL)

Offset address: 0x10

Reset value: 0x0000 0000

Bit Field      Name                      Description

31:10          Reserved                  Always read as 0.

9              NO_CARR_MODE              Carrier-free mode selection:

0: Carrier mode

1: Enable carrier-free mode, and signal output has no carrier modulation

8              IR_IRQ_EN                 IRC module interrupt enable selection

0: Disable IRC interrupt

1: Enable IRC interrupt

7              IR_LOGIC_ONE_FORMAT    Logic 1 format

0: MARK information comes before SPACE information

1: SPACE information comes before MARK information

6              IR_LOGIC_ZERO_FORMAT   Logical 0 format

0: MARK information comes before SPACE information

1: SPACE information comes before MARK information

5              IR_INVERT_OUTPUT       Infrared transmission signal reverse output enable

0: Normal output of infrared transmission signal

1: Reverse output of infrared transmission signal

4              Reserved               Always read as 0.

3              IR_TX_START            IR transmission control bit.

0: IR transmission end (automatically clear CODE or REPEAT FIFO)

1: IR transmission start command

> **Note: After writing CODE or REPEAT FIFO data, write 1 to start transmission;**

When the word number of CODE or REPEAT FIFO is 0, write 0 to end
transmission;

2              IR_ENABLE              IR module enable selection

0: The IR module is not enabled, and the internal FIFO pointer is at reset value

1: Start IR module enable

1              IR_REP_FIFO_RESET      REPEAT FIFO clear

0: Do not empty REPEAT FIFO

1: Clear REPEAT FIFO when IR_TX_START is valid

0              IR_CODE_FIFO_RESET     CODE FIFO clear

0: Do not clear the CODE FIFO

1: Clear CODE FIFO when IR_TX_START is valid

#### 21.4.7. IRC Status Register (IR_STATUS)

Offset address: 0x14

Reset value: 0x0000 0000

Bit Field       Name                   Description

31:12           Reserved               Always read as 0.

11              IR_IRQ_FLG             Transmission completion flag

If IR_TX_START in IR_CTRL register is set and both CODE and REPEAT FIFO
are cleared, the bit should be set by hardware.

This bit is cleared by the software.

If the IR_IRQ_EN in IR_CTRL register is set, an interrupt will be generated.

0: Transmission completion not detected.

1: Transmission completion detected.

10              IR_BUSY                IRC busy flag

0: IRC module idle

1: IRC module busy

9:6             IR_REP_FIFO_WORDS      Number of words in REPEAT FIFO

5:0             IR_CODE_FIFO_WORDS     Number of words in CODE FIFO

#### 21.4.8. IRC Repeat Time Register (IR_REPEAT_TIME)

Offset address: 0x18

Reset value: 0x0000 0000

Bit Field       Name                 Description

31:16           Reserved             Always read as 0.

15:0            IR_REPEAT_TIME      Define the repetition time (number of carrier clock cycles).
Repetition timer starts counting from the protocol frame header and stops after the
defined repetition time.

#### 21.4.9. IRC CODE FIFO Data Register (IR_CODE_FIFO)

Offset address: 0x1C

Reset value: 0x0000 0000

Bit Field       Name                Description

31:21           Reserved            Always read as 0.

20:0            IR_CODE_FIFO_DATA   Write data in CODE FIFO

#### 21.4.10. IRC REPEAT FIFO Data Register (IR_REPEAT_FIFO)

Offset address: 0x20

Reset value: 0x0000 0000

Bit Field       Name                      Description

31:21           Reserved                  Always read as 0.

20:0            IR_REPEAT_FIFO_DATA       Write data in REPEAT FIFO

## 22. Key Detection (KEYSCAN)


### 22.1. Introduction to KEYSCAN

As a common IO interaction module, KEYSCAN module is mainly used in remote control and other scenarios
where multiple buttons are required.

### 22.2. Main Features of KEYSCAN

Mounted on the APB bus, KEYSCAN supports automatic scanning or low-power scanning during system sleep/non-sleep.

•       Support 8/10/13 IO ports respectively corresponding to 44/65/104 keys.

•       Button debounce, and configurable time.

•       Support three modes: automatic scanning, software scanning and low-power scanning.

−     Automatic mode: it is able to configure an automatic scanning mode that allows for start at a fixed time interval.

−     Low power consumption mode: a canning mode that detects the key in the red box below to start a round
for a total of three times

−     Software mode: scanning mode triggered by software.

•       Support key interrupt and flag bit.

•       All keys includes matrix keyboard area and independent keyboard area.

•       Each column of independent key area only supports press one key. If, at this point, there is a key press
in the independent key area, the operation on the matrix keyboard area will be considered invalid. If no key
press occurs in the independent key area, the key detection module supports press one or more keys
without electrical direct connection attribute in the area.

### 22.3. Function Description of KEYSCAN

After the KEYSCAN function is enabled, a total of 13 IOs, including PA0/PA1/PA2/PA3/PA6 and
PB10/PB8/PB9/PA4/PA5/PB0/PB1/PB2, can be used for key scanning. KEYSCAN supports at most 104 keys,
including 78 keys at matrix keyboard area and 26 keys at independent keyboard area.

KEYSCAN will support 44 keys, including 28 keys at matrix keyboard area and 16 keys at independent
keyboard area, when 8 IO keys are configured for scanning. PA4/PA5/PB0/PB1/PB2 are not used as keys.

KEYSCAN will support 65 keys, including 45 keys at matrix keyboard area and 20 keys at independent
keyboard area, when 10 IO keys are configured for scanning. PB0/PB1/PB2 are not used as keys.

#### 22.3.1. KEYSCAN Function Description

After enabling the KEYSCAN function, PA0/PA1/PA2/PA3/PA6 and PB10/PB8/PB9/PA4/PA5/PB0/PB1/PB2,
a total of 13 IOs, can be used for key scanning, supporting up to 104 keys. This includes 78 keys in the
matrix keyboard area and 26 keys in the independent keyboard area.

When configured for 8 IO key scanning, it supports 44 keys, including 28 keys in the matrix keyboard area
and 16 keys in the independent keyboard area. PA4/PA5/PB0/PB1/PB2 are not used as keys.

When configured for 10 IO key scanning, it supports 65 keys, including 45 keys in the matrix keyboard area
and 20 keys in the independent keyboard area. PB0/PB1/PB2 are not used as keys.

When configured for 13 IO key scanning, it supports 104 keys, including 78 keys in the matrix keyboard area
and 26 keys in the independent keyboard area.

*Figure 22-1 Configurable IOs- Wakeup Key Area in Low Power Consumption Mode*


VCC

0R

100R
PA0

100R
PA1

100R
PA2

100R
PA3

100R
PA6

100R
PB10

100R
PB8

100R
PB9

3K

*Figure 22-2 Configurable IOs-Wakeup Key Area in Low Power ConsumptionMode*


VCC

0R

100R
PA0

100R
PA1

100R
PA2

100R
PA3

100R
PA6

100R
PB10

100R
PB8

100R
PB9

100R
PA4

100R
PA5

3K

*Figure 22-3 Configurable IOs- Wakeup Key Area in Low Power Consumption Mode*

VCC

0R

100R
PA0

100R
PA1

100R
PA2

100R
PA3

100R
PA6

100R
PB10

100R
PB8

100R
PB9

100R
PA4

100R
PA5

100R
PB0

100R
PB1

100R
PB2

3K

*Table 22-1 Key Information Sheet of KEYSCAN*


RE    BIT                                                        KEY_ID
G

KE    [15:                   KPB2   KPB1   KPB0   KPA5     KPA4     KPB9   KPB8   KPB1     KPA6   KPA3        KPA2   KPA1   KPA0
Y      0]                    L      L      L      L                 L      L      0L       L      L           L      L      L
L
INF
O0    [31:                   KPB2   KPB1   KPB0   KPA5     KPA4     KPB9   KPB8   KPB1     KPA6   KPA3        KPA2   KPA1   KPA0
16]                    H      H      H      H                 H      H      0H       H      H           H      H      H
H

KE    [15:   KPB1     KPB1   KPB1   KPB1   KPB1   KPA6     KPA6     KPA6   KPA6   KPA3     KPA3   KPA3        KPA2   KPA2   KPA1
Y      0]    0PB6     0PA3   0PA2   0PA1   0PA0   PA3      PA2      PA1    PA0    PA2      PA1    PA0         PA1    PA0
PA0
INF
O1    [31:                   KPB9   KPB9   KPB9   KPB9     KPB9     KPB9   KPB9   KPB8     KPB8   KPB8        KPB8   KPB8   KPB8
16]                    PB8    PB10   PA6    PA3      PA2      PA1           PB10     PA6    PA3         PA2    PA1    PA0
PA0

KE    [15:                                                          KPA4   KPA4   KPA4     KPA4   KPA4        KPA4   KPA4   KPA4
Y      0]                                                           PB9    PB8    PB10     PA6    PA3         PA2    PA1    PA0

INF   [31:                                                 KPA5     KPA5   KPA5   KPA5     KPA5   KPA5        KPA5   KPA5   KPA5
O2
16]                                                  PA4      PB9    PB8    PB10     PA6    PA3         PA2    PA1    PA0

KE         [15:                                                                                         KPB0             KPB0       KPB0              KPB0                    KPB0            KPB0              KPB0            KPB0         KPB0             KPB0
Y           0]                                                                                         PA5              PA4        PB9               PB8                     PB10            PA6               PA3             PA2          PA1              PA0

INF        [31:                                                                               KPB1      KPB1             KPB1       KPB1              KPB1                    KPB1            KPB1              KPB1            KPB1         KPB1             KPB1
O3
16]                                                                                PB0       PA5              PA4        PB9               PB8                     PB10            PA6               PA3             PA2          PA1              PA0

KE         [15:                                                        KPB2                   KPB2      KPB2             KPB2       KPB2              KPB2                    KPB2            KPB2              KPB2            KPB2         KPB2             KPB2
Y           0]                                                        PB1                    PB0       PA5              PA4        PB9               PB8                     PB10            PA6               PA3             PA2          PA1              PA0

INF
[31:
O4
16]

### 22.4. Description of KEYSCAN register


#### 22.4.1. KEYSCAN Register Overview

*Table 22-2 KEYSCAN Register Overview*


Off Regist
31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set    er
KEY_INFO_CL
SOFT_START

KEYSCAN_M
KEY_INTEN

KEY_MASK
KEYC

KEY_WTS

KEY_DTS
Reserved

Reserved

Reserved
KEY_IRP

KEY_EN
ODE
00     R

0h
Reset
0        0      0    0           0    0    0    0     0          0          0     0   0          0    0    0     0         0      0             0           0          0    0      0        0        0       0      0    0          0      0          0
Value

KEYD
Reserved                                              KEYINFO1                                                    Reserved                                                                  KEYINFO0
00 ATA0
4h Reset
0        0      0    0           0    0    0    0     0          0          0     0   0          0    0    0     0         0       0            0           0          0    0       0       0        0       0      0    0          0      0          0
Value
KEYINFO3

KEYINFO2

KEYD
Reserved

Reserved

00 ATA1
8h
Reset
0        0      0    0           0    0    0    0     0          0          0     0   0          0    0    0     0         0       0            0           0          0    0       0       0        0       0      0    0          0      0          0
Value

KEYD
Reserved                                                    KEYINFO5                                                           Reserved                                                            KEYINFO4
00 ATA2
Ch Reset
0        0      0    0           0    0    0    0     0          0          0     0   0          0    0    0     0         0       0            0           0          0    0       0       0        0       0      0    0          0      0          0
Value

01 KEYD
Reserved                                              KEYINFO7                                                       Reserved                                                                 KEYINFO6
0h ATA3

Off Regist

31
30
29
28
27
26
25
24
23
22
21
20
19
18
17
16
15
14
13
12
11
10
9
8
7
6
5
4
3
2
1
0
set    er

Reset
0    0    0     0   0    0    0    0    0     0   0     0    0    0    0    0    0    0    0    0    0    0    0   0   0    0   0     0   0   0   0   0
Value

KEYD
Reserved                                                                      KEYINFO8
01 ATA4
4h Reset
0    0    0     0   0    0    0    0    0     0   0     0    0    0    0    0    0    0    0    0    0    0    0   0   0    0   0     0   0   0   0   0
Value

#### 22.4.2. KEYSCAN Control Register (KEYCR)

Offset address: 0x00

Reset value: 0x0000 0000

Bit Field                   Name                            Description

31:24                       Reserved                        Always read as 0.

23                          KEY_IRP                         Keyboard detection interrupt status bit, write 1 and clear

0: Non interrupt

1: Key press and the existence of interrupt is detected

22                          KEY_INTEN                       Interrupt enable bit

0: Disable key interrupt

1: Enable key interrupt

21:14                       Reserved                        Always read as 0.

13                          KEY_INFO_CLR                    KEYDATA information area clearing

0: Do not clear any KEYDATA information

1: Clear all KEYDATA information

12                          SOFT_START                      Software mode enable signal

0: Do not start software mode scanning

Bit Field   Name           Description

1: Start software mode scan

11:10       KEY_MASK       IO selection configuration

00:13 IO, 104 keys at most

01: 8 IO, 44 keys at most

10: 10 IO, 65 keys at most

Default: 13 IO, 104 keys at most

9:7         KEY_WTS        Time interval of each round of keyboard scanning

000:0ms

001:32ms

010:64ms

011:96ms

100:128ms

101:160ms

110:192ms

111:224ms

Default:0ms

6:4         KEY_DTS        Debouncing time selection

000:10ms

001:20ms

010:40ms

011:80ms

100:160ms

101:320ms

110:640ms

Default:10ms

3:2         KEYSCAN_MODE   Keyboard scanning mode selection

00: Automatic mode

01: Software mode

10: Low power consumption mode

Bit Field       Name        Description

Default: Automatic mode

1               Reserved    Always read as 0.

0               KEY_EN      Enable/disable keyboard scanning module

0: Disable keyboard scanning

1: Enable keyboard scanning module

#### 22.4.3. KEYSCAN INFO Register 0 (KEYDATA0)

Offset address: 0x04

Reset value: 0x0000 0000

Bit Field       Name        Decription

31:29           Reserved    Always read as 0.

28:16           KEY_INFO1   Information on the first line of keys in the independent key area

15:13           Reserved    Always read as 0.

12:0            KEY_INFO0   Information on the second line of keys in the independent key area

#### 22.4.4. KEYSCAN INFO Register 1 (KEYDATA1)

Offset address: 0x08

Reset value: 0x0000 0000

Bit Field       Name        Description

31:29           Reserved    Always read as 0.

28:16           KEY_INFO3   Information on the keys from sixth line (PB8) to the seventh line (PB9) in the matrix
keyboard area

15              Reserved    Always read as 0.

14:0            KEY_INFO2   Information on the keys from first line (PA1) to the fifth line (PB10) in the matrix
keyboard area

#### 22.4.5. KEYSCAN INFO Register 2 (KEYDATA2)

Offset address: 0x0C

Reset value: 0x0000 0000

Bit Field       Name        Description

31:25           Reserved    Always read as 0.

24:16           KEY_INFO5   Information on the keys in the ninth line (PA5) in the matrix keyboard area

15:8            Reserved    Always read as 0.

7:0             KEY_INFO4   Information on the keys in the eighth line (PA4) in the matrix keyboard area

#### 22.4.6. KEYSCAN INFO Register 3 (KEYDATA3)

Offset address: 0x10

Reset value: 0x0000 0000

Bit Field       Name        Description

31:27           Reserved    Always read as 0.

26:16           KEY_INFO7   Information on the keys in the eleventh line (PB1) in the matrix keyboard area

15:10               Reserved            Always read as 0.

9:0                 KEY_INFO6           Information on the keys in the tenth line (PB0) in the matrix keyboard area

#### 22.4.7. KEYSCAN INFO Register 4 (KEYDATA4)

Offset address: 0x14

Reset value: 0x0000 0000

Bit Field           Name                Description

31:12               Reserved            Always read as 0.

11:0                KEY_INFO8           Information on the keys in the twelfth line (PB2) in the matrix keyboard area

For the detailed definition of the KEYSCAN INFO register, refer to the KEYSCAN key information table.

This register is readable, and the content of the INFO register is cleared by writing 1 to bit13 of the control
register, and the bit corresponding to the detected key is updated after each key scan.

## 23. Debug Support (DBG)


### 23.1. Overview

N32WB03x uses Cortex®-M0 core, which integrates hardware debugging module. Support instruction
breakpoint (stop when instruction fetches value) and data breakpoint (stop when data access). When the core is
stopped, the user can view the internal state of the core and the external state of the system. After the user's query
operation is completed, the core and peripherals can be restored, and the corresponding program can continue to
be executed.

The hardware debugging module of the N32WB03x core can be used when it is connected to the debugger (when
it is not disabled).

N32WB03x supports the following debugging interfaces:

•         Serial interface

*Figure 23-1 N32WB031x Level and Cortex®-M0 Level Debugging Block Diagram*


BPU

DWT

SWDIO                                   Internal dedicated
peripheral bus（PPB）                NVIC

SW-DP                 AHB-AP
MCU
Bridges
DBG

SWCLK

Cortex-M0                                System bus
Core
Data

Bus matrix

Cortex-M0 debug support

The ARM Cortex®-M0 core hardware debugging module can provide the following debugging functions:

•   SW-DP: Serial debugging port

•   AHP-AP: AHB access port

•   BPU: Breakpoint generation

•   DWT:

Data trigger

Reference:

•    Cortex®-M0 Technical Reference Manual (TRM)

•    ARM debug interface V5 structure specification

•    ARM Core Sight development tool set (r1p0 version) technical reference manual

### 23.2. SWD Function

The debugging tool can call the debugging function through the above-mentioned SWD debugging interface.

#### 23.2.1. Pin assignment

SWD (serial debug) interface consists of two pins: SWCLK (clock pin) and SWDIO (data input and output pin) to
provide two-pin interface.
Refer to the GPIO chapter for the pin assignment of the SW debugging interface.

## 24. Version History


Version   Date         Changes

V1.3      2022.09.19   Initial release

## 25. Disclaimer


This document is the exclusive property of NSING TECHNOLOGIES PTE. LTD. (Hereinafter
referred to as NSING SG). This document, and the product of NSING SG described herein
(Hereinafter referred to as the Product) are owned by NSING SG under the laws and treaties of
Republic of Singapore and other applicable jurisdictions worldwide. The intellectual properties
of the product belong to NSING TECHNOLOGIES INC. (Hereinafter referred to as NSING) and
NSING TECHNOLOGIES INC. does not grant any third party any license under its patents,
copyrights, trademarks, or other intellectual property rights. Names and brands of third party may
be mentioned or referred thereto (if any) for identification purposes only. NSING SG reserves the
right to make changes, corrections, enhancements, modifications, and improvements to this
document at any time without notice. Please contact NSING SG and obtain the latest version of
this document before placing orders. Although NSING has attempted to provide accurate and
reliable information, NSING assumes no responsibility for the accuracy and reliability of this
document. It is the responsibility of the user of this document to properly design, program, and test
the functionality and safety of any application made of this information and any resulting
product. In no event shall NSING be liable for any direct, indirect, incidental, special, exemplary,
or consequential damages arising in any way out of the use of this document or the Product.
NSING Products are neither intended nor warranted for usage in systems or equipment, any
malfunction or failure of which may cause loss of human life, bodily injury or severe property
damage. Such applications are deemed, Insecure Usage’. Insecure usage includes, but is not limited
to: equipment for surgical implementation, atomic energy control instruments, airplane or
spaceship instruments, all types of safety devices, and other applications intended to supporter
sustain life. All Insecure Usage shall be made at user's risk. User shall indemnify NSING and
hold NSING harmless from and against all claims, costs, damages, and other liabilities, arising
from or related to any customer's Insecure Usage Any express or implied warranty with regard to
this document or the Product, including, but not limited to. The warranties of merchantability,
fitness for a particular purpose and non-infringement are disclaimed to the fullest extent permitted
by law. Unless otherwise explicitly permitted by NSING, anyone may not use, duplicate, modify,
transcribe or otherwise distribute this document for any purposes, in whole or in part.
