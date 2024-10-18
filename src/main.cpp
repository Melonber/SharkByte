#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WiFi.h>

/* Uncomment the initialize the I2C address, uncomment only one. If you get a totally blank screen try the other */
#define i2c_Address 0x3C // Initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3D // Initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin, -1 if not used

#define MIDDLE_BUTTON 0
#define DOWN_BUTTON 1
#define UP_BUTTON 2

#define RECIEVER_PIN 4
IRrecv irrecv(RECIEVER_PIN);
decode_results results;

// Создаем объект дисплея
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int currentSelection = 0;
const int menuItems = 4;

unsigned long lastReceivedValue = 0; // Для хранения последнего полученного значения

int selectedNetwork = 0; // Индекс выбранной сети
int totalNetworks = 0;   // Количество доступных Wi-Fi сетей

bool debounceButton(int pin) {
  static unsigned long lastPress = 0; // Время последнего нажатия
  if (digitalRead(pin) == LOW && millis() - lastPress > 200) { // Проверка нажатия и временной интервал
    lastPress = millis(); // Обновление времени последнего нажатия
    return true; // Кнопка нажата
  }
  return false; // Кнопка не нажата
}

void initializeButtons() {
  pinMode(MIDDLE_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
}

void GUI_menu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);

  display.setCursor(0, 0);
  if (currentSelection == 0) {
    display.print(F("1)WiFi Networks<"));
  } else {
    display.print(F("1)WiFi Networks"));
  }

  display.setCursor(0, 16);
  if (currentSelection == 1) {
    display.print(F("2)Send IR Signal<"));
  } else {
    display.print(F("2)Send IR Signal"));
  }

  display.setCursor(0, 32);
  if (currentSelection == 2) {
    display.print(F("3)Display IR Signals<"));
  } else {
    display.print(F("3)Display IR Signals"));
  }
  display.setCursor(0, 48);
  if (currentSelection == 3) {
    display.print(F("4)Fourth Option<"));
  } else {
    display.print(F("4)Fourth Option"));
  }
  display.display();
}

void show_wifi() {
  // Сканируем сети
  totalNetworks = WiFi.scanNetworks();
  Serial.println("Scanning for WiFi networks...");
  
  if (totalNetworks == 0) {
    Serial.println("No networks found");
    return;
  }

  Serial.print("Found ");
  Serial.print(totalNetworks);
  Serial.println(" networks.");

  int displayLimit = 4; // Количество сетей для отображения на экране одновременно
  
  while (true) {
    // Отображаем сети на экране
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(1);
    
    for (int i = 0; i < displayLimit; i++) {
      int networkIndex = (selectedNetwork + i) % totalNetworks;
      display.setCursor(0, i * 10); // Смещаем текст вниз на 10 пикселей
      display.print(WiFi.SSID(networkIndex)); // Выводим название сети
      if (networkIndex == selectedNetwork) {
        display.print(" <"); // Выделяем выбранную сеть
      }
    }
    
    display.display();

    // Проверяем нажатие кнопок
    if (debounceButton(DOWN_BUTTON)) {
      selectedNetwork++;
      if (selectedNetwork >= totalNetworks) {
        selectedNetwork = 0; // Зацикливаем скроллинг
      }
    }
    
    if (debounceButton(UP_BUTTON)) {
      if (selectedNetwork == 0) {
        selectedNetwork = totalNetworks - 1; // Зацикливаем вверх
      } else {
        selectedNetwork--;
      }
    }
    
    // Возврат в GUI при нажатии на Middle Button
    if (debounceButton(MIDDLE_BUTTON)) {
      break; // Выход из функции и возврат в GUI
    }
    
    delay(200); // Небольшая задержка для отрисовки
  }

  display.clearDisplay();
  display.display(); // Очищаем экран после выхода из режима отображения Wi-Fi сетей
}

void send_IR() {
  // 
}

void display_IR() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);
  
  while (true) {
    if (irrecv.decode(&results)) {  // Если получен сигнал
      display.clearDisplay(); // Очистка экрана перед выводом нового сигнала
      display.setCursor(0, 0);
      
      // Выводим протокол
      display.print(resultToHumanReadableBasic(&results));
      display.println();
      
      // Выводим код в формате HEX
      //display.print("Received HEX: 0x");
      display.print(results.value, HEX); 
      display.display();
      
      irrecv.resume();  // Готовим приемник к следующему сигналу
    } else {
      display.setCursor(65, 55);
      display.print("Listening");
      display.display();
    }

    // Выход из режима отображения сигналов, если нажата кнопка
    if (debounceButton(MIDDLE_BUTTON)) {
      break; // Выход из функции
    }
    delay(100); // Задержка, чтобы не перегружать дисплей
  }
}

void handleSelection() {
  display.clearDisplay(); // Очистка экрана

  display.setCursor(0, 0); // Устанавливаем курсор в начало

  if (currentSelection == 0) {
    show_wifi();
  } else if (currentSelection == 1) {
    send_IR();
  } else if (currentSelection == 2) {
    display_IR(); // Переход к отображению сигналов
  }

  display.display(); // Обновляем экран
}

void say_hello() {
  display.clearDisplay();
  display.setTextSize(1); // Установка размера текста
  display.setTextColor(1); // Установка цвета текста

  // Текст для первой и второй строки
  const char* line1 = "Welcome to";
  const char* line2 = "SharkByte !";

  // Установим высоту шрифта для текста размера 1
  int textHeight = 8; // Высота шрифта для размера 1 (может варьироваться)
  
  // Вычисляем ширину текста
  int16_t x1 = (SCREEN_WIDTH - strlen(line1) * 6) / 2; // X координата для первой строки
  int16_t x2 = (SCREEN_WIDTH - strlen(line2) * 6) / 2; // X координата для второй строки
  int16_t y1 = 32; // Y координата для первой строки
  int16_t y2 = y1 + textHeight + 5; // Y координата для второй строки (высота текста + 5 пикселей)

  // Устанавливаем курсор и выводим текст
  display.setCursor(x1, y1);
  display.println(line1);
  display.setCursor(x2, y2);
  display.println(line2);

  // Обновляем дисплей
  display.display();
  delay(3000);

  // Очистка дисплея
  display.clearDisplay();
}

void setup() {
  Serial.begin(115200);
  initializeButtons();
  WiFi.mode(WIFI_STA);
  Serial.println("Инициализация дисплея...");
  if (!display.begin(i2c_Address)) {
    Serial.println("Не удалось инициализировать дисплей.");
    while (1);
  }
  Serial.println("Дисплей инициализирован.");

  // Инициализация ИК-приёмника
  irrecv.enableIRIn(); // Включаем ИК-приёмник

  say_hello();
  GUI_menu();
}

void loop() {
  if (debounceButton(MIDDLE_BUTTON)) {
    Serial.println("Middle pressed!");
    handleSelection();
  }
  
  if (debounceButton(DOWN_BUTTON)) {
    Serial.println("Down pressed!");
    currentSelection = (currentSelection + 1) % menuItems;
    GUI_menu();
  } 
  
  if (debounceButton(UP_BUTTON)) {
    Serial.println("Up pressed !");
    currentSelection = (currentSelection - 1 + menuItems) % menuItems;
    GUI_menu();
  }
}