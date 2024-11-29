/* -----------------------------------------------------------
Recuerda que si tienes dudas de cómo funcionan los sensores,
puedes ver la documentación https://tinacosdocu.short.gy/main
------------------------------------------------------------*/
#include <AdafruitIO_WiFi.h>
#include <max6675.h> //para el sensor de temperatura

//variables externas los feeds
extern AdafruitIO_Feed *TemperaturaFeed;
extern AdafruitIO_Feed *TurbidezFeed;
extern AdafruitIO_Feed *PHFeed;
extern AdafruitIO_Feed *UltrasonicoFeed;
extern AdafruitIO_Feed *TDSFeed;

//variables para que agarre el shutdown
String inputString = "";
boolean stringComplete = false;
String commandString = "";
boolean isConnected = false;

//tds
#define TDS_REF_VOLTAGE 3.3
#define TDS_MAX_VOLTAGE 2.3
#define TDS_CALIBRATION 0.5

//-- ADAFRUIT -----------------------------
#define IO_USERNAME "octomagician"
#define IO_KEY "aio_QwgH65hnjTq2yQJMPXOcRiX3jWnQ"

#define WIFI_SSID "examen_2"
#define WIFI_PASS "exameniot"

// Inicializar el objeto de Adafruit IO con WiFi
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//se activa cuando Adafruit IO envía un mensaje al Arduino 
//Recibido <- [dato que llegó]
void handleMessage(AdafruitIO_Data *data) {
  Serial.print("Recibido <- ");
  Serial.println(data->value());
}

//---TEMPERATURA-------------------------------------------------------------------
class Temperatura {
  public:
    const int thermoDO;
    const int thermoCS;
    const int thermoCLK;
    String numero;
    MAX6675 thermocouple;

    Temperatura(int DO, int CS, int CLK, String num)
    : thermoDO(DO), thermoCS(CS), thermoCLK(CLK), numero(num), thermocouple(CLK, CS, DO) {
    }

    float leerTemperatura() {
      return thermocouple.readCelsius();
    }
};
                                             //DO, CS, CLK, i
Temperatura temperatura[] = {Temperatura(19, 21, 22, "1")}; 

//---TURBIDEZ-------------------------------------------------------------------
class Turbidez {
  public:
    const int turbPin;          // Pin analógico conectado al sensor
    const String numero;        // Identificador del sensor
    const float refVoltage;     // Voltaje de referencia del ADC (en V)
    const float maxVoltage;     // Voltaje máximo del sensor (en V)
    const int adcResolution;    // Resolución del ADC (por ejemplo, 4095 o 1023)

    // Constructor
    Turbidez(int pin, String num, float maxV, float refV, int adcMax)
    : turbPin(pin), numero(num), maxVoltage(maxV), refVoltage(refV), adcResolution(adcMax) {}

    float obtenerTurbi() {
      int turbidityRaw = analogRead(turbPin); // Leer valor ADC
      float turbidityVoltage = (turbidityRaw / (float)adcResolution) * refVoltage; // Convertir a voltaje
      float turbidityValue = (turbidityVoltage / maxVoltage) * 1000;  // Escalar a rango arbitrario
      return turbidityValue;
    }
};

// Instancia del sensor (para ADC de 12 bits)
                              //pin, i, maxV, refV, adc 12 bits
Turbidez turbidez[] = {Turbidez(35, "1", 4.5, 3.3, 4095)};

//---PH-------------------------------------------------------------------
class PH {
  public:
    const int phPin;
    String numero;

  PH(int pin, String num) 
  : phPin(pin), numero(num) {
    pinMode(phPin, INPUT);
  }

  float obtenerPH() {
    int phValue = analogRead(phPin);
    float voltage = phValue * (5.0 / 1023.0);
    float pH = 3.5 * voltage;
    return pH;
  }
};

PH sensorPH[] = {PH(34, "1")};

//---PH-------------------------------------------------------------------

class Ultrasonico { //nivel de agua JSN-SR04T-2.0
  public:
    const int tPin;
    const int ePin;
    String numero;

    Ultrasonico(int trigPin, int echoPin, String num) 
    : tPin(trigPin), ePin(echoPin), numero(num) {
    }

    long obtenerDistancia() {
      long duracion, distancia;

      // trigger
      digitalWrite(tPin, LOW);
      delayMicroseconds(2);
      digitalWrite(tPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(tPin, LOW);

      // eco
      pinMode(ePin, INPUT);
      duracion = pulseIn(ePin, HIGH);

      distancia = duracion * 0.034 / 2;
      Serial.print("SON"); Serial.print(numero); Serial.print(":");
      return distancia;
    }
};


Ultrasonico ultrasonico[] = {
    Ultrasonico(5, 18, "1")
};
#define TDS_REF_VOLTAGE 3.3
#define TDS_MAX_VOLTAGE 2.3
#define TDS_CALIBRATION 0.5

//TDS
class TDS {
  public:
    const int TDSPin;
    String numero;
    const int maxV;
    const int refV;
    const int cali;

  TDS(int pin, String num, int max, int ref, int cali)
  : TDSPin(pin), numero(num), maxV(max), refV(ref), cali(cali){}

    float obtenerTDS() {
    int tdsRaw = analogRead(TDSPin);
    float tdsVoltage = (tdsRaw / 4095.0) * TDS_REF_VOLTAGE;
    float tdsV = (tdsVoltage / TDS_MAX_VOLTAGE) * 1000 + TDS_CALIBRATION;
    return tdsV;
  }
};

TDS tds[] = {
  TDS(32, "1", 2.3, 3.3, 0.5)
};

