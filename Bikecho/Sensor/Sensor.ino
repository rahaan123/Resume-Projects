/*
 * BikeEcho Sensor Module Code
 * 
 * Authors:
 *    Chris Annen
 *    Dan Even
 *    Rahaan Gandhi
 *    Moe Nakira 
 *  
 */

#include "Task.h"
#include "TaskScheduler.h"
#include <SoftwareSerial.h>
#include <FreqMeasure.h>


// Pin Assignments (random pin numbers for now)
#define RX_BLUETOOTH 6
#define TX_BLUETOOTH 7

#define RX_LIDAR_0 10
#define TX_LIDAR_0 11

#define RX_LIDAR_1 2
#define TX_LIDAR_1 3

#define RADAR_GPIO 8
#define RADAR_RATE 250

#define LED_PIN_R 8
#define LED_PIN_G 9
#define LED_PIN_B 10

#define CLOCK_PERIOD 0.0000000625
#define FREQ_SAMPLES 5
#define FREQ_TIME_ALLOTTED 50


bool DATA_READY = false;

// Holds data to be transmitted via BT
// MUST MATCH CORRESPONDING STRUCT IN RECEIVER
typedef struct{
  uint16_t lidar0; // Lidar distance values (cm): Error = -1
  uint16_t lidar1;
  uint16_t radar; // Radar speed values (mph*100): No Motion: 0, Error: -1
  uint8_t parity;
}Data;

/* class Radar
 * Description:
 * Author:
 */
class Radar : public TimedTask
{
public:
  Radar(uint8_t _pin, uint32_t _rate, Data* _data);
  virtual void run(uint32_t now);
  
private:
  uint8_t pin; // Pin 8 is used regardless b/c of FreqMeasure library. Just for organizational purposed
  uint32_t rate;
  Data* data;
  
  uint32_t count;
  uint32_t sum;
  float frequency;
  float spd;

  void updateData(float spd);
  void filter();
  void printData();
  void printPlot(uint32_t duration);
};

  Radar::Radar(uint8_t _pin, uint32_t _rate, Data* _data) : TimedTask(millis()), pin(_pin), rate(_rate), data(_data) {
    // All code to initialize the Radar goes here and will run once
    Serial.println("\tRadar starting...");
    FreqMeasure.begin();
  }

  void Radar::run(uint32_t now) {
    // All code to read the Radar goes here and will run at millisecond intervals
    // specified by uint32_t rate which is set by the constructor. This can also be changed
    //Serial.println("\tRadar running...");

    // SET INITIAL CONDITIONS -> counters/summer set to 0.
    sum = 0;
    count = 0;
    uint32_t duration = 0;
    // DATA CAPTURE
    // Return if there is no signal to measure
    if ( !FreqMeasure.available() ) {
      updateData(0);
      incRunTime(rate); 
      return;
    }
    
    uint32_t time_s = millis(); // Start timer
    
    // Attempt to capture #FREQ_SAMPLES samples, stop after FREQ_TIME_ALLOTTED regardless.
    for (count = 0; count < FREQ_SAMPLES; ++count) {
      duration = millis() - time_s;
      if (duration > 100) {
        Serial.print("Breaking radar loop after "); Serial.println(duration);
        break;
      }
      sum += FreqMeasure.read(); 
    }
    uint32_t capture_duration = millis() - time_s;
    
    frequency = FreqMeasure.countToFrequency(sum/count);
    spd = frequency / 31.36; //conversion from frequency to miles per hour
    
    updateData(spd);
    
    incRunTime(rate);  
  }

  void Radar::updateData(float spd) {
    data->radar = (int)(spd*100);
    printData();
  }

  void Radar::printData() {
    Serial.print("Speed: "); Serial.print((float)(data->radar) / 100);
    Serial.print("\tDist 1: "); Serial.print(data->lidar0);
    Serial.print("\tDist 2: "); Serial.print(data->lidar1);
    Serial.println();
  }

  void Radar::printPlot(uint32_t duration) {
    Serial.print( data->radar / 100 );
    Serial.print(",");
    Serial.print( data->lidar0 );
    Serial.print(",");
    Serial.println( data->lidar1 );
  }

/* class StatusLED
 * Description:
 * Author:
 */
class StatusLED : public TriggeredTask
{
public:
  StatusLED(uint8_t _pin_R, uint8_t _pin_G, uint8_t _pin_B );
  virtual void run(uint32_t now);
  virtual bool canRun(uint32_t now);
  
private:
  uint8_t pin_R;
  uint8_t pin_G;
  uint8_t pin_B;
  
};

  StatusLED::StatusLED(uint8_t _pin_R, uint8_t _pin_G, uint8_t _pin_B ) : TriggeredTask(), pin_R(_pin_R), pin_G(_pin_G), pin_B(_pin_B) {
    // All code to initialize the StatusLED goes here and will run once
    
  }

  void StatusLED::run(uint32_t now) {
    
  }

  bool StatusLED::canRun(uint32_t now) {
    return false;
  }

/* class Lidar
 * Description:
 * Author: Dan
 */
class Lidar : public TriggeredTask
{
public:
  Lidar(SoftwareSerial* _ss, uint8_t _ID, Data* _data);
  virtual void run(uint32_t now);
  virtual bool canRun(uint32_t now);
  
private:  
  SoftwareSerial* serial;
  const int HEADER=0x59;  //frame header of data package
  uint32_t uart[9];       //save data measured by LiDAR
  uint32_t dist;          //actual distance measurements of LiDAR
  uint32_t strength;      //signal strength of LiDAR
  float temperature;      //temp of sensor
  uint8_t ID;             //Unique identifier between LiDAR units
  static uint8_t lidarTurn;
  Data* data;

  // ⏷⏷⏷⏷ Variables for testing/debugging only ⏷⏷⏷⏷
  uint32_t readCount;     //Stores the number of reads
  uint32_t timeBegun;     //Stores the time the unit was instantiated
  // ⏶⏶⏶⏶ Variables for testing/debugging only ⏶⏶⏶⏶
  
  void displayData();
  void updateData();
  bool verifyData();
  bool getData();
  void clearUART();
};

  uint8_t Lidar::lidarTurn = 0;

  Lidar::Lidar(SoftwareSerial* _ss, uint8_t _ID, Data* _data) : TriggeredTask(), serial(_ss), ID(_ID), data(_data) {
    // All code to initialize the Lidar goes here and will run once
    readCount = 0;
    timeBegun = millis();
    //serial->begin(115200);
    Serial.print("\tLidar "); Serial.print(ID); Serial.println(" starting...");
  }

  bool Lidar::canRun(uint32_t now) {
    //Serial.println("Lidar checking...");
    //if ( ID == lidarTurn && !DATA_READY) {
    if ( ID == lidarTurn) {
      //Serial.println("Running");
      serial->begin(115200);
      while (!serial->available()) {}
      //Serial.println("\tLiDAR running...");
      return true;    
    }
    return ( false );
  }

  void Lidar::run(uint32_t now) {
    // All code to read the Lidar goes here and will run when bool canRun() return true  
      
    while (!getData() || !verifyData()) {}
    updateData();
    //displayData();
    
//    // ⏷⏷⏷⏷ Testing/Debugging ⏷⏷⏷⏷
//    readCount++;
//    if ((readCount & 0b1111111) == 0) {
//      //Serial.print("Rate "); Serial.print(ID); Serial.print(": "); Serial.println((float)readCount/((millis()-timeBegun)/1000));
//      displayData();
//      readCount = 0;
//      timeBegun = millis();
//    }
//    // ⏶⏶⏶⏶ Testing/Debugging ⏶⏶⏶⏶
    
    lidarTurn = !lidarTurn;
  }

  bool Lidar::getData() {
  if(serial->read() == HEADER) { //assess data package frame header 0x59
      uart[0]=HEADER;
      if (serial->read() == HEADER) { //access data package frame header 0x59
        uart[1] = HEADER;
        for (int i = 2; i < 9; i++) { //save data in array
          uart[i] = serial->read();
        }
        return true;
      }
    }
    
    return false;
  }

  bool Lidar::verifyData() {
    uint32_t check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
    if (uart[8] == (check & 0xff)){
      return true;
    }
    else {
      return false;
    }
  }

  void Lidar::updateData() {
    dist = uart[2] + uart[3] * 256; //calculate distance value
    if (ID == 0) {
      data->lidar0 = dist;
    } else {
      data->lidar1 = dist;
      DATA_READY = true;
    }
  }

  void Lidar::displayData() {
    dist = uart[2] + uart[3] * 256; //calculate distance value
    Serial.print("Dist "); Serial.print(ID); Serial.print(": "); Serial.print(dist);
    Serial.println();
  }

  void Lidar::clearUART() {
    for (int i = 0; i < 9; ++i) {
      uart[i] = 0;
    }
  }

/* class Bluetooth
 * Description:
 * Author: Dan
 */
class Bluetooth : public TriggeredTask
{
public:
  Bluetooth(SoftwareSerial* _ss, Data* _data);
  virtual void run(uint32_t now);
  virtual bool canRun(uint32_t now);
  
private:
  SoftwareSerial* serial;
  Data* data;
  uint8_t data_size;
  uint8_t start;
  void setParityByte();
  void printData();

   // ⏷⏷⏷⏷ Variables for testing/debugging only ⏷⏷⏷⏷
  uint32_t readCount;     //Stores the number of reads
  uint32_t timeBegun;     //Stores the time the unit was instantiated
  // ⏶⏶⏶⏶ Variables for testing/debugging only ⏶⏶⏶⏶
};

  bool Bluetooth::canRun(uint32_t now) {
    // This boolean function determines when the Bluetooth:run() function will run. Once this function returns true
    // run() will immediately run
    Serial.println("Bluetooth checking...");
    return DATA_READY;
  }

  Bluetooth::Bluetooth(SoftwareSerial* _ss, Data* _data) : TriggeredTask(), serial(_ss), data(_data) {
    // All code to initialize the BT transceiver goes here and will run once
    Serial.println("\tBluetooth starting...");
    serial->begin(9600);
    data_size = sizeof(Data);
    start = 255;
    
    readCount = 0;
    timeBegun = millis();
  }

  void Bluetooth::run(uint32_t now) {
    // All code to send/receive data via Bluetooth will go here and will run when Bluetooth::canRun() return true.
    //Serial.println("\tBluetooth running...");
    printData();
    setParityByte();
//    byte* data_ptr = (byte*)data;
//    uint8_t data_size = sizeof(Data);
//    serial->write(start);
//    for (int i = 0; i < data_size; ++i) {
//      serial->write(*data_ptr);
//      data_ptr++;
//    }

    DATA_READY = false;


    // ⏷⏷⏷⏷ Testing/Debugging ⏷⏷⏷⏷
    readCount++;
    if ((readCount & 0b1111) == 0) {
      Serial.print("Sent: "); Serial.print(readCount); Serial.print("\tTime Elapsed: "); Serial.println((float)(millis()-timeBegun)/1000);
      Serial.print("Rate: "); Serial.println((float)readCount/((float)(millis()-timeBegun)/1000)); Serial.println();

      Serial.print("Dist: "); Serial.println(data->lidar0);
      Serial.print("Dist: "); Serial.println(data->lidar1);
      
      readCount = 0;
      timeBegun = millis();
    }
    // ⏶⏶⏶⏶ Testing/Debugging ⏶⏶⏶⏶

  }

  void Bluetooth::setParityByte() {
    data->parity = data->lidar0 + data->lidar1 + data->radar;
  }

  void Bluetooth::printData() {
    Serial.println("===========Data==============");
    Serial.print("Speed: "); Serial.println(data->radar);
    Serial.print("Dist0: "); Serial.println(data->lidar0);
    Serial.print("Dist1: "); Serial.println(data->lidar1);
    Serial.println("=============================\n");
  }
  
void setup() {
  Serial.begin(9600);
  Serial.println("Sensor module starting...");
}

void loop() {
  //--------------Data to be sent----------------
  Data data;
  data.lidar0 = 0;
  data.lidar1 = 0;
  data.radar = 0;
  data.parity = 0;
  //--------------SoftwareSerials----------------
  SoftwareSerial lidarSS0(RX_LIDAR_0, TX_LIDAR_0);
  SoftwareSerial lidarSS1(RX_LIDAR_1, TX_LIDAR_1);
  SoftwareSerial btSS(RX_BLUETOOTH, TX_BLUETOOTH);
  //--------------Scheduler Init-----------------
  //Bluetooth bluetooth(&btSS, &data);
  Lidar lidar_0(&lidarSS0, 0, &data);
  Lidar lidar_1(&lidarSS1, 1, &data);
  Radar radar(RADAR_GPIO, RADAR_RATE, &data);
  //StatusLED led(LED_PIN_R, LED_PIN_G, LED_PIN_B);
  
  
  Task *tasks[] = {
    &radar, 
    //&bluetooth,
    &lidar_0,
    &lidar_1
    
    //&led
    //...Add task objects here
  };

  
  // ***
  // *** Instantiate the TaskScheduler and fill it with tasks.      
  // ***
  TaskScheduler scheduler(tasks, NUM_TASKS(tasks));
  
  // GO! Run the scheduler - it never returns.
  scheduler.runTasks();
  //--------------Scheduler End------------------

}
