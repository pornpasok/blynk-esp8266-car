
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define BLYNK_TEMPLATE_ID "XXX"
#define BLYNK_TEMPLATE_NAME "XXX"
#define BLYNK_AUTH_TOKEN "XXX"

// Include the library files
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>



// Define the motor pins
#define IN1 D1
#define IN2 D2
#define IN3 D5
#define IN4 D6
#define IN5 D7
#define LIGHT D0

// Variables for the Blynk widget values
int fwd = 0;
int bwd = 0;
int left = 0;
int right = 0;
int speed = 255;
int light = 1;
int realspeed = 255;
int cleanspeed = 0;
int i = 0;

char auth[] = BLYNK_AUTH_TOKEN;  //Enter your Blynk auth token

#ifndef STASSID
#define STASSID "SookYenFarm"
#define STAPSK "XXX"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(115200);
  
  //Set the motor pins as output pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT, OUTPUT);

  // Stop motor
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  analogWrite(IN5, 0);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the Blynk library
  Blynk.begin(auth, ssid, password, "blynk.cloud", 80);
}

// Get the joystick values
BLYNK_WRITE(V0) {
  fwd = param.asInt();
}
// Get the joystick values
BLYNK_WRITE(V1) {
  bwd = param.asInt();
}
// Get the joystick values
BLYNK_WRITE(V2) {
  left = param.asInt();
}
// Get the joystick values
BLYNK_WRITE(V3) {
  right = param.asInt();
}
//Get the slider values speed
BLYNK_WRITE(V4) {
  cleanspeed = param.asInt();
}

void smartcar() {
  // Motor Clean Speed
  Serial.print("Motor Clean Speed: ");
  Serial.println(cleanspeed);
  analogWrite(IN5, cleanspeed);

  Serial.println("carForward");
  if (fwd == 1) {
    carForward();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("carForward");
  } else if (bwd == 1) {
    carBackward();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("carBackward");
  } else if (left == 1) {
    carLeft();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("carLeft");
  } else if (right == 1) {
    carRight();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("carRight");
  } else {
    carStop();
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("carstop");
  }
}

// Motor movement functions
void carForward() {
  //realspeed = (Speed*y)/100;
  realspeed = speed;
  Serial.print("realspeed: ");
  Serial.println(realspeed);
  //analogWrite(ENA, Speed);
  //analogWrite(ENB, Speed);
  analogWrite(IN1, realspeed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, realspeed);
}
void carBackward() {
  //realspeed = (Speed*(100-y))/100;
  realspeed = speed;
  Serial.print("realspeed: ");
  Serial.println(realspeed);
  //analogWrite(ENA, Speed);
  //analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  analogWrite(IN2, realspeed);
  analogWrite(IN3, realspeed);
  digitalWrite(IN4, LOW);
}
void carLeft() {
  //realspeed = (Speed*(100-x))/100;
  realspeed = speed;
  Serial.print("realspeed: ");
  Serial.println(realspeed);
  //analogWrite(ENA, Speed);
  //analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  analogWrite(IN2, realspeed);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, realspeed);
}
void carRight() {
  //realspeed = (Speed*x)/100;
  realspeed = speed;
  Serial.print("realspeed: ");
  Serial.println(realspeed);
  //analogWrite(ENA, Speed);
  //analogWrite(ENB, Speed);
  analogWrite(IN1, realspeed);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, realspeed);
  digitalWrite(IN4, LOW);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  ArduinoOTA.handle();

  Blynk.run();// Run the blynk function
  smartcar();// Call the main function
  
}

