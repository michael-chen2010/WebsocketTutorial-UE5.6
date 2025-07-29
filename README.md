# UE5实时通讯：用WebSocket构建在线互动体验

![项目演示GIF](https://media.githubusercontent.com/media/michael-chen2010/WebsocketTutorial-UE5.6/refs/heads/main/docs/demo.gif)

这是一个基于 **Unreal Engine 5.6** 和 **NWebSocket** 插件的教学示例项目，旨在演示如何构建一个健壮、可重用的WebSocket通信解决方案。

本项目完整地实现了PPT大纲中所述的核心功能，包括模块化的组件设计、心跳保活、断线自动重连以及结构化的JSON消息协议。

---

## 目录

- [项目特色](#-项目特色)
- [项目结构](#-项目结构)
- [环境与依赖](#-环境与依赖)
- [快速启动指南](#-快速启动指南)
- [核心架构解析](#-核心架构解析)
- [核心机制详解](#-核心机制详解)
- [如何测试（成果演示）](#-如何测试成果演示)
- [许可证](#-许可证)

---

## ✨ 项目特色

*   **模块化组件设计**: 核心网络逻辑封装在 `BP_WSManagerComponent` 中，高内聚、低耦合，可作为插件轻松集成到任何Actor上。
*   **生产级健壮性**: 内置连接状态管理、**心跳保活**和**断线自动重连**机制，确保网络连接在不稳定环境下的高可用性。
*   **结构化JSON协议**: 采用 `{"type": "...", "payload": ...}` 的统一消息格式，逻辑清晰，易于扩展新功能。
*   **清晰的Controller/View分层**: 聊天功能采用控制器(`BP_Chat`)与视图(`UI_Chat`)分离的设计，代码结构清晰，易于维护。
*   **开箱即用**: 项目包含一个简单的Node.js测试服务器，克隆后可立即运行和测试。

---

## 📂 项目结构

本仓库包含UE项目和配套的Node.js服务器，目录结构如下：

```
/
├── server/              <--  NodeJS WebSocket测试服务器
│   ├── node_modules/
│   ├── package.json
│   └── server.js
│
├── WebsocketTest/       <-- Unreal Engine 5 项目源码
│   ├── Content/
│   ├── Config/
│   └── WebsocketTest.uproject
│
└── README.md            <-- 就是本文件
```

---

## 🛠️ 环境与依赖

1.  **Unreal Engine**: `5.6`
2.  **UE插件**: 采用自定义插件**NWebSocket**，请确保已在项目中启用。
3.  **Node.js**: `v16.0` 或更高版本，用于运行测试服务器。

---

## 🚀 快速启动指南

请严格按照以下步骤操作，以确保项目正常运行。

### 1. 启动WebSocket服务器

首先，我们必须先启动后端服务器。

```bash
# 1. 进入server目录
cd server

# 2. 安装依赖 (仅首次运行时需要)
npm install

# 3. 启动服务器
node server.js
```

当您在终端看到 `WebSocket Server is running on ws://localhost:8080` 时，表示服务器已成功启动。

### 2. 运行Unreal Engine项目

1.  **打开项目**: 导航到 `WebsocketTest/` 目录，直接双击 `WebsocketTest.uproject` 文件。
2.  **自动编译**: 首次打开时，Unreal Engine可能会弹出窗口提示“Missing ... Modules... Would you like to rebuild them now?”。请点击 **“是” (Yes)**，编辑器将自动编译插件和项目代码。
3.  **运行游戏**: 项目成功加载后，点击编辑器顶部的 `运行` (Play) 按钮。

---

## 🏗️ 核心架构解析

本项目的架构设计遵循了**模块化**与**分层**的设计思想，尤其在聊天功能的实现上采用了经典的 **Controller/View** 模式。

### 1. 基础网络层：`BP_WSManagerComponent`
这是一个可重用的Actor组件，是整个网络通信的基石。
*   **职责**: 封装了所有底层的WebSocket操作，包括连接、断开、发送/接收原始数据、心跳和自动重连逻辑。
*   **设计优势**: 它不关心任何具体的业务逻辑（如聊天、同步等），只负责提供稳定可靠的通信通道。可以被“即插即用”地添加到任何需要网络功能的Actor上。

### 2. 业务逻辑层：`BP_Chat` (Controller)
这是一个放置在关卡中的Actor，作为聊天功能的“控制器”。
*   **职责**:
    1.  **集成组件**: 它自身**添加并持有了 `BP_WSManagerComponent` 组件**，利用其能力进行网络通信。
    2.  **处理逻辑**: 负责聊天相关的业务逻辑，例如：在收到UI的发送请求后，将文本包装成指定的JSON格式再发送；在从服务器收到消息后，解析JSON并处理。
    3.  **连接UI**: 作为UI层和网络层之间的桥梁，向UI提供简洁的接口（函数和事件）。

### 3. 表现层：`UI_Chat` (View)
这是一个UMG控件，是用户直接交互的界面。
*   **职责**:
    1.  **显示数据**: 负责将聊天消息显示在屏幕上。
    2.  **捕获输入**: 获取用户在输入框中键入的文本。
*   **交互方式**: `UI_Chat` **不直接与WebSocket组件通信**。当用户点击发送按钮时，它会调用 `BP_Chat` 提供的函数（如 `SendChatMessage`）；同时，它会绑定 `BP_Chat` 上的事件分发器（如 `OnChatMessageReceived`）来接收新消息并更新UI。

这种分层设计将**底层网络(Component)**、**业务逻辑(Controller)** 和 **界面显示(View)** 清晰地分离开来，使得每一部分都可以独立修改和扩展，极大地提高了项目的可维护性。

---

## ⚙️ 核心机制详解

### 1. 心跳机制 (Heartbeat)
通过 `Set Timer by Event` 定时（例如每5秒）向服务器发送一个 "ping" 消息，以防止网络设备（如路由器、防火墙）因连接“空闲”而将其断开。这是一种低开销、高效率的连接保活方式。

### 2. 断线自动重连 (Auto-Reconnect)
在 `OnClosed` 或`OnConnectionError`事件被触发，或者websocket超时，`BP_WSManagerComponent` 会自动启动一个延迟计时器 (`Set Timer by Event`)，在几秒后尝试重新连接服务器，极大地提升了用户体验。

### 3. JSON消息协议 (Protocol)
所有客户端与服务器之间的通信都遵循统一的JSON结构，以区分不同类型的消息：
```json
{
  "type": "chat",
  "payload": {
    "clientName": "Player123",
    "Message": "hello"
  }
}
```
服务器和客户端通过检查 `type` 字段来决定如何处理 `payload` 中的数据，这使得添加新类型的消息（如玩家位置同步、游戏事件等）变得非常简单。

---

## ✅ 如何测试（成果演示）

您可以按照PPT中`Slide 3: 最终成果演示`的流程来验证项目功能。

1.  **启动两个客户端**: 在UE编辑器的运行按钮旁边，将 `玩家数量` 设置为 `2`，然后点击 `New Editor Window(PIE)`运行。
2.  **自动连接**: 两个客户端窗口启动后，UI上应显示 "已连接" 或类似的状态信息。
3.  **实时聊天**: 在一个客户端的输入框中输入消息并发送。您会看到两个客户端的聊天窗口中都**立即**显示了这条消息。
4.  **断线重连测试**:
    *   保持UE客户端运行。
    *   切换到您之前打开的Node.js服务器终端，按 `Ctrl + C` 关闭服务器。
    *   观察UE客户端，UI应显示 "连接已断开，尝试重连中..."。
    *   在终端中再次运行 `node server.js` 重启服务器。
    *   观察UE客户端，它们应能自动恢复连接，并显示 "已重新连接"。

---

## 📄 许可证

本项目采用 [MIT License](LICENSE.md) 许可证。
