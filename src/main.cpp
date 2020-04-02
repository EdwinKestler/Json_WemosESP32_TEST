#include <NTPClient.h>                          //Liberia de manejode consulta de hora en intenet
// change next line to use with another board/shield
//#include <ESP8266WiFi.h>
#include <WiFi.h>                                       // for WiFi shield
#include <WiFiUdp.h>                                    //Libreria para el manejo de paquetes UDP sobre Wifi
#include <FS.h>                                         //Manejo de sistema de archivos JSON
#include <ArduinoJson.h>                                //serializacion y desearilizacion de docuemntos
#include <SPIFFS.h>
#include "commsettings.h"                               //Libreria con variables de configuracion
/*-----------------------------------------------------Defeniciones-----------------------------------------------------------*/
const char *fileName = "/SetupConfig.json";               //Archivo JSON de configuracion
/*-----------------------------------------------------Variables--------------------------------------------------------------*/
WiFiUDP ntpUDP;
WConfig Wificonfig;                                         //Cliente UDP para Wifi
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, NTP_Server, Connection.NTP_port, Connection.NTP_interval);
/*-----------------------------------------------------Funciones de Inicio----------------------------------------------------*/
void cargarConfiguracion(const char *filename, WConfig &Wificonfig)
{
  //File configFile = SPIFFS.open(fileName, "r");
  //File file = SD.open(filename);
  File configFile = SPIFFS.open(filename);
  // fijamos espacio para un Documento JSON temporal
  // No olvidemos cambiar la capcidad de acuerdo a sus rquerimientos
  //use el asistente de calculo de capaicdad visitando arduinojson.org/v6/assistant
  StaticJsonDocument<512> doc;
  // Deserializar el documento JSON
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
    Serial.println(F("Error al leer el archivo, usando la configuracion por defecto"));
  //Copiamos los valores del Documento JsonDocument al Config
  strlcpy(Wificonfig.SSID,
          doc["SSID"] | "HITRON-2F50",
          sizeof(Wificonfig.SSID));
  strlcpy(Wificonfig.password,                          // <- Destino
          doc["password"] | "PSZ2ZFT2BF77",             // <- source
          sizeof(Wificonfig.password));                  // <- tamañano del destino
  // Close the file (Curiously, File's destructor doesn't close the file)
  configFile.close();
}
void listAllFiles()
{
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file)
  {
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
}

void guardarConfiguracion(const char *filename, WConfig &Wificonfig)
{
  //Limpiar FS, para pruebas
  Serial.println("\n\n---BEFORE REMOVING---");          //imprimir todos los archivos antes de borrar
  listAllFiles();
  SPIFFS.remove(filename);
  // Open file for writing
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("Failed to create file"));
    return;
  }
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<256> doc;

  // Set the values in the document
  doc["SSID"] = Wificonfig.SSID;
  doc["password"] = Wificonfig.password;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}
void imprimirArchivo(const char *filename)
{
  // Open file for reading
  File file = SPIFFS.open(filename);
  if (!file)
  {
    Serial.println(F("Failed to read file"));
    return;
  }
  // Extract each characters by one by one
  while (file.available())
  {
    Serial.print((char)file.read());
  }
  Serial.println();
  // Close the file
  file.close();
}

  bool InitializeFileSystem()
  {
    Serial.println("Montando el Sistema de Archivos");
    if (!SPIFFS.begin(true))
    {
      Serial.println("Fallo el montar el sistema de Archivos");
      return false;
    }

    // Se cargan los parametros por defecto
    Serial.println(F("Cargando Configuracion..."));
    cargarConfiguracion(fileName, Wificonfig);
    delay(2000);

    // Create configuration file
    Serial.println(F("Guardando la Configuracion..."));
    guardarConfiguracion(fileName, Wificonfig);
    delay(2000);

    // Dump config file
    Serial.println(F("Print config file..."));
    imprimirArchivo(fileName);
    delay(2000);
    return true;
}
/*-----------------------------------------------------SETUP------------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);
  while (!Serial) continue;
  Serial.println(Wificonfig.SSID);
  delay(100);
  
  //allows serving of files from SPIFFS
  Serial.println("Montando sistema de archivos");
  if (!SPIFFS.begin(true))
  {
    Serial.println("Fallo en montar sistema de archivos");
    return;
  }

  // Se cargan los parametros por defecto
  Serial.println(F("Cargando Configuracion..."));
  cargarConfiguracion(fileName, Wificonfig);
  delay(2000);

  // Create configuration file
  Serial.println(F("Guardando la Configuracion..."));
  guardarConfiguracion(fileName, Wificonfig);
  delay(2000);

  // Dump config file
  Serial.println(F("Print config file..."));
  imprimirArchivo(fileName);
  delay(2000);
  
  WiFi.begin( Wificonfig.SSID, Wificonfig.password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
}
/*-----------------------------------------------------LOOP-------------------------------------------------------------------*/
void loop()
{
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  Serial.println(Connection.NTPClient_Server);
  delay(1000);
}