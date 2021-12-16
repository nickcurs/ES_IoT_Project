# ES_IoT_Project
Fall 2021 Embedded Systems IoT Final Project
This repo contains all involved files for our IoT project. 

-The main.c file is the code that is to be uploaded to the MSP430. The code gets the values from the sensors, processes the raw data, then sends it over UART to the comport on the computer. 

-The folder containing the Visual Studio project in C# contains the code for the GUI and IoT aspect of the project. The GUI reads out the data from the com port and allows the dataflow to ThingSpeak to be started and stopped. 
