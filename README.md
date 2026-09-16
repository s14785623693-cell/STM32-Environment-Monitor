# STM32 环境监测系统

基于 STM32F103C8T6 的环境监测与超限报警系统，实现温度、湿度和距离数据采集，并通过 OLED 和 USART 进行数据显示和输出，同时根据设定阈值进行蜂鸣器报警。

## 1. 项目简介

本项目使用 STM32F103C8T6 作为主控。

通过 DHT11 获取温度和湿度，通过 HC-SR04 获取距离数据，并使用 0.96 寸 OLED 显示，同时通过 USART 将数据发送到电脑串口工具。

当温度、湿度或距离超过设定阈值时，系统通过 OLED 显示 `ALERT`，并控制蜂鸣器进行报警。

项目主要用于练习 STM32 GPIO、定时器、输入捕获、中断、I2C、USART 以及多模块综合开发。

## 2. 项目功能

* DHT11 温度采集
* DHT11 湿度采集
* HC-SR04 超声波测距
* OLED 显示温度、湿度、距离及报警状态
* USART 串口输出温度、湿度和距离
* TIM3 输入捕获测量 HC-SR04 ECHO 高电平时间
* TIM3 中断处理
* 超时检测
* 温度、湿度、距离超限报警
* 蜂鸣器报警
* 多模块综合运行

报警条件：

```text
Temperature > 35℃
Humidity > 80%
Distance < 10cm
```

满足任意一个条件时进入报警状态。

## 3. 硬件清单

| 器件            | 用途      |
| ------------- | ------- |
| STM32F103C8T6 | 主控      |
| 0.96 寸 OLED   | 数据显示    |
| DHT11         | 温湿度采集   |
| HC-SR04       | 超声波测距   |
| 有源蜂鸣器         | 超限报警    |
| CH340         | USB 转串口 |
| ST-Link V2    | 程序下载与调试 |

## 4. 硬件连接

### OLED

| OLED | STM32 |
| ---- | ----- |
| VCC  | 3.3V  |
| GND  | GND   |
| SCL  | PB8   |
| SDA  | PB9   |

### DHT11

| DHT11 | STM32 |
| ----- | ----- |
| VCC   | 3.3V  |
| DATA  | PA0   |
| GND   | GND   |

### HC-SR04

| HC-SR04 | STM32          |
| ------- | -------------- |
| VCC     | 5V             |
| GND     | GND            |
| TRIG    | PA1            |
| ECHO    | PA6 / TIM3_CH1 |

PA6 使用 TIM3_CH1 输入捕获功能，用于测量 HC-SR04 的 ECHO 高电平持续时间。

### 蜂鸣器

| 蜂鸣器  | STM32 |
| ---- | ----- |
| 控制引脚 | PA2   |

本项目使用低电平触发方式：

```text
PA2 = 0 → 蜂鸣器响
PA2 = 1 → 蜂鸣器关闭
```

### USART1

| STM32           | CH340 |
| --------------- | ----- |
| PA9 / USART1_TX | RX    |
| GND             | GND   |

串口参数：

* 波特率：115200
* 数据位：8
* 停止位：1
* 校验位：无

## 5. 开发环境

* Keil MDK
* ARMCC V5
* STM32F1 Standard Peripheral Library
* VS Code
* Git
* GitHub

## 6. 软件结构

```text
STM32-Environment-Monitor
|
|-- Hardware
|   |-- OLED
|   |-- DHT11
|   |-- HCSR04
|   |-- Buzzer
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
|-- .gitignore
|-- Project.uvprojx
`-- README.md
```

## 7. 系统工作流程

```text
                    STM32F103C8T6
                          |
             +------------+------------+
             |                         |
           DHT11                    HC-SR04
             |                         |
        温度 / 湿度                 TRIG / ECHO
                                       |
                                TIM3 输入捕获
                                       |
                                     距离
             |                         |
             +------------+------------+
                          |
                    数据处理与判断
                          |
              +-----------+-----------+
              |                       |
             OLED                  USART1
              |                       |
          数据显示                  串口输出
              |
          报警状态判断
              |
          +---+---+
          |       |
        正常     超限
          |       |
        Status   Status
          OK      ALERT
                  |
                蜂鸣器
```

## 8. DHT11 数据读取

DHT11 使用单总线方式进行数据传输。

STM32 首先向 DHT11 发送开始信号，然后切换 GPIO 为输入模式，等待 DHT11 响应并读取 40 bit 数据。

数据包含：

* 湿度整数
* 湿度小数
* 温度整数
* 温度小数
* 校验和

读取完成后进行校验和判断，校验正确后更新温湿度数据。

## 9. HC-SR04 测距原理

HC-SR04 通过 TRIG 引脚启动测量。

STM32 向 TRIG 输出约 10 us 的高电平后，HC-SR04 开始测量，并通过 ECHO 输出一个高电平脉冲。

本项目使用 TIM3_CH1 输入捕获测量 ECHO 高电平持续时间。

TIM3 预分频设置为 71：

```text
72 MHz / (71 + 1) = 1 MHz
```

因此：

```text
1 个计数 = 1 us
```

距离计算：

```text
Distance = EchoTime * 0.034 / 2
```

除以 2 是因为超声波需要经历一次发送和一次返回。

输入捕获过程：

```text
ECHO 上升沿
    |
    v
记录开始时间
    |
    | ECHO 保持高电平
    |
    v
ECHO 下降沿
    |
    v
记录结束时间
    |
    v
EchoTime = 结束时间 - 开始时间
    |
    v
计算距离
```

同时增加超时检测，避免没有正确收到 ECHO 信号时程序一直等待。

## 10. USART 数据输出

系统通过 USART1 将采集到的数据发送到电脑串口工具。

串口输出示例：

```text
Temp:31C  Humi:45%  Dist:30cm
Temp:31C  Humi:45%  Dist:31cm
Temp:31C  Humi:46%  Dist:31cm
```

## 11. 超限报警

系统对温度、湿度和距离进行阈值判断。

```c
if(Temperature > 35 || Humidity > 80 || Distance < 10)
{
    Buzzer_ON();
    OLED_ShowString(4, 8, "ALERT");
}
else
{
    Buzzer_OFF();
    OLED_ShowString(4, 8, "OK");
}
```

正常情况下：

```text
Status:OK
```

满足任意报警条件时：

```text
Status:ALERT
```

同时控制蜂鸣器进行报警。

## 12. 项目调试过程

### 12.1 模块逐个测试

项目开发过程中采用逐个模块测试的方法，而不是一次性整合所有模块。

测试过程：

```text
OLED
  |
  v
DHT11
  |
  v
HC-SR04
  |
  v
USART
  |
  v
Buzzer
  |
  v
多模块整合
```

这样可以在出现问题时快速定位具体模块。

### 12.2 HC-SR04 输入捕获

HC-SR04 的 ECHO 连接到 PA6，也就是 TIM3_CH1。

通过输入捕获分别检测 ECHO 的上升沿和下降沿：

```text
上升沿 -> 记录开始时间

下降沿 -> 记录结束时间
```

两个时间相减即可得到 ECHO 高电平持续时间。

同时增加超时检测，避免没有正确收到 ECHO 信号时程序一直等待。

### 12.3 USART 数据输出

在项目整合过程中，USART 数字数据显示曾出现问题。

后来单独编写 `Serial_SendNumber()` 函数，将数字转换为 ASCII 字符后发送，最终实现温度、湿度和距离数据的正常输出。

### 12.4 蜂鸣器报警

增加蜂鸣器后，根据实际硬件的低电平触发方式编写 `Buzzer_ON()` 和 `Buzzer_OFF()` 函数，并通过温度、湿度和距离阈值进行报警控制。

## 13. 当前项目完成情况

| 功能           | 状态  |
| ------------ | --- |
| OLED 显示      | 已完成 |
| DHT11 温度采集   | 已完成 |
| DHT11 湿度采集   | 已完成 |
| HC-SR04 距离测量 | 已完成 |
| TIM3 输入捕获    | 已完成 |
| TIM3 中断      | 已完成 |
| USART 数据输出   | 已完成 |
| 蜂鸣器报警        | 已完成 |
| 超限状态显示       | 已完成 |
| 超时检测         | 已完成 |
| 多模块综合运行      | 已完成 |
| Git 版本管理     | 已完成 |
| GitHub 项目托管  | 已完成 |

## 14. 后续计划

### V2.0：FreeRTOS 任务化改造

计划使用 FreeRTOS 对当前项目进行任务化改造，将不同功能拆分为独立任务，例如：

```text
DHT11 数据采集任务
        |
HC-SR04 数据采集任务
        |
OLED 显示任务
        |
USART 输出任务
        |
报警处理任务
```

进一步学习 FreeRTOS 任务调度、任务间数据共享和同步机制。

## 15. 项目总结

通过本项目完成了多个 STM32 外设模块从单独学习、调试到综合运行的过程。

主要涉及：

* GPIO
* Timer
* Input Capture
* Interrupt
* I2C
* USART
* 传感器数据采集
* 超限报警
* 模块化程序设计
* Git
* GitHub

通过实际项目进一步熟悉 STM32 外设之间的配合方式，并积累嵌入式项目开发、模块调试和综合开发经验。
