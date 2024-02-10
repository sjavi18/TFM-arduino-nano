#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <AdafruitIO_WiFi.h>

#define IO_USERNAME "username"
#define IO_KEY "key_username"
#define WIFI_SSID "nombre_red"
#define WIFI_PASS "contrasena_red"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *humidityFeed = io.feed("humidity");
AdafruitIO_Feed *motionFeed = io.feed("motion");

#define MOTION_PIN 2 // Pin digital conectado al sensor de movimiento PIR

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(10);

  Wire.begin();

  if (!aht.begin()) {
    Serial.println("Error al iniciar el sensor de temperatura y humedad!");
    while (1);
  }

  pinMode(MOTION_PIN, INPUT);

  Serial.println("Conectando a Adafruit IO...");
  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Conectado!");
}

void loop() {
  io.run();

  // Lectura de temperatura y humedad
  sensors_event_t temp_event, humidity_event;
  aht.getEvent(&temp_event, &humidity_event);
  if (isnan(temp_event.temperature) || isnan(humidity_event.relative_humidity)) {
    Serial.println("Error al leer el sensor!");
  } else {
    float temperature = temp_event.temperature;
    float humidity = humidity_event.relative_humidity;
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumedad: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Enviar datos al feed de temperatura y humedad
    temperatureFeed->save(temperature);
    humidityFeed->save(humidity);
  }

  // Lectura del sensor de movimiento
  int motionDetected = digitalRead(MOTION_PIN);
  if (motionDetected == HIGH) {
    Serial.println("Movimiento detectado!");
    // Enviar datos al feed de movimiento
    motionFeed->save("Movimiento detectado!");
  } else {
    Serial.println("Sin movimiento");
    // Enviar datos al feed de movimiento
    motionFeed->save("Sin movimiento");
  }

  delay(5000); // Espera 5 segundos antes de volver a leer los sensores
}
