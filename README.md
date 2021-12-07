# UEVC Universal Exhaust Valve Control #

The funtions of this device will be to control a servo or a solenoid to open an exhaust valve on a two stroke engine.

## &#x1F534; STATUS: WORK IN PROGRESS!!! :red_circle: ## 
## &#x1F534; USE AT YOUR OWN RISK! :red_circle: ##
### &#x1F7E0; BEFOR USING THIS CODE: :orange_circle: ###
* delete or modify for your needs in the platformio.ini-file:
    * <code> monitor_port = COM3</code>
    * <code> upload_port = COM3</code>
* setup your setting in UEVC_Settings.h
    * select your board with or without display
    * maybe change WiFi-name an password

### Features: ###
* saving setting even after power off
* change open and close settings via webserver
* check and cleaning cycle at startup of the valve 
* open at define RPM or open linear at closing to open RPM

### Setting can be edit an changed via Webserver: ###
* RPM for opening valve
* Threshold ( OpenRPM - Threshold = Closing RPM)
* Servo position opened
* Servo position closed
* How many pluse per Revolution
* activate linear transition for open an close valve

### Ideas to be discussed: ###
* add accelerometer(-interface) (e.g. bno055/adxl345) for analyse and evaluate settings via logfile
* add gps(-interface) for analyse and evaluate settings via logfile
* download the logfile via webinterface
