To build: go to your Documents folder, C:\Users\USERNAME\Documents\Arduino\libraries\Adafruit_GFX_Library\Fonts and replace the font file with the one included in this repo

For board use the ISP header and another arduino flashed with the ISP programmer see https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/ (I use an uno)

"Burn bootloader" first, then flash your firmware using "Flash using programmer"

I know it's confusing, but the main branch here is Spirit + rev trigger code. 

Main is for Spirit + rev (works fine without rev)
G19 conversion is for existing (neutron based) g19's to convert to spririt based firmware
G19H is for G19H (meaning the PCB or spirit wiring)
Ophidian is for ophidian

Changes from default spirit code:

    Uses EEPROM (settings save after being powered off)
    
    4 boot selectable profiles:
      Profile 1 (default): No idle
      Profile 2 (hold trigger): No idle
      Idle Profile (hold menu): Idle
      Tournament (hold both): Idle, fire mode is the only setting that can be changed by long holding the menu button
    
    Usage:
         Hold menu button to change settings in the 3 profiles.
         Use the menu button to switch between settings, and trigger to change values. 
         Long hold menu to exit and save settings to profile.
