
/****************************************************************************
  CAN-Bus Demo

  Toni Klopfenstein @ SparkFun Electronics
  September 2015
  https://github.com/sparkfun/CAN-Bus_Shield

  This example sketch works with the CAN-Bus shield from SparkFun Electronics.

  It enables the MCP2515 CAN controller and MCP2551 CAN-Bus driver, and demos
  using the chips to communicate with a CAN-Bus.

  Resources:

  Development environment specifics:
  Developed for Arduino 1.6.5

  Based off of original example ecu_reader_logger by:
  Sukkin Pang
  SK Pang Electronics www.skpang.co.uk

  This code is beerware; if you see me (or any other SparkFun employee)
  at the local, and you've found our code helpful, please buy us a round!

  For the official license, please check out the license file included with the library.

  Distributed as-is; no warranty is given.
*************************************************************************/
#include <SPI.h>
#include <SD.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

const int chipSelect = 9;
File dataFile;
char UserInput;
int data;
float timeStamp = 0;
float holdSpeed;
float speed2;
float accelaration;
float pressure;
char buffer[456];  //Data will be temporarily stored to this buffer before being written to the file

//********************************Setup Loop*********************************//


void setup() {
  Serial.begin(9600);

  pinMode(chipSelect, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }


  //Inicia conexion

  if (Canbus.init(CANSPEED_500))
  {
  } else
  {
    Serial.println("Can't init CAN");
  }

  //establece conexion por medio de un mensaje cualquiera

  tCAN message;
  float engine_data;
  char *buffer;

  message.id = 0x7DF; //PID REQUEST message.id =0x7DF
  message.header.rtr = 0;
  message.header.length = 8;
  message.data[0] = 0x02;
  message.data[1] = 0x01;
  message.data[2] = 0x0D; //vehicle speed PID
  message.data[3] = 0x00;
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;

  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  mcp2515_send_message(&message);

  delay(200);

  delay(1000);

  dataFile = SD.open("logA.csv", FILE_WRITE);
  dataFile.print("Tiempo, Parametro, Valor");
  dataFile.println();




}



//********************************Main Loop*********************************//

void loop() {

  dataFile = SD.open("logA.csv", FILE_WRITE);



  if (dataFile)   {

    timeStamp = timeStamp + 3.5;
    
    //Cambio de mensurando
    Canbus.ecu_req(VEHICLE_SPEED, buffer);
    delay(200);
    
    Canbus.ecu_req(VEHICLE_SPEED, buffer);
    delay(500);

    Canbus.ecu_req(VEHICLE_SPEED, buffer);
    holdSpeed = atof(buffer);
    delay(1500);

    Canbus.ecu_req(VEHICLE_SPEED, buffer);
    Serial.print("Velocidad: ");
    Serial.println(buffer);
    speed2 = atof(buffer);
    dataFile.print(timeStamp);
    dataFile.print(",");
    dataFile.print("Speed,");
    dataFile.print(speed2);
    dataFile.println();
    delay(1500);

    timeStamp = timeStamp + 1.5;
    accelaration = (speed2 - holdSpeed) / 1.5;
    Serial.print("Aceleracion: ");
    Serial.println(accelaration);
    dataFile.print(timeStamp);
    dataFile.print(",");
    dataFile.print("Acceleration,");
    dataFile.print(accelaration);
    dataFile.println();
    delay(1500);

    Canbus.ecu_req(PRESSURE, buffer);
    delay(500);

    timeStamp = timeStamp + 2;

    //Cambio de mensurando
    Canbus.ecu_req(PRESSURE, buffer);
    delay(200);
    
    Canbus.ecu_req(PRESSURE, buffer);
    Serial.print("Presion: ");
    Serial.println(buffer);
    pressure = atof(buffer);
    dataFile.print(timeStamp);
    dataFile.print(",");
    dataFile.print("Pressure,");
    dataFile.print(pressure);
    dataFile.println();
    delay(1500);
    dataFile.close();

  }

  else
  {
    Serial.println("error opening log.txt");

  }





}
