const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

console.log("WebSocket 服务端已启动在端口 8080...");

wss.on('connection', function connection(ws) {
    console.log("ESP32 已连接！");

    ws.on('message', function incoming(message) {
        try {
            const data = JSON.parse(message);
            console.log("--- 收到数据表 ---");
            console.table(data); // 像表格一样漂亮地打印 MPU6050 和传感器数据
        } catch (e) {
            console.log("收到原始数据: %s", message);
        }
    });

    ws.on('close', () => console.log("ESP32 断开连接"));
});
