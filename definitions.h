// Define constants
#define LED_PIN       13
#define NB_LEDS      24
#define NB_SENSORS    7
#define MAX_NBR_MEASURE_BL 100
#define THRESHOLD     3 // difference between the filtered data and baseline above which the sensors is considered as touched
#define BL_UPDT_RATE  5 // Baseline update rate (how many loops before we update the baselines)
#define DELAY_MS   20 // delay to be waited between the main loops in [ms]
#define TIME_FOR_ACTIVE  1000 // time a button must be pressed before it becomes active in [ms]
#define TIME_STEP 750
#define ADVANCE_TIME 5000  // The time allowed to advance on the grid 
#define BUZZER_BUTTON 3 // the ide of the button corresponding to asking for attention (SOS)
#define BOTTOM_BUTTON 5
#define VALIDATE_BUTTON 1
#define FUNCTIONING_BUTTON 0
// the LED states (OFF, WHITE, RED, BLUE or GREEN) we could define other mixed colors too
enum led_states{OFF, WHITE, RED, GREEN, BLUE};
