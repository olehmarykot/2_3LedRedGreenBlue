#include <Arduino.h>

// ---------- Піни ----------

constexpr uint8_t RED_PIN = 4;
constexpr uint8_t GREEN_PIN = 5;
constexpr uint8_t BLUE_PIN = 6;
constexpr uint8_t BUTTON_PIN = 7;

// ---------- Інтервали, мс ----------

constexpr uint32_t RED_INTERVAL = 200;
constexpr uint32_t GREEN_INTERVAL = 500;
constexpr uint32_t BLUE_INTERVAL = 1000;
constexpr uint32_t DEBOUNCE_INTERVAL = 50;

// ---------- Режими ----------

enum class Mode {
    All,
    Red,
    Green,
    Blue
};

Mode currentMode = Mode::All;

// ---------- Стани світлодіодів ----------

bool redState = false;
bool greenState = false;
bool blueState = false;

// ---------- Таймери світлодіодів ----------

uint32_t redPreviousMillis = 0;
uint32_t greenPreviousMillis = 0;
uint32_t bluePreviousMillis = 0;

// ---------- Кнопка ----------

int previousReading = HIGH;
int stableButtonState = HIGH;

uint32_t lastButtonChange = 0;

// ---------- Перемикання режиму ----------

void nextMode() {
    switch (currentMode) {
        case Mode::All:
            currentMode = Mode::Red;
            Serial.println("Mode: Red");
            break;

        case Mode::Red:
            currentMode = Mode::Green;
            Serial.println("Mode: Green");
            break;

        case Mode::Green:
            currentMode = Mode::Blue;
            Serial.println("Mode: Blue");
            break;

        case Mode::Blue:
            currentMode = Mode::All;
            Serial.println("Mode: All");
            break;
    }
}

// ---------- Перевірка кнопки ----------

void updateButton(uint32_t now) {
    int reading = digitalRead(BUTTON_PIN);

    // Кожна зміна сигналу починає відлік заново
    if (reading != previousReading) {
        lastButtonChange = now;
        previousReading = reading;
    }

    // Приймаємо стан лише після 50 мс без змін
    if (now - lastButtonChange >= DEBOUNCE_INTERVAL) {
        if (reading != stableButtonState) {
            stableButtonState = reading;

            // Перемикаємо режим тільки при натисканні
            if (stableButtonState == LOW) {
                Serial.println("Button: pressed");
                nextMode();
            }
            else {
                Serial.println("Button: released");
            }
        }
    }
}

// ---------- Початкові налаштування ----------

void setup() {
    Serial.begin(115200);

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.println("ESP32-S3 started");
    Serial.println("Mode: All");
}

// ---------- Головний цикл ----------

void loop() {
    uint32_t now = millis();

    updateButton(now);

    // ---------- Червоний ----------

    if (currentMode == Mode::All || currentMode == Mode::Red) {
        if (now - redPreviousMillis >= RED_INTERVAL) {
            redPreviousMillis = now;
            redState = !redState;
            digitalWrite(RED_PIN, redState);
        }
    } else {
        redState = false;
        digitalWrite(RED_PIN, LOW);
        redPreviousMillis = now;
    }

    // ---------- Зелений ----------

    if (currentMode == Mode::All || currentMode == Mode::Green) {
        if (now - greenPreviousMillis >= GREEN_INTERVAL) {
            greenPreviousMillis = now;
            greenState = !greenState;
            digitalWrite(GREEN_PIN, greenState);
        }
    } else {
        greenState = false;
        digitalWrite(GREEN_PIN, LOW);
        greenPreviousMillis = now;
    }

    // ---------- Синій ----------

    if (currentMode == Mode::All || currentMode == Mode::Blue) {
        if (now - bluePreviousMillis >= BLUE_INTERVAL) {
            bluePreviousMillis = now;
            blueState = !blueState;
            digitalWrite(BLUE_PIN, blueState);
        }
    } else {
        blueState = false;
        digitalWrite(BLUE_PIN, LOW);
        bluePreviousMillis = now;
    }
}
