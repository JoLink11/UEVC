## UEVC Universal Exhaust Valve Control ##

The funtions of this device will be to control a servo or a solenoid to open an exhaust valve on a two stroke engine.

STATUS: WORK IN PROGRESS!!!
USE AT YOUR OWN RISK!

BEFOR USING THIS CODE:
Please delete or modify for your needs:
    monitor_port = COM3
    upload_port = COM3
in the platformio.ini-file


Features:
* saving setting even after power off
* change open and close settings via webserver
* check and cleaning cycle at startup of the valve 
* open at define RPM or open linear at closing to open RPM

Setting changeable via Webserver:
* RPM for opening valve
* Threshold ( OpenRPM - Threshold = Closing RPM)
* Servo position opened
* Servo position closed
* How many pluse per Revolution
* activate linear transition for open an close valve
