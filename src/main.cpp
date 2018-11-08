#include "Arduino.h"
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>

// The circuit:
// SD card attached to SPI bus as follows:
// ** MOSI - pin 11
// ** MISO - pin 12
// ** CLK - pin 13
// ** CS - pin 4
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

#define PIN            13
#define NUMPIXELS      38

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

BluetoothSerial SerialBT;

char seperator = ':';
char EOL = ';';

uint8_t storeData = 0;
uint8_t updatePixels = 0;

struct storeStruct {
        uint8_t pixels;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alarm;
        uint8_t hrs;
        uint8_t mins;
        uint8_t alarmRed;
        uint8_t alarmGreen;
        uint8_t alarmBlue;

        uint8_t pixelsIndex;
        uint8_t redIndex;
        uint8_t greenIndex;
        uint8_t blueIndex;
        uint8_t alarmIndex;
        uint8_t hrsIndex;
        uint8_t minsIndex;
};

struct storeStruct store;

void initialiseStore(){
        store.pixels = 1;
        store.red = 20;
        store.green = 20;
        store.blue = 20;
        store.alarm = 0;
        store.hrs = 0;
        store.mins = 0;
        store.alarmRed = 0;
        store.alarmGreen = 0;
        store.alarmBlue = 0;


        store.pixelsIndex = 0;
        store.redIndex = 1;
        store.greenIndex = 2;
        store.blueIndex = 3;
        store.alarmIndex = 4;
        store.hrsIndex = 5;
        store.minsIndex = 6;
}

int storeInput(uint8_t index, int value){
        if(index == store.pixelsIndex) {
                Serial.println("pixelsIndex");
                Serial.println(value);
                return store.pixels = value;
        }

        if(index == store.redIndex) {
                Serial.println("redIndex");
                Serial.println(value);
                return store.red = value;
        }

        if(index == store.greenIndex) {
                Serial.println("greenIndex");
                Serial.println(value);
                return store.green = value;
        }

        if(index == store.blueIndex) {
                Serial.println("blueIndex");
                Serial.println(value);
                return store.blue = value;
        }

        if(index == store.alarmIndex) {
                Serial.println("alarmIndex");
                Serial.println(value);
                store.alarmRed = store.red;
                store.alarmGreen = store.green;
                store.alarmBlue = store.blue;
                return store.alarm = value;
        }

        if(index == store.hrsIndex) {
                Serial.println("hrsIndex");
                Serial.println(value);
                return store.hrs = value;
        }

        if(index == store.minsIndex) {
                Serial.println("minsIndex");
                Serial.println(value);
                return store.mins = value;
        }
}

void setPixels() {
        if(store.pixels == 0) {
                for(int i = 0; i < NUMPIXELS; i++) {
                        pixels.setPixelColor(i, 0, 0, 0);
                }
        } else {
                for(int i = 0; i < NUMPIXELS; i++) {
                        pixels.setPixelColor(i, store.red, store.green, store.blue);
                }
        }

        pixels.show();
}

void checkBluetoothInput() {
        uint8_t tempIndex = 0;
        String inString = "";

        while(SerialBT.available()) {
                int inChar = SerialBT.read();

                if (isDigit(inChar)) {
                        inString += (char)inChar;
                }

                if (inChar == seperator || inChar == EOL) {
                        storeInput(tempIndex, inString.toInt());
                        tempIndex++;
                        inString = "";
                }

                if(inChar == EOL) {
                        updatePixels = 1;
                        storeData = store.pixels;

                        break;
                }
        }
}

void transferStoreToSd(){
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("/datalog.dat", FILE_WRITE);

        // if the file is available, write to it:
        if (dataFile) {
                dataFile.write((const uint8_t *)&store, sizeof(store));
                dataFile.close();
        } else {
                Serial.println("error opening datalog.dat");
        }
}

bool transferSdtoStore() {
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("/datalog.dat");

        // if the file is available, read from it:
        if (dataFile) {
                dataFile.read((uint8_t *)&store, sizeof(store));
                dataFile.close();
                return true;
        } else {
                Serial.println("error opening datalog.dat");
                return false;
        }
}

void initialiseSerial() {
        Serial.begin(115200);

        while (!Serial) {
                // wait for serial port to connect. Needed for native USB port only
        }
}

void initialiseBluetooth() {
        if(!SerialBT.begin("ESP32")) {
                Serial.println("An error occurred initializing Bluetooth");
        }
}

void initialisePixels() {
        pixels.begin();
        setPixels();
}

void initialiseSd() {
        Serial.print("Initializing SD card...");

        while(!SD.begin(chipSelect)) {
                //
        }

        Serial.println("card initialized.");
}

void setup() {
        delay(1000);
        initialiseSerial();

        delay(1000);
        initialiseSd();

        delay(1000);
        // if(!transferSdtoStore()) {
        initialiseStore();
        // }

        delay(1000);
        initialiseBluetooth();

        delay(1000);
        initialisePixels();

        Serial.print("store.pixels: ");
        Serial.println(store.pixels);

        Serial.print("store.red: ");
        Serial.println(store.red);

        Serial.print("store.blue: ");
        Serial.println(store.blue);

        Serial.print("store.green: ");
        Serial.println(store.green);

        Serial.print("store.alarm: ");
        Serial.println(store.alarm);

        Serial.print("store.hrs: ");
        Serial.println(store.hrs);

        Serial.print("store.mins: ");
        Serial.println(store.mins);

        Serial.print("store.alarmRed: ");
        Serial.println(store.alarmRed);

        Serial.print("store.alarmGreen: ");
        Serial.println(store.alarmGreen);

        Serial.print("store.alarmBlue: ");
        Serial.println(store.alarmBlue);

        Serial.println("All Good!");
}

void loop() {
        checkBluetoothInput();

        if(updatePixels) {
                setPixels();
                updatePixels = 0;
        }

        if(storeData) {
                transferStoreToSd();
                storeData = 0;
        }

        delay(50);
}
