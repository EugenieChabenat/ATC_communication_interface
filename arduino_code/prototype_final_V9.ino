/**************************************
 * Test pour activer les capteurs en glissant la main sur la cover window
 * 
 * 
 **************************************/



#include <SoftwareSerial.h>   // Software Serial Port  
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <FastLED.h>
#include "definitions.h"
#include "pitches.h"

#define buz_pin 3
#define RxD 6                // Pin 4 pour la réception dans l'arduino
#define TxD 7                // Pin 5 pour l'émission de l'arduino
#define led_pin 5

// Initialize the bluetooth Serial 
SoftwareSerial BTSerie(RxD,TxD);  
// Variables
CRGB leds[NB_LEDS];
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

uint16_t lp_cnt = 0;
uint16_t baseline[NB_SENSORS]; // we should use structures instead...
bool sensor_touched[NB_SENSORS];// keeps track of the sensors that are currently touched (have boolean value 1, 0 otherwise)
bool sensor_active[NB_SENSORS];// keeps track of the sensor (at most one) that is currently active (have boolean value 1, 0 otherwise)

bool start = false; // linked to starting or turning off the feedback and control 


const byte numChars = 32;
char receivedChars[numChars]; // used to take a specific value from the bluetooth serial communication and set it to the Threshold
bool newData = false;  // will be set to true if there is new given value
char value_change = 'a';  // defining letters to chose which value to change


// Those values can be set by the user using the bluetooth connection  
int threshold_value = 0;
long advance_time = ADVANCE_TIME;
long time_step = TIME_STEP;
long activ_time = TIME_FOR_ACTIVE;


long time_touched[NB_SENSORS];  // the time at which the sensors are touched for last time
long compt_time;
long time_print = 0 ;  // used to control the time of printing and frequency of print 



uint8_t priority[NB_SENSORS] = {0, 3, 5, 2, 1, 4, 6}; // order of priority, order in which we need to check if the sensors are touched
uint8_t threshold_min[NB_SENSORS] = {4,2,3,2,2,1,4};      // define the lower range for each sensor to adjust the sensing, tested across several 
uint8_t threshold_max[NB_SENSORS] = {30,30,30,30,25,33,33};  // Define the upper range of each sensor to avoid miss sensing, tested across several runs


bool buzz = false ; // useful to have a buzzer feedback , calling for attention
//bool first_touched = false; // the first logic was to take the first button touched

uint16_t bl_candidate = 0; // baseline candidate
uint16_t nbrMeasureBaseline = 0; 
uint16_t value = 0; // Insure the selection of the button only once
uint16_t sumBaseline[NB_SENSORS]; 




// Here we declare the melody notes to be played when the buzzer is on : (default melody, should be changed)
int melody[] = {
  NOTE_C7, NOTE_G6, NOTE_G6, NOTE_A6, NOTE_G6, 0, NOTE_B6, NOTE_C7

  
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
 
};





/***********************SET UP*************************/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NB_LEDS);
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);  
  BTSerie.begin(9600);

  
  // We set up to high the led to indicate the arduino is working and the led pin to output
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
  delay(1000);
 
  initialize_baseline();
  initialize_sensor_touched();
  initialize_led();
  cap.setThreshholds(2,1);

  
}


/***********************MAIN LOOP*************************/
void loop() {
  // put your main code here, to run repeatedly:
  // adapt baseline
  // get filtered Data
  // compare filtered Data with Baseline
  // detect which sensor is touched or released
  // switch the LEDs
   
  if(nbrMeasureBaseline == MAX_NBR_MEASURE_BL){
     nbrMeasureBaseline = 0; 
     update_baseline();
      //Serial.print("\t*******BASELINE UPDATED********\n");
        
     }

      bluetooth_change_value(); // This function is used to update some values  to control grid 3 like time for activation etc ...
    
      //debugging_printings();  // this function is very useful to track the different sensors values and give us the raw values
      
      
      // update the touched array: sense which sensors are currently pressed (bool array of length NB_SENSORS)
      // update the time_touched array: contains the time at which the sensors passed from untouched to touched
      update_touched();
  
      // Determine if the On/Off button is activated  
      sense_start();
  
      if(start)
      {
          // update which button is active according to touched_array and the priority (only one active button at a given time)
          // we visit the sensors by order of priority, if it is touched and been touched for more than TIME_FOR_ACTIVE ms then it is active
          update_active();
          // call the function to update the LED status to the active button
          update_led();
          if(buzz)
          {
            tone_time_pressed();
            buzz=false;
          }
      }

      nbrMeasureBaseline++;
      delay(DELAY_MS); 
  
}





/**********************FUNCTIONS**************************/
/* Useful functions in the main loop */
void initialize_sensor_touched(void)
{
  for(uint8_t i = 0; i < NB_SENSORS; i++)
  {
    delay(10);
    sensor_touched[i] = 0;
  }
  
}
void initialize_baseline(void)
{
  for(uint8_t i = 0; i < NB_SENSORS; i++)
  {
    delay(10);
    baseline[i] = cap.baselineData(i);
  }
}





// Define if the board is in its On/Off state : (Feedback from the pictograms and the tablet control)
void sense_start(void)
{
  if( (sensor_touched[FUNCTIONING_BUTTON]) && (millis()-time_touched[FUNCTIONING_BUTTON] >=  activ_time) )
  {


    if(!sensor_active[FUNCTIONING_BUTTON])
    {
      
      start = (!start);
      if(start)
          Serial.println("Setting On the controler");
      else
      {
          Serial.println("Setting Off the controler");
      }
      

    }      
    sensor_active[FUNCTIONING_BUTTON] = true;
    update_led();
  }
  else
  {
      sensor_active[FUNCTIONING_BUTTON] = false;
      update_led();
  }
    


}





// Detect if the buttons are touched and set to one the buttons and their time of activation
void update_touched(void)
{
  int count = 0;
  int thresh =   threshold_value ; // local variable to define the threshold 
  
  for(uint8_t i = 0; i < NB_SENSORS; i++)
  { 
    uint8_t j = priority[i] ; // We also update the touched sensors going in the priority order set as before
    if(!thresh)
        thresh = threshold_min[j];
    if ((cap.filteredData(j) + thresh  <= baseline[j])  && (cap.filteredData(j) + threshold_max[j] >= baseline[j]) ) // sensor is currently pressed when it's in the range of sensing
    {
      if(!sensor_touched[j]) // if was not touched before, we record the time
      {
        time_touched[j] = millis();
        
        
      }
      
        sensor_touched[j] = true;

      // if inthe priority table one sensor is set to touched we have to initialize also the time of the other sensors  
      for(uint8_t k = i+1; k < NB_SENSORS; k++ )
        {
          // if the button is not active we  set his timing to zero to avoid his activation before the needed button
          if (sensor_touched[priority[k]])
          {
                time_touched[priority[k]] = millis();
          }
        }
        return;

        
    }
    else
    {
      sensor_touched[j] = false;// check whether we need to have a hysteresis (if glitching happen)
    }
  }
}





// This function insures that only one button is activated 
void update_active(void)
{
  for(uint8_t i = 1; i < NB_SENSORS; i++)
  { 
    
    // if the sensor in i-th level of priority is touched AND has been touched for more than TIME_FOR_ACTIVE [ms], then it is activated
    if( (sensor_touched[priority[i]]) && (millis()-time_touched[priority[i]] >= activ_time) )
    {
      // if it is the buzzer that has just been activated (the buzzer will be activated AND wasn't yet) then we have to play a tone
      if((priority[i] == BUZZER_BUTTON) && (!sensor_active[BUZZER_BUTTON]))
      {
        buzz = true;
      }
      

      // Activate only once the printed message at each time we have an activated sensor
      if(!sensor_active[priority[i]] && (millis()-time_touched[priority[i]] <= advance_time))
      {
              BTSerie.println(priority[i]);
              Serial.println(priority[i]);  // Backup  for the bluetooth module  
              compt_time = millis();
      }

      
      // This logic will permit to sweep the digital pictograms on the tablet grid
      if(sensor_active[priority[i]] && millis() - compt_time > time_step &&  (priority[i] != VALIDATE_BUTTON) &&  (priority[i] != BUZZER_BUTTON) )
      {
              BTSerie.println(priority[i]);
              Serial.println(priority[i]);  //Backup for the bluetooth module 
              compt_time = millis();
      }

      
      // This condition is needed to insure that after the sweeping time the sensor stays inactive 
      if(millis()-time_touched[priority[i]] > advance_time)
            sensor_active[priority[i]] = false;
      else
            sensor_active[priority[i]] = true;
      
      
      // set all other sensor_active to false and leave the function
      for(uint8_t j = i+1; j < NB_SENSORS; j++ )
      {
        // if it is the buzzer that has just been desactivated (the buzzer was activated) (AND isn't now) then we have to stop playing the tone
        sensor_active[priority[j]] = false;
        // if the button is not active we can also set his timing to zero
        time_touched[priority[j]] = millis();
      }
      return;
    }
    else
    {
      // Set every state of sensors not touched to zero 
      sensor_active[priority[i]] = false;
      
    }
  }
  
}




// This function set the leds to an intial state
void initialize_led(void)
{
  for (uint8_t i = 0; i < NB_LEDS; i++)
  {
    leds[i] = CRGB::Black;//(255,125,0);// LEDs initialized to Black
  }
  FastLED.show();// apply to the LED strip
}


// Depending on the touched sensor it will trun it On
void update_led(void)
{
  led_states state_led_i;// see in the definition section what values can state_led_i take (OFF, WHITE, RED, GREEN, BLUE...)
  for (uint8_t i = 0; i < NB_SENSORS; i++) //update the state of the i-th led
  {
    state_led_i = OFF;
    if(sensor_active[i] == true)
    {
      state_led_i = WHITE;
      // set the led lights green when we open the device 
      if( i == FUNCTIONING_BUTTON && start)
               state_led_i = GREEN;
      else if(i == FUNCTIONING_BUTTON && (!start))
               state_led_i = RED;
           
      
    }
     write_led(i, state_led_i);// this function is need because of each sensors has not the same number of LEDs (prototyping hazard :( )
  }
  //delay(100);
  //FastLED.show();// apply to the LED strip
}





void write_led(uint8_t index_sensor, led_states state)
{
  CRGB colour;
  uint8_t offset = 0;
  switch(state)
  {
    case OFF:  colour = CRGB::Black;//CRGB colour(0,0,0);// colour = (0,0,0);
    
      break; 
    case WHITE: colour = CRGB::White;//CRGB colour(255,255,255);// colour = (255,255,255);
      //Serial.println("WHITE"); 
      break; 
    case RED:   colour = CRGB::Red;//CRGB colour(255,0,0);// colour = (255,0,0);
      break; 
    case GREEN: colour = CRGB::Green;//CRGB colour(0,255,0);// colour = (0,255,0);
      break; 
    case BLUE:  colour = CRGB::Blue;//CRGB colour(0,0,255);// colour = (0,0,255);
      break; 
  }
  switch(index_sensor)
  {
    case 0: offset = 0;
      break; 
    case 1: offset = 3;
          break; 
    case 2: offset = 6;
          break; 
    case 3: offset = 9;
          break; 
    case 4: offset = 18;      
          break; 
    case 5: offset = 12;
          break; 
    case 6: offset = 15;
          break; 
    case 7: offset = 21;
          break; 
    case 8: offset = 21;
          break; 
    case 9 : offset = 21;
          break; 
  }
  leds[offset] = colour;
  leds[offset + 1] = colour;
  leds[offset + 2] =  colour; 
  FastLED.show();// apply to the LED strip

}


void update_baseline(void)
{
  for(uint8_t i = 0; i < NB_SENSORS; i++)
  {
    // the baseline is updated only if the current baselinData (baseline_candidate) is larger than the one recorded 
    // this avoids a problem if the user has his hand on a sensor when resetting the µcontroller
     bl_candidate = cap.baselineData(i);
     if (baseline[i] < bl_candidate)
     {
        baseline[i] = bl_candidate;
     }
     
  }
}

void print_led_colour(void)
{
    Serial.print("\n Colour:");
  for (uint8_t i = 0; i < NB_LEDS; i++)
  {
    Serial.print("\t"); Serial.print(leds[i][12]);
  }
  
}


void debugging_printings(void)
{
  Serial.print("\n\t\tlp_count: ");Serial.print(lp_cnt);
  Serial.print("\n nb:\t");
  for (uint8_t i = 0; i < NB_SENSORS; i++)
  {
    Serial.print("\t"); Serial.print(i);
  }
  Serial.print("\n Touched:");
  for (uint8_t i = 0; i < NB_SENSORS; i++)
  {
    Serial.print("\t"); Serial.print(sensor_touched[i]);
  }
  Serial.print("\n Baseline:");
  for (uint8_t i = 0; i < NB_SENSORS; i++)
  {
    Serial.print("\t"); Serial.print(baseline[i]);
  }
  Serial.print("\n Filtered:");
  for (uint8_t i = 0; i < NB_SENSORS; i++)
  {
    Serial.print("\t"); Serial.print(cap.filteredData(i));
  }
  Serial.print("\n");

  delay(200);
}

void send_funct(void){
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print(",");
  }
  Serial.println();
  Serial.print("Base mean: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print( baseline[i]); Serial.print(",");
  }
    Serial.println();

 Serial.println();
  Serial.print("Base true: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print(",");
  }
    Serial.println();

  delay(500);
}





// This function will be useful to use the python executable as a place where you can change the different numbers

void bluetooth_change_value(void)
{
  
  bool bt_start = false;
  char rc ; 
  if(BTSerie.available())
  {
     rc = BTSerie.read();
     if(rc == '1')
        bt_start = true;
     else
        bt_start = false;
  }
  while(bt_start)
  {
  recvWithEndMarker();
  bt_start = set_threshold_value();
  delay(50);
  if(!bt_start)
        Serial.println("Finished setting the values");
  }
}



// The function is inspired from a forum given the method to read numbers using Serial communication
void recvWithEndMarker(void) {
    static byte ndx = 0;
    char endMarker = 'f';
    char rc;
    
      if(BTSerie.available() > 0  && newData == false) {
        rc = BTSerie.read();
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            value_change = receivedChars[ndx-1] ; // Will define which value to change 
            receivedChars[ndx-1] ='\0' ;  // reset the last string value to terminates it
            ndx = 0;
            newData = true;
        }
    }
}

// We will try to change the threshold value 
bool set_threshold_value(void)
{
  if(newData == true)
  {

      switch(value_change)
    {
      case 'a':
              threshold_value = atoi(receivedChars);
              Serial.print("The new value of threshold is ...");
              Serial.println(threshold_value);
              BTSerie.print("The threshold is set to the value ...");
              BTSerie.println(threshold_value);
              break; 
      case 'b': 
              advance_time = atol(receivedChars);
              Serial.print("The new value of sweeping time is ...");
              Serial.println(advance_time);  
              BTSerie.print("The time to sweep is set to the value...");
              BTSerie.println(advance_time);
              break; 
      case 'c': 
              time_step = atol(receivedChars);
              Serial.print("The new value of step advance is ...");
              Serial.println(time_step);  
              BTSerie.print("The step advance time is set to the value ...");
              BTSerie.println(time_step);
              break; 
      case 'd':
              activ_time = atol(receivedChars);
              Serial.print("The new value of activation time is ...");
              Serial.println(activ_time);  
              BTSerie.print("The activation time is set to  the value ...");
              BTSerie.println(activ_time);
              return false;
              break;
  
    }
    newData = false;
  }
  return true;
  
}





void tone_time_pressed(){

  for (int thisNote = 0; thisNote < (sizeof(noteDurations) / sizeof(int)); thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buz_pin, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buz_pin);
  }
                                    
  }
