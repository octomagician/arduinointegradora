//lógica central e inicialización
#include "config.h"

//a dónde va a mandar con respecto al adafruit
//los feeds del Adafruit empiezan vacíos
AdafruitIO_Feed *TemperaturaFeed = nullptr;
AdafruitIO_Feed *TurbidezFeed = nullptr;
AdafruitIO_Feed *PHFeed = nullptr;
AdafruitIO_Feed *UltrasonicoFeed = nullptr;
AdafruitIO_Feed *TDSFeed = nullptr;

void setup() {
  Serial.begin(115200);
  while (!Serial); //hace que el programa espere hasta que la compu esté lista para escucharlo

  Serial.print("START");
  io.connect(); //iniciear sesión en adafruit

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500); //muestra puntitos en lo que está completamente conectado al Adafruit
  }

  Serial.println();
  Serial.println(io.statusText());

  // inicia los feeds
  TemperaturaFeed = io.feed("Temperatura");
  TurbidezFeed = io.feed("Turbidez");
  PHFeed = io.feed("PH");
  UltrasonicoFeed = io.feed("Ultrasonico");
  TDSFeed = io.feed("TDS");

  //"si alguien envía info a este feed, llama una función para manejarlo"
  TemperaturaFeed->onMessage(handleMessage);
  TurbidezFeed->onMessage(handleMessage);
  PHFeed->onMessage(handleMessage);
  UltrasonicoFeed->onMessage(handleMessage);
  TDSFeed->onMessage(handleMessage);

  TemperaturaFeed->get();
  TurbidezFeed->get();
  PHFeed->get();
  UltrasonicoFeed->get();
  TDSFeed->get();
}

void loop() {
  io.run();

    //descomentar el if is coneected para poder leerlo fuera del programa de c#, quitando lo de star/stop
/*   if (isConnected){ */

  //inician impresión de lecturas en formato del protocolo
  //---TEMPERATURA-------------------------------------------------------------------
 for (int i = 0; i < sizeof(temperatura) / sizeof(temperatura[0]); i++) {
    float temp = temperatura[i].leerTemperatura();

    //esto es para validar si se está guardando o algo así ya no me acuerdo
    if (isnan(temp)) {
        Serial.println("E-NAN");
    } else {
        if (!TemperaturaFeed->save(temp))
        { 
          Serial.println("E-SAVE");
        } else {
          Serial.print("TEM"); Serial.print(temperatura[i].numero); Serial.print(":");
          Serial.println(temp);
        }
    }
    delay(5000);
    // delay(180000);
    }
  
  //---TURBIDEZ-------------------------------------------------------------------
  for (int i = 0; i < sizeof(turbidez) / sizeof(turbidez[0]); i++) {
      float turbi = turbidez[i].obtenerTurbi();
/*         Serial.print("TUR"); Serial.print(turbidez[i].numero); Serial.print(":"); */
        
          if (!TurbidezFeed->save(turbi))
        { 
          Serial.println("ERROR");
        } else {
            Serial.print("TUR"); Serial.print(turbidez[i].numero); Serial.print(":");
            Serial.println(turbi);
        }
       // delay(60000);
     }
    
    //---PH-------------------------------------------------------------------
     for (int i = 0; i < sizeof(sensorPH) / sizeof(sensorPH[0]); i++) {
      float pH = sensorPH[i].obtenerPH();
/*       Serial.print("SPH"); Serial.print(sensorPH[i].numero); Serial.print(":"); */
      
      if (!PHFeed->save(pH))
      { 
      Serial.println("ERROR");
      } else {
        Serial.print("SPH"); Serial.print(sensorPH[i].numero); Serial.print(":");
        Serial.println(pH);
      }
      delay(2000);
    }

//---ULTRASONICO-------------------------------------------------------------------
 for (int i = 0; i < sizeof(ultrasonico) / sizeof(ultrasonico[0]); i++) {
      long distancia = ultrasonico[i].obtenerDistancia();
/*     Serial.print("SON"); Serial.print(sensorPH[i].numero); Serial.print(":"); */
        
        if (!UltrasonicoFeed->save(distancia))
            { 
            Serial.println("ERROR");
            } else {
            Serial.print("SON"); Serial.print(sensorPH[i].numero); Serial.print(":");
            Serial.println(distancia);

            //el tope máx del recipiente es 20cm de la orilla
            if (distancia > 20) {
               bomba[i].kaboom(); 
               }
               else {
               bomba[i].nokaboom(); 
               }
            }
            delay(2000);    
            }

//---TDS-------------------------------------------------------------------
     for (int i = 0; i < sizeof(tds) / sizeof(tds[0]); i++) {
      float TDS = tds[i].obtenerTDS();
/*         Serial.print("TDS"); Serial.print(tds[i].numero); Serial.print(":"); */

    if (!UltrasonicoFeed->save(TDS))
        { 
     Serial.println("ERROR");
      } else {
    Serial.print("TDS"); Serial.print(tds[i].numero); Serial.print(":");
    Serial.println(TDS);
      }
      delay(2000);
    }

    /*}*/
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
