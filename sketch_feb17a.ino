#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

// Настройки датчиков
#define DHTPIN 7         // Подключение DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;

// Подключение TCS230/TCS3200
const int S2 = 8;      // Фильтр цвета
const int S3 = 9;      // Фильтр цвета
const int sensorOut = 5; // Частотный выход TCS230

// Подключение MQ-7
#define MQ7_ANALOG A7  // Аналоговый выход MQ-7
#define MQ7_DIGITAL 37 // Цифровой выход MQ-7

#define EXTRA_SENSOR 46 // Дополнительный цифровой вход на 42 пине

void setup() {
    Serial.begin(9600);  // Связь с Raspberry Pi через UART
    Wire.begin();        // Инициализация I2C

    dht.begin();         // Инициализация DHT11
    pinMode(31, INPUT);  // MQ-3 цифровой выход
    pinMode(MQ7_DIGITAL, INPUT); // MQ-7 цифровой выход
    pinMode(EXTRA_SENSOR, INPUT); // Инициализация 42 пина как вход

    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);

    if (!bmp.begin()) {
        Serial.println("Ошибка: BMP180 не найден!");
        while (1);
    }
    Serial.println("BMP180 найден!");
}

void loop() {
    // Чтение данных с датчиков
    int soilValue = analogRead(A0);     // Влажность почвы
    int mq3Analog = analogRead(A2);     // MQ-3 (аналоговый)
    int mq3Digital = digitalRead(31);   // MQ-3 (цифровой)
    int mq7Analog = analogRead(MQ7_ANALOG);  // MQ-7 (аналоговый)
    int mq7Digital = digitalRead(MQ7_DIGITAL);  // MQ-7 (цифровой)
    int extraSensorValue = digitalRead(EXTRA_SENSOR); // Чтение с 42 пина

    float dhtTemperature = dht.readTemperature();
    float dhtHumidity = dht.readHumidity();
    float bmpTemperature = bmp.readTemperature();  // Температура
    float pressure = bmp.readPressure() / 100.0;   // Давление
    float altitude = bmp.readAltitude();           // Высота

    // Проверяем корректность данных DHT11
    if (isnan(dhtTemperature) || isnan(dhtHumidity)) {
        dhtTemperature = -1;  // Ошибка датчика
        dhtHumidity = -1;
    }

    // Читаем цветовые данные с TCS230/TCS3200
    int red, green, blue;

    // Красный
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    red = pulseIn(sensorOut, LOW);

    // Зелёный
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    green = pulseIn(sensorOut, LOW);

    // Синий
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blue = pulseIn(sensorOut, LOW);

    // Нормализация значений в диапазон 0-255
    red = map(red, 500, 5000, 0, 255);
    green = map(green, 500, 5000, 0, 255);
    blue = map(blue, 500, 5000, 0, 255);

    // Ограничение значений в диапазоне 0-255
    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);

    // Формируем строку данных в CSV-формате и отправляем по UART
    Serial.print(soilValue);
    Serial.print(",");
    Serial.print(mq3Analog);
    Serial.print(",");
    Serial.print(mq3Digital);
    Serial.print(",");
    Serial.print(mq7Analog);
    Serial.print(",");
    Serial.print(mq7Digital);
    Serial.print(",");
    Serial.print(extraSensorValue);
    Serial.print(",");
    Serial.print(dhtTemperature);
    Serial.print(",");
    Serial.print(dhtHumidity);
    Serial.print(",");
    Serial.print(bmpTemperature);
    Serial.print(",");
    Serial.print(pressure);
    Serial.print(",");
    Serial.print(altitude);
    Serial.print(",");
    Serial.print(red);
    Serial.print(",");
    Serial.print(green);
    Serial.print(",");
    Serial.println(blue); // END строки

    delay(2000); // Задержка 2 секунды
}
