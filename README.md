# IOT Winter 2024/2025 Surfer Data Collector by:
1. Loay
2. Shada
3. Mousa

# Details about the project
// to be added later

# Folder description:
1. SurfboardMainUnit:
   
   This folder is used to program the Surfboard main unit which consists of the controller and a sampler.
   The device should only include one control unit.
   The control unit's properties, descriptions, and features are below.
   
2. SurfboardSamplingUnit:
   
   This folder is used to program the Surfboard sampling unit(s).
   The device is modular and can support multiple sampling units.
   The sampling unit's properties, descriptions, and features are below.

3. libraries/IOT_TECHNION_SURFBOARD:

   This folder includes a library that is used in SurfboardSamplingUnit and SurfboardMainUnit.
   Make sure to move this library to the Arduino libraries folder locally!
4. FireStore Files:

   This Folder includes the files that were integrated with fireStore to support files uploading and merging and exporting.
   File index.js includes the endpoints used to connect with FireBase and Storage.
   File index.HTML is the user interface page hosted in FireStore.

# Arduino/ESP libraries used for the project:
   RTClib (by Adafruit) - version 2.1.4
   HX711 (by Rob Tillaart) - version 0.5.2
   SparkFun BNO080 Cortex Based IMU (by SparkFun) - version 1.1.12

# Installation:
1. Wire the necessary hardware as seen in the diagram below and assemble your main and sampling units boxes
2. Move the libraries/IOT_TECHNION_SURFBOARD to your Arduino libraries folder (Documents\Arduino\libraries folder in Windows)
3. Add your parameters to device.config file and move it to the SD card attached to the main unit
4. Open SurfboardMainUnit/SurfboardMainUnit.ino file in your Arduino IDE and compile the code into your main unit
5. For every sampling unit open SurfboardSamplingUnit/SurfboardSamplingUnit.ino in your Arduino IDE, update your sensors' data pins in the file and compile it
   
# Project Poster:
// to be edited later - saved for template use

This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion https://icst.cs.technion.ac.il/

   
