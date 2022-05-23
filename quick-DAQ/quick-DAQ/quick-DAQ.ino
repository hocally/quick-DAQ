#include <SPI.h>
#include <SD.h>

// A simple data logger for the Arduino analog pins

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL 5 // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0;     // time of last sync()

#define ECHO_TO_SERIAL 1 // echo data to serial port
#define WAIT_TO_START 0  // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define RED_LED_PIN 2
#define GREEN_LED_PIN 3

// The analog pins that connect to the sensors
#define SHOCK_POT_FL 0    // analog 0
#define SHOCK_POT_FR 1    // analog 1
#define SHOCK_POT_RL 2    // analog 2
#define SHOCK_POT_RR 3    // analog 3
#define SHOCK_POT_POWER 4 // analog 4

#define AREF_VOLTAGE 5.0 // we tie 3.3V to ARef and measure it with a multimeter!
#define ADC_LEVELS 1023.0

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;
const float adcConversionConstant = AREF_VOLTAGE / ADC_LEVELS;

// the logging file
File logfile;

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  // red LED indicates error
  digitalWrite(RED_LED_PIN, HIGH);

  while (1)
    ;
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();

  // use debugging LEDs
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available())
    ;
#endif // WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");

  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (!SD.exists(filename))
    {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break; // leave the loop!
    }
  }

  if (!logfile)
  {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);

  logfile.println("millis,shock pot FL, shock pot FR, shock pot RL, shock pot RR");
#if ECHO_TO_SERIAL
  Serial.println("millis,shock pot FL, shock pot FR, shock pot RL, shock pot RR");
#endif // ECHO_TO_SERIAL

  // If you want to set the aref to something other than 5v
  // analogReference(EXTERNAL);
}

void loop(void)
{
  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  digitalWrite(GREEN_LED_PIN, HIGH);

  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m); // milliseconds since start
  logfile.print(", ");
#if ECHO_TO_SERIAL
  Serial.print(m); // milliseconds since start
  Serial.print(", ");
#endif

  int shockPotFLRaw = analogRead(SHOCK_POT_FL);
  int shockPotFRRaw = analogRead(SHOCK_POT_FR);
  int shockPotRLRaw = analogRead(SHOCK_POT_RL);
  int shockPotRRRaw = analogRead(SHOCK_POT_RR);

  // converting that reading to voltage, for 3.3v arduino use 3.3, for 5.0, use 5.0
  float shockPotFLVoltage = shockPotFLRaw * adcConversionConstant;
  float shockPotFRVoltage = shockPotFRRaw * adcConversionConstant;
  float shockPotRLVoltage = shockPotRLRaw * adcConversionConstant;
  float shockPotRRVoltage = shockPotRRRaw * adcConversionConstant;

  logfile.print(shockPotFLVoltage);
  logfile.print(", ");
  logfile.print(shockPotFRVoltage);
  logfile.print(", ");
  logfile.print(shockPotRLVoltage);
  logfile.print(", ");
  logfile.print(shockPotRRVoltage);
  logfile.print(", ");

#if ECHO_TO_SERIAL
  Serial.print(shockPotFLVoltage);
  Serial.print(", ");
  Serial.print(shockPotFRVoltage);
  Serial.print(", ");
  Serial.print(shockPotRLVoltage);
  Serial.print(", ");
  Serial.print(shockPotRRVoltage);
  Serial.print(", ");
#endif // ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

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
