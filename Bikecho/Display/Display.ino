#include "Task.h"
#include "TaskScheduler.h"
#include <SoftwareSerial.h>
#include <LedControl.h>

// Pin Assignments (random pin numbers for now)
#define RX_BLUETOOTH 6
#define TX_BLUETOOTH 7

#define DIN_PIN 4
#define CS_PIN 5
#define CLK_PIN 9

bool DATA_VALID = false;

typedef struct{
  uint16_t lidar0;
  uint16_t lidar1;
  uint16_t radar;
  uint8_t parity;
}Data;

bool NEW_DATA = false;

/* class Bluetooth
 * Description:
 * Author:
 */
class Bluetooth : public TriggeredTask
{
public:
  Bluetooth(SoftwareSerial* _ss, Data* _data);
  virtual void run(uint32_t now);
  virtual bool canRun(uint32_t now);
  
  
private:
  SoftwareSerial* serial;
  bool parityCheck();
  void flushUntilNew();
  
  uint8_t data_size;
  Data* data;
  
  // ⏷⏷⏷⏷ Variables for testing/debugging only ⏷⏷⏷⏷
  uint32_t readCount;     //Stores the number of reads
  uint32_t invalidCount;
  uint32_t timeBegun;     //Stores the time the unit was instantiated
  // ⏶⏶⏶⏶ Variables for testing/debugging only ⏶⏶⏶⏶
  
};

  Bluetooth::Bluetooth(SoftwareSerial* _ss, Data* _data) : TriggeredTask(), serial(_ss), data(_data) {
    // All code to initialize the BT transceiver goes here and will run once
    //Serial.println("\tBluetooth starting...");
    serial->begin(9600);

    data_size = sizeof(Data);

    readCount = 0;
    invalidCount = 0;
    timeBegun = millis();
  }

  bool Bluetooth::canRun(uint32_t now) {
    // This boolean function determines when the Bluetooth:run() function will run. Once this function returns true
    // run() will immediately run
    //Serial.println("Checking BT");
    if (NEW_DATA == true) {
      return false;
    }    
    return (serial->available() >= data_size );
  }

  void Bluetooth::run(uint32_t now) {
    // All code to send/receive data via Bluetooth will go here and will run when Bluetooth::canRun() return true.
    //Serial.println("Reading BT");
    if (DATA_VALID) {
      uint8_t trash;
      serial->readBytes(&trash, 1);
    }
        
    byte* data_ptr = (byte*)data; 
    serial->readBytes(data_ptr, data_size);
    
    DATA_VALID = parityCheck();
    if(!DATA_VALID) {
      flushUntilNew();
      invalidCount++;
    }

    //Serial.print("radar: "); Serial.println( ((float)data->radar) / 100 );

    // ⏷⏷⏷⏷ Testing/Debugging ⏷⏷⏷⏷
    //displayData();
    readCount++;
    //if ((readCount & 0b11) == 0) {
      Serial.print("Received: "); Serial.print(readCount); Serial.print("\tTime Elapsed: "); Serial.println((float)(millis()-timeBegun)/1000);
      Serial.print("Rate: "); Serial.print(readCount/((float)(millis()-timeBegun)/1000)); Serial.print("\tInvalid: "); 
      Serial.print(invalidCount); Serial.println();
      readCount = 0;
      invalidCount = 0;

      Serial.print("lidar0: "); Serial.println(data->lidar0);
      Serial.print("lidar1: "); Serial.println(data->lidar1);
      Serial.print("radar: "); Serial.println( ((float)data->radar) / 100 );

      Serial.println();
      timeBegun = millis();
    //}
    // ⏶⏶⏶⏶ Testing/Debugging ⏶⏶⏶⏶

    NEW_DATA = true;
  }

  bool Bluetooth::parityCheck() {
    uint8_t checksum = data->lidar0 + data->lidar1 + data->radar;
    return checksum == data->parity;
  }

  void Bluetooth::flushUntilNew() {
    byte startChar;
    serial->readBytes(&startChar, 1);
    
    while ( startChar != 255 ) {
      serial->readBytes(&startChar, 1);
    }
    
  }
  
void setup() {
  Serial.begin(9600);
  Serial.println("Display module starting...");

}

class Display : public TriggeredTask {
public:
  Display(LedControl* _lc, Data* _data, int _D_in, int _CS, int _CLK);
  virtual void run(uint32_t now);
  virtual bool canRun(uint32_t now);

private:
  int D_in;
  int CS;
  int CLK;
  Data* data;
  LedControl* lc;

  void Display::printByte(byte character[]);

  //Facial Expression
  byte smile[8]=   {0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
  byte neutral[8]= {0x00,0x66,0xFF,0xFF,0x7E,0x3C,0x18,0x00};
  byte sad[8]=   {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};
   
  //Arrow
  byte arrow_up[8]= {0x18,0x3C,0x7E,0xFF,0x18,0x18,0x18,0x18};
  byte arrow_down[8]= {0x18,0x18,0x18,0x18,0xFF,0x7E,0x3C,0x18};

  byte exc1[8] = { 0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x00 };
  byte exc2[8] = { 0x00,0x66,0x66,0x66,0x66,0x00,0x66,0x00 };
  byte exc3[8] = { 0x00,0xDB,0xDB,0xDB,0xDB,0x00,0xDB,0x00 };
  byte off[8] =  { 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  
};

  Display::Display(LedControl* _lc, Data* _data, int _D_in, int _CS, int _CLK) : TriggeredTask(), lc(_lc), data(_data), D_in(_D_in), CS(_CS), CLK(_CLK) {
    //Serial.println("\tDisplay Starting...");
    lc->shutdown(0, false);
    lc->setIntensity(0, 15);
    lc->clearDisplay(0);
  }

  bool Display::canRun(uint32_t now) {
    //Serial.println("\tChecking Display...");
    if (data->lidar0 < 300 ) {
      // Car is closer than 300 cm (3m)
      return true;
    }
    return true;
  }

  void Display::run(uint32_t now) {
    Serial.println("Display active");
//    if (data->lidar0 > 300) {
//      printByte(smile);
//    }
//    else {
//      printByte(neutral);
//      printByte(sad); 
//    }

    uint32_t dist = min(data->lidar0, data->lidar1);

    if (dist < 200) {
      if (data->radar <= 4) {
        printByte(exc1);
      } 
      else if (data->radar > 4 && data->radar <= 10) {
        printByte(exc2);
      }
      else if (data->radar > 10 && data->radar <= 35) {
        printByte(exc3);
      }
      else if (data->radar > 35) {
        printByte(exc3);
      }
    }
    else if (dist > 200 && dist <= 400) {
      if (data->radar <= 4) {
        printByte(exc1);
      } 
      else if (data->radar > 4 && data->radar <= 10) {
        printByte(exc1);
      }
      else if (data->radar > 10 && data->radar <= 35) {
        printByte(exc2);
      }
      else if (data->radar > 35) {
        printByte(exc3);
      }
    }
    else if (dist > 500 && dist <= 600) {
      if (data->radar <= 4) {
        printByte(off);
      } 
      else if (data->radar > 4 && data->radar <= 10) {
        printByte(exc1);
      }
      else if (data->radar > 10 && data->radar <= 35) {
        printByte(exc1);
      }
      else if (data->radar > 35) {
        printByte(exc2);
      }
    }
    else if (dist > 600) {
      if (data->radar <= 4) {
        printByte(off);
      } 
      else if (data->radar > 4 && data->radar <= 10) {
        printByte(off);
      }
      else if (data->radar > 10 && data->radar <= 35) {
        printByte(exc1);
      }
      else if (data->radar > 35) {
        printByte(exc1);
      }
    }
    NEW_DATA = false;
  }

  void Display::printByte(byte character[]) {
    int i = 0;
    for(i=0;i<8;i++)
    {
      lc->setRow(0,i,character[i]);
    }
  }

  
  

class Speaker : TriggeredTask {
  
};

void loop() {
  //-------------Data to be received-------------
  Data data;
  //--------------SoftwareSerials----------------
  SoftwareSerial btSS(RX_BLUETOOTH, TX_BLUETOOTH);
  //--------------Scheduler Init-----------------
  Bluetooth bluetooth(&btSS, &data);
  LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 0);
  Display disp(&lc, &data, DIN_PIN, CLK_PIN, CS_PIN);
  
  Task *tasks[] = {
    &bluetooth,
    &disp
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
