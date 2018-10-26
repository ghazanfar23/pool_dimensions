
/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial
// #define BLYNK_PRINT DebugSerial
#include <SoftwareSerial.h>
#include <Console.h>
#include <NewPing.h>
#include <MedianFilter.h>
#include <Wire.h>
// #include <Bridge.h>
// #include <BlynkSimpleYun.h>
#include <BlynkSimpleStream.h>
#include <Servo.h>

#define TRIGGER_PIN_1 12 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define TRIGGER_PIN_2 10 // Arduino pin tied to trigger pin on the ultrasonic sensor.

#define ECHO_PIN_1 11 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define ECHO_PIN_2 9  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define MAX_DISTANCE_1 350  // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define MAX_DISTANCE_2 3350 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

SoftwareSerial DebugSerial(2, 3);                           // RX, TX
NewPing sonar_1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE_1); // NewPing setup of pins and maximum distance.
NewPing sonar_2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE_2); // NewPing setup of pins and maximum distance.
MedianFilter filter_1(10, 0);
MedianFilter filter_2(10, 0);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "b6861aae0cd7440e9aa95006a0fd751a";

Servo servo;

//BLYNK_WRITE(V5)
//{
//    servo.write(param.asInt());
//}
// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
int pinValue = 13;
//BLYNK_WRITE(D13)
//{
//    pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
//    // Blynk.virtualWrite(V1, pinValue);
//}
//     // process received value
//     // pinValue = 10;
//     // Blynk.virtualWrite(V2, pinValue);
// }
// // BLYNK_READ(V1)
// // {

// //     Blynk.virtualWrite(V1, pinValue);
// // }

// BLYNK_READ(V2)
// {
//     Blynk.virtualWrite(V2, pinValue);
// }
// BLYNK_READ(V3)
// {
//     Blynk.virtualWrite(V3, pinValue);
// }
BlynkTimer timer_1;
BlynkTimer timer_2;

int close_range_count_1 = 0;
int close_range_count_2 = 0;
int far_range_count_1 = 0;
int far_range_count_2 = 0;
int incomingByte_1 = 0; // for incoming serial data
int incomingByte_2 = 0; // for incoming serial data
int i_1 = 0;
int i_2 = 0;
int x_1 = 20;
int x_2 = 20;
int val;
long distance_1;
long distance_2;
long sensor_b;
long sensor_s;

//char read_serial = Serial.read();
//char read_serial = Console.read();

void findWall()
{
  if (sensor_b < 30 && sensor_b > 23 && sensor_s < 30 && sensor_s > 23)
  {

    Blynk.virtualWrite(V5, "Proceed Forward"); // sending sensor value to Blynk app
    Blynk.virtualWrite(V1, "Range");           // sending sensor value to Blynk app
    Blynk.virtualWrite(V3, "Range");           // sending sensor value to Blynk app
    // servo.write(68);
    return;
  }
}

void proceedForward()
{
  if (sensor_b < 300 && (sensor_s < 30 && sensor_s > 23))
  {

    Blynk.virtualWrite(V5, "Proceed Forward"); // sending sensor value to Blynk app
    Blynk.virtualWrite(V1, "Range");           // sending sensor value to Blynk app
    Blynk.virtualWrite(V3, "Range");           // sending sensor value to Blynk app
    // Blynk.virtualWrite(V5, 120);     // sending sensor value to Blynk app
    // servo.write(68);
    // flipSensors();
    return;
  }

  // else
  // {

  //     findWall();
  // }
}

void flipSensors()
{
  long sensor_f = sensor_s;
  long sensor_s = sensor_b;
  if (sensor_b > 290 && sensor_s < 35)
  {
    servo.write(68);
    delay(20);
  }

  if (sensor_f > 290 && sensor_s < 30)
  {
    Blynk.virtualWrite(V5, "Proceed Forward"); // sending sensor value to Blynk app
    Blynk.virtualWrite(V1, "Switched");        // sending sensor value to Blynk app
    Blynk.virtualWrite(V3, "Switched");        // sending sensor value to Blynk app
    return;
  }
  else
  {
    findWall();
  }
}

void sensorDataSend()
{

  //     delay(29);

  // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int o_1, uS_1 = sonar_1.ping(); // Send ping, get ping time in microseconds (uS).

  filter_1.in(uS_1);

  o_1 = filter_1.out();

  distance_1 = o_1 / US_ROUNDTRIP_CM;

  //  Serial.print("Ping: ");
  //    Console.print("Ping: ");
  Blynk.virtualWrite(V2, distance_1); // sending sensor value to Blynk app

  //     delay(29);

  // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int o_2, uS_2 = sonar_2.ping(); // Send ping, get ping time in microseconds (uS).

  filter_2.in(uS_2);

  o_2 = filter_2.out();

  distance_2 = o_2 / US_ROUNDTRIP_CM;

  //  Serial.print("Ping: ");
  //    Console.print("Ping: ");
  Blynk.virtualWrite(V4, distance_2); // sending sensor value to Blynk app
  sensor_b = distance_1;
  sensor_s = distance_2;

  // Start point Both sensor are in range to both side walls
  if ((sensor_b < 300 && sensor_b > 25) && (sensor_s < 300 && sensor_s > 25))
  {
    Blynk.virtualWrite(V5, "Ready, Find Walls"); // sending sensor value to Blynk app

    findWall();
    proceedForward();
    flipSensors();
    // close_range_count_1++;
    // far_range_count_1 = 0;

    //    Serial.print("too Close\n");     // Convert ping time to distance in cm and print result (0 = outside set distance range)
    //    Serial.print(close_range_count); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    //        Console.print("too Close\n");        // Convert ping time to distance in cm and print result (0 = outside set distance range)
    //        Console.print(close_range_count);    // Convert ping time to distance in cm and print result (0 = outside set distance range)
  }

  // Start point Both sensor are in range to both side walls

  else
  {
    //    Serial.print(distance); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    //    Serial.println("cm");
    //        Console.print(distance); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    //        Console.println("cm");
    // close_range_count_1 = 0;
    // far_range_count_1 = 0;
    Blynk.virtualWrite(V1, "ISSUE"); // sending sensor value to Blynk app
    Blynk.virtualWrite(V3, "ISSUE"); // sending sensor value to Blynk app
                                     // Blynk.virtualWrite(V5, "STOP");  // sending sensor value to Blynk app
  }
  i_1++;
}

void setup()
{

  // Debug console
  //    Serial.begin(9600);
  //    Bridge.begin();
  //    Console.begin();
  // Debug console
  DebugSerial.begin(9600);

  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  servo.attach(8);
  servo.write(68);

  // Blynk.begin(auth);
  // You can also specify server:
  //Blynk.begin(auth, "blynk-cloud.com", 80);
  //Blynk.begin(auth, IPAddress(192,168,1,100), 8080);
  timer_1.setInterval(50L, sensorDataSend); //timer will run every sec
                                            // timer_2.setInterval(20L, sensorDataSend_2); //timer will run every sec
}

void loop()
{
  Blynk.run();

  timer_1.run(); // run timer
                 // timer_2.run(); // run timer

  //        sensorDataSend_1();
  //        sensorDataSend_2();
}