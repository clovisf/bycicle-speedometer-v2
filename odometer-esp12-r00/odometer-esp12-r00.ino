// First thing, install the ESP8266 support on the Arduino IDE,
// with a link from here https://blog.eletrogate.com/nodemcu-esp12-usando-arduino-ide-2/

// board pinout for Wemos D1 https://forum.arduino.cc/t/weird-wemos-d1-r1/920394/29?page=2
// board pinout for NodeMCU V3 https://cyberblogspot.com/nodemcu-v3-esp8266-pinout-and-configuration/

#include <ESP8266WiFi.h> // just to turn WiFi off, info from here https://arduino.stackexchange.com/questions/43376/can-the-wifi-on-esp8266-be-disabled

// for light sleep woken by a GPIO, I used this reference https://efcomputer.net.au/blog/esp8266-light-sleep-mode/
// there is also something here https://kevinstadler.github.io/notes/esp8266-deep-sleep-light-sleep-arduino/

#define led_a 14 // charlieplex pin 1
#define led_b 12 // charlieplex pin 2
#define led_c 13 // charlieplex pin 3
#define sensor 16 // reed sensor

// Required for LIGHT_SLEEP_T delay mode
/*extern "C" {
  #include "user_interface.h"
  #include "gpio.h"
}*/

long currenttime;
long oldtime;
int sleepcounter; 
bool previoussensorstatus;
long timeelapsed;
long previoustime;
float speedms;
float speedkmh;
float wheelperimeter= 2.0 * 3.14159265 * 0.365; // perimeter= 2 * pi * radius
int calldisplaycounter;
int requiredled;
int zerospeedcounter;
int callserialprint;
int contador; 

/*void light_sleep(){
   wifi_station_disconnect();
   wifi_set_opmode_current(NULL_MODE);
   wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // set sleep type, the above    posters wifi_set_sleep_type() didnt seem to work for me although it did let me compile and upload with no errors 
   wifi_fpm_open(); // Enables force sleep
   gpio_pin_wakeup_enable(GPIO_ID_PIN(4), GPIO_PIN_INTR_HILEVEL); // GPIO_ID_PIN(2) corresponds to GPIO2 on ESP8266-01 , GPIO_PIN_INTR_HILEVEL for a logic HIGH, can also do other interrupts, see gpio.h above
   //gpio_pin_wakeup_enable(4, GPIO_PIN_INTR_HILEVEL);
   wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
 }*/

void ledon(int requiredled){
  //Serial.println("entrei");
  switch (requiredled) {
  case 0:
    pinMode(led_a, INPUT);
    pinMode(led_b, INPUT);
    pinMode(led_c, INPUT);
    //digitalWrite(led_a, LOW);
    //digitalWrite(led_b, LOW);
    //digitalWrite(led_c, LOW);
    break;
  case 1:
    pinMode(led_a, OUTPUT);
    pinMode(led_b, OUTPUT);
    pinMode(led_c, INPUT);
    digitalWrite(led_a, HIGH);
    digitalWrite(led_b, LOW);
    //digitalWrite(led_c, LOW);
    break;
  case 2:
    pinMode(led_a, OUTPUT);
    pinMode(led_b, OUTPUT);
    pinMode(led_c, INPUT);
    digitalWrite(led_a, LOW);
    digitalWrite(led_b, HIGH);
    //digitalWrite(led_c, LOW);
    break;
  case 3:
    pinMode(led_a, INPUT);
    pinMode(led_b, OUTPUT);
    pinMode(led_c, OUTPUT);
    //digitalWrite(led_a, LOW);
    digitalWrite(led_b, HIGH);
    digitalWrite(led_c, LOW);
    break;
  case 4:
    pinMode(led_a, INPUT);
    pinMode(led_b, OUTPUT);
    pinMode(led_c, OUTPUT);
    //digitalWrite(led_a, LOW);
    digitalWrite(led_b, LOW);
    digitalWrite(led_c, HIGH);
    break;
  case 5:
    pinMode(led_a, OUTPUT);
    pinMode(led_b, INPUT);
    pinMode(led_c, OUTPUT);
    digitalWrite(led_a, HIGH);
    //digitalWrite(led_b, LOW);
    digitalWrite(led_c, LOW);
    break;
  case 6:
    pinMode(led_a, OUTPUT);
    pinMode(led_b, INPUT);
    pinMode(led_c, OUTPUT);
    digitalWrite(led_a, LOW);
    //digitalWrite(led_b, LOW);
    digitalWrite(led_c, HIGH);
    break;
  default:
    // comando(s)
    break;
}
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFiMode(WIFI_STA);
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(100);
    
  pinMode(sensor, INPUT);
  //gpio_init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  currenttime= micros();
  if(currenttime - oldtime > 5000){ //verifies the sensor every 5ms
    oldtime= micros(); // after 5ms have passed, reset the time counter
    zerospeedcounter++;
    if(zerospeedcounter > 600){ // if more than three seconds have passed without sensor detection (less than an avarage of 3km/h)
      speedkmh= 0; // zero the speed variables
      speedms= 0;
      timeelapsed= 0; // zero the last elapsed time
    }
    if(digitalRead(sensor) && !previoussensorstatus){ // if sensor is HIGH and just was LOW
      zerospeedcounter= 0; // if the sensor was detected within 3 seconds, zero the zero speed flag
      timeelapsed= millis() - previoustime; // calculates the elapsed time between now and the last sensor state change
      speedms= wheelperimeter / (timeelapsed / 1000.0); // THIS calculates speed, wheel perimeter over time elapsed between sensor detections. Dividing by 1000 to get seconds
      speedkmh= speedms * 3.6; // converts m/s to km/h and rounds it for displaying
      previoustime= millis(); // reset the previoustime variable to the current millis()
      previoussensorstatus= true; // when the sensor goes to HIGH this variables goes too
    }else if(!digitalRead(sensor) && previoussensorstatus){ // if sensor is LOW and just was HIGH
      previoussensorstatus= false; // when the sensor goes LOW, reset this variable
    }
    // function to call and light the LED display
    calldisplaycounter++;
    if(calldisplaycounter == 50){ // every 250ms
      calldisplaycounter= 0;

      if(speedkmh >= 30){
        ledon(6);
      }else if(speedkmh >= 25){
        ledon(5);
      }else if(speedkmh >= 20){
        ledon(4);
      }else if(speedkmh>= 15){
        ledon(3);
      }else if(speedkmh >= 10){
        ledon(2);
      }else if(speedkmh >= 5){
        ledon(1);
      }else{
        ledon(0);
      }
    }
    /*callserialprint++;
    if(callserialprint == 100){
      callserialprint= 0;
      Serial.println(timeelapsed); 
    }*/
  }  
}
