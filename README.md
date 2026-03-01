#  边狱巴士指令终端(Limbus Company Prescript Terminal)

![Status: PoC](https://img.shields.io/badge/Status-PoC-orange)
![MCU: STM32F103C8T6](https://img.shields.io/badge/MCU-STM32F103C8T6-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-green)

[English Version Below](#english-version)

> 一个基于 STM32 的实体终端机项目，旨在现实中还原《边狱巴士》(Limbus Company) 与《废墟图书馆》中的指令分发交互，目标是达到开源社区的“电子方舟通行证”效果。

**⚠️ 开发备注**：本项目最初为一次即兴的 PoC（概念验证）开发，核心代码与硬件解密动效大部分由 AI 辅助快速构建。当前版本侧重于效果验证，后续将进行深度的项目结构优化与硬件升级。

## 📸 效果演示 (Demo)


## ✨ 当前特性 (Current Features)

目前的 V1.0 单机验证版本已实现以下基础功能：

* **非阻塞式并发动效**：基于 0.96 寸 OLED 屏幕，实现多字符独立并发的乱码跳动解密特效。
* **纯软件音频引擎**：利用 SysTick 精确延时与软件循环，驱动蜂鸣器生成频率随机的机械/运算电火花碎音。
* **状态机交互逻辑**：支持无操作自动息屏（防烧屏）显示待机 Logo，按键无缝打断与唤醒。
* **硬件真随机模拟**：利用内部 ADC 读取悬空引脚噪声作为随机种子，实现指令的随机抽取。
* **自定义指令集**：修改prescript_data.c中的数组即可。

## 🚀 演进路线 (Roadmap)

本项目致力于最终打造为一个完整的物联网桌面终端摆件。接下来的重构与升级计划如下：

- [ ] **项目结构优化**：重构现有的初期代码，模块化底层驱动（IIC、Delay、OLED），规范工程目录。
- [ ] **引入网络接入**：接入 ESP8266 / ESP32 等 Wi-Fi 模块，使设备具备联网与通讯能力。
- [ ] **服务端推送架构**：设计并搭建配套的推送服务端（Backend），取代当前的本地纯随机抽取。
- [ ] **异步指令下发**：实现由服务端在一天中的随机未知时间点，主动向终端下发指令的机制。
- [ ] **全彩视效跃迁**：弃用目前的 0.96 单色 OLED，硬件全面升级为 **TFT 长条全彩屏**，完美还原游戏内 UI 的色彩与精细度。
- [ ] **交互拓展**：后续准备加入翻硬币决定器功能和社交功能,最终目的是让它成为像明日方舟电子通行证那种效果。

## 🛠️ 硬件依赖 (Hardware Requirements)

运行当前的 PoC 版本，你需要以下基础硬件：

* **主控**：STM32F103C8T6 核心板
* **显示**：0.96 寸 OLED 屏幕 (4 Pin IIC 接口)
* **音频**：无源/有源蜂鸣器模块
* **输入**：普通机械微动按键模块

### 引脚接线参考 (Pinout)
| 模块 | 引脚 | STM32 接口 |
| --- | --- | --- |
| OLED | SCL | PB8 |
| OLED | SDA | PB9 |
| 蜂鸣器 | I/O | PA8 |
| 按键 | I/O | PA0 |

*(注：引脚配置可在 `main.c` 宏定义中自行修改)*

## ⚙️ 编译与烧录 (Getting Started)

1.  克隆本项目到本地。
2.  使用 **Keil uVision 5** 打开项目工程文件（`.uvprojx`）。
3.  根据上述引脚定义连接好硬件。
4.  编译工程，并使用 ST-Link / DAP-Link 将程序烧录至 STM32 开发板。

## 📂 目录结构 (Project Structure)
*(后续优化项目结构后，在此补充完整的目录树)*
## 📄 协议 (License)

本项目基于 [MIT License](LICENSE) 开源。

---

<span id="english-version"></span>

# English Version

> A physical terminal project based on STM32, aiming to recreate the "Prescript" distribution mechanism from *Limbus Company* and *Library of Ruina* in reality. The visual and animation designs are inspired by the open-source "Arknights Electronic Pass" effect.

**⚠️ Development Note**: This project originated as an impromptu PoC (Proof of Concept). Most of the core code and hardware decoding animations were rapidly built with AI assistance. The current version focuses on effect validation. Subsequent updates will feature deep project structure optimization and hardware upgrades.

## 📸 Demo

## ✨ Current Features

The current V1.0 standalone validation version implements the following:

* **Non-blocking Concurrent Animations**: Utilizes a 0.96-inch OLED to achieve multi-character, independent concurrent glitch/decoding effects.
* **Pure Software Audio Engine**: Uses precise SysTick delays and software loops to drive the buzzer, generating randomized mechanical/computational spark sounds.
* **State Machine Logic**: Supports auto-screen-off (burn-in protection) showing a standby logo, with seamless interrupt and wake-up via button press.
* **Hardware TRNG Simulation**: Reads the electromagnetic noise from a floating ADC pin as the random seed for prescript extraction.
* **Customizedable Prescript Set:** Simply modify the array in prescript_data.c.

  
## 🚀 Roadmap

The ultimate goal is to evolve this project into a complete, IoT-enabled desktop terminal. The upcoming refactoring and upgrade plans are as follows:

- [ ] **Project Structure Optimization**: Refactor initial code, modularize low-level drivers (IIC, Delay, OLED), and standardize the directory structure.
- [ ] **Network Integration**: Introduce Wi-Fi modules like ESP8266 / ESP32 to enable network communication.
- [ ] **Server Push Architecture**: Design and build a backend server to replace the current local RNG extraction.
- [ ] **Asynchronous Prescript Delivery**: Implement a mechanism where the server actively pushes prescripts to the terminal at random, unknown times of the day.
- [ ] **Full-Color Visual Leap**: Deprecate the 0.96-inch monochrome OLED and upgrade to a **TFT Bar Display** to perfectly recreate the in-game UI colors and details.
- [ ] **Interaction Expansion**: Consider adding RFID card verification, vibration motor force feedback, etc.

## 🛠️ Hardware Requirements

To run the current PoC version, you will need:

* **MCU**: STM32F103C8T6 Core Board
* **Display**: 0.96-inch OLED Screen (4-Pin IIC)
* **Audio**: Passive/Active Buzzer Module
* **Input**: Standard Micro Switch Button

### Pinout Reference
| Module | Pin | STM32 Port |
| --- | --- | --- |
| OLED | SCL | PB8 |
| OLED | SDA | PB9 |
| Buzzer | I/O | PA8 |
| Button | I/O | PA0 |

*(Note: Pin configurations can be modified in the macros within `main.c`)*

## ⚙️ Getting Started

1.  Clone this repository to your local machine.
2.  Open the project file (`.uvprojx`) using **Keil uVision 5**.
3.  Wire the hardware according to the pinout defined above.
4.  Compile the project and flash the firmware to the STM32 board using an ST-Link or DAP-Link.

## 📂 Project Structure
*(Will be populated once the directory is optimized)*
## 📄 License

This project is licensed under the [MIT License](LICENSE).
