// Do not remove the include below
#include "Sensors.h"

// Specify data and clock connections and instantiate SHT1x object
#define DATA_PIN 6
#define CLOCK_PIN 7

#define NETWORKID 100 //the same on all nodes that talk to each other
#define GATEWAYID 1
#define FREQUENCY RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define FLASH_ADDR 1
#define BLINK_MS 50
#define LED_PIN 9
#define INTER 1

#define SLEEP_CYCLES 40

#define NODEID 5 //unique for each node on same network
#define REPORT_PULSES 1

byte cycle = 0;

//#define DEBUG

#define D(input) {Serial.print(input); Serial.flush();}
#define Dln(input) {Serial.println(input); Serial.flush();}

RFM69 radio;
SHT1x sht1x(DATA_PIN, CLOCK_PIN);
Adafruit_BMP085 bmp;

void blink(int cnt) {
        for (int i = 0; i < cnt; i++) {
                pinMode(LED_PIN, OUTPUT);
                digitalWrite(LED_PIN, HIGH);
                delay(BLINK_MS);
                digitalWrite(LED_PIN, LOW);
                delay(BLINK_MS);
                pinMode(LED_PIN, INPUT);
        }
}

int float_part(float val) {
         return (val - (int)val) * 100;
}

void transmit(float temp, float hum, unsigned long int press) {
        char buff[20];
    sprintf(buff,"%0d.%d,%0d.%d,%lu",
    		(int)temp,
    		float_part(temp),
    		(int)hum,
    		float_part(hum),
    		press);
        //sprintf(buff, "%.2f,%.2f", temp, hum);
#ifdef DEBUG
        D("Transmit: ");
        Dln(buff);
#endif
        boolean sent = radio.sendWithRetry(GATEWAYID, buff,(byte) strlen(buff));
        radio.sleep();
#ifdef DEBUG
        D("Sent ");
        Dln(sent?"OK":"Fail");
#endif

}

void setup() {
#ifdef DEBUG
        Serial.begin(115200);
        D("Start node ");
        Dln(NODEID);
#endif
        bmp.begin(BMP085_STANDARD);
        radio.initialize(FREQUENCY, NODEID, NETWORKID);
        radio.encrypt(ENCRYPTKEY);
        radio.sleep();

        blink(4);
}

void loop() {

        cycle++;

        if ( cycle >= SLEEP_CYCLES) {
                        transmit(sht1x.readTemperatureC(),sht1x.readHumidity(),bmp.readPressure());
                        cycle = 0;
                } else {
#ifdef DEBUG
                D(cycle);
                Dln(" - Sleeping");
#endif
                }

        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
