// SETTINGS

// HARDWARE BOARD

// Select uncomment which board you have

#define ESP32_GENERIC // generic ESP32 with external wired I2C 0.96" OLED display (SSD1306)
//#define WEMOSLOLIN32OLED // WemosLoLin32 with integrated I2C 0.96" OLED display (SSD1306)

// WIFI 
// Change the accesspoint ssid and password to your needs
#define wifi_ssid "UEVC_AcPo" // ssid
#define wifi_pass "UEVC_AcPo" // password

//
// the following settings can be change and store via the webserver!
//
// DEFAULT EXHAUST VALVE PRESETTINGS
#define rpm_default 8100 // rpm to Open
#define threshold_default 100 // rpm_default - threshold = closing rpm
#define PulsesPerRev_default 1 //default ppr # 1 für Drehzahlmessung am Pickup oder Zündspule; 6 für Drehzahlmessung an der Lima (ROTAX)
#define trans_default 0 // default switch config = 0 / linear opening between openrpm and closerpm = 1
//
// DEFAULT SERVO PRESETTINGS
#define positionclose_default 170 // Servo Position Closed UEVC
#define positionopen_default 10 // Servo Position Open UEVC

/////////////////////////////////////////////
// DO NOT CHANGE ANYTHING BELOW THIS LINE! //
// --------------------------------------- //
// ONLY IF YOU ARE KNOW WHAT YOU ARE DOING //
/////////////////////////////////////////////

// SAFETY SETTINGS
#define minrpmopen 1000 // minimum rpm for open vavle
// #define DEBUG_SERIAL