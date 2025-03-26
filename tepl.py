from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app, async_mode="eventlet", cors_allowed_origins="*")

# Храним актуальные данные с датчиков
sensor_data = {
    "soilValue": 0,
    "mq3Analog": 0,
    "mq3Digital": 0,
    "extraSensorValue": 0,
    "dhtTemperature": 0,
    "dhtHumidity": 0,
    "bmpTemperature": 0,
    "pressure": 0,
    "altitude": 0,
    "mq7Analog": 0,
    "mq7Digital": 0,
    "green": 0
}


@app.route('/')
def index():
    return render_template('tepl.html')


@app.route('/receive_data', methods=['POST'])
def receive_data():
    global sensor_data
    try:
        data = request.json
        if not isinstance(data, dict):
            return jsonify({"status": "error", "message": "Неверный формат данных"}), 400

        # Обновляем данные
        sensor_data.update(data)
        print(f"✅ Получены данные: {sensor_data}")

        # Отправляем данные всем клиентам
        socketio.emit('update_data', sensor_data)

        return jsonify({"status": "success", "message": "Данные обновлены!"})

    except Exception as e:
        return jsonify({"status": "error", "message": f"Ошибка сервера: {str(e)}"}), 500


@socketio.on('connect')
def handle_connect():
    print("🔗 Клиент подключился")
    socketio.emit('update_data', sensor_data)  # Отправляем актуальные данные при подключении


if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5050, debug=True)
