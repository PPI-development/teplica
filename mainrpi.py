import serial
import requests
import time

# Подключение к UART
ser = serial.Serial('/dev/serial0', 9600, timeout=1)
server_url = "http://192.168.1.100:5050/receive_data"  # ⚠️ ЗАМЕНИ НА IP СЕРВЕРА!

def read_arduino():
    try:
        line = ser.readline().decode('utf-8').strip()  # Читаем строку и убираем пробелы
        if line:
            values = line.split(",")  # Разбиваем CSV
            if len(values) == 8:  # Должно быть **ровно 8 значений**
                try:
                    data = {
                        "soilValue": int(values[0]),       # Влажность почвы
                        "mq3Analog": int(values[1]),       # Газ MQ-3 (аналог)
                        "mq3Digital": int(values[2]),      # Газ MQ-3 (цифровой)
                        "dhtTemperature": float(values[3]),  # Температура DHT11
                        "dhtHumidity": float(values[4]),     # Влажность DHT11
                        "bmpTemperature": float(values[5]),  # Температура BMP180
                        "pressure": float(values[6]),        # Давление BMP180
                        "altitude": float(values[7])         # Высота BMP180
                    }
                    return data
                except ValueError:
                    print(f"❌ Ошибка преобразования данных: {line}")
            else:
                print(f"❌ Ошибка формата данных от Arduino (не 8 значений): {line}")
        return None
    except Exception as e:
        print(f"❌ Ошибка UART: {e}")
        return None

while True:
    data = read_arduino()
    if data:
        print(f"📡 Отправка данных: {data}")
        try:
            response = requests.post(server_url, json=data)
            print(f"✅ Ответ сервера: {response.status_code}, {response.text}")
        except Exception as e:
            print(f"❌ Ошибка при отправке на сервер: {e}")
    time.sleep(2)  # Задержка 2 секунды
