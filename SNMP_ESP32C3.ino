#define TTGO true


#include <WiFi.h>
//#include <WiFiUdp.h>
//#include <Adafruit_SHT4x.h>
#include <Wire.h>
#include <SNMP_Agent.h>

#include <ClosedCube_HDC1080.h>
#if TTGO
#include <TFT_eSPI.h>
#endif
// WIFI credentials
const char* SSID = "gc64test";
const char* W_PWD = "aow897t43u82o8";


#define I2C_SCL_PIN 22
#define I2C_SDA_PIN 21
//#define SHT40_ADDR 0x44


#if TTGO
#define BTN_1 35
#define BTN_2 0
TFT_eSPI Tft = TFT_eSPI();
#endif

//Adafruit_SHT4x sht4 = Adafruit_SHT4x();
ClosedCube_HDC1080 hdc1080;
SNMPAgent _SNMP("public");
WiFiUDP UDP;

// Initializing variables
unsigned long UptimeStart = 0;
float Temp;
float Hum;

// Setting up some constants for the OIDs
const char* Temperature_OID = ".1.3.6.1.4.1.64.1";
const char* Humidity_OID = ".1.3.6.1.4.1.64.2";
const char* Uptime_OID = ".1.3.6.1.4.1.64.0";


#if TTGO
unsigned long ScreensaverTimer = 0;
int Menu = 0;  // 0: IP, Mac, Uptime; 1: Temp, Humidity
bool RefreshScreen = false;
bool b_BTN1_Pressed = false;
bool b_BTN2_Pressed = false;

// Preparing interrupts to manage the screens
void IRAM_ATTR h_BTN_1() {
  b_BTN1_Pressed = true;
}
void IRAM_ATTR h_BTN_2() {
  b_BTN2_Pressed = true;
}
#endif


void setup() {
  Serial.begin(115200);
  /*
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  */

  hdc1080.begin(0x40);

#if TTGO
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(TFT_BL, OUTPUT);
  attachInterrupt(BTN_1, h_BTN_1, FALLING);
  attachInterrupt(BTN_2, h_BTN_2, FALLING);
  InitScreen();
#endif
  if (ConnectWifi()) {
    SetupSNMP();
  }
  UptimeStart = millis();
}


#if TTGO
void SetBrightness(int _Value) {
  analogWrite(TFT_BL, _Value);
}

void InitScreen() {
  Tft.init();
  Tft.setRotation(1);
  Tft.fillScreen(TFT_BLACK);
  Tft.setTextColor(TFT_GREEN);
  Tft.setTextSize(2);
  SetBrightness(128);
}

void BtnControll() {
  if (b_BTN1_Pressed) {
    b_BTN1_Pressed = false;
    Menu = 0;
    ShowInfo();
    ScreensaverTimer = 0;
  }
  if (b_BTN2_Pressed) {
    b_BTN2_Pressed = false;
    Menu = 1;
    ShowInfo();
    ScreensaverTimer = 0;
  }
}

void Screensaver() {
  if (ScreensaverTimer <= 10) {
    ScreensaverTimer++;
    SetBrightness(128);
  } else {
    SetBrightness(1);
  }
}

void ShowInfo() {
  Tft.fillScreen(TFT_BLACK);
  Tft.setCursor(0, 0);
  Tft.setTextColor(TFT_GREEN);

  if (Menu == 0) {
    Tft.printf("IP:\n%s\n", WiFi.localIP().toString());
    Tft.setTextColor(TFT_WHITE);
    Tft.println("MAC:");
    Tft.println(WiFi.macAddress());
    Tft.setTextColor(TFT_BLUE);
    Tft.printf("Uptime:\n%lus\n", ((millis() - UptimeStart) / 1000));
  } else if (Menu == 1) {
    Tft.printf("Temp:\n%.*f C\n", 3, ReadTemperature());
    Tft.setTextColor(TFT_RED);
    Tft.printf("Humidity:\n%.*f", 3, ReadHumidity());
    Tft.print("%\n");
  } else {
    Menu = 0;
  }
}

void ClearLine() {
  int X = Tft.getCursorX();
  int Y = Tft.getCursorY();
  Tft.setCursor(X, Y);
  Tft.setTextColor(TFT_WHITE, TFT_BLACK);
  Tft.println("                   ");
  Tft.setCursor(X, Y);
}

#endif

void loop() {
  _SNMP.loop();
  GetHDC1080SensorData();
#if TTGO
  ShowInfo();
  BtnControll();
  Screensaver();
#else
  ShowInfoSerial();
#endif

  delay(1000);
}

bool ConnectWifi() {
  WiFi.begin(SSID, W_PWD);
  unsigned long startAttemptTime = millis();

  // Wait for connection, but not more than 10 seconds
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(1000);
    Serial.println("Connecting to WIFI");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WIFI!");
#if TTGO
    Tft.fillScreen(TFT_BLACK);
    Tft.setCursor(0, 0);
    Tft.println("Connected to WIFI!");
#endif
    delay(1000);
    return true;
  } else {
    Serial.println("Failed to connect to WIFI!");
#if TTGO
    Tft.fillScreen(TFT_BLACK);
    Tft.setCursor(0, 0);
    Tft.println("Failed to connect to WIFI!");
#endif
    return false;
  }
}

float ReadTemperature() {
  /* sensors_event_t temp, humidity;
  sht4.getEvent(&humidity, &temp);
  return temp.temperature;*/
  Serial.println(FloatToInt(Temp));
  return Temp;
}

float ReadHumidity() {
  /* sensors_event_t temp, humidity;
  sht4.getEvent(&humidity, &temp);
  return humidity.relative_humidity;*/
  return Hum;
}

void ShowInfoSerial() {
  CLS();


  Serial.printf("IP: %s\n\r", WiFi.localIP().toString());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Uptime: %lus\n\r", ((millis() - UptimeStart) / 1000));
  Serial.printf("Temp: %f C\n\r",
                ReadTemperature());
  Serial.printf("Humidity: %f", ReadHumidity());
  Serial.print("%\n\r");
}

void GetHDC1080SensorData() {
  /**
  * Resolution types:
  * - HDC1080_RESOLUTION_8BIT
  * - HDC1080_RESOLUTION_11BIT
  * - HDC1080_RESOLUTION_14BIT
  **/
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  HDC1080_Registers reg = hdc1080.readRegister();
  Temp = hdc1080.readTemperature();
  Hum = hdc1080.readHumidity();
}

void CLS() {
  Serial.print("\033[0H\033[0J");
}

int FloatToInt(float Value) {
  return static_cast<int>(Value * 1000);
}

int DynTemp() {
  return FloatToInt(Temp);
}

int DynUptime() {
  return ((millis() - UptimeStart) / 1000);
}

int DynHumidity() {
  return FloatToInt(Hum);
}

void SetupSNMP() {
  Serial.println(FloatToInt(Temp));
  _SNMP.setUDP(&UDP);
  _SNMP.begin();
  _SNMP.addDynamicIntegerHandler(Uptime_OID, DynUptime);
  _SNMP.addDynamicIntegerHandler(Temperature_OID, DynTemp);
  _SNMP.addDynamicIntegerHandler(Humidity_OID, DynHumidity);

  _SNMP.sortHandlers();
}
