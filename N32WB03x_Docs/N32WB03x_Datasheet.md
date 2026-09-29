# N32WB03x Series Datasheet

**Bluetooth® Low Energy SoC**

**Manufacturer:** NSING Technologies Pte. Ltd. (nsing.com.sg)

---


N32WB03x Series Bluetooth® Low Energy SoC

Datasheet
N32WB03x series adopts a 32-bit ARM Cortex-M0 core with a maximum operating frequency of 64MHz. It
supports BLE 5.1 and SIG Mesh, has a transmit current of 4.2A, a receive current of 3.8Ma, a maximum
transmit power of +6 dBm, and a receive sensitivity of -96 dBm @BLE 1 Mbps.

Key Features
•   CPU core
−    32-bit ARM Cortex-M0 core
−    Maximum frequency of 64 MHz
•   Memories
−    256K/512K bytes Flash
−    48K bytes SRAM
•   Power consumption
−    Receive current: 3.8 mA@3.3V
−    Transmit current: 4.2 mA @0dBm/3.3V
−    Sleep mode (48 KB RAM retained): 1.4 μA@3V
−    PD mode: 130nA
•   RF specification
−    RX sensitivity: -96 dBm @BLE 1Mbps
−    RX sensitivity: -93 dBm @BLE 2Mbps
−    Programmable transmit power, up to +6dBm
−    Single-ended antenna interface
•   Clock
−    HSE: 32 MHz high speed external crystal
−    LSE: 32.768 kHz low speed external crystal
−    HSI: high speed internal RC 64 MHz
−    LSI: low speed internal RC 32 kHz
−    Support one clock output; different clock output can be configured; clock can be output after divided by four.
•   Reset
−    Power-on/off/external pin reset
−    Watchdog reset.
•   Communications interfaces
−    2× USART interfaces, with rate up to 4 Mbps (configurable as ISO7816, IrDA, LIN)
−    1× LPUART interface, featuring low-power dissipation, supporting communication rate up to 9,600 bps and
low-power wakeup in Sleep mode.
−    2× SPI interfaces, with rates up to 16 MHz, master/slave configurable, supporting I2S.
−    1× I2C interface, with rate up to 1 MHz, master/slave configurable
•   Timers
−    1× 16-bit advanced counter, supporting functions like input capture, output compare, PWM output, and
quadrature encoder input; 4 independent channels, 3 of which support 6 complementary PWM outputs.
−    1× 16-bit general-purpose counter, supporting functions like input capture, output compare, PWM output, and
mono-pulse output, with 4 independent channels.

−    1× 16-bit basic counter
−    1× 24-bit system timer
−    1× 7-bit window watchdog (WWDG)
−    1× 12-bit independent watchdog (IWDG)
•   Analog interface
−    1× 10-bit 1.33 Msps ADC (configurable as 16-bit 16 Ksps), supporting 5 external single-ended channels, 1
differential MIC channel, 2 internal channels.
−    Built-in PGA up to 128x
−    MIC BIAS voltage, adjustable between 1.6 V and 2.3 V
•   21× GPIO, supporting multiplexing.
•   1× high speed 5-channel DMA controller
•   1× IR transmission controller, supporting all infrared remote-control protocols.
•   1× KEYSCAN module, where 8/10/13 GPIOs support 44/65/104 key functions respectively
•   RTC real-time clock, supporting perpetual calendar (that can identify leap years), alarm events, and periodic
wakeup.
•   Support hardware CRC16 and CRC32 operations.
•   Operating conditions
−    Operating voltage: 1.8V/2.32 V~3.6 V
−    Operating temperature: -40°C~85°C
−    ESD: ±2 KV (HBM)
•   Package
−    QFN32 (4 mm × 4 mm)
•   Ordering Information

Reference           Part Number

N32WB031KCQ6-1
N32WB03x
N32WB031KEQ6-2


## 1. Overview


## 2. Features Overview


## 3. Definition and Description


## 4. Electrical Characteristics


## 5. Package Size


## 6. Version History

7.   Disclaimer                                                                                              61


N32WB03x series Bluetooth® Low Energy SoC is Nsing high performance, ultra-low-power consumption Bluetooth 5.1
chips, featuring a 32-bit ARM Cortex®-M0 core with a maximum operating frequency of 64 MHz, it integrates 48 KB of
SRAM and 256/512 KB of Flash memory on-chip.

Integrated with an advanced BLE 5.1 RF transceiver, it is compliant with the BLE 5.1 standard and provided with multiple
modes including standard 1Mbps BLE mode, enhanced 2Mbps BLE mode, 125 kbps BLE remote mode (S8), and 500 kbps
BLE remote mode (S2). In the 1Mbps or 2Mbps BLE mode, it supports AoA (Angle of Arrival) and AoD (Angle of
Departure), RSSI (Received Signal Strength Indication), master/slave role, multi-connection, data packet length extension,
KEYSCAN, IRC, 10-bit 1.33 Msps ADC (configurable as 16-bit 16 Ksps), analog MIC input, PGA, basic, general, and
advanced timers, RTC, WWDG, IWDG, LPUART, USART, SPI, I2C, and other peripherals.

It is suitable for various application scenarios such as Bluetooth KEY, OBU, data transmission module, Bluetooth voice
remote controller, and smart home devices, and more.

*Figure 1-1 shows the block diagram of N32WB03x series.*


*Figure 1-1 Block Diagram of N32WB03x Series*


256/512KB

SRAM
Fmax:64MHz                 Core

BLE Subsystem

BASEBAND
MODEM      Radio
DMA

AFIO
APB1 Max:32MHz
APB2 Max:64MHz

AFIO

USART 2


### 1.1 Naming Convention

*Figure 1-2 Structure of N32WB03x Series Ordering Code*

N   32   WB     0    31    K        E   Q     6   - 2
Company
abbreviation
NSING Tech

MCU bit width                                                                  Power supply
32 bit                                                                            1 = 1.8~3.6V
2 = 2.32~3.6V
Bluetooth chip
Bluetooth                                                                 Temperature range
MCU core                                                                           6 = -40~85
7 = -40~105
0 = ARM Cortex-M0
Encapsulation type
Product series
Q = QFN
31 = Value line                                                                     A = ESOP

Number of pins                                                                      Capacity
W = 16 Pins                                                                            0 = 0K
K = 32 Pins                                                                          4 = 16K
C = 256K
E = 512K


### 1.2 Product Configurations

*Table 1-1 Resource Configuration of N32WB03x Series*


Model                           N32WB031KEQ6-2                          N32WB031KCQ6-1
Bluetooth Protocol                                               BLE5.1
Flash Capacity (KB)                            512                                       256
SRAM Capacity (KB)                                                   48
CPU Frequency                                        ARM Cortex-M0 @64MHz
Operating Environment                   2.32 V~3.6 V/-40~85°C                    1.8 V~3.6 V/-40~85°C
General                                        1 (TIM3)
Advanced                                       1 (TIM1)
Timer
Basic                                         1 (TIM6)
RTC                                           1 (RTC)
SPI                                        2 (SPI1, SPI2)
I2S                                        2 (I2S1, I2S2)
Communication
I2C                                           1 (I2C)
Interface
USART                                   2 (USART1, USART2)
LPUART                                       1 (LPUART)
GPIO                                                     21
DMA (channels)                                                  1(5)
10-bit ADC (channels)                                               1(8)
KEYSCAN                                     Supports 8/10/13 GPIOs for 44/65/104 keys
IRC                                                       1
CRC                                                                   CRC16/CRC32
Package                                                          QFN32 (4 mm × 4 mm)


### 2.1 Processor Core

The N32WB03x series integrates an ARM Cortex®-M0 processor.


### 2.2 Memory

*Figure 2-1 Memory Mapping*


Reserved
BLE baseband
CRC
NVIC/ SCS                                         MODEM
BPU                                               IRC
RCC

AHB
DWT
ADC
DMA

Reserved                                        SPI2_I2S2
AHB Peripheral                                    USART1
Reserved                                          TIM1
APB 2 Peripheral                                 SPI1_I2S1

APB2
Reserved                                        KEYSCAN
APB 1 Peripheral                                   GPIOB
GPIOA
EXTI
AFIO

PWR
I2C
LPUART
USART2
APB1

IWDG
WWDG
RTC
FLASH                                             TIM6
TIM3


#### 2.2.1 Flash

256k or 512K bytes of Flash, with a maximum program memory of 256k bytes, and the remaining space for
data storage.


#### 2.2.2 SRAM

48K bytes of SRAM, which can be fully retained in Sleep mode.


### 2.3 Low Power Modes

N32WB03x series supports four low power modes.
•    Idle Mode
Only the CPU stops running. All peripherals are working and can wake up the CPU in the event of any interruption
or event.
•    Standby Mode
The power supply works as usual. The CORE power domain is turned off. The BLE is available.
•    Sleep Mode
The high-speed clock is switched off. The power supply runs in the low power mode. The CORE power domain
and BLE are turned off.
•    PD Mode
All systems are shut down. Only WAKEUP IO and NRST can be woken up.


### 2.4 Clock System

Two high speed clocks:
•    HSI oscillator clock (64 MHz)
•    HSE crystal or ceramic resonator clock (32 MHz)
Two secondary clock sources:
•    LSI oscillator clock (32 KHz)
•    LSE crystal or ceramic resonator clock (32.768 KHz)
After the system is powered on and reset, HSI and HSE are enabled, and the system clock is set to HSI by default. LSI can
be used to drive the IWDG. Both LSI and LSE can selectively drive RTC, KEYSCAN and LPUART through a program.
Moreover, LSI/LSE can automatically wake up the system in the Idle/Standby/Sleep/PD mode. If not in use, either clock
source can be turned on or off independently to optimize system power dissipation.
*Figure 2-2 Clock Tree*


Clock Tree
Legend:
HSE = High-speed external clock signal
HSI = High-speed internal clock signal
LSE = Low-speed external clock signal
LSI = Low-speed internal clock signal

/2         BLE_CLK

ADC1MSEL
HSE
/8
ADC_CLK_16K
/64                      /4
PLL                 ADC1_CLK             ADC_CLK_64K
4.096M
I2S_CLK

SCLKSW
FCLK 核时钟
HCLK
XO32MM_OUT               HSE OSC                                 HSE                                                                             CPU AHB BUS
32MHz
XO32MP_IN                                                                                                                     /8               SysTick
SYSCLK              AHB     HCLK
Prescaler
64MHz MAX           /1/2/4
HSI RC                                                                                                                 DMA_CLK/CRC_CLK
64MHz
HSI
APB1
Prescaler       32MHz MAX          PCLK1 to
/1/2/4/8/16                       APB1 peripherals

XO32KM_OUT                 LSE OSC                               RTC_CLK
XO32KP_IN               32.768KHz     LSE                      PWR_LSX_CLK
TIM 3/6
BLE_LSX_CLK                                    If(APB1 Prescaler = 1) x1         TIM3/6_CLK
KEYSCAN_LSX_CLK                                                  else x2
LSXSEL
LSI

LSI RC                                                                               APB2
32KHz                            LPUART_CLK                                                          64MHz MAX         PCLK2 to
Prescaler
/1/2/4/8/16                       APB2 peripherals
LPUART_PCLK1

TIM 1
HSI                                                      If(APB2 Prescaler = 1) x1
HSE                                                                           else x2      TIM1_CLK
SYSCLK                                                          SYSCLK

MCO                    /4
LSI
LSE                                                            SYSCLK
HCLK
TIMCLKSEL
MCO


### 2.5 General Purpose Input/Output (GPIO)

GPIO stands for general purpose input/output. AFIO stands for alternate-function input/output. A chip supports up to 21
GPIOs, which are divided into 2 groups (GPIOA/GPIOB). GPIOA has 7 ports and GPIOB has 14 ports. GPIO ports share
pins with other multiplexed peripheral functions and can be flexibly configured as required. Each GPIO pin can be
independently configured as an output, input, or multiplexed peripheral functional port, as well as its high current drive
capability.
Main features:
•     GPIO ports can be configured through software to the following modes:

−      Input floating
−      Input pull-up
−      Input pull-down
−      Analog function
−      Output open-drain
−      Output push-pull
−      Alternate function push-pull
−      Alternate function open-drain

•       Separate bit setting or bit clearing function
•       All I/O supports external interrupt
•       All I/Os can be woken up from low power modes, with configurable rising or falling edge
•       Eight EXTIs can wake up the Sleep mode and all I/O can be multiplexed as EXTI
•       PB3 can wake up I/O from the PD mode
•       Support remapping of AFIO through software
•       Support GPIO locking mechanism and removal of locking status through resetting
•       Each I/O port register bit is freely programmable, but it must be accessed as 32-bit words (16-bit half-word or 8-bit
byte access is not allowed).


### 2.6 External Interrupt/Event Controller (EXTI)

The EXTI consists of 14 edge detectors for generating interrupt/event requests. Each interrupt line can be independently
configured as an event or interrupt and the corresponding trigger event (rising edge or falling edge or both). Each line can
also be masked independently. A pending register maintains the status line of the interrupt request. This request can be
cleared by writing a '1' in the corresponding bit of the pending register.


### 2.7 Direct Memory Access (DMA)

DMA is integrated with one 5-channel DMA controller to manage data transfer from memory to memory, from peripheral
to memory and from memory to peripheral.
Each channel has a dedicated hardware DMA request logic and can be triggered by software. The transfer length, source
address and destination address of each channel can be set separately by software.
DMA is applicable to main peripherals including SPI, I2S, I2C, USART, ADC, and basic, general, and advanced control
TIMx.


### 2.6 Cyclic Redundancy Check Calculation Unit (CRC)

It integrates CRC32 and CRC16 functions. The Cycle Redundancy Check (CRC) calculation unit generates any CRC
calculation result based on a fixed generator polynomial. In many applications, CRC-based techniques are used to verify
the consistency of data transmission or storage. Within the scope specified in EN/IEC 60335-1 standard, it provides a
means to detect flash memory errors. The CRC calculation unit can be used to calculate the signature of software in real-
time and compare it with the signature generated during the linking and generation of the software.
Main features:
•       CRC16: supports the polynomial X16+X15+X2+X0
•       CRC16 computing time: one AHB clock period (HCLK)
•       CRC32: supports the polynomial X32 + X26 + X23 + X22 + X16 + X12 + X11 + X10 + X8 + X7 + X5 + X4 + X2 + X +1
•       CRC32 computing time: four AHB clock periods (HCLK)
•       Configurable CRC initial value

•       Support DMA mode


### 2.9 Timer and Watchdog

It includes 1× advanced timer, 1× general-purpose timer, 1× basic timer, 2× watchdog timer, and 1× system tick timer.


#### 2.9.1 Basic Timer (TIM6)

The basic timer (TIM6) contains a 16-bit auto-loading counter driven by a programmable prescaler. It can provide a time
baseline for general-purpose timers.
Main features:
•       16-bit automatic reloading accumulating counter
•       16-bit programmable (can be modified in real-time) prescaler, used to divide the input clock by any value between
1 and 65536
•       Generate an interrupt/DMA request when an event is updated (counter overflow)


#### 2.9.2 General-purpose Timer (TIM3)

The General Purpose Timer (TIM3) contains a 16-bit auto-load up/downcounter, a 16-bit prescaler, and 4 independent
channels. Each channel can be used for input capture (to measure pulse width), output compare, PWM, and single-pulse
output.
Main features:
•       16-bit up, down, or up/down auto-loading counter
•       16-bit programmable (can be modified in real-time) prescaler, with a counter clock frequency divisor between 1 and
65536.
•       4 independent channels:
−      Input capture
−      Output compare
−      PWM generation (edge or middle alignment)
−      Single-pulse mode output
•       Use external signal to control the timer and synchronize the interconnected circuits of the timers
•       Generate interrupt/DMA in the case of the following events:
−      Update: up/downcounter overflow, counter initialization (software-based trigger or internal trigger)
−      Trigger events (startup, shutdown, and initialization of counter, or internally triggered counting)
−      Input capture
−      Output compare
•       Support the incremental (quadrature) encoders for positioning and Hall sensor circuits
•       Use trigger input signal as the input of external clock, or perform periodic current management


#### 2.9.3 Advanced Timer (TIM1)

TIM1 consists of a 16-bit auto-loading counter driven by a programmable prescaler. It provides multiple functions,
including measuring the pulse width of the input signal (input capture) or generating output waveform (such as output
compare, PWM, and complementary PWM outputs with dead time in between). Using the timer's prescaler and RCC clock
control prescaler can adjust the pulse width and waveform period from several microseconds to several milliseconds.
Main features:
•       16-bit up, down, or up/down auto-load counter

•   16-bit programmable (can be modified in real time) prescaler; with   a counter clock frequency division factor

between 1 and 65536
•    Supports up to 64 MHz as timer input clock
•    Up to four independent channels:
−    Input capture
−    Output compare
−    PWM generation (edge or middle alignment)
−    Single pulse mode output
•    Trigger time points can be configured by software in the entire PWM cycle
•    Complementary output with programmable dead time
•    A synchronous circuit that uses an external signal control timer or allows interconnection of multiple timers
•    A repetitive counter to update the timer registers only after a given number of cycles of the counter
•    Break input signal can put the timer's output signals in a reset state or in a known state
•    Generate interrupt/DMA in the case of the following events:
−    Update: up/downcounter overflow, counter initialization (software-based trigger or internal/external trigger);
−    Trigger events (startup, shutdown, and initialization of counter, or internally/externally triggered counting);
−    Input capture
−    Output compare
−    Break signal input
•    Support the incremental (quadrature) encoders for positioning and Hall sensor circuits.
•    Use trigger input signal as the input of external clock, or perform periodic current management.
In the debug mode, a counter can be frozen and PWM outputs are disabled, cutting off the switches controlled by those
outputs. The advanced timer has many functions similar to and the same internal structure as that of the standard TIM timer.
Therefore, it can work with the TIM timer through the timer's link function to provide synchronization or event link function.


#### 2.9.4 System Tick Timer (Systick)

This timer is tailored to a real-time operating system and can also be used as a standard downcounter. It has the following
features:
•    24-bit downcounter
•    Auto-reload feature
•    Generate a maskable system interrupt when the counter reaches zero.
•    Programmable clock source


#### 2.9.5 Watchdog Timer (WDG)

It supports both independent watchdog (IWDG) and window watchdog (WWDG). Two watchdogs ensure higher security,
time accuracy, and flexibility in use.
Independent Watchdog (IWDG)
The IWDG is based on a 12-bit downcounter and an 8-bit prescaler, and is driven by a separate, low speed RC oscillator.
It still can work even if the master clock fails. Once activated, the IWDG will generate a reset signal when the counter
reaches 0x000 if the watchdog's counter is not cleared within a given period. Moreover, it can also be used to reset the
entire system in the event of an application error, or as a free timer to provide timeout management for applications.
Window Watchdog (WWDG)
The WWDG is typically used to monitor software failures caused by application deviating from the normal running
sequence due to external disturbances or unforeseen logical conditions. Unless the value of a downcounter is refreshed

before the T6 bit reaches zero, the watchdog circuit will generate a chip reset when the preset time cycle is reached. A chip
reset is also generated if the value of a 7-bit downcounter (in the control register) is refreshed before the downcounter
reaches the value of the window register. This indicates that the downcounter needs to be refreshed with a limited time
window.
Main features:
•    WWDG is driven by the clock derived from the presaled APB1 clock.
•    Programmable free-running downcounter
•    Conditional reset
•    A reset occurs when the value of the downcounter is less than 0x40 if the watchdog is enabled
•    A reset occurs when the downcounter is reloaded outside the window if the watchdog is enabled
•    If the watchdog is enabled and interrupt is allowed, an EWI is generated when the value of the downcounter reaches
0x40, and it can be used to reload the counter to avoid resetting WWDG.


### 2.10 ADC

Support 10-bit 1.33 Msps ADC (configurable as 16-bit 16 Ksps), single-ended or differential AMIC, and built-in PGA with
a gain up to 42 dB.
Provide adjustable (1.6–2.3 V) MIC BIAS voltage for MIC.
Up to 8 channels, including 5 external single-ended channels, 1 differential MIC channel, and 2 internal channels. Two
internal channels are VCC detection channel and temperature sensor channel. For 5 external channels, the detection range
of channels 1 (PB10) and 2 (PB9) is 0V-1V, and that of channels 3 (PB8), 4 (PB7), and 5 (PB6) is 0V-3.6V. the input
voltage of channels 3 (PB8), 4 (PB7), and 5 (PB6) <=VCC+300 mV.
In the audio mode, using the built-in PGA and microphone bias, MIC signals are amplified by the PGA, and then converted
to digital signals by an ADC. After the audio input control (low-pass decimation filter and optional energy and zero-cross
detection), the audio data is stored in the system RAM through DMA. Finally, 16-bit 16 kHz audio signal format data is
output.
Main features:
•    Support analog microphone (AMIC) input, with adjustable microphone bias
•    PGA supports single-ended or differential input and adjustable gain
•    Support one ADC, which can measure 5 external single-ended channels, 1 differential MIC channel, and 2 internal
channels (input channels are optional)
•    Support two internal channels, including TempSensor and VCC
•    10-bit 1.33 Msps ADC (configurable as 16-bit 16 Ksps)
•    Support digital decimation filter to 16-bit and noise filter
•    Support single and continuous conversion modes
•    A DMA request can be generated during channel switching
•    The analog watchdog feature allows an application to detect whether the input PB10 voltage exceeds a user-defined
high/low threshold
•    An interrupt occurs at the end of switching or in the case of an analog watchdog event
•    In the audio mode, the filter's output data is stored in a 16-bit data register, and in the general mode, the data is right-
aligned and stored in a 16-bit data register
•    In the audio mode, data is output in 16-bit 16 Ksps signed mono PCM format, and in the general mode, data is output
in 10-bit 1.33 Msps unsigned format


### 2.11 I2C Bus Interface (I2C)

The I2C bus interface provides multi-master function that controls all the I2C bus specific sequences, protocol, arbitration
and timing. It supports multiple communication rate modes (up to 1 MHz), DMA operations, and SMBus 2.0. The I2C

module suits multiple purposes, including CRC code generation and verification, SMBus (System Management Bus), and
PMBus (Power Management Bus).
Main features:
•    Multi-master capability: The module can be either a master device or a slave device
•    I2C master functionality
−    Clock generation
−    Start and stop signal generation
•    I2C slave functionality
−    Programmable address detection
−    Supports 7-bit or 10-bit addressing, dual slave address response in 7-bit slave mode
−    Stop bit detection
•    Generates and detects 7-bit/10-bit addresses and broadcast call
•    Support different communication speeds
−    Standard rate (up to 100 kHz)
−    Rapid (up to 400 kHz)
−    Rapid+(up to 1 MHz)
•    Support various status flags
−    Transmitter/receiver mode flag
−    End of byte transmission flag
−    I2C bus busy flag
•    Support various error flags
−    Arbitration is lost in the master mode
−    Acknowledgement (ACK) error after address/data transfer
−    Misplaced start or stop condition detected
−    Overflow or underflow when the clock stretching is disabled
•    One interrupt vector:
−    Event interrupt and error interrupt share an interrupt vector
•    Optional clock stretching
•    Single-byte buffer with DMA capability
•    Generation or verification of configurable PEC
−    The PEC value can be transmitted as the last byte in the transmitter mode
−    PEC for the last received byte
•    Compatible with SMBus 2.0
−    25 ms clock low timeout delay
−    10 ms cumulative time for low clock extension for master device
−    25 ms cumulative time for low clock extension for slave device
−    Generation or verification of hardware PEC with ACK control
−    Support address resolution protocol (ARP)
•   Compatible with SMBus


### 2.12 Universal Synchronous Asynchronous Receiver Transmitter (USART)

The USART is integrated with 3 serial receiver/transmitter interfaces, including USART1, USART2, and LPUART.
USART1 and USART2 interfaces support synchronous/asynchronous communication, IrDA SIR ENDEC transmission
coding and decoding, multiprocessor communication mode, single-wire half-duplex communication mode, and LIN
master/slave function.
USART1 and USART2 interfaces also support the CTS and RTS hardware flow control, and the single-wire mode such as
the ISO7816 smart card standard. DMA is available to both interfaces.
Main features:
•    Full-duplex asynchronous communication
•    Single-wire half-duplex communication
•    NRZ standard format
•    Fractional baud rate generator system – a common programmable transmitting and receiving baud rate up to 4
Mbits/s
•    Programmable data word length (8 or 9 bits)
•    Configurable stop bits – support for 1 or 2 stop bits
•    LIN master is capable of sending synchronous break and LIN slave is capable of detecting the break: 13-bit break is
generated, and 10/11-bit break is detected when USART hardware is configured to LIN
•    Transmitter clock output for synchronous transmission
•    IRDA SIR encoder-decoder: Support for 3/16-bit duration in normal mode
•    Smart card emulation capability
−    The smart card interface supports the asynchronous smart card protocol as defined in the ISO7816-3 standard.
−    0.5 and 1.5 stop bits for smart card
•    Single-wire half-duplex communication
•    Configurable multi-buffer communication using DMA – buffering of received/transmitted bytes in SRAM using
centralized DMA
•    Separate enable bits for transmitter and receiver
•    Detection flags
−    Receive buffer full
−    Transmit buffer empty
−    End of transmission flags
•    Parity control
−    Transmits check bits
−    Check the received data parity
•    Four error detection flags
−    Overflow error
−    Noise error
−    Frame error
−    Parity error
•    Ten USART interrupt sources with flags

−   CTS change
−   LIN break detection

−     Transmit data register empty
−     Transmission completed
−     Receive data register full
−     Detected idle bus
−     Overflow error
−     Frame error
−     Noise error
−     Parity error
•    Multiprocessor communication – If addresses do not match, enable the silent mode
•    Wake up from silent mode (by detecting idle bus or address flag)
•    Two ways to wake up the receiver: address bit (MSB, 9th bit), bus idle
•    Mode configuration:

Communication mode                  USART1                 USART2                  LPUART

Asynchronous mode                Supported             Supported               Supported

Hardware flow control               Supported             Supported               Supported

Multi-buffer
Supported             Supported               Supported
communication (DMA)

Multiprocessor
Supported             Supported             Not supported
communication

Synchronous                   Supported             Supported             Not supported

Single-wire half-duplex              Supported             Supported             Not supported

Smart card                   Supported             Supported             Not supported

IrDA                     Supported             Supported             Not supported

LIN                      Supported             Supported             Not supported


### 2.13 Serial Peripheral Interface (SPI)

It supports Two SPI interfaces. SPI allows the chip to communicate with external devices in a half/full-duplex, synchronous,
and serial communication between chips and peripherals. SPI can be configured to operate in the master mode and provide
a communication clock (SCK) for the external slave device. Moreover, The interface can also work in a multi-master
configuration and support reliable communication with CRC verification.
Main features:
•    Full-duplex synchronous transmission
•    Double-wire simplex synchronous transmission with/without the third two-way data cable
•    8-bit or 16-bit transmission frame format
•    Support master or slave mode
•    Support multi-master mode
•    Rapid communication between master and slave modes

•    NSS management can be performed by software or hardware in the master or slave mode: dynamic switching of
master/slave mode
•    Programmable clock polarity and phase
•    Programmable data sequence, with MSB or LSB in the first order
•    Dedicated transmit and receive flags to trigger interrupt
•    SPI bus busy flag
•    Support reliable communication based on hardware CRC
−    The CRC value can be transmitted as the last byte in the transmitter mode
−    Automatic CRC for the last received byte in the full-duplex mode
•    Master mode failure, overload, and CRC error flags that trigger interrupt
•    Single-byte transmit and receive buffers with DMA support: Generate transmission and receiving requests
•    Maximum interface speed: 16 Mbps


### 2.14 Serial Audio Port (I2S)

I2S is a 3-pin synchronous serial interface communication protocol that can work in master or slave mode, it can be
configured for 16/24/32-bit transfers, and also can be configured a input or output channel, supporting audio sampling rates
from 8 kHz to 96 kHz. It supports four audio standards, including the Philips' I2S standard, the MSB and LSB alignment
standards, and the PCM standard.
It works in both master and slave modes in half-duplex communication. When it works as the master device, it provides
clock signals to external slave devices through the interface.
Main features:
•    Half-duplex communication (either transmitting or receiving only at a time)
•    Master or slave operation
•    8-bit linear programmable prescaler to obtain an accurate audio sample frequency (8 kHz~96 kHz)
•    16/24/32-bit data format
•    16-bit (16-bit data frame) or 32-bit (16/24/32-bit data frame) fixed packet frame for audio channel
•    Programmable clock polarity (stable state)
•    Underflow flag bit in the slave transmitter mode and overflow flag bit in the master/slave receiving mode
•    16-bit data register to transmit and receive, one in each end of the channel
•    Supported I2S protocols
−    Philips' I2S standard
−    MSB alignment standard (left-justified)
−    LSB alignment standard (right-justified)
−    PCM standard (a 16-bit channel frame with long or short frame synchronization; or extension from a 16-bit data
frame to a 32-bit channel frame)
•    Data sequence: MSB is always in the first order
•    DMA is available to both transmitter and receiver


### 2.15 Real Time Clock (RTC)

RTC has a set of BCD timers/counters that count independently continuously. In the corresponding software configuration,
RTC can provide the calendar function. RTC also has a programmable alarm clock interrupt.

The two 32-bit registers contain the subsecond, second, minute, hour (in 12- or 24-hour format), day of the week, day (day
of the month), month, and year data in decimal format (BCD).

The subsecond value is provided by a separate 32-bit register in binary format. The other 32-bit register contains
programmable second, minute, hour, day of the week, day, month, and year data.
RTC has the feature of automatic wakeup in low power mode.


### 2.16 Infrared Controller (IRC)

The infrared controller can generate different infrared protocol signals by configuring different types of coding through
software and supports the software-based infrared self-learning function.
Main features:
•    Carrier frequency range: 30 kHz~60 kHz
•    Support pulse width coding and pulse spacing coding
•    Support Manchester coding
•    Support carrier-free mode
•    Support any combination of Mark code and Space code
•    Provide 16 (depth) × 21-bit (width) Code FIFO for storing coded commands
•    Support repeatedly sending commands
•    Generate interrupt upon the end of transmission


### 2.17 Automatic Key Scanning (KEYSCAN)

Support 8/10/13 IO ports, corresponding to 44/65/104 keys respectively.
Support debouncing of keys
Support three modes: automatic scan, software scan, low-power scan
•    Automatic mode: Configurable automatic scanning mode starting at fixed time intervals
•    Low-power mode: Initiates a three-round scan mode when detecting a key press
•    Software mode: Software-triggered scanning mode


### 2.18 Serial SWD Debugging Interface (SWD)

A SWJ-DP interface with built-in ARM, it integrates JTAG and serial single-wire debugging interfaces and supports
connecting to the serial single-wire debugging interface or JTAG interface. The TMS and TCK signals of JTAG share
pins with SWDIO and SWCLK respectively.


### 3.1 QFN32 Package


#### 3.1.1 QFN32 Pin Assignment

*Figure 3-1 QFN32 Pin Distribution of N32WB03x Series*


### 3.2 Definition of Pin Multiplexing

*Table 3-1 Pin Definition*


Pin No.       Pin Name                                                                 Function
Type                   Alternate Function
(QFN32)    (Default Function)                                                          Description

SPI1_MOSI(I2S1_SD)

1. PA2            I/O

KEY3

SPI1_MISO(I2S1_MCK)

2. PA3            I/O

KEY4


3. RESET           AIO


TIM1_CH1
SPI2_NSS(I2S2_WS)

4. PB0            I/O                     USART1_RTS

LPUART_RTS
KEY11

TIM1_CH3N
PA4
5                             I/O             USART1_TXD(7816_TX1)
(SWDCLK)
KEY9

TIM1_ETR
PA5
6                             I/O            USART1_RXD(7816_RST1)
(SWDIO)
KEY10

TIM1_BKIN
USART2_TXD

7. PA6            I/O

USART1_CK(7816_CLK1)
KEY5

External 2.2 uF

8. VDD_FLASH            S

capacitor

TIM1_CH1
PB8                                      IIC_SDA
9                             I/O                                                      ADC3
(XO32KP_IN)                                 USART1_RTS
KEY7

TIM1_CH2
PB9                                       IIC_SCL
10                             I/O                                                      ADC2
(XO32KM_OUT)                                  USART1_CTS
KEY8

TIM1_CH2

11. PB1            I/O               SPI2_CLK(I2S2_CLK)

USART1_CTS

LPUART_TXD
KEY12
ANT_SW4

TIM1_CH3
SPI2_MOSI

12. PB2    I/O       LPUART_RXD

KEY13
ANT_SW5

TIM1_CH4
SPI2_MISO(I2S_MCK)

13. PB3    I/O        LPUART_CTS                WAKEUP

PA_LDO_EN
ANT_SW1

SPI2_CLK(I2S2_CLK)
TIM3_CH1(IRC_TX)

14. PB4    I/O

USART2_TXD(7816_TX2)
ANT_SW6

SPI2_MISO(I2S2_MCK)
TIM3_CH2(IRC_RX)

15. PB5    I/O   USART2_RXD(7816_RST2)

RCC_MCO
ANT_SW7

SPI2_MOSI(I2S2_SD)
TIM3_CH3
IIC_SDA

16. PB6    I/O                                    ADC5

USART1_TXD
SWDCLK
ANT_SW2

SPI2_NSS(I2S2_WS)
TIM3_CH4
IIC_SCL

17. PB7    I/O                                    ADC4

USART1_RXD
SWDIO
ANT_SW3

TIM1_CH3
LPUART_RTS

18. PB10   I/O       USART2_RXD                   ADC1

IIC_SMBA
KEY6

TIM1_CH1N

19. PB12                   I/O                     LPUART_TXD                  AMIC_BIAS

USART2_CTS

TIM1_CH4
LPUART_RXD

20. PB11                   I/O                                                   AMIC_N

USART2_RTS
IIC_SMBA

TIM1_CH2N

21. PB13                   I/O                     LPUART_CTS                    AMIC_P

USART2_CK

Power supply for

22. VCC                    S

chip

DCDC external

23. SWITCH                   S

Interface


24. VDCDC                    S                                                  DCDC output


25. RFIOP                  AIO                                                 Antenna port


26            VDD_PA                    S                                                 PA power supply

DCDC output,
27           VDCDCRF                    S                                                  connecting to
VDCDC

Power supply for
28             VCCRF                    S
chip

External 32 MHz
29         XO32MM_OUT                  AIO
crystal

External 32 MHz
30           XO32MP_IN                 AIO
crystal

SPI1_NSS(I2S1_WS)
31               PA0                   I/O
KEY1

SPI1_CLK(I2S1_CLK)
32               PA1                   I/O
KEY 2

33              GND                     S                                                     Ground

I = input, O = output, S = power supply, AIO = analog IO
After reset, the I/O port is configured to the analog input mode. But the following signals are not applicable:
•    Input pull-up mode for NRST by default
•    PA4: SWCLK under the input pull-down mode
•    PA5: SWDIO under the input pull-up mode


### 4.1 Test Condition

All voltages are based on VSS unless otherwise specified.


#### 4.1.1 Minimum and Maximum

Unless otherwise specified, all minimums and maximums will be guaranteed under the worst ambient temperature, supply
voltage and clock frequency conditions by testing 100% of the product at ambient temperature TA=25°C and TA=TAmax
(TAmax matches the specified temperature range) on the production line.
The annotations listed below each table are the data obtained through laboratory tests, design simulations and/or process
characteristics, and will not be tested on the production line. On the basis of laboratory tests, the minimums and maximums
are obtained by taking the average of the samples tested and adding to or subtracting from it three times the standard
deviation (average ±3∑).


#### 4.1.2 Typical Values

Unless otherwise specified, the typical data is based on TA=25°C and VCC=3.3 V (1.8V/2.32 V≤VCC≤3.6 V). Such data
is used for guiding the design only and is not tested.


#### 4.1.3 Typical Curve

Unless otherwise specified, the typical curve is used for guiding the design only and is not tested.


#### 4.1.4 Load Capacitance

*Figure 4-1 shows the load conditions for measuring pin parameters.*

*Figure 4-1 Pins Load Conditions*


PIN

CL<=30pF


#### 4.1.5 Pin Input Voltage


*Figure 4-2 shows how to measure the input voltage of a pin.*


*Figure 4-2 Pin Input Voltage*


VI


#### 4.1.6 Power Supply Plan


*Figure 4-3 Power Supply Scheme*


VCC

10uF

0.1uF
VDD_FLASH

0.1uF
3V

2.2uF
1uF(2)

GND

X032MP
12pF(1)
VDCDCRF

SWITCH
2.2uH                                      12pF(1)

0
RFIOP

1.6nH
N32WB03x
VDD_PA

(a) VDCDC/VDCDCRF uses the internal DCDC power supply

(b) VDCDC/VDCDCRF uses the external power supply

> **Notes:**

(1) The load capacitance CL required by different crystals or resonators is usually different,

refer to section 4.3.6 for details.

(2) In the case of low ripple requirements, the 1uF capacitor can be used without soldering.


#### 4.1.7 Current Consumption Measurement


*Figure 4-4 Current Consumption Measurement Plan*


ICC
VCC
VCCRF

VSS


### 4.2 Absolute Maximum Rating

The load applied to the device more than the value given in the "absolute maximum rating" tables (Tables 4-1, 4-2, and 4-
3) may cause permanent damage to the device. The maximum allowable load is given here, but it does not mean that the
functions of the device work well under these conditions. The device reliability will be affected if the device works at the
maximum conditions for a long time.

*Table 4-1 Voltage Characteristics*


Symbol                                   Description                            Minimum                Maximum        Unit

External main supply voltage (including VCCRF and
VCC-VSS                                                                             -0.3                       3.6
VCC)(1)
V

VIN            Input voltage on other pins(2)                                 VSS-0.3                VCC+0.3

Electrostatic discharge (ESD) voltage (human body
VESD(HBM)                                                                                See Section 4.3.10
model)

> **Notes:**
(1)
All power supply (VCC, VCCRF) and ground (VSS) pins must be connected to an external power supply system within
the permissible range.
(2)
IINJ(PIN) must not exceed its limit (see Table 4-2), ensuring that VIN does not exceed its maximum. If impossible, you
need to guarantee that the external limit IINJ(PIN) does not exceed its maximum. When VIN<VSS, there is a reverse injection
current.

*Table 4-2 Current Characteristics*


Symbol                                                  Description                                 Maximum (1)      Unit

IVCC            Total current (supply current) passing through VCC/VCCRF power cable(1)                  150

IVSS            Total current (output current) passing through VSS ground wire(1)                        150

Output sink current of any I/O and control pins                                           12
IIO
Output current of any I/O and control pins                                               -12
mA
Injection current of NRST pin                                                            +/-5

IINJ(PIN)(2)(3)     Injection current of HSE's OSC_IN pin and LSE's OSC_IN pin                               +/-5

Injection current of other pins(4)                                                       +/-12

∑IINJ(PI (2)        Total injection current of all I/O and control pins(4)                               +/-150
N)

> **Notes:**

(1) All power supply (VCC, VCCRF) and ground (VSS) pins must be connected to an external power supply

system within the permissible range.

(2) IINJ(PIN) must not exceed its limit, ensuring that VIN does not exceed its maximum. If impossible,

you need to guarantee that the external limit IINJ(PIN) does not exceed its maximum.

(3) When several I/O ports have injection current simultaneously, the maximum ∑IINJ(PIN) is the sum

of immediate absolute values of forward injection current and reverse injection current.

*Table 4-3 Temperature Characteristics*


Symbol                         Description                            Value                           Unit

TSTG                  Storage temperature range                   -40 to + 125                       °C

TJ              Maximum junction temperature                       105                            °C


### 4.3 Operating Conditions


#### 4.3.1 General Operating Conditions


*Table 4-4 General Operating Conditions*


Symbol                      Parameter                           Condition               Minimum    Maximum         Unit

fHCLK         Internal AHB clock frequency                                                           64

fPCLK1        Internal APB1 clock frequency                                                          32           MHz

fPCLK2        Internal APB2 clock frequency                                                          64

VCC           Standard operating voltage                                                1.8(1)       3.6           V

VCCRF          Analog operating voltage                                                  1.8(1)       3.6           V

TA           Ambient temperature                                                        -40         85           °C

TJ           Junction temperature range                                                 -40         105          °C

> **Note: (1) When the voltage on VCC/VCCRF fluctuates, N32WB031KEQ6-2 ensure that the minimum voltage**

fluctuation is greater than 2.32 V.


#### 4.3.2 Power-on and Power-off Operating Conditions

The parameters in the following table are obtained by testing at the ambient temperature listed in Table 4-4.
*Table 4-5 Power-on and Power-off Operating Conditions*


Symbol                       Parameter                           Condition              Minimum            Maximum             Unit

VCC rising speed                                                               20                  ∞
TVCC                                                          VCC=3.3 V                                                   μs/V
VCC lowering speed                                                             100                 ∞


#### 4.3.3 Characteristics of Built-in Reset and Power Control Module

The parameters given in the following table are obtained by testing at the ambient temperature and VCC supply voltage
listed in Table 4-4.
*Table 4-6 Characteristics of Built-in Reset and Power Control Module(1)*


Symbol                 Parameter                   Condition                Minimum       Typical Value         Maximum        Unit

VCC threshold power-on
TA=25°C                                   1.65(2)
voltage
VBOR                                                                                                                           V
VCC threshold power-off
TA=25°C                                   1.60(2)
voltage

VBORhyst        BOR delay                          TA=25°C                                       20                             mV

> **Notes:**
(1)
They are guaranteed by the design and are not tested in production.
(2)
N32WB031KEQ6-2 VCC threshold power on voltage 2.27V, VCC threshold power off voltage 2.25V.


#### 4.3.4 Characteristics of DCDC

DCDC is an internal voltage generation module. The parameters given in the following table are obtained by testing at the
ambient temperature and VCC supply voltage listed in Table 4-4.
*Table 4-7 Built-in DCDC Power Management Module Characteristics (1)*


Parameter                                                                         Typical
Symbol                                                Condition                   Minimum                            Maximum     Unit
Value

VCC               Supply voltage                                                                        3.3                      V

VDCDC           DCDC output voltage                                                                     1.15                      V

DCDC current carrying
Iload                                      Output current@ VDCDC=1.15 V                                               20        mA
capacity

η         DCDC conversion efficiency(2)                                                               82.5                      %

DCDC output voltage
VRPL                                                                                                    10                      mV
fluctuation

L            DCDC load inductance                                                    1                 2.2           10         μH

COUT          DCDC load capacitance                                                   0.5               2             10         μF

DCDC output voltage setup
tSTAR                                                                                                   90                       uS
time

> **Notes:**
(1)
They are guaranteed by the design and are not tested in production.
(2)
The FDK MIPSDZ1608G2R2PA inductor is used in test. Different inductor models may vary in DCDC efficiency.


#### 4.3.5 Characteristics of Supply Current

Current consumption is a composite indicator evaluated based on multiple parameters and factors, including operating
voltage, ambient temperature, I/O pin load, software configuration, operating frequency, I/O pin switching rate, position
of a program in memory, and code executed.
For the detailed method to measure current consumption, see Figure 4-4.
4.3.5.1 Typical Current Consumption
*Table 4-8 Typical Current Consumption in Sleep Mode(1)*


Minimum        Typical
Symbol              Parameter                          Condition                                                 Maximum(1)   Unit
Value(1)

Low speed clock: ON; 48 KB SRAM
Current in Sleep mode                                                                     1.6         3.8        uA
retention; I/O state unchanged
ICC

VCC is maintained; WAKEUP IO and NRST
Current in PD mode                                                                        0.13        1.0        uA
can be woken up

> **Note: (1) The test condition is TA=25°C, VCC=3.3 V.**
4.3.5.2 Typical Current Consumption in Operating Mode
The chip is under the following conditions:
•       All I/O pins are reset.
•       All peripherals are turned off unless otherwise specified.
•       Ambient temperature and VCC supply voltage conditions are listed in Table 4-4.
*Table 4-9 Typical Current Consumption in Operating Mode*


Symbol               Parameter                         Condition                Typical Value(1)          Maximum        Unit

Supply current in operating High speed internal RC oscillator
ICC                                                                                 2.0                                mA
mode                        (HSI)(2)

> **Notes:**
(1)
The typical value is measured at TA=25°C and VCC=3.3 V.
(2)
High speed internal clock is 64 MHz.
*Table 4-10 BLE Power Consumption*


Symbol              Parameter                         Condition                 Typical Value(1)          Maximum        Unit

0 dbm transmitting power, VCC               4.2
mA
current

Supply current in operating Minimum RX sensitivity, VCC                   3.8
ICC                                                                                                                    mA
mode                        current

1 s broadcast interval, VCC average         13
uA
current

100 ms broadcast interval, VCC            109
uA
average current

100 ms connection         interval,       70
uA
VCC average current

> **Note: (1) The typical value is measured at TA=25°C and VCC=3.3 V.**


#### 4.3.6 Characteristics of External Clock Source

4.3.6.1 High Speed External Clock Generated Using a Crystal/Ceramic Resonator
The high-speed external clock (HSE) can be generated by an oscillator consisting of a 32 MHz crystal/ceramic resonator.
The data presented in this section is obtained from the overall characteristic evaluation using the typical external
components listed in the table below. In applications, the resonator and load capacitor must be as close to the pin of the
oscillator as possible to reduce output distortion and stabilization time at startup. For more parameters (such as frequency,
encapsulation, and precision) of a crystal resonator, please contact the manufacturer. (The crystal resonator mentioned here
usually means the passive crystal oscillator.)
*Table 4-11 Characteristics of HSE 32 MHz Oscillator(1)(2)*


Typical
Symbol                       Parameter                      Condition             Minimum             Maximum      Unit
Value

fOSC_IN      Oscillator frequency                                                            32                   MHz

CL1       Recommended load capacitance and
corresponding crystal serial impedance       RS = 100Ω(4)                       12                    pF
CL2(3)     (RS)(4)

ID        HSE drive current                        VCC=3.3 V, 12 pF load                  0.2                   mA

tSU(HSE (5)   Startup time                                                                    0.2                   ms
)

> **Notes:**
(1)
Resonator's characteristic parameters are given by the manufacturers of crystal/ceramic resonators.
(2)
They are obtained from laboratory tests and are not tested in production.
(3)(4)
For CL1 and CL2, it is recommended to use high-quality ceramic dielectric capacitors designed for high frequency
applications and select suitable crystals or resonators. CL1 and CL2 usually share same parameters. Crystal manufacturers
usually give the parameter of load capacitance as a serial combination of CL1 and CL2. When selecting CL1 and CL2, you
should consider the capacitive reactance of PCB and chip pins.
(5)
tSU(HSE) is the startup time, which is measured from the moment when the software enables HSE until a stable 32 MHz
oscillation is obtained. This value is measured on a standard crystal resonator and may vary greatly depending on the
crystal manufacturer.

*Figure 4-5 Typical Application with a 32 MHz Crystal*


Resonator
integrated with
capacitor         CL1
fHSE

32 MHz resonator             Rr
control

CL2             REXT (1)

> **Note: The value of REXT depends on the characteristics of the crystal.**
4.3.6.2 ow Speed External Clock Generated Using a Crystal/Ceramic Resonator
The low-speed external clock (LSE) can be generated by an oscillator consisting of a 32.768 kHz crystal/ceramic resonator.
The data presented in this section is obtained from the overall characteristic evaluation using the typical external
components listed in the table below. In applications, the resonator and load capacitor must be as close to the pin of the
oscillator as possible to reduce output distortion and stabilization time at startup. For more parameters (such as frequency,
encapsulation, and precision) of a crystal resonator, please contact the manufacturer. (The crystal resonator mentioned here
usually means the passive crystal oscillator.)
> **Notes: For CL1 and CL2, it is recommended to use ceramic dielectric capacitors between 8 pF and 20 pF and select suitable**
crystals or resonators. CL1 and CL2 usually share the same parameters. Crystal manufacturers usually give the parameter of
load capacitance as a serial combination of CL1 and CL2. Different crystals or resonators usually require different load
capacitance (CL). The selected CL1 and CL2 must match the crystal or resonator used.
The load capacitance CL is calculated by the formula: CL = CL1 × CL2 / (CL1 + CL2) + Cstray, where Cstray is the capacitance
of the pin and the capacitance associated with the PCB or PCB, typically between 2 pF and 7 pF.
*Table 4-12 Characteristics of LSE Oscillator (fLSE=32.768kHz)(1)*


Typical
Symbol                   Parameter                           Condition           Minimum             Maximum       Unit
Value

Recommended load
CL1           capacitance and
RS: 30KΩ~90KΩ                          10                   pF
CL2(2)         corresponding crystal serial
impedance (RS)(3)

VCC=3.3 V, CL1=CL2=10 Pf,
I2           LSE drive current                                                              0.2                  μA
RS=30 KΩ

tSU(LSE)(4)      Startup time                                                                  0.84                   s

> **Notes:**
(1)
They are obtained from laboratory tests and are not tested in production.
(2)
See the "Notes" at the top of this table.
(3)
Using a high-quality oscillator with a small RS value can optimize the current consumption. For more details,
please contact the crystal manufacturer.
(4)
tSU(LSE) is the startup time, which is measured from the moment when the software enables LSE until a stable

32.768 KHz oscillation is obtained. This value is measured on a standard crystal resonator and may vary

greatly depending on the crystal manufacturer.

*Figure 4-6 Typical Application with a 32.768 kHz Crystal*


Gain
control

XO32KM_OUT


#### 4.3.7 Characteristics of Internal Clock Source

The characteristic parameters given in the following table are obtained by testing at the ambient temperature and supply
voltage listed in Table 4-4.
High Speed Internal (HSI) RC Oscillator
*Table 4-13 Characteristics of HSI Oscillator(1)(2)*


Typical
Symbol               Parameter                          Condition                    Minimum                      Maximum     Unit
Value

fHSI       Frequency                   TA = 25°C                                   63.36           64           64.64      MHz

TA = -40~105°C, temperature drift            -3                            3           %
Temperature drift of HSI
ACCHSI                                 TA = -10~85°C, temperature drift                -2                            2           %
oscillator
TA = 0~70°C, temperature drift               -1                            1           %

Startup time     of   HSI
tSU(HSI)                                                                                                           0.3          μs
oscillator

Power dissipation of HSI
ICC(HSI)                                                                                             180           260          μA
oscillator

> **Notes:**
(1)
VCC = 3.3 V, TA = -40~105°C.
(2)
They are guaranteed by the design and are not tested in production.
Low Speed Internal (LSI) RC Oscillator
*Table 4-14 Characteristics of LSI Oscillator(1)*


Typical
Symbol               Parameter                      Condition                    Minimum                   Maximum              Unit
Value

Calibrated at 25°C                         31.9            32           32.2            KHz
fLSI(2)     Output frequency
TA = -40~85°C, temperature drift            -1                            1              %

tSU(LSI) (3) Startup time of LSI                      200           μs

oscillator

Power dissipation of
ICC(LSI) (3)                                                                                    0.23                        μA
LSI oscillator

> **Notes:**
(1)
VCC = 3.3 V, TA = -40~85°C.
(2)
They are obtained from laboratory tests and are not tested in production.
(3)
They are guaranteed by the design and are not tested in production.


#### 4.3.8 Time Required to Wakeup from Low Power Modes

The wakeup time listed in Table 4-15 is measured during the wakeup phase of a 64 MHz HSI RC oscillator. The clock
source used in wakeup depends on the current operating mode:
Sleep or PD mode: The clock source is a RC oscillator
All the time is measured under the ambient temperature and supply voltage listed in Table 4-4.
*Table 4-15 Time Required to Wake Up from Low Power Modes*


Minimum       Typical        Maximum
Symbol                                    Parameter                                                                     Unit
Value

tWUSLEEP(1)      Wake up from Sleep mode                                                      0.2
ms
tWUPD(1)        Wake up from PD mode                                                         42

> **Note: (1) The wakeup time is measured from the start of the wakeup event to the reading of the first command by the user**
program.


#### 4.3.9 Characteristics of FLASH Memory

Unless otherwise specified, all characteristic parameters below are obtained at TA = -40~85°C.
*Table 4-16 Characteristics of Memory*


Typical
Symbol                   Parameter                          Condition           Minimum                    Maximum(1)     Unit
Value(1)

Page (256 bytes) programming
tPP                                           TA = -40~85°C                                       2            3            ms
time

tPE         Page (256 bytes) erasing time     TA = -40~85°C                                     16            30            ms

tSE         Sector (4K byte) erase            TA = -40~85°C                                     16            30            ms

tCE         Chip erasing time                 TA = -40~85°C                                     16            30            ms

> **Note: (1) They are guaranteed by the design and are not tested in production.**
*Table 4-17 Flash Memory Life and Data Retention Period*


Symbol                   Parameter                     Condition              Minimum(1)                       Unit

TA = -40~85°C                     10                     Ten thousand times
NEND         Life (note: erasure times)
TA = -40~105°C                    1                      Ten thousand times

tRET        Data retention period             TA = 105℃                         20                           Year

> **Note:(1) They are obtained from laboratory tests and are not tested in production.**


#### 4.3.10 Absolute Maximum (Electrical Sensitivity)

Electrical sensitivity is determined by testing the chip strength using specified measurement methods based on three
different tests (ESD, LU).
Electrostatic Discharge (ESD)
Electrostatic discharge (one positive pulse followed by one negative pulse after one second) is applied to all pins of all
samples, and the sample size depends on the number of power supply pins on the chip (3 × (n+1) power supply pins). This
test complies with the MIL-STD-883K and ESDA/JEDEC JS -002-2018 standards.
*Table 4-18 Absolute Maximum of ESD*


Symbol                      Parameter                             Condition                 Type     Minimum(1)           Unit

TA = +25°C,
VESD(HBM)      ESD voltage (human body model)                                                 II          2000
MIL-STD-883K compliant
V
TA = +25°C,
ESD voltage (charging device
VESD(CDM)                                   ESDA/JEDEC JS -002-                               II          1000
model)
2018 compliant

> **Note:(1) They are obtained from laboratory tests and are not tested in production.**
Static Latch-Up
Provide a supply voltage exceeding the limit for each power supply pin.
Inject current into each input, output and configurable I/O pin.
This test complies with JEDEC78E integrated circuit latch-up standard.
*Table 4-19 Electrical Sensitivity*


Symbol                    Parameter                                    Condition                              Type

LU            Static latch-up                   TA = +25/+85°C, JEDEC78E compliant                      Class II A


#### 4.3.11 I/O port characteristics

Characteristics of General-Purpose Input/Output
Unless otherwise specified, the parameters given in the following table are obtained by measuring under the conditions
listed in Table 4-4. All I/O ports are compatible with CMOS and TTL.
*Table 4-20 Static Characteristics of I/O(1)(2)*


Symbol                   Parameter                          Condition                 Minimum        Maximum            Unit

VCC=3.3 V                           VSS              0.8
VIL         Input low level voltage
VCC=2.5 V                           VSS              0.7
V
VCC=3.3 V                             2             VCC
VIH         Input high level voltage
VCC=2.5 V                            1.7            VCC

Hysteresis     voltage        of
Vhys                                             VCC=3.3 V/2.5 V                      200                             mV
Schmitt trigger(1)

VPAD=0
Ilkg       Input leakage current(3)                                           -1              1             μA
VPAD=VCC

Weak pull-up        equivalent     VCC=3.3 V
RPU                                                                           120            140            kΩ
resistance(4)                      VIN= VIL

Weak pull-down equivalent          VCC=3.3 V
RPD                                                                           120            140            kΩ
resistance(4)                      VIN= VIH

CIO        Capacitance of I/O pin                                                            0.1            pF

> **Note:**
(1)(2)
Hysteresis voltage of Schmitt trigger's switching level. They are obtained from laboratory tests and are not tested in
production.
(3)
The leakage current may exceed the maximum if there is reverse sink current on adjacent pins.
Output Drive Current
GPIO (general purpose input/output port) can absorb or output up to +/-12 mA current.
Output Voltage
*Table 4-21 I/O Output Voltage*


Symbol               Parameter                        Condition                Minimum      Maximum           Unit

VCC=3.3, IOH=2 mA,
VSS            0.4
4 mA, 8 mA, 12 mA
VOL        Output low level
VCC=2.5, IOH=2 mA,
VSS            0.4
4 mA, 8 mA, 12 mA
V
VCC =3.3 V, IOH = -2 mA,

### 2.4 VCC

-4 mA, -8 mA, -12 mA
VOH        Output high level
VCC =2.5 V, IOH = -2 mA,
2             VCC
-4 mA, -8 mA, -12 mA

Input and Output AC Characteristics
The definitions and values of input and output AC characteristics are given in Table 4-22.
Unless otherwise specified, the parameters given in the following table are obtained by measuring at the ambient
temperature and supply voltage listed in Table 4-4.
*Table 4-22 Input and Output AC Characteristics(1)*


Register
Symbol           Parameter                     Condition            Minimum       Maximum       Unit
Configuration

CL=5 pF, VCC =3.3 V                               64
Maximum
00          fmax(IO)out                                                                                    MHz
frequency
CL=5 pF, VCC =2.5 V                               50
(2 mA)
t(IO)out         Output delay       CL=5 pF, VCC =3.3 V                               3.66         ns

CL=5 pF, VCC =2.5 V                                     4.72

CL=50 fF, VCC =2.97 V
t(IO)in            Input delay                                                                1.2    ns
VCC=2.5 V

CL=10 pF, VCC =3.3 V                                    64
Maximum
fmax(IO)out                                                                                          MHz
frequency
CL=10 pF, VCC =2.5 V                                    60


01. CL=10 pF, VCC =3.3 V                                    3.5

t(IO)out           Output delay                                                                      ns
(4 mA)                                              CL=10 pF, VCC =2.5 V                                    4.5

CL=50 fF, VCC =2.97 V
t(IO)in            Input delay                                                                1.2    ns
CL=50 fF, VCC=2.5 V

CL=20 pF, VCC =3.3 V                                    64
Maximum
fmax(IO)out                                                                                          MHz
frequency
CL=20 pF, VCC =2.5 V                                    50


10. CL=20 pF, VCC =3.3 V                                    3.42

t(IO)out           Output delay                                                                      ns
(8 mA)                                             CL=20 pF, VCC =2.5 V                                    4.73

CL=50 fF, VCC =2.97 V
t(IO)in            Input delay                                                                1.2    ns
CL=50 fF, VCC = 2.5 V

CL=30 pF, VCC =3.3 V                                    64
Maximum
fmax(IO)out                                                                                          MHz
frequency
CL=30 pF, VCC =2.5 V                                    50

CL=30 pF, VCC =3.3 V                                    3.34
11 (12 mA)      t(IO)out           Output delay                                                                      ns
CL=30 pF, VCC =2.5 V                                    4.26

CL=50 fF, VCC =2.97 V
t(IO)in            Input delay                                                                1.2    ns
CL=50 fF, VCC=2.5 V

> **Note:(1) They are guaranteed by the design and are not tested in production.**
*Figure 4-7 Definition of Input and Output AC Characteristics*


50%V1                  50%V1

tdr
tdf

50%V1                  50%V1

tf                 tr


#### 4.2.12 Characteristics of NRST

The NRST pin's input drive uses the CMOS process and is connected to a pull-up resistor RPU that cannot be disconnected.
Unless otherwise specified, the parameters given in Table 4-23 are obtained by measuring at the ambient temperature and
supply voltage listed in Table 4-4.
*Table 4-23 Characteristics of NRST Pins*


Typical
Symbol                   Parameter                    Condition           Minimum               Maximum         Unit
Value

V IL(NRST)(1)    NRST input low level voltage         VCC = 3.3 V            Vss        -             0.8
V
NRST input        high       level
V IH(NRST)(1)                                         VCC = 3.3 V             2         -             VCC
voltage

Hysteresis voltage of NRST
V hys(NRST)(1)                                              -                  -        200             -          mV
Schmitt trigger

Weak pull-up        equivalent
RPU                                               VCC = 3.3 V            40         50            60          kΩ
resistance

VF(NRST)(1)      NRST input filter pulse                   -                  -         -             500          ns

V NF(NRST)(1)     NRST input unfiltered pulse               -                  5         -              -           us

> **Note:(1) They are guaranteed by the design and are not tested in production.**
*Figure 4-8 Recommended NRST Pin Protection*


External reset
(1)
network
2
NRST（ ）                                             Internal reset
Filter

> **Note:**
(1)
The external reset network is to avoid a parasitic reset.
(2)
Users must ensure that the potential of the NRST pin is below the maximum VIL(NRST) listed in Table 4-23. Otherwise, the
chip cannot be reset.


#### 4.3.13 Characteristics of TIM Timer


The parameters given in Table 4-24 are obtained by measuring at the ambient temperature and supply voltage listed in
*Table 4-4.*


*Table 4-24 Characteristics of TIMx(1)(2)*


Symbol                      Parameter                        Condition        Minimum Maximum                 Unit

1                        tTIMxCLK
tres(TIM)   Timer resolution time
15.625                         ns

0         fTIMxCLK/2       MHz
Timer's    external     clock   frequency
fEXT
(CH1~CH4)
0            32            MHz

ResTIM      Timer resolution                            fTIMxCLK= 64 MHz                       16             Bit

1          65536         tTIMxCLK
Clock period of a 16-bit counter when an
tCOUNTER
internal clock is selected
0.015625        1024            μs

65536x65536       tTIMxCLK
tMAX_COUNT    Maximum possible count
67.1             s

> **Note:**
(1)
TIMx is a generic name that represents TIM1/TIM3/TIM6.
(2)
Parameters are guaranteed by the design.


#### 4.3.14 Characteristics of I2C Interface


Unless otherwise specified, the parameters below are obtained by measuring at the ambient temperature, fPCLK1 frequency,
and VCC supply voltage listed in Table 4-4.

The I2C interface conforms to the standard I2C communication protocol but is subject to the following limitations: SDA
and SCL are not "real" open-drain pins, and when they are configured as output open-drain, the PMOS transistor between
the lead-out pin and VCC is turned off, but still exists.

The characteristics of I2C interface are shown in the table below. For more details on the characteristics of the input/output
alternate function pins (SDA and SCL), see Section 4.3.11.

*Table 4-25 Characteristics of I2C Interface(1)*


Standard Mode                  Fast Mode
Symbol                         Parameter                                                               Unit
Minimum      Maximum      Minimum       Maximum

fSCL              I2C Interface Frequency                         100                      1000     KHz

Start condition holding
th(STA)                                               4.0                      0.6                     μs
time

tw(SCLL)            SCL clock low time                 4.7                      1.3                     μs

tw(SCLH)            SCL clock high time                4.0                      0.6                     μs

Setup time for a repeated
tsu(STA)                                               4.7                      0.6                     μs
start condition

th(SDA)            SDA data hold time                              3.4                       0.9       μs

tsu(SDA)            SDA setup time                    250.0                     100                     ns

tr(SDA)
SDA and SCL rise time                          1000          20           300       ns
tr(SCL)

tf(SDA)
SDA and SCL fall time                           300                       300       ns
tf(SCL)

Setup time for a stop
tsu(STO)                                               4.0                      0.6                     μs
condition

Time from stop condition
tw(STO:STA)          to start condition (bus            4.7                      1.3                     μs
idle)

Cb               Capacitive load per bus                         400                       100       pf

tv(SDA)            Data validity time                3.45                      0.9                     μs

Valid          time       of
tv (ACK)                                              3.45                      0.9                     μs
acknowledgement

> **Note:**
(1)
They are guaranteed by the design and are not tested in production.
(2)
To reach the maximum frequency of standard mode I2C, fPCLK1 must be greater than 2 MHz. To reach the maximum
frequency of fast mode I2C, fPCLK1 must be greater than 4 MHz.

*Figure 4-9 AC Waveform and Measuring Circuit of I2C Bus(1)*


VCC VCC

(2)            (2)

### 4.7 KΩ                    4.7 KΩ


{
1 00 Ω
I2C bus                                                                                                 SDA
SCL
100 Ω

Repeated start
condition
Start condition

{
SDA

tsu(STA)                   Start condition

tr(SDA)

tf(SDA)                                                            tsu(SDA)                                                                                  tw(STA:STO)
Stop condition
tv(SDA)                        th(SDA)
tv(ACK)

tw(SCKH
th(STA)
)
SCL

tr(SCK)                                                                                 tsu(STO)
tf(SCK)
tw(SCKL)                                                                               9th clock
1/fSCL
1st clock cycle

> **Note:(1) The measuring points are set at CMOS levels: 0.3 VCC and 0.7 VCC.**


#### 4.3.15 Characteristics of SPI


Unless otherwise specified, the SPI parameters are obtained by measuring at the ambient temperature, fPCLK2 frequency,
and VCC supply voltage listed in Table 4-4.

For more details on the characteristics of the input/output alternate function pins (NSS, SCLK, MOSI, MISO), see Section
4.3.11.

*Table 4-26 Characteristics of SPI(1)*


Symbol                                     Parameter                                                    Condition                 Minimum              Maximum                  Unit

fSCLK                                                                               Master mode                                      -                     16
SPI clock frequency                                                                                                                              MHz
1/tc(SCLK)                                                                              Slave mode                                       -                     16

tr(SCLK)tf(SCLK)                  SPI's clock rise and fall time Load capacitance: C = 30 pF                                               -                         8                 ns

Duty cycle of SPI's slave
DuCy(SCK)                                  SPI slave mode      30           70         %
input clock

tsu(NSS) (1)   NSS setup time              Slave mode         4tPCLK         -         ns

th(NSS)(1)    NSS hold time               Slave mode         2tPCLK         -         ns

tw(SCLKH)(1)
SCLK high and low time       Master mode                        tPCLK - 2    tPCLK + 2    ns
tw(SCLKL)(1)

SPI1        5             -
(1)
tsu(MI )                                      Master mode
SPI2        6                      ns
Data input setup time
SPI1        5
tsu(SI)(1)                                   Slave mode
SPI2        6

th(MI)(1)                                    Master mode                           4
Data input hold time                                                                      ns
th(SI)(1)                                    Slave mode                            3

t(1)(2)
a(SO)           Data output access time      Slave mode, fPCLK = 32 MHz            0          3tPCLK      ns

tdis(SO)(1)(3)    Data output disabled time    Slave mode                            2            10        ns

Slave      mode           SPI1                     16
tv(SO)(1)                                    (after   enabled
edge)                     SPI2                     20
Data output valid time                                                                    ns
Master     mode           SPI1                     8
tv(MO)(1)                                    (after   enabled
edge)                     SPI2                     10

th(SO)(1)                                    Slave mode (after enabled edge)       2
Data output hold time                                                                     ns
th(MO)(1)                                     Master mode (after enabled edge)      0

> **Note:**
(1)
They are obtained from laboratory tests and are not tested in production.
(2)
The minimum represents the minimum time to drive the output, and the maximum represents the maximum time to obtain
the data correctly.
(3)
The minimum represents the minimum time to turn off the output, and the maximum represents the maximum time to put
the data line in a high impedance state.

*Figure 4-10 SPI Sequence Diagram–Slave Mode and CPHA=0*


tsu(NSS)                      tc(SCLK)                                   th(NSS)

tw(SCLKH)
tw(SCLKL)

CLKPOL=1            ta(SO)                                                                                           tdls(SO)
v(SO)              h(SO)       tr(SCLK)
tf(SCLK)

tsu(SI)

th(SI)

*Figure 4-11 SPI Sequence Diagram–Slave Mode and CPHA=1(1)*


tc(SCLK)                                                  th(NSS)
tsu(NSS)
CLKPHA=1
CLKPOL=0                                tw(SCLKH)
tw(SCLKL)

tr(SCLK)
ta(SO)                                                           th(SO)                        tdls(SO)
tv(SO)                               tf(SCLK)

th(SI)

> **Note:(1) The measuring points are set at CMOS levels: 0.3 VCC and 0.7 VCC.**

*Figure 4-12 SPI Sequence Diagram–Master Mode(1)*


tc(SCLK)

CLKPOL=0

CLKPOL=1

CLKPOL=0

tr(SCLK)
CLKPOL=1                                                                                         tf(SCLK)
tsu(MI)                        tw(SCLKH)
tw(SCLKL)

th(MI)

tv(MO)                      th(MO)

> **Note:(1) The measuring points are set at CMOS levels: 0.3 VCC and 0.7 VCC.**


#### 4.3.16 Characteristics of Temperature Sensor (TS)


Unless otherwise specified, the parameters below are obtained by measuring at the ambient temperature, fHCLK frequency,
and VCC supply voltage listed in Table 4-4.

*Table 4-27 Characteristics of Temperature Sensor*


Typical
Symbol                                 Parameter                                Minimum                   Maximum      Unit
Value

TL(1)       VSENSE Linearity with respect to temperature                                     ±4                       °C

Avg_Slope(1)    Average slope                                                                  2.14(2)                  mV/°C

tSTART(1)     Setup time                                                                                     10         μs

> **Note:**
(1)
They are guaranteed by the design and are not tested in production.
(2)
They are obtained from laboratory tests and are not tested in production.


#### 4.3.17 Characteristics of ADC


Unless otherwise specified, the parameters below are obtained by measuring at the ambient temperature, fHCLK frequency,
and VCC supply voltage listed in Table 4-4.

*Table 4-28 Characteristics of ADC*


Test Condition                    Typical
Symbol               Parameter                                              Minimum               Maximum      Unit
Value

VREF+      Positive reference voltage                                                     1.0                    V

fADC      ADC sampling rate                                                                         1.33      MHz

Voltage    conversion       range,
0                     1000       mV
external low voltage path
VAIN
Voltage    conversion       range,
0           -        3600(2)     mV
external high voltage path

RADC      Sampling switch resistance                                                                           kΩ

Internal sampling and holding
CADC                                                                                                           pF
capacitance

Input Frequency=1.03 KHz,
SNDR       Ingal noise distortion ration          VCC=3.3 V, TA=25°C                      46                   dBFS
fADC=1Msps

Input Frequency=0.98 KHz,
SNDR       Ingal noise distortion ration          VCC=3.3 V, TA=25°C                      58                   dBFS
fADC=16 Ksps

tSTAB(1)   Power-on time                                                                  16                    us

tCONV(1)   Conversion time                                                    752                               ns

DNL       Differential linear error              VCC=3.3 V, TA=25°C,         -1                       6       LSB

INL       Integral linear error                  VCC=3.3 V, TA=25°C          -8                       2       LSB

> **Note:**
(1)
They are obtained from laboratory tests and are not tested in production.
(2)
The maximum value 3600 mV and <= VCC+ 300 mV.


#### 4.3.18 Characteristics of PGA


Unless otherwise specified, the parameters below are obtained by measuring the ambient temperature and VCC supply
voltage listed in Table 4-4.

*Table 4-29 Characteristics of PGA*


Test Condition                      Typical
Symbol                      Parameter                                          Minimum                      Maximum   Unit
Value

GAIN        PGA gain                                                             0                            42      dB

GAIN STEP      PGA gain step                                                                       6                     dB

Gain=0dB                                                            73             82             85      dB
THD+N(1)
Gain=42 dB                                                          73             83             87      dB

In-band ripple(1) Gain fluctuation in 300–3400 Hz band                                             0.78                    dB

TPGA(1)      PGA setup time                                                                     15                     ms

MIC_BIAS
MIC bias voltage, step=0.1 V                                        1.6                          2.3      V
voltage

MIC_BIAS
20Hz to 8kHz A-weighted with 4.7uF                                                 -92                   dBV
Noise(1)

> **Note:(1) They are obtained from laboratory tests and are not tested in production.**

4.3 19 Characteristics of KEYSCAN

Unless otherwise specified, the parameters below are obtained by measuring the ambient temperature and VCC supply
voltage listed in Table 4-4.

*Table 4-30 Characteristics of KEYSCAN*


Test Condition                        Typical
Symbol                   Parameter                                                Minimum                    Maximum   Unit
Value

Time interval for each round of
TWTS                                                                                                 32       224     ms
keyboard scanning

TDTS       Key jitter elimination time                                                 10                     640     ms

Power    dissipation   in     automatic    TDTS=40 ms, TWTS=32 ms
Icc(1)                                                                                               2.9              uA
scanning mode (104 keys)                    VCC=3.3 V, TA=25 °C

Power dissipation in low power mode         VCC=3.3 V, TA=25 °C
2.3                 uA
(104 keys)

> **Note:(1) They are obtained from laboratory tests and are not tested in production.**


#### 4.3.20 Characteristics of BLE


Unless otherwise specified, the parameters below are obtained by measuring the ambient temperature and VCC supply
voltage listed in Table 4-4.

*Table 4-31 BLE Receiving Characteristics(1)*


No.                     Parameter                        Test Condition       Minimum        Typical     Maxim     Unit
Value      um


1. Sensitivity, 1 Mbps                         VCC=3.3 V, TA=25 °C                       -96                dBm


2. Sensitivity, 2 Mbps                                                                   -93                dBm


3. Co-channel interference                                                                8                  dB


Adjacent channel interference, +-1
4                                                                                               1                  dB
MHz

Adjacent channel interference, +-2
5                                                                                              -31                 dB
MHz

Adjacent channel interference, >=+-3
6                                                                                              -40                 dB
MHz


7. Mirror channel interference                                                           -24                 dB


Adjacent mirror channel interference,
8                                                                                              -28                 dB
+-1 MHz


9. Maximum input power                                                                    6                 dBm


> **Note:(1) They are obtained from laboratory tests and are not tested in production.**

*Table 4-32 BLE Transmitting Characteristics(1)*


No.                    Parameter                     Test Condition       Minimum    Typical     Maximum         Unit
Value


1. Output power                             VCC=3.3 V, TA=25 °C                   6                        dBm


2. Frequency accuracy                                                            7.5                       kHz


3. Frequency drift rate                                                    -9.4                 kHz/50us


4. Frequency drift                                                         -15.1                  kHz


5. Initial frequency drift                                                 -13.2                  kHz


6       Δf1 average                                                             258                    kHz

7       Δf2 99.9%                                                               218                    kHz

8       Δf2/Δf1                                                                 1.06                     -


9. Harmonic power, second harmonic                                         -26                    dBm


10. Harmonic power, third harmonic                                          -28                    dBm


11. Harmonic power, fourth harmonic                                         -54                    dBm


12. Harmonic        power,    quintuple                                     -55                    dBm

harmonic

> **Note:(1) They are obtained from laboratory tests and are not tested in production.**


### 5.1 QFN32

*Figure 5-1 QFN32 Package Size*


Version     Date        Changes
V1.3        2022.9.30   Initial release

`
7. Disclaimer

This document is the exclusive property of NSING TECHNOLOGIES PTE. LTD. (Hereinafter referred to as NSING
SG). This document, and the product of NSING SG described herein (Hereinafter referred to as the Product) are owned
by NSING SG under the laws and treaties of Republic of Singapore and other applicable jurisdictions worldwide.
The intellectual properties of the product belong to NSING TECHNOLOGIES INC. (Hereinafter referred to as
NSING) and NSING TECHNOLOGIES INC. does not grant any third party any license under its patents,
copyrights, trademarks, or other intellectual property rights. Names and brands of third party may be mentioned or
referred thereto (if any) for identification purposes only. NSING SG reserves the right to make changes, corrections,
enhancements, modifications, and improvements to this document at any time without notice. Please contact NSING
SG and obtain the latest version of this document before placing orders. Although NSING has attempted to
provide accurate and reliable information, NSING assumes no responsibility for the accuracy and reliability of
this document. It is the responsibility of the user of this document to properly design, program, and test the
functionality and safety of any application made of this information and any resulting product. In no event
shall NSING be liable for any direct, indirect, incidental, special, exemplary, or consequential damages arising in any
way out of the use of this document or the Product. NSING Products are neither intended nor warranted for
usage in systems or equipment, any malfunction or failure of which may cause loss of human life, bodily injury or
severe property damage. Such applications are deemed, Insecure Usage’. Insecure usage includes, but is not limited
to: equipment for surgical implementation, atomic energy control instruments, airplane or spaceship instruments,
all types of safety devices, and other applications intended to supporter sustain life. All Insecure Usage shall be
made at user's risk. User shall indemnify NSING and hold NSING harmless from and against all claims, costs,
damages, and other liabilities, arising from or related to any customer's Insecure Usage Any express or implied
warranty with regard to this document or the Product, including, but not limited to. The warranties of merchantability,
fitness for a particular purpose and non-infringement are disclaimed to the fullest extent permitted by law. Unless
otherwise explicitly permitted by NSING, anyone may not use, duplicate, modify, transcribe or otherwise distribute
this document for any purposes, in whole or in part.
