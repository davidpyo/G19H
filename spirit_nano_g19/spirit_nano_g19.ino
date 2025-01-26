//"If you see me, it's already too late"
//SPIRIT
//A brushless flywheel blaster
//By wonderboy
//Software revision 25.1.8
//FOR NANO

#include <ClickButton.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
#include <EEPROM.h>

#define EEPROMOFFSET 0 //add 24 to this value if your EEPROM starts acting funky. max 1000

const byte VOLT_PIN = A0;
const byte TACH_PIN_0 = 2;
const byte TACH_PIN_1 = 3;
const byte TRIG_PIN = 4;
const byte ESC_PIN = 5;
const byte MENU_PIN = 6;
const byte SOLENOID_PIN = 7;
const byte REV_PIN = 8;

const byte MOTOR_POLES = 14;
const unsigned int T0H_PULSE = 100;
const unsigned int T1H_PULSE = 400;
const unsigned int TL_PULSE = 500;
const unsigned int OFF = 1000;
const unsigned int FULL = 2000;

const unsigned int RAMPWAIT = 1000;
const unsigned int FAILTIME = 1000;
const unsigned int BATTFREQ = 5000;
const unsigned int SAFETYTIMEOUT = 15000;
const float VOLTAGE_DIVIDER = 0.164;

const unsigned int MAXRPM = 40000;
const unsigned int MINRPM = 5000;
const unsigned int FIVEPERCENT = 1750;

const byte MAXFIRERATE = 14;
const byte MINFIRERATE = 4;
const byte MAXROFDELAY = 35;

const byte SEMI = 1;
const byte AUTO = 2;
const byte REVERSE = 3;
const byte BURST = 4;
const byte BINARY = 5;
const byte DEVOTION = 6;
const byte RAMPING = 7;
const byte RBURST = 8;

//startup splash bitmap
const unsigned char splash[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0xff, 0x00, 0x07, 0xfc, 0x0f, 0xff, 0x03, 0xc7, 0xff, 0x81, 0xe3, 0xff, 0xf8, 0x00,
  0x00, 0x01, 0xff, 0xe0, 0x0f, 0xfe, 0x0f, 0xff, 0x83, 0xc7, 0xff, 0xc1, 0xe3, 0xff, 0xf8, 0x00,
  0x00, 0x00, 0x7f, 0xf8, 0x1f, 0xff, 0x0f, 0xff, 0xc3, 0xc7, 0xff, 0xe1, 0xe3, 0xff, 0xf8, 0x00,
  0x00, 0x00, 0x7f, 0xf8, 0x3f, 0xff, 0x8f, 0xff, 0xe3, 0xc7, 0xff, 0xf1, 0xe3, 0xff, 0xf8, 0x00,
  0x00, 0x00, 0x7f, 0xf8, 0x3c, 0x0f, 0x8f, 0x03, 0xe3, 0xc7, 0x81, 0xf1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xf8, 0x3c, 0x00, 0x0f, 0x01, 0xe3, 0xc7, 0x80, 0xf1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x7f, 0xf8, 0x3c, 0x00, 0x0f, 0x01, 0xe3, 0xc7, 0x80, 0xf1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x1f, 0xf8, 0x3c, 0x00, 0x0f, 0x01, 0xe3, 0xc7, 0x80, 0xf1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x1f, 0xf8, 0x3e, 0x00, 0x0f, 0x03, 0xe3, 0xc7, 0x81, 0xf1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x1f, 0xf8, 0x3f, 0xf8, 0x0f, 0xff, 0xc3, 0xc7, 0xff, 0xe1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x07, 0xf8, 0x1f, 0xfc, 0x0f, 0xff, 0x83, 0xc7, 0xff, 0xc1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x07, 0xf8, 0x0f, 0xfe, 0x0f, 0xff, 0x03, 0xc7, 0xff, 0x81, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x07, 0xf8, 0x07, 0xff, 0x0f, 0xfe, 0x03, 0xc7, 0xff, 0x01, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x78, 0x00, 0x0f, 0x8f, 0x00, 0x03, 0xc7, 0x8f, 0x01, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x78, 0x00, 0x07, 0x8f, 0x00, 0x03, 0xc7, 0x8f, 0x81, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x78, 0x00, 0x07, 0x8f, 0x00, 0x03, 0xc7, 0x87, 0x81, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x78, 0x3c, 0x07, 0x8f, 0x00, 0x03, 0xc7, 0x87, 0x81, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x78, 0x3c, 0x0f, 0x8f, 0x00, 0x03, 0xc7, 0x83, 0xc1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x38, 0x3f, 0xff, 0x0f, 0x00, 0x03, 0xc7, 0x83, 0xc1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x18, 0x1f, 0xfe, 0x0f, 0x00, 0x03, 0xc7, 0x81, 0xe1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x0f, 0xfc, 0x0f, 0x00, 0x03, 0xc7, 0x81, 0xe1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x07, 0xf8, 0x0f, 0x00, 0x03, 0xc7, 0x81, 0xe1, 0xe0, 0x1e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned int HIGHPOWER = MINRPM + (FIVEPERCENT * 15);  //target RPM hi power (15*5 = 75%)
const unsigned int MIDPOWER = MINRPM + (FIVEPERCENT * 11);   //target RPM med power (11*5 = 55%)
const unsigned int LOWPOWER = MINRPM + (FIVEPERCENT * 7);    //target RPM low power (7*5 = 35%)
const byte singleShotPulse = 35;                             //power pulse time for solenoid
const bool batteryPicture = true;                            //battery display mode

const byte PROFILE_LOW = 0;
const byte PROFILE_MED = 1;
const byte PROFILE_HIGH = 2;
const byte PROFILE_T = 3;

struct Profile {
  unsigned int targetRPM;
  byte burstCount;
  byte fireRate;
  unsigned int spinDownTime;
};


const Profile DEFAULT_PROFILES[4] PROGMEM = {
  { HIGHPOWER, 3, 12, 500 },                 // Idle
  { HIGHPOWER, 3, 12, 500 },                // Profile2
  { HIGHPOWER, 3, 12, 500 },               //  Default
  { HIGHPOWER + FIVEPERCENT, 3, 12, 500 }  // Tournament
};

Servo esc;                                         //ESC object
ClickButton trig(TRIG_PIN, LOW, CLICKBTN_PULLUP);  //trigger button
ClickButton rev(REV_PIN, LOW, CLICKBTN_PULLUP);    //rev button
ClickButton menu(MENU_PIN, LOW, CLICKBTN_PULLUP);  //menu button
Adafruit_SSD1306 uView(128, 64);                   //display

byte currentProfile = PROFILE_HIGH;
unsigned int targetRPM = HIGHPOWER;                                             //target RPM value
byte fireRate = 10;                                                             //target fire rate (darts per second)
byte singleShotDelay = ceil((1000 - (fireRate * singleShotPulse)) / fireRate);  //how long to wait after powering solenoid before it can be powered again
byte mode = SEMI;                                                               //fire mode
byte burstCount = 3;                                                            //how many shots to fire in burst mode
unsigned int spinDownTime = 500;                                                //how long to wait before powering off flywheels after firing

unsigned int shotCount = 0;       //fired counter
byte devotionCount = 0;           //devotion mode fired counter
byte rampCount = 0;               //ramping mode fired counter
byte selected = 1;                //menu selection
bool revved = false;              //flywheels spun up?
bool settings = false;            //in settings mode?
bool idling = false;              //flywheels idling (state)?
bool idle = false;                //idle mode
bool fired = false;               //shot was fired? prevent additional shots until trigger reset (for all non fully-automatic fire modes)
bool ramp = false;                //ramped to full auto? (ramping mode)
bool lock = false;                //mode locked? when true, fire mode cannot be changed
bool tourney = false;             //when true, restrict available fire modes for competitive play
bool lowBatt = false;             //when true, low battery condition has been tripped. lock blaster for recharge
bool updateDisplay = true;        //when true, write display buffer to screen in the next loop
unsigned long spinDownTimer = 0;  //counts up while the flywheels are spinning down
unsigned long lastRevTime = 0;    //stores timestamp of last revved
unsigned long safetyTimer = 0;    //counts up while revving for safety shutoff
unsigned long rampTime = 0;       //stores timestamp of last trigger pull in Ramping mode
float voltage = 0.0;              //numerical voltage

//tach stuff
unsigned int speedSetpoint;
const byte goodTachsToFire = 5;
byte speedOffsetMargin;
const byte speedOffsetMarginMin = 45;
const byte speedOffsetMarginMax = 22;
byte goodTachCount = 0;
volatile unsigned long thisPulseTime0;
volatile unsigned long thisPulseTime1;
volatile unsigned long pulseLength0;
volatile unsigned long pulseLength1;
volatile unsigned long lastPulseTime0;
volatile unsigned long lastPulseTime1;
volatile boolean drive0TachValid = false;
volatile boolean drive1TachValid = false;

void loadProfile(byte profile) {
  if (EEPROM.read(profile * sizeof(Profile) + EEPROMOFFSET) == 0xFF) {
    Profile defaultProfile;
    memcpy_P(&defaultProfile, &DEFAULT_PROFILES[profile], sizeof(Profile));
    EEPROM.put(profile * sizeof(Profile) + EEPROMOFFSET, defaultProfile);
  }

  // Load profile from EEPROM
  Profile loadedProfile;
  EEPROM.get(profile * sizeof(Profile) + EEPROMOFFSET, loadedProfile);

  // Apply settings
  targetRPM = loadedProfile.targetRPM;
  burstCount = loadedProfile.burstCount;
  fireRate = loadedProfile.fireRate;
  spinDownTime = loadedProfile.spinDownTime;
  singleShotDelay = ceil((1000 - (fireRate * singleShotPulse)) / fireRate); 
}

void saveCurrentProfile() {
  Profile currentSettings = {
    targetRPM,
    burstCount,
    fireRate,
    spinDownTime
  };
  EEPROM.put(currentProfile * sizeof(Profile) + EEPROMOFFSET, currentSettings);
}

void setup() {
  //tach stuff
  pinMode(TACH_PIN_0, INPUT_PULLUP);
  pinMode(TACH_PIN_1, INPUT_PULLUP);
  cli();
  EICRA = _BV(ISC11) | _BV(ISC10) | _BV(ISC01) | _BV(ISC00);  //INT0 and INT1 triggered on rising edge
  sei();

  pinMode(VOLT_PIN, INPUT);
  pinMode(SOLENOID_PIN, OUTPUT);
  menu.debounceTime = 40;
  trig.debounceTime = 10;
  rev.debounceTime = 10;
  trig.longClickTime = 333;   //how long to wait before locking fire mode when menu and trigger held
  menu.longClickTime = 1500;  //how long to wait before switching to settings mode when menu button held
  trig.multiclickTime = 0;
  menu.multiclickTime = 0;
  rev.multiclickTime = 0;

  //startup animation
  uView.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  uView.setCursor(0, 0);
  uView.setTextColor(1);
  uView.setTextSize(2);
  uView.setTextWrap(false);
  uView.clearDisplay();
  uView.drawBitmap(0, 0, splash, 128, 64, 1);
  uView.setCursor(8, 40);

  String str = "Profile 1";
  currentProfile = PROFILE_HIGH;
  if (digitalRead(MENU_PIN) == LOW) {
    currentProfile = PROFILE_LOW;
    str = "Idle Pro";
    idle = true;
  }
  if (digitalRead(TRIG_PIN) == LOW) {
    if (currentProfile == PROFILE_HIGH) {
      currentProfile = PROFILE_MED;
      str = "Profile 2";
    } else {
      currentProfile = PROFILE_T;
      menu.longClickTime = 500;
      str = "Tournament";
      tourney = true;
      idle = true;
    }
  }

  loadProfile(currentProfile);

  uView.print(str);
  uView.display();
  uView.clearDisplay();
  delay(1000);

  //initialize ESCs
  esc.attach(ESC_PIN, OFF, FULL);
  esc.writeMicroseconds(OFF);
  updateSpeed(idle ? MINRPM : targetRPM, 10);

  voltage = floor((analogRead(VOLT_PIN) * VOLTAGE_DIVIDER));
}

void loop() {
  //read button states
  trig.Update();
  menu.Update();
  rev.Update();

  //low power lockout
  if ((byte)voltage < 128) {
    lowBatt = true;
    updateDisplay = true;
  }

  //menu button handling
  //if menu button is held switch to settings screen. if it is pressed on the main screen, change fire mode
  if (!trig.depressed && menu.clicks != 0) {
    //normal behavior (non-tournament mode)
    if (!tourney) {
      if (menu.clicks < 0) {
        updateDisplay = true;
        settings = !settings;
        menu.clicks = 0;
        //update target RPM in case it was changed on settings screen
        if (!settings) {
          idling = false; //reset this
          saveCurrentProfile();
          updateSpeed(targetRPM, 10);
          spinOff();
        }
      } else if (menu.clicks > 0 && !settings && !lock) {
        updateDisplay = true;
        mode++;

        //initialize ramping timer
        if (mode == RAMPING) {
          rampTime = millis();
        }

        if (mode > RBURST) {
          mode = SEMI;
        }
      }
      //tournament mode (disable settings screen and require menu button hold to switch firing mode)
    } else {
      if (menu.clicks < 0) {
        updateDisplay = true;
        mode++;

        if (mode > REVERSE) {
          mode = SEMI;
        }
      }
    }
  }

  //trigger button handling
  //if not in settings mode, while trigger is pressed, spin up the flywheels and fire
  if (!settings) {
    
    if(rev.depressed){
      spinOn();
    } else {
      //if in idle mode and flywheel speed set to idle RPM, spin up the flywheels for pre-rev
      if (idle && idling) {
        esc.writeMicroseconds(FULL);
      }
    }
    
    //make sure we haven't been revving for longer than the safety timeout and battery voltage is OK
    if (trig.depressed && !menu.depressed && safetyTimer < SAFETYTIMEOUT && !lowBatt) {
      spinOn();
      if (revved) {
        switch (mode) {
          case SEMI:
          case BINARY:
            //fire once then set flag that prevents additional shots until trigger is released
            if (!fired) {
              fire(1);
              fired = true;
            }
            break;
          case BURST:
            if (!fired) {
              fire(burstCount);
              fired = true;
            }
            break;
          case AUTO:
          case DEVOTION:
            trig.Update();
            if (trig.depressed) {
              fire(1);
            }
            break;
          case RAMPING:
            //semi auto behavior while rampCount < 3
            if (rampCount < 3 && !fired) {
              fire(1);
              rampCount++;
              fired = true;
              //switch to full auto if 3 shots are fired within RAMPWAIT milliseconds
            } else if (rampCount >= 3) {
              ramp = true;
              trig.Update();
              if (trig.depressed) {
                fire(1);
              }
              rampTime = millis();
            }

            //reset ramping counter after rampWait milliseconds
            if (!ramp && (millis() - rampTime >= RAMPWAIT)) {
              rampTime = millis();
              rampCount = 0;
            }
            break;
          case REVERSE:
          case RBURST:
            //do nothing, but set the fired flag to cause a shot to be fired on trigger release
            fired = true;
            break;
        }
      }
    } else {

      if (safetyTimer >= SAFETYTIMEOUT) {
        spinOff();
        while (trig.depressed) {
          trig.Update();
          delay(5);
        }
      }

      //if in binary or reverse mode, fire on trigger release
      if (fired && revved) {
        if (mode == BINARY || mode == REVERSE) {
          fire(1);
        } else if (mode == RBURST) {
          fire(burstCount);
        }
      }

      //disable full auto ramping if wait period has elapsed, clear ramping counter
      if (ramp && (millis() - rampTime >= RAMPWAIT)) {
        ramp = false;
        rampCount = 0;
      }

      devotionCount = 0;
      fired = false;
      if (millis() - spinDownTimer >= spinDownTime) {
        if (!rev.depressed){
          spinOff();
        }
      }
      digitalWrite(SOLENOID_PIN, LOW);
    }

    //if trigger is held whilst menu is held, lock/unlock the fire mode
    if (menu.depressed && trig.clicks < 0) {
      updateDisplay = true;
      lock = !lock;
      menu.clicks = 0;
    }

    //update battery voltage reading every BATTFREQ milliseconds
    //only when not revving to prevent low readings due to sag
    if (millis() % BATTFREQ == 0 && !revved) {
      updateDisplay = true;
      voltage = floor((analogRead(VOLT_PIN) * VOLTAGE_DIVIDER));
    }

    //write the display buffer if flag is set & only when not revving
    //prevents screen write delay from affecting operation
    if (updateDisplay && !revved) {
      displayMain();
      updateDisplay = false;
    }
  } else {
    //settings menu
    //update screen every second for runtime display
    if (millis() % 1000 == 0) {
      updateDisplay = true;
    }

    //menu press switches selected parameter
    if (menu.clicks > 0) {
      if (trig.depressed) {
        // Reset current profile to defaults
        Profile defaultProfile;
        memcpy_P(&defaultProfile, &DEFAULT_PROFILES[currentProfile], sizeof(Profile));

        // Apply default values
        targetRPM = defaultProfile.targetRPM;
        burstCount = defaultProfile.burstCount;
        fireRate = defaultProfile.fireRate;
        spinDownTime = defaultProfile.spinDownTime;

        // Save to EEPROM
        EEPROM.put(currentProfile * sizeof(Profile) + EEPROMOFFSET, defaultProfile);

        // Visual feedback
        uView.clearDisplay();
        uView.setCursor(0, 28);
        uView.print(F("Profile Reset!"));
        uView.display();
        delay(1000);  // Show message briefly

        updateDisplay = true;
        menu.clicks = 0;  // Clear the click so it doesn't change selection
      } else {
        updateDisplay = true;
        selected++;

        if (selected > 4) {
          selected = 1;
        }
      }
    }

    //trigger press changes parameter value
    if (trig.clicks > 0) {
      updateDisplay = true;
      switch (selected) {
        case 1:
          targetRPM += FIVEPERCENT;
          if (targetRPM > MAXRPM) {
            targetRPM = MINRPM;
          }
          break;
        case 2:
          burstCount++;
          if (burstCount > 10) {
            burstCount = 2;
          }
          break;
        case 3:
          fireRate += 1;
          if (fireRate > MAXFIRERATE) {
            fireRate = MINFIRERATE;
          }
          singleShotDelay = ceil((1000 - (fireRate * singleShotPulse)) / fireRate);
          break;
        case 4:
          spinDownTime += 100;
          if (spinDownTime > 2000) {
            spinDownTime = 0;
          }
          break;
      }
    }
    esc.writeMicroseconds(OFF);
    digitalWrite(SOLENOID_PIN, LOW);
    shotCount = 0;

    if (updateDisplay) {
      displaySettings(selected);
      updateDisplay = false;
    }
  }
}

//actuate the solenoid, and increment fired counter
void fire(byte shots) {
  for (byte k = 0; k < shots; k++) {
    digitalWrite(SOLENOID_PIN, HIGH);
    delay(singleShotPulse);
    digitalWrite(SOLENOID_PIN, LOW);
    if (mode != DEVOTION) {
      //reduce delay to minimum when in binary, semi, or ramping for best trigger response
      delay((mode == BINARY || mode == SEMI || (mode == RAMPING && !ramp) || mode == REVERSE) ? MAXROFDELAY : singleShotDelay);
    } else {
      if (devotionCount >= 10) {
        delay(MAXROFDELAY);  //clamp to max ROF after 10 shots
      } else {
        delay(165 + (0.5 * devotionCount * devotionCount) - (18 * devotionCount));  //polynomial fire rate ramping for Devotion mode
        devotionCount++;
      }
    }
  }
  shotCount += shots;
  updateDisplay = true;
}

//spin up the flywheels, delay activities for set period to allow flywheels to accelerate. reset the spin down timer (thanks snakerbot!)
void spinOn() {
  //if in idle mode and flywheel speed set to idle RPM, first update the targetRPM to the actual RPM
  if (idle && idling) {
    idling = false;
    updateSpeed(targetRPM, 1);
  }
  if (!revved) {
    cli();
    EIMSK = 0b00000011;  //INT0 and INT1 on
    sei();
    esc.writeMicroseconds(FULL);
    //Record trigger down event timestamp
    long lastTriggerDown = millis();
    //Wait for both motors to have vaild tach. This effectively checks that they are actively toggling their tach lines
    //These flags are cleared before we get here, so the only thing that would have turned them on is the tach ISR
    while (!(drive0TachValid && drive1TachValid) && ((millis() - lastTriggerDown) <= FAILTIME)) {
      delay(1);
    }
    if ((millis() - lastTriggerDown) >= FAILTIME) {
      //Whoops. We exited the invalid-tach trap because one or more motors failed to produce any speed feedback in a reasonable time. Perhaps something got unplugged
      //Shut down motors
      spinOff();
      //Then trap trigger down state here. Require a trigger reset to reattempt
      while (trig.depressed) {
        trig.Update();
        delay(5);
      }
      goodTachCount = 0;
    } else {
      //We exited the invalid tach trap because both tachs went valid. This is what's supposed to happen
      //Wait for motors to reach speed and stay there for goodTachsToFire *consecutive* reads, and then fire
      while ((goodTachCount < goodTachsToFire) && ((millis() - lastTriggerDown) <= FAILTIME)) {
        //Increment goodTachCount every time both speeds are in range, with poll cycle about as long as the target period (little longer in practice)
        if ((pulseLength0 <= (speedSetpoint + speedOffsetMargin)) && (pulseLength1 <= (speedSetpoint + speedOffsetMargin))) {
          goodTachCount++;
        } else {
          goodTachCount = 0;  //Strictly reject any out of range reads (This is in the range of 3-7 reads in a row we're looking for to fire so no sense in allowing "mulligans")
        }
        delayMicroseconds(speedSetpoint);  //Abuse target signal period as polling period
      }
      //We returned. But why? If it's a timeout, then goodTachCount will not have reached goodTachsToFire
      if (goodTachCount < goodTachsToFire) {
        //Whoops. We exited because motors failed to reach speed setpoint in a reasonable time
        //Shut down motors
        spinOff();
        while (trig.depressed) {
          trig.Update();
          delay(5);
        }
        goodTachCount = 0;
      } else {
        cli();
        EIMSK = 0b00000000;  //INT0 and INT1 off
        sei();
        revved = true;  //say it's ready to fire, close everything out so it'll go to the fire control code
        spinDownTimer = millis();
        safetyTimer = millis() - lastRevTime;
      }
    }
  } else {
    spinDownTimer = millis();
    safetyTimer = millis() - lastRevTime;
  }
}

//cut power to the flywheels
void spinOff() {
  //if in idle mode and flywheel speed is not at idle RPM, set flywheel speed to idle RPM
  if (idle && !idling) {
    idling = true;
    updateSpeed(MINRPM, 1);
  } else { //only if not in idle mode, shut off the flywheels
    if(!idle){
      esc.writeMicroseconds(OFF);
    }
  }
  revved = false;
  lastRevTime = millis();
  safetyTimer = 0;
  goodTachCount = 0;
  drive0TachValid = false;
  drive1TachValid = false;
  cli();
  EIMSK = 0b00000000;  //INT0 and INT1 off
  sei();
}

//main (firing) screen display output
void displayMain() {
  byte throttle = map(targetRPM, MINRPM, MAXRPM, 0, 100);
  byte countH = 60;
  uView.clearDisplay();
  uView.setTextSize(1);
  uView.setCursor(0, 0);
  uView.print(F("SPIRIT"));

  //print the battery picture or numerical voltage
  if (batteryPicture) {
    battPicture();
  } else {
    uView.setCursor(99, 0);
    uView.print(voltage / 10.0, 1);
    uView.print(F("V"));
  }
  uView.drawFastHLine(0, 9, 128, 1);

  if (!lowBatt) {
    uView.setFont(&FreeSansBoldOblique24pt7b);
    countH = 50;
    if (shotCount > 999) {
      shotCount = 0;
    }
    if (shotCount > 9) {
      countH -= 14;
    }
    if (shotCount > 99) {
      countH -= 14;
    }
    uView.setCursor(countH, 47);
    uView.print(shotCount);
  } else {
    uView.setCursor(0, 26);
    uView.print(F("BATTERY\n CRITICAL!"));
  }

  countH = 117;
  uView.setFont();
  uView.drawFastHLine(0, 53, 128, 1);
  if (lowBatt) {
    throttle = 0;
  }
  if (throttle > 9) {
    countH -= 6;
  }
  if (throttle > 99) {
    countH -= 6;
  }
  uView.setCursor(countH, 56);
  uView.print(throttle);
  uView.print(F("%"));
  uView.setCursor(0, 56);

  if (!lowBatt) {
    switch (mode) {
      case SEMI:
        uView.print(F("Semi"));
        break;
      case BURST:
        uView.print(F("Burst"));
        break;
      case AUTO:
        uView.print(F("Auto"));
        break;
      case BINARY:
        uView.print(F("Binary"));
        break;
      case DEVOTION:
        uView.print(F("Devotion"));
        break;
      case RAMPING:
        uView.print(F("Ramping"));
        break;
      case REVERSE:
        uView.print(F("Reverse"));
        break;
      case RBURST:
        uView.print(F("R.Burst"));
        break;
    }
    if (lock) {
      uView.print(F("*"));
    }
  } else {
    uView.print(F("STOP"));
  }

  uView.display();
}

//settings screen display output
void displaySettings(byte selected) {
  //run timer
  unsigned int t = millis() / 1000;
  byte hours = t / 3600;
  t %= 3600;
  byte minutes = t / 60;
  t %= 60;
  byte seconds = t;

  byte throttle = map(targetRPM, MINRPM, MAXRPM, 0, 100);

  uView.clearDisplay();
  uView.setCursor(0, 0);
  uView.print(F("0"));
  uView.print(hours);
  uView.print(F(":"));
  if (minutes < 10) {
    uView.print(F("0"));
  }
  uView.print(minutes);
  uView.print(F(":"));
  if (seconds < 10) {
    uView.print(F("0"));
  }
  uView.print(seconds);
  uView.drawFastHLine(0, 9, 128, 1);
  uView.setCursor(0, 11);
  uView.print(F("Throttle: "));
  uView.setCursor(96, 11);
  if (selected == 1) {
    uView.setTextColor(0, 1);
  }
  uView.print(throttle);
  uView.setTextColor(1, 0);
  uView.setCursor(0, 25);
  uView.print(F("Burst Count: "));
  uView.setCursor(96, 25);
  if (selected == 2) {
    uView.setTextColor(0, 1);
  }
  uView.print(burstCount);
  uView.setTextColor(1, 0);
  uView.setCursor(0, 39);
  uView.print(F("Fire Rate: "));
  uView.setCursor(96, 39);
  if (selected == 3) {
    uView.setTextColor(0, 1);
  }
  uView.print(fireRate);
  uView.setTextColor(1, 0);
  uView.setCursor(0, 53);
  uView.print(F("Spin Down: "));
  uView.setCursor(96, 53);
  if (selected == 4) {
    uView.setTextColor(0, 1);
  }
  uView.print(spinDownTime);
  uView.setTextColor(1, 0);
  uView.display();
}

//update the Flyshot set speed via PWM signal (thanks dpairsoft!)
void updateSpeed(long RPM, byte attempts) {
  speedOffsetMargin = map(RPM, MINRPM, MAXRPM, speedOffsetMarginMin, speedOffsetMarginMax);
  esc.detach();
  unsigned int setPoint = 320000000 / (RPM * (MOTOR_POLES / 2));
  speedSetpoint = ((3 * setPoint) / 16);
  unsigned int packet = setPoint | 0x8000;

  for (byte pksend = 0; pksend < attempts; pksend++) {
    // Send the leading throttle-range pulse
    digitalWrite(ESC_PIN, HIGH);
    delayMicroseconds(1000);

    digitalWrite(ESC_PIN, LOW);
    delayMicroseconds(10);

    // Send the packet MSB first
    for (short j = 15; j >= 0; j--) {
      if (packet & (0x0001 << j)) {
        // Send a T1H pulse
        digitalWrite(ESC_PIN, HIGH);
        delayMicroseconds(T1H_PULSE);
        digitalWrite(ESC_PIN, LOW);
        delayMicroseconds(TL_PULSE);
      } else {
        // Send a T0H pulse
        digitalWrite(ESC_PIN, HIGH);
        delayMicroseconds(T0H_PULSE);
        digitalWrite(ESC_PIN, LOW);
        delayMicroseconds(TL_PULSE);
      }
    }

    // Send the trailing throttle-range pulse
    digitalWrite(ESC_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(ESC_PIN, LOW);
    delayMicroseconds(10);
  }

  esc.attach(ESC_PIN, OFF, FULL);
}

//read battery voltage and display as image
void battPicture() {
  byte v = (byte)voltage;

  if (v >= 160) {
    bFull();
  } else if (v < 160 && v >= 150) {
    bOK();
  } else if (v < 150 && v >= 135) {
    bLow();
  } else {
    bDead();
  }
}

//battery bitmaps
void bFull() {
  uView.drawPixel(119, 0, 1);
  uView.drawPixel(120, 0, 1);
  uView.drawPixel(121, 0, 1);
  uView.drawPixel(122, 0, 1);
  uView.drawPixel(123, 0, 1);
  uView.drawPixel(124, 0, 1);
  uView.drawPixel(125, 0, 1);
  uView.drawPixel(126, 0, 1);
  uView.drawPixel(127, 0, 1);
  uView.drawPixel(119, 1, 1);
  uView.drawPixel(127, 1, 1);
  uView.drawPixel(118, 2, 1);
  uView.drawPixel(119, 2, 1);
  uView.drawPixel(121, 2, 1);
  uView.drawPixel(123, 2, 1);
  uView.drawPixel(125, 2, 1);
  uView.drawPixel(127, 2, 1);
  uView.drawPixel(118, 3, 1);
  uView.drawPixel(119, 3, 1);
  uView.drawPixel(121, 3, 1);
  uView.drawPixel(123, 3, 1);
  uView.drawPixel(125, 3, 1);
  uView.drawPixel(127, 3, 1);
  uView.drawPixel(119, 4, 1);
  uView.drawPixel(127, 4, 1);
  uView.drawPixel(119, 5, 1);
  uView.drawPixel(120, 5, 1);
  uView.drawPixel(121, 5, 1);
  uView.drawPixel(122, 5, 1);
  uView.drawPixel(123, 5, 1);
  uView.drawPixel(124, 5, 1);
  uView.drawPixel(125, 5, 1);
  uView.drawPixel(126, 5, 1);
  uView.drawPixel(127, 5, 1);
}
void bOK() {
  uView.drawPixel(119, 0, 1);
  uView.drawPixel(120, 0, 1);
  uView.drawPixel(121, 0, 1);
  uView.drawPixel(122, 0, 1);
  uView.drawPixel(123, 0, 1);
  uView.drawPixel(124, 0, 1);
  uView.drawPixel(125, 0, 1);
  uView.drawPixel(126, 0, 1);
  uView.drawPixel(127, 0, 1);
  uView.drawPixel(119, 1, 1);
  uView.drawPixel(127, 1, 1);
  uView.drawPixel(118, 2, 1);
  uView.drawPixel(119, 2, 1);
  uView.drawPixel(123, 2, 1);
  uView.drawPixel(125, 2, 1);
  uView.drawPixel(127, 2, 1);
  uView.drawPixel(118, 3, 1);
  uView.drawPixel(119, 3, 1);
  uView.drawPixel(123, 3, 1);
  uView.drawPixel(125, 3, 1);
  uView.drawPixel(127, 3, 1);
  uView.drawPixel(119, 4, 1);
  uView.drawPixel(127, 4, 1);
  uView.drawPixel(119, 5, 1);
  uView.drawPixel(120, 5, 1);
  uView.drawPixel(121, 5, 1);
  uView.drawPixel(122, 5, 1);
  uView.drawPixel(123, 5, 1);
  uView.drawPixel(124, 5, 1);
  uView.drawPixel(125, 5, 1);
  uView.drawPixel(126, 5, 1);
  uView.drawPixel(127, 5, 1);
}
void bLow() {
  uView.drawPixel(119, 0, 1);
  uView.drawPixel(120, 0, 1);
  uView.drawPixel(121, 0, 1);
  uView.drawPixel(122, 0, 1);
  uView.drawPixel(123, 0, 1);
  uView.drawPixel(124, 0, 1);
  uView.drawPixel(125, 0, 1);
  uView.drawPixel(126, 0, 1);
  uView.drawPixel(127, 0, 1);
  uView.drawPixel(119, 1, 1);
  uView.drawPixel(127, 1, 1);
  uView.drawPixel(118, 2, 1);
  uView.drawPixel(119, 2, 1);
  uView.drawPixel(125, 2, 1);
  uView.drawPixel(127, 2, 1);
  uView.drawPixel(118, 3, 1);
  uView.drawPixel(119, 3, 1);
  uView.drawPixel(125, 3, 1);
  uView.drawPixel(127, 3, 1);
  uView.drawPixel(119, 4, 1);
  uView.drawPixel(127, 4, 1);
  uView.drawPixel(119, 5, 1);
  uView.drawPixel(120, 5, 1);
  uView.drawPixel(121, 5, 1);
  uView.drawPixel(122, 5, 1);
  uView.drawPixel(123, 5, 1);
  uView.drawPixel(124, 5, 1);
  uView.drawPixel(125, 5, 1);
  uView.drawPixel(126, 5, 1);
  uView.drawPixel(127, 5, 1);
}
void bDead() {
  uView.drawPixel(119, 0, 1);
  uView.drawPixel(120, 0, 1);
  uView.drawPixel(121, 0, 1);
  uView.drawPixel(122, 0, 1);
  uView.drawPixel(123, 0, 1);
  uView.drawPixel(124, 0, 1);
  uView.drawPixel(125, 0, 1);
  uView.drawPixel(126, 0, 1);
  uView.drawPixel(127, 0, 1);
  uView.drawPixel(119, 1, 1);
  uView.drawPixel(127, 1, 1);
  uView.drawPixel(118, 2, 1);
  uView.drawPixel(119, 2, 1);
  uView.drawPixel(127, 2, 1);
  uView.drawPixel(118, 3, 1);
  uView.drawPixel(119, 3, 1);
  uView.drawPixel(127, 3, 1);
  uView.drawPixel(119, 4, 1);
  uView.drawPixel(127, 4, 1);
  uView.drawPixel(119, 5, 1);
  uView.drawPixel(120, 5, 1);
  uView.drawPixel(121, 5, 1);
  uView.drawPixel(122, 5, 1);
  uView.drawPixel(123, 5, 1);
  uView.drawPixel(124, 5, 1);
  uView.drawPixel(125, 5, 1);
  uView.drawPixel(126, 5, 1);
  uView.drawPixel(127, 5, 1);
  uView.drawPixel(114, 0, 1);
  uView.drawPixel(114, 1, 1);
  uView.drawPixel(114, 2, 1);
  uView.drawPixel(114, 3, 1);
  uView.drawPixel(114, 5, 1);
}

//tach ISRs
ISR(INT1_vect) {
  //pin 2 tach input handler - motor 0
  lastPulseTime0 = thisPulseTime0;
  thisPulseTime0 = micros();
  pulseLength0 = thisPulseTime0 - lastPulseTime0;
  drive0TachValid = true;
}

ISR(INT0_vect) {
  //pin 3 tach input handler - motor 1
  lastPulseTime1 = thisPulseTime1;
  thisPulseTime1 = micros();
  pulseLength1 = thisPulseTime1 - lastPulseTime1;
  drive1TachValid = true;
}