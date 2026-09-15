# STM32 环境监测系统

## 1. 项目简介

本项目基于 STM32F103C8T6 开发，实现温度、湿度和距离数据的采集，并通过 OLED 显示和 USART 串口输出。

项目主要用于练习 STM32 GPIO、定时器、输入捕获、中断、I2C、USART 以及多模块综合应用。

## 2. 项目功能

* DHT11 温度采集
* DHT11 湿度采集
* HC-SR04 超声波测距
* OLED 实时显示数据
* USART 串口输出数据
* TIM3 输入捕获测量 HC-SR04 ECHO 时间
* 超时检测
* 多模块综合运行

## 3. 硬件清单

| 器件            | 用途      |
| ------------- | ------- |
| STM32F103C8T6 | 主控      |
| 0.96 寸 OLED   | 数据显示    |
| DHT11         | 温湿度采集   |
| HC-SR04       | 超声波测距   |
| CH340         | USB 转串口 |
| ST-Link V2    | 程序下载与调试 |

## 4. 硬件连接

### 4.1 OLED

| OLED | STM32 |
| ---- | ----- |
| VCC  | 3.3V  |
| GND  | GND   |
| SCL  | PB8   |
| SDA  | PB9   |

### 4.2 DHT11

| DHT11 | STM32 |
| ----- | ----- |
| VCC   | 3.3V  |
| DATA  | PA0   |
| GND   | GND   |

### 4.3 HC-SR04

| HC-SR04 | STM32          |
| ------- | -------------- |
| VCC     | 5V             |
| GND     | GND            |
| TRIG    | PA1            |
| ECHO    | PA6 / TIM3_CH1 |

PA6 使用 TIM3_CH1 输入捕获功能，用于测量 HC-SR04 的 ECHO 高电平持续时间。

### 4.4 USART1

| STM32           | CH340 |
| --------------- | ----- |
| PA9 / USART1_TX | RX    |
| GND             | GND   |

串口参数：

Baud Rate: 115200
Data Bits: 8
Stop Bits: 1
Parity: None

## 5. 开发环境

* Keil MDK
* ARMCC V5
* STM32F1 Standard Peripheral Library
* VS Code
* Git
* GitHub

## 6. 软件结构

STM32-Environment-Monitor
|
|-- Hardware
|   |-- OLED
|   |-- DHT11
|   |-- HCSR04
|   `-- Serial
|
|-- Library
|
|-- Start
|
|-- System
|   `-- Delay
|
|-- User
|   `-- main.c
|
`-- README.md

## 7. 系统工作流程

DHT11
  |
  |-- Temperature
  |
  `-- Humidity

HC-SR04
  |
  |-- TRIG
  |
  |-- ECHO
  |
  `-- TIM3 Input Capture
          |
          `-- Distance

Temperature
Humidity
Distance
      |
      +--------> OLED
      |
      `--------> USART

## 8. DHT11 数据读取

DHT11 使用单总线方式进行数据传输。

STM32 首先向 DHT11 发送开始信号，然后切换 GPIO 为输入模式，等待 DHT11 响应并读取 40 bit 数据。

数据格式：

Humidity Integer
Humidity Decimal
Temperature Integer
Temperature Decimal
Checksum

读取完成后进行校验和判断，校验正确后更新温湿度数据。

## 9. HC-SR04 测距原理

HC-SR04 通过 TRIG 引脚启动测量。

STM32 向 TRIG 输出约 10 us 的高电平后，HC-SR04 开始测量，并通过 ECHO 输出高电平脉冲。

本项目使用 TIM3_CH1 输入捕获测量 ECHO 高电平持续时间。

TIM3 的计数频率设置为：

72 MHz / (71 + 1) = 1 MHz

因此：

1 个计数 = 1 us

距离计算公式：

Distance = EchoTime * 0.034 / 2

除以 2 是因为超声波需要经历一次发送和一次返回。

输入捕获过程：

ECHO Rising Edge
      |
      `-- Record Start Time
              |
              | ECHO remains high
              |
ECHO Falling Edge
      |
      `-- Record End Time
              |
              `-- Echo Time = End Time - Start Time

## 10. USART 数据输出

系统通过 USART1 将采集到的数据发送到电脑串口工具。

示例：

Temp:31C  Humi:45%  Dist:30cm
Temp:31C  Humi:45%  Dist:31cm
Temp:31C  Humi:46%  Dist:31cm

## 11. 项目调试过程

### 11.1 模块逐个测试

项目开发过程中没有一次性将所有模块全部整合，而是采用逐个测试的方法。

测试顺序：

OLED
  |
DHT11
  |
HC-SR04
  |
USART
  |
Multi-module Integration

这样可以在出现问题时快速定位具体模块。

### 11.2 HC-SR04 输入捕获

HC-SR04 的 ECHO 信号连接到 PA6，也就是 TIM3_CH1。

通过输入捕获分别检测 ECHO 的上升沿和下降沿：

上升沿 -> 记录开始时间
下降沿 -> 记录结束时间

两个时间相减即可得到 ECHO 高电平持续时间。

同时增加超时检测，避免没有正确收到 ECHO 信号时程序一直等待。

### 11.3 USART 数据输出

在项目整合过程中，USART 的数字数据显示曾出现问题。

后来单独编写 Serial_SendNumber() 函数，将数字转换为 ASCII 字符后发送，最终实现温度、湿度和距离数据的正常输出。

12. 当前项目完成情况
OLED Display                 [完成]
DHT11 Temperature            [完成]
DHT11 Humidity               [完成]
HC-SR04 Distance             [完成]
TIM3 Input Capture           [完成]
TIM3 Interrupt               [完成]
USART Data Output            [完成]
Multi-module Integration     [完成]
Git Version Control          [完成]
GitHub Repository            [完成]

13. 后续计划
V1.1
增加按键
增加蜂鸣器
增加温度报警
增加距离报警
增加 OLED 页面切换
V2.0

计划使用 FreeRTOS 对项目进行任务化改造，将数据采集、显示、串口输出和报警功能拆分为不同任务。

14. 项目总结

通过本项目熟悉了 STM32 多个外设的实际使用，并完成了多个模块从单独调试到综合运行的过程。

主要涉及：

GPIO
Timer
Input Capture
Interrupt
I2C
USART
Sensor Data Acquisition
Git
GitHub

通过实际项目进一步理解了 STM32 外设之间的配合方式，并积累了嵌入式项目开发和调试经验。
