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

//-- ADAFRUIT -----------------------------
/* #define IO_USERNAME "octomagician"
#define IO_KEY "aio_GHfA32Pk81exdR8lJI73kNFjuuCl" */

#define IO_USERNAME "Treckersillo"
#define IO_KEY "aio_eZoh40EOaQ9hrWLgMiEGua8C7yE0"

/* #define WIFI_SSID "examen_2"
#define WIFI_PASS "exameniot" */


#define WIFI_SSID "TheOffice2.4G"
#define WIFI_PASS "15TamalesDeRojo@"


// Inicializar el objeto de Adafruit IO con WiFi
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//se activa cuando Adafruit IO envía un mensaje al Arduino 
//Recibido <- [dato que llegó]
void handleMessage(AdafruitIO_Data *data) {
  Serial.print("Recibido <- ");
  Serial.println(data->value());
}

//---BOMBA-------------------------------------------------------------------
class Bomba {
  public:
    const int relayPin;
    String numero;

    Bomba(int logica, String num)
    : relayPin(logica), numero(num){
    }

    float kaboom() {
      pinMode(relayPin, OUTPUT); // Configura el pin del relé como salida
      digitalWrite(relayPin, HIGH);
    }
};
                  //logica, i
Bomba bomba[] = {Bomba(14, "1")}; 

//---TEMPERATURA-------------------------------------------------------------------
class Temperatura {
  public:
    const int thermoDO; //verde
    const int thermoCS; //amarillo
    const int thermoCLK; //morado
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
      float turbidityValue = (turbidityVoltage / maxVoltage) * 1000;  // Escalar a rango NTU
      return turbidityValue;
    }
};

// Instancia del sensor (para ADC de 12 bits)
                              //pin, i, maxV, refV, adc 12 bits
Turbidez turbidez[] = {Turbidez(35, "1", 4.5, 3.3, 4095)};

//---PH-------------------------------------------------------------------
class PH {
  public:
    const int phPin;  // Pin del sensor de pH
    String numero;    // Identificador del sensor
    const float refVoltage; // Voltaje de referencia del ADC (5V)

    PH(int pin, String num, float refV)
    : phPin(pin), numero(num), refVoltage(refV) {
      pinMode(phPin, INPUT);
    }

    float obtenerPH() {
      int phRaw = analogRead(phPin); // Leer valor analógico
      float pH = (phRaw / 4095.0) * refVoltage; // Convertir a voltaje
      return pH;
    }
};

                  //Pin, num, referencia
PH sensorPH[] = {PH(34,   "1",  5.0)};

//---ULTRASONICO-------------------------------------------------------------------
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
      return distancia;
    }
};


Ultrasonico ultrasonico[] = {
              //trig, echo, i
              //v,    a
    Ultrasonico(5,    18,   "1")
};

//---TDS-------------------------------------------------------------------
class TDS {
public:
    const int TDSPin;      // Pin analógico conectado al sensor TDS
    String numero;
    const float maxV;      // Máximo voltaje de salida del sensor
    const float refV;      // Voltaje de referencia del sensor
    const float cali;      // Calibración del sensor

    TDS(int pin, String num, float maxVoltage, float referenceVoltage, float calibration)
        : TDSPin(pin), numero(num), maxV(maxVoltage), refV(referenceVoltage), cali(calibration) {}

    float obtenerTDS() {
        int tdsRaw = analogRead(TDSPin);  // Leer valor analógico del sensor
        float tdsVoltage = (tdsRaw / 4095.0) * refV; // Calcula el voltaje basado en la referencia
        float tdsV = (tdsVoltage / maxV) * 1000 + cali; // Escala el voltaje a ppm con calibración
        return tdsV;
    }
};

 TDS tds[] = {
	//pin, i, maxV, refV, cali
        TDS(32, "1", 2.3, 3.3, 0.5)
    };