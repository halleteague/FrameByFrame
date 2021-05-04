/*
 * Frame by Frame 
 * the concept is based on:
 * https://www.instructables.com/Arduino-Animation/
 * 
 * all code by me, Halle! :D 
 * 
 * this code powers a NeoPixel strip and an Ultrasonic Range Finder. 
 * It lights up 8 panes of acrylic, one at a time, making an 
 * animation. The speed it lights each pane up is controlled by 
 * the URF. 
 * 

 */

/*
   blue: 5V
   black: DI
   orange: GND
  -
   URF
   red:vcc
   green: trig
   blue: echo
   black: gnd
*/

//NEOPIXELS
//initialize adafruit
#include<Adafruit_NeoPixel.h>
//number of pixels in strip
int stripNum = 48;
//declaring strip as a _ pixel strip in pin 5
Adafruit_NeoPixel strip(stripNum, 6);
//variable keeping track of how many pixels per plate strip
int platePix = 6;
//neopixel appearance
uint32_t white = strip.Color(255, 255, 255);

//PLATE VARIABLES
/*
   these keep track of which portion of the strip will be
   lit up. They refer to the index of the last pixel
   on that plate's strip.
*/
int stripMax = 5;
int stripMin = 0;

//LIGHT TIMER
unsigned long lightStartTime = millis();
unsigned long lightEndTime = millis();
//this guy will eventually be controlled by the URF
int lightInterval = 500;

//RANGE FINDER
//pins
int trigPin = 12;
int echoPin = 10;
//array variables
const int arrayLen = 10; //how  many slots in the array
int arrayNum = 0; //which slot you're on
int placeholder;
int distanceArray[arrayLen]; //distance array with length of 10
float avgDist; //average value in array of distances

//RANGE FINDER TIMERS
unsigned long startTime = millis();
unsigned long endTime;
const int interval = 500;
unsigned long startTime2 = millis();
unsigned long endTime2 = millis();
const int interval2 = 250;

void setup() {

  //neopixely stuff
  strip.begin();
  strip.clear();
  strip.show();

  //URF
  //setting trigger as output
  pinMode(12, OUTPUT);
  //setting echo as input
  pinMode(10, INPUT);

  //start COM channel
  Serial.begin(9600);
}

void loop() {
  //setting strip brightness
  strip.setBrightness(255);

  //timers start
  lightEndTime = millis();
  endTime = millis();
  endTime2 = millis();

  //------------------------------------------------

  //neopixel strip timer
  /*
    this timer makes the different panes light up.
    if a certain time has passed, it changes the
    minimum and maximum indexes of the neopixels
    that are lit up by the for loop utilizing those
    minimums and maximums below.
    the interval speed will be controlled by an
    ultrasonic range finder.
  */
  //if time has passed
  if (lightEndTime - lightStartTime >= lightInterval) {
    //shift to the next plate's lights
    stripMin = (stripMin + platePix);
    stripMax = (stripMax + platePix);
    //reset timer
    lightStartTime = millis();

    //if you've reached the end of the strip, go back to 1st pane
    if (stripMin > stripNum - 5) {
      stripMin = 0;
    }
    if (stripMax > stripNum) {
      stripMax = 5;
    }
  }

  //--------------------------------------------------------

  //neopixel for loop
  /*
     this for loop lights up the neopixel strip.
     while the strip is within the determined
     mins and maxes for that plate, it lights
     up the plate.
  */

  for (int i = stripMin; i <= stripMax; i++) {
    strip.setPixelColor(i, white);
  }
  strip.show();
  strip.clear();

  //range finder array
  /*
     fills array with values.
     can't use a for loop or it fricks with the
     neopixels. fills array with values, and if the array
     gets too big, it resets the array to 0.
  */


  distanceArray [arrayNum] = getDistance(echoPin, trigPin);
  arrayNum ++;
  if (arrayNum >= arrayLen) {
    arrayNum = 0;
  }

  //calling return average function
  returnAverage(distanceArray, arrayLen);
  //give it a second, then print the distance
  if (endTime2 - startTime2 > interval2) {
    startTime2 = millis();
    Serial.println(returnAverage(distanceArray, arrayLen));
  }



  placeholder = returnAverage(distanceArray, arrayLen);

  //CONTROLLING THE SPEED OF THE LIGHT MOVEMENT
  /*
     the closer you move to the URF, the faster
     the interval between the lights gets
  */
  if (placeholder >= 3 && placeholder < 6) {
    lightInterval = 100;
  }
  if (placeholder >= 6 && placeholder < 9) {
    lightInterval = 200;
  }
  if (placeholder >= 9 && placeholder < 14) {
    lightInterval = 300;
  }
  if (placeholder >= 12 && placeholder < 17) {
    lightInterval = 400;
  }
  if (placeholder >= 15 && placeholder < 20) {
    lightInterval = 500;
  }
  if (placeholder >= 18 && placeholder < 23) {
    lightInterval = 600;
  }
  if (placeholder >= 21 && placeholder < 26) {
    lightInterval = 700;
  }
  if (placeholder >= 24 && placeholder < 27) {
    lightInterval = 800;
  }
  //min and max boundaries
  if (placeholder > 27) {
    lightInterval = 800;
  }
  if (placeholder < 3 ) {
    lightInterval = 100;
  }

}

//---------------------------------------------
//FUNCTIONS

//return average function
/*
   This function returns the average value of an array of integers.
   2 inputs: Array of integers, integer representing length of array
   of integers.
*/
float returnAverage(int * anArray, int arrayLength) {

  //local variable saving sum of values in array
  int arraySum;

  //for loop that goes through the array and adds the contents
  for (int i = 0; i < 9; i++) {
    arraySum = (anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i] + anArray[i]);
  }

  //divide sum by total number of things in array
  avgDist = arraySum / arrayLength;

  //return the result
  return avgDist;
}

//get distance function
/*
   this is a function that returns the reading of the US
   range finder with a distance in centimeters.
*/
float getDistance(int aEchoPin, int aTrigPin) {

  float duration; //how long to shoot out sound wave & come back
  float distance; //references distance

  //turn trigger pin off
  digitalWrite(aTrigPin, LOW);
  //turn pin on, shooting out sound waves
  digitalWrite(aTrigPin, HIGH);
  //stop shooting things out
  digitalWrite(aTrigPin, LOW);

  //now calculate how long it took for signal to return

  /*
     PulseIn helps us record how much time the pin registers as high

     When you shoot out sound waves, we start listening. As soon as
     that turns high, we record how much time has passed.

     So make duration equal to the amount of time between when the
     signal was shot out and when it returns.
  */
  duration = pulseIn(aEchoPin, HIGH);

  /*
     We only care about the distance one way, not on the way back, so
     divide the distance by 2.

     We multiply it by 0.0344 to convert the speed of sound into
     centimeters.
  */
  distance = duration / 2 * 0.0344;

  //and return the result, in centimeters.
  return distance;
}
