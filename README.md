# IOT Winter 2024/2025 Surfer Data Collector by:
1. Loay Khateeb
2. Shada Hijazi
3. Mousa Abu Alhija

# Details about the project
Surfer Data Collection, a project under the ICST Lab at the Technion – Israel Institute of Technology, captures acceleration force data using multiple sensors strategically placed on the surfer's board. This innovative system provides deep insights into athlete movements and maneuvers, enhancing training and performance analysis with precise data collection.
Our project seamlessly manages wireless connectivity between sensors, ensuring efficient data transmission to the cloud for processing and analysis.

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
   
5. FireStore Files:

   This folder contains files integrated with Firestore to support file uploading, merging, and exporting.
   index.js includes the endpoints used to connect with Firebase and Cloud Storage.
   index.html is the user interface page hosted on Firestore.

6. tests:

   This folder includes tests for different parts of our project.
   
8. unit.config:

   This file includes WIFI details and sensor params that need to be privided.

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
   
# wiring Diagrams:
Main Unit Wiring Diagram:

<img src="https://github.com/user-attachments/assets/a86154c1-af3f-4eb6-b046-5d46ad49b0bc" alt="Image" width="600">

Sampling Unit Wiring Diagram:

<img src="https://github.com/user-attachments/assets/d2808a2c-2120-4290-80fc-dc2f74cc72e5" alt="Image" width="600">
   
# Project Poster:
// to be edited later - saved for template use

This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion https://icst.cs.technion.ac.il/

   
