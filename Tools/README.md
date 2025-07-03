Servo motor debugging:
Write the SteeringEngineDebug code into the mainframe, then open the folder starting with "FD", open FD.exe, and set the baud rate to 1000000.
Attention!!! The first calibration of the servo motor may cause a bus conflict due to the same ID. In this case, you need to unplug the other servo motor.





Writing emoticons into ESP32:
1. Prepare the GIF file, convert each frame to a bitmap modulus using gif_to_bin, and finally generate a.bin file. Create a new "data" folder in the main program directory, and copy the bin file of the emoji package to the "data" folder.
2. You need to install Arduino IDE 1.8. Since the spiffs flashing plugin used does not support Arduino 2 version, the plugin's compressed package name is: ESP32FS-1.1. Installation method can be referred to https://github.com/me-no-dev/arduino-esp32fs-plugin
3. After the above installation, open the project in Arduino IDE 1.8, then click the menu bar's "tool>ESP32 Sketch Data Upload", and the plugin will flash the contents in the "data" folder to the esp32.



