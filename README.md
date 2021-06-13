
# Assistive Technology Challenge: Team Communication
## HackaHealth Geneva 

## Context 
A device  has been developed by this team, to serve as a communication interface for an 8-year-old child that cannot speak but is eager to interact with his environment. 
He needs an efficient tool to communicate and control his tablet (that runs the Grid3 software). 
The goal is to give him more autonomy and a more accurate communication system. 
Our challenges are first of all to create a device easy to use for a child and that does not increase his fatigue. 
We have to consider the low precision of the movements as well as a not very well controlled head hold. 
Our device must meet the criteria of safety, biocompatibility and be easy to use for both the family and the therapists. 
In addition, we aim to produce an object that is easily reproducible, open source and within a reasonable budget. 
This repository contains the codes used for this device, as well as some files for reproducibility of the device itself. 

## File description 
**CAD_files**

CAD_files/DFX_laser_cutting/ : *CAD files that can be used for laser cutting the pieces of the device*

- cover_window_PMMA_3mm.dxf
- frame_window_PMMA_8mm.dxf
- support_1_CP_aviation_1mm.dxf
- support_2_MDF_4mm.dxf
- support_3_MDF_4mm.dxf

 

CAD_files/F3D_files/ : *Fusion files of the different pieces for 3d printing*

- Support_1.f3d
- Support_2.f3d
- Support_3.f3d
- closing_electronic_box.f3d
- cover_window.f3d
- electronic_boc.f3d
- fixation.f3d
- safety_ring.f3d
- tube_cap.f3d
- window_frame_final v4.f3d


CAD_files/STEP_files/ : *Files for 3D printing*

- Support_1.step
- Support_2.step
- Support_3.step
- closing_electronic_box.step
- cover-window.step
- electronic_box.step
- fixation.step
- frame_window.step
- safety_ring.step
- tube_cap.step

CAD_files/STL_3D_printing/

- closing_electronic_box_1x.stl
- electronic_box_1x.stl
- fixation_4x.stl
- safety_ring_2x.stl
- tube_cap_4x.stl




**arduino_code**  : *Files for communication with the arduino MCU*

- arduino_code/definitions.h   : *Definition of useful variables*
                                 Important constants:
                                 
  - TIME_FOR_ACTIVATE: Define the time that a sensor is touched before it becomes active [ms]
                                 
  - ADVANCE_TIME: Define the time for which the sensor is activated. (allowed time to scroll) [ms]
                                 
  - TIME_STEP: Define the time spend to navigate between each case in Grid 3 [ms]

- arduino_code/pitches.h	:   *Definition of sound notes for the buzzer*

- arduino_code/prototype_final_V9.ino : *Useful file implementing the module MPR121 and the module LED WS2813*

  - Sensing logic:  
      - *We keep track of the touched values and  taking into consideration a specific priority order we activate a sensor after TIME_FOR_ACTIVATION* 
      - *Activating a sensor means sending a message through Serial communication with the value identifying it and then waiting TIME_STEP to send another identification value until reaching the ADVANCE_TIME*
           
  
                              
                                                      
**python_code**

- python_code/Serial_1.0.exe  : *file to be executed on the terminal when lauching the device connected to the tablet computer*


- python_code/Serial_1.0.py   : *useful functions for controling the device*





## Requirements 
pip install -r requirements.txt to install all the dependencies

## Launch 
- Launch Grid3 software
- Execute Serial_1.0.exe on the terminal of the computer
- Choose parameters of use via the terminal 

