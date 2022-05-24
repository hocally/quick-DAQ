#include <SPI.h>
#include <SD.h>

// A simple data logger for the Arduino analog pins

// How many milliseconds between grabbing data and logging it
// MAYBE TOUCH if logging too slow or too fast
#define LOG_INTERVAL 5

// How many milliseconds before writing the logged data permanently to SD
// PROBABLY DON'T TOUCH unless you really need to optimize how 
// much data you will retain after power off
#define SYNC_INTERVAL 1000
uint32_t syncTime = 0;     // time of last sync()

// The digital pins that connect to the LEDs
// DON'T TOUCH
#define RED_LED_PIN 2
#define GREEN_LED_PIN 3

// TODO: No electrical connections exist yet for this and no code uses it
// The driver data logging pin
#define DRIVER_LED_PIN 4

// The analog pins that connect to the sensors
// DON'T TOUCH
#define SHOCK_POT_FL 0    // analog 0
#define SHOCK_POT_FR 1    // analog 1
#define SHOCK_POT_RL 2    // analog 2
#define SHOCK_POT_RR 3    // analog 3

// Left front shock pot cal
// Voltage reading at bottom of travel, volts
#define SPFL_BOT_VOLTAGE 0.0 // CALIBRATE ME
// Voltage reading at top of travel, volts
#define SPFL_TOT_VOLTAGE 5.0 // CALIBRATE ME
// Measured displacement reading at bottom of travel, inches
#define SPFL_BOT_DISPLACEMENT 0.0 // CALIBRATE ME
// Measured displacement reading at top of travel, inches
#define SPFL_TOT_DISPLACEMENT 8.0 // CALIBRATE ME

// Right front shock pot cal
// Voltage reading at bottom of travel, volts
#define SPFR_BOT_VOLTAGE 0.0 // CALIBRATE ME
// Voltage reading at top of travel, volts
#define SPFR_TOT_VOLTAGE 5.0 // CALIBRATE ME
// Measured displacement reading at bottom of travel, inches
#define SPFR_BOT_DISPLACEMENT 0.0 // CALIBRATE ME
// Measured displacement reading at top of travel, inches
#define SPFR_TOT_DISPLACEMENT 8.0 // CALIBRATE ME

// Left rear shock pot cal
// Voltage reading at bottom of travel, volts
#define SPRL_BOT_VOLTAGE 0.0 // CALIBRATE ME
// Voltage reading at top of travel, volts
#define SPRL_TOT_VOLTAGE 5.0 // CALIBRATE ME
// Measured displacement reading at bottom of travel, inches
#define SPRL_BOT_DISPLACEMENT 0.0 // CALIBRATE ME
// Measured displacement reading at top of travel, inches
#define SPRL_TOT_DISPLACEMENT 8.0 // CALIBRATE ME

// Right rear shock pot cal
// Voltage reading at bottom of travel, volts
#define SPRR_BOT_VOLTAGE 0.0 // CALIBRATE ME
// Voltage reading at top of travel, volts
#define SPRR_TOT_VOLTAGE 5.0 // CALIBRATE ME
// Measured displacement reading at bottom of travel, inches
#define SPRR_BOT_DISPLACEMENT 0.0 // CALIBRATE ME
// Measured displacement reading at top of travel, inches
#define SPRR_TOT_DISPLACEMENT 8.0 // CALIBRATE ME

#define AREF_VOLTAGE 5.0 // DON'T TOUCH
#define ADC_LEVELS 1023.0 // DON'T TOUCH

// For the data logging shield, we use digital pin 10 for the SD cs line
// DON'T TOUCH
const int chipSelect = 10;
const float adcConversionConstant = AREF_VOLTAGE / ADC_LEVELS;

// The logging file
File logfile;

void error(char *str)
{
  Serial.print("Error: ");
  Serial.println(str);

  // Red LED indicates error
  digitalWrite(RED_LED_PIN, HIGH);

  while (1)
    ;
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  // Use debugging LEDs
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Initialize the SD card
  Serial.print("Initializing SD card.");
  // Make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // See if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    error("card initialization failed, or not present.");
  }
  Serial.println("Card initialized.");

  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (!SD.exists(filename))
    {
      // Only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break; // leave the loop!
    }
  }

  if (!logfile)
  {
    error("couldn't create file.");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);

  logfile.println("Time (ms),Shock pot FL displacement (in),Shock pot FL voltage (V),Shock pot FR displacement (in),Shock pot FR voltage (V),Shock pot RL displacement (in),Shock pot RL voltage (V),Shock pot RR displacement (in),Shock pot RR voltage (V)");
  Serial.println("Time (ms),Shock pot FL displacement (in),Shock pot FL voltage (V),Shock pot FR displacement (in),Shock pot FR voltage (V),Shock pot RL displacement (in),Shock pot RL voltage (V),Shock pot RR displacement (in),Shock pot RR voltage (V)");
}

void loop(void)
{
  // Delay for the amount of time we want between readings
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  digitalWrite(GREEN_LED_PIN, HIGH);

  // Log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m); // Milliseconds since start
  logfile.print(", ");
  Serial.print(m); // Milliseconds since start
  Serial.print(", ");

  // TODO: This section could be converted to a single mult up per channel 
  // if it's slow (using the preprocessor) Keeping it verbose for now for debugging 

  // Read raw ADC values
  int shockPotFLRaw = analogRead(SHOCK_POT_FL);
  int shockPotFRRaw = analogRead(SHOCK_POT_FR);
  int shockPotRLRaw = analogRead(SHOCK_POT_RL);
  int shockPotRRRaw = analogRead(SHOCK_POT_RR);

  // Convert to intermediate voltage values
  float shockPotFLVoltage = shockPotFLRaw * adcConversionConstant;
  float shockPotFRVoltage = shockPotFRRaw * adcConversionConstant;
  float shockPotRLVoltage = shockPotRLRaw * adcConversionConstant;
  float shockPotRRVoltage = shockPotRRRaw * adcConversionConstant;

  // Convert to calibrated displacement
  float shockPotFLDisplacement = mapfloat(shockPotFLVoltage, SPFL_BOT_VOLTAGE, SPFL_TOT_VOLTAGE, SPFL_BOT_DISPLACEMENT, SPFL_TOT_DISPLACEMENT);
  float shockPotFRDisplacement = mapfloat(shockPotFRVoltage, SPFR_BOT_VOLTAGE, SPFR_TOT_VOLTAGE, SPFR_BOT_DISPLACEMENT, SPFR_TOT_DISPLACEMENT);
  float shockPotRLDisplacement = mapfloat(shockPotRLVoltage, SPRL_BOT_VOLTAGE, SPRL_TOT_VOLTAGE, SPRL_BOT_DISPLACEMENT, SPRL_TOT_DISPLACEMENT);
  float shockPotRRDisplacement = mapfloat(shockPotRRVoltage, SPRR_BOT_VOLTAGE, SPRR_TOT_VOLTAGE, SPRR_BOT_DISPLACEMENT, SPRR_TOT_DISPLACEMENT);

  logfile.print(shockPotFLDisplacement);
  logfile.print(", ");
  logfile.print(shockPotFLVoltage);
  logfile.print(", ");
  logfile.print(shockPotFRDisplacement);
  logfile.print(", ");
  logfile.print(shockPotFRVoltage);
  logfile.print(", ");
  logfile.print(shockPotRLDisplacement);
  logfile.print(", ");
  logfile.print(shockPotRLVoltage);
  logfile.print(", ");
  logfile.print(shockPotRRDisplacement);
  logfile.print(", ");
  logfile.print(shockPotRRVoltage);
  logfile.print(", ");

  Serial.print(shockPotFLDisplacement);
  Serial.print(", ");
  Serial.print(shockPotFLVoltage);
  Serial.print(", ");
  Serial.print(shockPotFRDisplacement);
  Serial.print(", ");
  Serial.print(shockPotFRVoltage);
  Serial.print(", ");
  Serial.print(shockPotRLDisplacement);
  Serial.print(", ");
  Serial.print(shockPotRLVoltage);
  Serial.print(", ");
  Serial.print(shockPotRRDisplacement);
  Serial.print(", ");
  Serial.print(shockPotRRVoltage);
  Serial.print(", ");

  logfile.println();
  Serial.println();

  digitalWrite(GREEN_LED_PIN, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL)
    return;
  syncTime = millis();

  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(RED_LED_PIN, HIGH);
  logfile.flush();
  digitalWrite(RED_LED_PIN, LOW);
}

// Helper function to map floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}