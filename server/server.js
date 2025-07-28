// -----------------------------------------------------------------------------
//  UE5 WebSocket课程 - 配套Node.js服务器
//  使用 'ws' 库: https://github.com/websockets/ws
//
//  功能:
//  - 监听WebSocket连接
//  - 处理结构化的JSON消息
//  - 响应心跳(ping/pong)
//  - 将收到的聊天消息广播给所有连接的客户端
// -----------------------------------------------------------------------------

const WebSocket = require('ws');

const PORT = 8080; // 定义WebSocket服务器监听的端口

// 创建WebSocket服务器实例
const wss = new WebSocket.Server({ port: PORT, host: '0.0.0.0' });

// 模拟一个简单的玩家数据库
let playerData = {
    name: "Player123",
    score: 100
};

console.log('----------------------------------------------------');
console.log(`WebSocket Server is running on ws://localhost:${PORT}`);
console.log('----------------------------------------------------');

// 记录每个客户端的最后活跃时间
const clientActivityMap = new Map();
const TIMEOUT_MS = 10000; // 10秒无响应则判定超时

// 统一定时器，定期检查所有客户端
setInterval(() => {
    const now = Date.now();
    wss.clients.forEach((client) => {
        const lastActive = clientActivityMap.get(client) || 0;
        if (now - lastActive > TIMEOUT_MS) {
            console.log(`[Server] Client timeout, terminating connection. Remaining clients: ${wss.clients.size - 1}`);
            client.terminate();
            clientActivityMap.delete(client);
        } else {
            // 主动发送ping，促使客户端响应
            /*if (client.readyState === WebSocket.OPEN) {
                client.ping();
            }*/
        }
    });
}, 10000); // 每10秒检查一次

// 'connection'事件：当有新客户端连接时触发
wss.on('connection', (ws) => {
    console.log(`[Server] A new client connected. Remote Address: ${ws._socket.remoteAddress}, Total clients: ${wss.clients.size}`);

    // 新连接时记录活跃时间
    clientActivityMap.set(ws, Date.now());

    // 'message'事件：当收到来自该客户端的消息时触发
    ws.on('message', (message) => {
        let parsedMessage;
        try {
            // 尝试将收到的消息（Buffer）转为字符串，再解析为JSON
            parsedMessage = JSON.parse(message.toString());
        } catch (e) {
            console.log('[Server] Received non-JSON message:', message.toString());
            return;
        }

        // 收到消息时更新活跃时间
        clientActivityMap.set(ws, Date.now());

        console.log('[Server] Received:', JSON.stringify(parsedMessage));

        // 根据消息类型进行处理
        switch (parsedMessage.type) {
            case 'ping':
                // 如果是ping，立即回复pong
                const pongMessage = { type: 'pong', timestamp: Date.now() };
                ws.send(JSON.stringify(pongMessage));
                break;

            case 'chat':
                // 如果是聊天消息，将其广播给所有连接的客户端
                console.log('[Server] Broadcasting chat message...');
                // 我们将收到的消息原样转发，服务器不修改内容
                const broadcastMessage = JSON.stringify(parsedMessage);

                let broadcastCount = 0;
                wss.clients.forEach((client) => {
                    // 不要发给自己，且确保客户端处于打开状态再发送
                    if (client !== ws && client.readyState === WebSocket.OPEN) {
                        client.send(broadcastMessage);
                        broadcastCount++;
                    }
                });
                console.log(`[Server] Broadcasted chat message to ${broadcastCount} clients.`);
                break;

            // 处理获取玩家信息的请求
            case 'getProfile':
                const response = {
                    type: 'profileData',
                    payload: playerData
                };
                ws.send(JSON.stringify(response));
                console.log('[Server] Sent profile data to client.');
                break;

            // 处理更新玩家昵称的请求
            case 'updateName':
                if (parsedMessage.payload && parsedMessage.payload.name) {
                    playerData.name = parsedMessage.payload.name;
                    console.log(`[Server] Player name updated to: ${playerData.name}`);
                    // 可以在此广播给所有用户或仅发回确认消息
                }
                break;
            
            default:
                console.log(`[Server] Unknown message type: ${parsedMessage.type}`);
                break;
        }
    });

    // 收到pong时也更新活跃时间
    ws.on('pong', () => {
        clientActivityMap.set(ws, Date.now());
    });

    // 'close'事件：当该客户端连接断开时触发
    ws.on('close', () => {
        console.log('[Server] A client disconnected.');
    });

    // 'error'事件：当连接发生错误时触发
    ws.on('error', (error) => {
        console.error('[Server] WebSocket error:', error);
    });
});
