

// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>
#include <MedianFilter.h>
#include <Wire.h>

#define TRIGGER_PIN 13   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 12      // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

MedianFilter filter(31, 0);

void setup()
{

  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
}
int close_range_count = 0;
int far_range_count = 0;
int incomingByte = 0; // for incoming serial data
int i = 0;
int x = 20;
char read_serial = Serial.read();

void loop()
{
  //   // send data only when you receive data:
  //        if (read_serial == "stop") {
  //
  //                Serial.print("I received: ");
  //                Serial.print(read_serial);
  //
  ////                Serial.println(incomingByte, DEC);
  //                Serial.println("\n");
  ////                x= 0;
  ////               x=incomingByte;
  //exit(0);
  //        }
  //  while (Serial.available() > 0 && Serial.read() == 's'){

  delay(50);
  long distance;
  // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int o, uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  filter.in(uS);
  o = filter.out();
  distance = o / US_ROUNDTRIP_CM;
  Serial.print("Ping: ");

  if (distance < 25 && read_serial == 'c')
  {
    close_range_count++;
    far_range_count = 0;

    Serial.print("too Close\n");     // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.print(close_range_count); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  }
  else if (distance > 100)
  {
    far_range_count++;
    close_range_count = 0;

    Serial.print("too far\n");     // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.print(far_range_count); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  }

  else
  {
    Serial.print(distance); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
    close_range_count = 0;
    far_range_count = 0;
  }
  i++;
}
