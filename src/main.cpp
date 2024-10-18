#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address, uncomment only one. If you get a totally blank screen try the other */
#define i2c_Address 0x3C // Initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3D // Initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin, -1 if not used

// Создаем объект дисплея
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Начинаем последовательную связь для отладки
  Serial.begin(115200);
  Serial.println("Инициализация дисплея...");

  // Инициализация дисплея
  if(!display.begin(i2c_Address)) {
    Serial.println("Не удалось инициализировать дисплей. Проверьте соединения.");
    while(1); // Зацикливаемся в случае ошибки
  }

  Serial.println("Дисплей инициализирован.");

  // Очистка дисплея
  display.clearDisplay();

  // Установка шрифта
  display.setTextSize(1);      // Устанавливаем размер текста
  display.setTextColor(1);     // Устанавливаем цвет текста (1 = белый)
  display.setCursor(0, 0);     // Устанавливаем курсор в верхний левый угол

  // Выводим текст на дисплей
  display.println("Hello, SH1106!");
  display.println("Display Test");
  
  // Отправляем данные на дисплей
  display.display();
}

void loop() {
  // Здесь можно добавить логику для обновления экрана
}