#include <max6675.h> //temperatura

#define TURBIDITY_REF_VOLTAGE 3.3
#define TURBIDITY_MAX_VOLTAGE 4.5
#define TDS_REF_VOLTAGE 3.3
#define TDS_MAX_VOLTAGE 2.3
#define TDS_CALIBRATION 0.5

String inputString = "";
boolean stringComplete = false;
String commandString = "";
boolean isConnected = false;

//-------------------------------------------------------------------

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

//-------------------------------------------------------------------

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

//-------------------------------------------------------------------

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

//-------------------------------------------------------------------

class Turbidez {
  public:
    const int turbPin;
    String numero;
    const int maxV;
    const int refV;

  Turbidez(int pin, String num, int max, int ref)
  : turbPin(pin), numero(num), maxV(max), refV(ref) {}

    float obtenerTurbi() {
    int turbidityRaw = analogRead(turbPin);
    float turbidityVoltage = (turbidityRaw / 4095.0) * TURBIDITY_REF_VOLTAGE;
    float turbidityValue = (turbidityVoltage / TURBIDITY_MAX_VOLTAGE) * 1000;
    return turbidityValue;
  }
};

//-------------------------------------------------------------------

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

//-------------------------------------------------------------------
Ultrasonico ultrasonico[] = {
  Ultrasonico(5, 18, "1")
};

Temperatura temperatura[] = {
  Temperatura(19, 21, 22, "1")
};

PH sensorPH[] = {
  PH(34, "1")
};

Turbidez turbidez[] = {
  Turbidez(35, "1", 4.5, 3.3)
};

TDS tds[] = {
  TDS(32, "1", 2.3, 3.3, 0.5)
};

//-------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < sizeof(ultrasonico) / sizeof(ultrasonico[0]); i++) {
    pinMode(ultrasonico[i].tPin, OUTPUT);
    pinMode(ultrasonico[i].ePin, INPUT);
  }
}

//-------------------------------------------------------------------

void loop() {
  //descomentar el if is coneected para poder leerlo fuera del programa de c#, quitando lo de star/stop
/*   if (isConnected){ */
    // Lectura del nivel de agua
    for (int i = 0; i < sizeof(ultrasonico) / sizeof(ultrasonico[0]); i++) {
      long distancia = ultrasonico[i].obtenerDistancia();
      Serial.println(distancia);
      delay(1000);
    }

    // Lectura de temperatura
    for (int i = 0; i < sizeof(temperatura) / sizeof(temperatura[0]); i++) {
      float temp = temperatura[i].leerTemperatura();
      Serial.print("TEM"); Serial.print(temperatura[i].numero); Serial.print(":");
      Serial.println(temp);
      delay(2000);
    }

    // Lectura de pH
     for (int i = 0; i < sizeof(sensorPH) / sizeof(sensorPH[0]); i++) {
      float pH = sensorPH[i].obtenerPH();
      Serial.print("SPH"); Serial.print(sensorPH[i].numero); Serial.print(":");
      Serial.println(pH);
      delay(2000);
    }

    // Lectura de Turbidez
     for (int i = 0; i < sizeof(turbidez) / sizeof(turbidez[0]); i++) {
      float turbi = turbidez[i].obtenerTurbi();
      Serial.print("TUR"); Serial.print(turbidez[i].numero); Serial.print(":");
      Serial.println(turbi);
      delay(2000);
    }

    // Lectura de TDS
     for (int i = 0; i < sizeof(tds) / sizeof(tds[0]); i++) {
      float TDS = tds[i].obtenerTDS();
      Serial.print("TDS"); Serial.print(tds[i].numero); Serial.print(":");
      Serial.println(TDS);
      delay(2000);
    }
/*   } */
}

//-------------------------------------------------------------------

// Detección de inicio de datos por puerto serie
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '#') { //inicio
      inputString = ""; // Reinicia
    }
    inputString += inChar;
    if (inChar == '\n') { //final
      stringComplete = true;
    }
  }
}

// Extraer y procesar inicio y fin de datos
void getCommand() {
  commandString = inputString.substring(1, inputString.length() - 1); // Elimina # y \n

  if (commandString.equals("STAR")) {
    isConnected = true;
    Serial.println("Conexión iniciada.");
  }
  else if (commandString.equals("STOP")) {
    isConnected = false;
    Serial.println("Conexión detenida.");
  }
  else {
    Serial.println("Comando desconocido: " + commandString);
  }
}
