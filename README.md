# Electric Steering for Solar Boat

Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina - Campus Florianópolis

Electronics Academic Department

Degree in Electronics Engineering

Course: Integrative Project III

Professors:

* Robinson Pizzio and Daniel Lohmann

Student:

* Gabriel Ayres Rodrigues

# Summary

1. [Introduction](#introduction)
2. [Conception](#conception)
3. [Design](#design)
4. [Implementation](#implementation)
5. [Operation](#operation)
6. [Considerations](#considerations)
7. [References](#references)

# Introduction

This module aims to control the direction of the tail of the Guarapuvu II vessel, belonging to the IFSC Zênite Solar team.

It is inspired by the previously implemented system, developed by Cesar Dias Parente. The project files can be found [here](https://github.com/ZeniteSolar/MDE18/tree/main).

The Zênite team vessel is operated in solar boat competitions, where high performance and robustness are required. Being the only boat to use electric steering on the competition's context, the team counts on a more reliable system and is able to outmaneuver competition, which is a significant advantage on the Slalom race

The current electronic system was damaged after the competition in March 2022, which is a motivator for its upgrade.

# Conception

Thinking about a steering system, choosing to use an electric steering can bring greater responsiveness even though it has a fraction of the weight of a mechanical system. In addition, mechanical systems can require physical effort from the driver and a greater need for maintenance. 

Below is the diagram representing an electric steering system and its parts.
![Diagram](https://github.com/ZeniteSolar/MDE22/blob/0794ea51c2428cf0b5f956f4646af8164ad6c92d/Imagens/Block%20Diagram.PNG)

Taking into account the above elements, the following problems must be confronted:
- Electronic systems in vehicles must have high robustness due to the vibrations to which they are exposed. In the case of high speed vessels, especially for marine operations, impacts are a big problem (besides oxidation of the circuits).
- As the target vehicle is used in competitions whose focus, and therefore the challenge, is on energy generation and consumption, the solution must be as efficient as possible to ensure the success of the team.
- A competitive environment between vehicles requires high performance from this equipment, not only to achieve victory but to ensure everyone's safety. Therefore, the functionality must be met as well as the other parameters.

To implement the whole, it is necessary to define the requirements of the parts, and the possible proposals to satisfy them.

* ## Sensors

There are some options for rotation sensors such as: inductive, optical, Hall effect sensor, rotation potentiometer, encoder. The most economical and simple-to-implement alternative is a common linear potentiometer, currently used in the Zênite team system.

The most used types for steering sensors on the market are the following: resistive, encoders, and those with GMR technology ([Giant magnetoresistance](https://en.wikipedia.org/wiki/Giant_magnetoresistance)). (see the companies considered in [References](#references))

Sensors of this level are state-of-the-art and therefore out of the question, however it is worth noting that the resistive sensor is still being used (even in common vehicles, such as the [butterfly position sensor](https://www.dpk.com.br/como-funciona-o-sensor-de-posicao-da-borboleta/)).

For a contrast between the leaders: resistive sensors have a disadvantage due to wear and possible poor contact, GMR can suffer interference in environments with a strong magnetic field, and encoders (because they are digital) present a challenge in the trade-off Precision X Complexity (thus, price).

Seeking the balance between robustness, ease of implementation and price, the resistive sensor stands out.
 
* ## Controller

The implemented CAN network eliminates the need for cabling with analog signal between the stern and the bow. This ensures noise reduction in sensor readings and takes advantage of existing CAN cabling, compacting the system.

In addition, communication over the network makes it possible for the module to send data and control the direction using messages from other modules. In this way, the range is open for future implementations that may include:

	Redundancy messages and/or sensors
	Low Power Mode - Enabled by the Data Processing Module
	remote steering
    


A commercial line microcontroller will be enough to guarantee the execution of the desired tasks and response time. In addition to PWM ports and ADC inputs, a microcontroller with a CAN interface like the STM32F103xx or equivalent would be interesting. However, this alternative exceeds the price of IC's transceiver and CAN interface, in addition its programming is more laborious.

Due to its ease of programming and price, the [ATmega328P](https://br.mouser.com/ProductDetail/Microchip-Technology-Atmel/ATMEGA328P-PU?qs=K8BHR703ZXguOQv3sKbWcg%3D%3D) is proposed. For the connection to the CAN network, the most common and affordable IC's are the transceiver
[MCP2551](https://br.mouser.com/datasheet/2/268/20001667G-1115479.pdf) and the CAN/SPI interface circuit [MCP2515](https://br.mouser.com/datasheet/ 2/268/MCP2515_Family_Data_Sheet_DS20001801K-2303489.pdf).

The steering wheel potentiometer reading function will be performed by another module already present in the bow: [MIC19](https://github.com/ZeniteSolar/MIC19)


* ## Power Circuit

The brushed DC motor is driven by an H bridge, currently powered by a 12V battery. The current circuit is implemented by two half-bridge integrated [BTS7960](https://pdf1.alldatasheet.com/datasheet-pdf/view/152657/INFINEON/BTS7960.html).

This component was discontinued, and the best alternative found was [FAN7093](https://www.mouser.com/datasheet/2/149/FAN7093-76982.pdf). As the current one is already in hand and this project is focused on integration and telemetry, the same integrated circuit was used.


* ## Measurements and Feedback

It is essential to provide the CAN network with data on power consumption, as it is a battery-powered power system. Therefore, a current sensor is proposed for each arm of the H-bridge.

Conveniently, the BTS7960 half-bridge integrated circuit has an output pin for current sensing — a current source is connected to the output, and the current IS is proportional to the load current IL according to the measuring resistor RIS.

Alternatively, a circuit like the [INA240](https://www.ti.com/lit/ds/symlink/ina240.pdf?ts=1649772128538) ultra-precise bidirectional current sensor with PWM rejection could be used. From the same family of IC's, the INA826 will be used to measure battery voltage (Hbridge voltage).

These measurements should contribute to the feedback, which will also have the tail position data. Bringing this data to a display ensures that the pilot is aware of the state of the system and the consumption that his riding is causing.

# Design

The module was developed with Kicad software, the project is in the hardware folder of this git and its complete schematic can be viewed [here](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/main/hardware/ PDF/steeringmodule.pdf). Below is the general schematic:

![General Schematic](https://github.com/ayresgit/Modulo-Direcao-Eletrica/blob/a6de4a60345ccac29f99505179099171639378f8/Imagens/steeringmodule_esquema.PNG)

The components chosen from the design analysis are the following: Linear Potentiometers, Atmega328P, BTS7960, MCP2515, MCP2551 (or MCP2561), and INA826.

To conform to the vessel's system modules, the following components will also be required:

	2 female RJ45 connectors,
	2 2-way 2EDGRC female terminal (robust and practical, alternative is to use terminal block),
	28-pin stamped socket for the atmega328p,
	zener diodes to protect the microcontroller's ADC,
	2 oscillator crystals

The Kicad software has a function to export a complete list of materials (BOM - Bill Of Materials). It can be found in the hardware folder, at this [link](https://github.com/ZeniteSolar/MDE22/blob/f59e0732b5ff4f93045a860adb85f9820dcc4e83/hardware/steeringmodule.csv).

Right ahead is the PCB 3D preview. As will be explained in the implementation stage, the PCB was made with the help of a milling machine from the institution.

| Front | Back |
| -----  | ----- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/pcb%20front.PNG" width ="325" height="460">|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/pcb%20back.PNG" width="325" height="460">|


It is worth noting that this repository has an exclusive branch for an ordered board, whose layout was improved and the silkscreen was properly designed to assist in the Implementation stage and possible repairs. This board was not used during the project due to its delivery date.

| Ordered Front | Ordered Back |
| -----  | ----- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/a99deb80abc7bee7c553f9edaa48115d4b41ed4c/Imagens/Encomendada%20Frente.jpg" width ="325" height="530">|<img src="https://github.com/ZeniteSolar/MDE22/blob/a99deb80abc7bee7c553f9edaa48115d4b41ed4c/Imagens/Encomendada%20Tr%C3%A1s.jpg" width="325" height="530">|

# Implementation

### PCB manufacturing

The PCB was made by DAELN's CNC milling machine, operated by the department's technicians, the result is shown below. As the manufactured PCB did not have the desired minimum spacing for the power grid, a grinder was used to exclude unwanted tracks — it is important to remember that this type of tool requires the use of PPE.

| Dremel Adjustments | PPE |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Ajustes%20com%20a%20dremel.jpg" width="250" height="315">|<img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Use%20%C3%B3culos%20de%20prote%C3%A7%C3%A3o.jpg" width="250" height="315">

The board was tinned to protect the circuit against oxidation, and the pathways were made with wires from network cable, cut to size and then soldered in place.

From the order of the components, the soldering was done as follows: SMD, CI, Through-hole connectors being the last ones (to avoid physical stress on the board).

After work on the board, the conclusion was reached that the H-bridge ICs must come before the SMD. This change is interesting because the model needs to be welded with heat, in this case with the aid of a heat gun. The soldered PCB can be seen in the figure.

| Front | Back |
|:-----:|:-----:|
|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/PCB%20soldada%20frente.jpg" width="200" height="325">|<img src="https://github.com/ZeniteSolar/MDE22/blob/3640950bb22653374b23fce8999609d81420ede7/Imagens/PCB%20soldada%20tr%C3%A1s.jpg" width="200" height="325">|

Once the preparation processes are finished, the microcontroller programming begins, which is done in the C language. The libraries used and the source code can be found in the firmware folder of this repository.

# Operation

The operation stage is divided into two parts: Programming and bench tests. Starting with the basic logic, such as which microcontroller peripherals are used, these steps develop together.

#### Programming

Throughout the process, the microchip datasheet for the atmega328p was used as a reference, which can be found in the [References](#references) of this work.

In the configuration of the peripherals, starting with the ADC, a dilemma was found about the orientation of the system potentiometers. As the system is being upgraded, the current configuration was discarded. It is then defined that the developed code presupposes the following relationships:

| Potentiometer | Tail position |
| ------------- | ------------------- |
| Maximum value | starboard boundary |
| Average value | centered |
| Minimum value | port limit |

- ADC

The mechanical system that couples the DC motor to the rotation reading potentiometer has a limit close to 270°. The ability to maneuver over 90° to either side has proven useful in the past.
With that in mind, and considering that extreme precision is not required in controlling this angle, the 8-bit ADC is sufficient for all four channels:

- voltage sensor
- current sensor 1 and 2
- Rotation sensor

Each one is configured by the coefficients in adc.c, just operate some measures with unitary coefficients and then do the calculation.

$$ coefficient = {DesiredValue \over ObtainedValue} $$
    
It is interesting to do this test with the full scale value, as the "Desired Value" is naturally known.



- CAN network

Working with the CAN library: the CAN network is implemented from ID's, identifying the modules on the bus and their messages. To generate these IDs, the Zênite team developed a script that generates these values for the module under development.

This path also brings security, ensuring that there will be no equal identities or other conflicts caused by human error and/or parallel development. All of these can be found in the [CAN_IDS](https://github.com/ZeniteSolar/CAN_IDS) repository.

Having the ID's generated for the module and its messages, it is necessary to change the can_filter.h to include the ID's of who you want to listen to (in this case, this project needs to listen to [MIC19](https://github.com/ZeniteSolar/MIC19 )), and create the request and message sending functions in can_app.c/h.

- PWM

Some key points for PWM operation: Chosen timer, prescaler (desired frequency), duty cycle definition (in this case implemented by the set_pwm function in hbridge.c).

Due to an error in the Design stage, the PWM outputs are connected to the ATmega328P's basic timer pins, Timer/Counter 0. The timer's operating mode must be phase correct PWM (which guarantees greater synchronization in relation to Fast PWM). As described in the datasheet, the frequency can be calculated as follows:

$$ fOCnxPWM = {fclkI/O \over {N * 510}} $$

	fOCnxPWM is the PWM frequency on the OCnx ports (in the case of OC0A and OC0B, in PD5 and PD6)
	fclkI/O is the crystal oscillator clock frequency (16MHz)
	N is the configured prescaler

This timer has fewer bits and less functionality compared to the other two. For the application, the only impact was the use of a lower frequency than desired (~3921.5Hz). This happens due to the lack of prescaler options in Timer/Counter0:
    
	prescaler = 1, fPWM = 31.37kHz (exceeds half-bridge IC limits)
	prescaler = 8 --> 3.92kHz (used, suboptimal)


#### Circuit Tests

The bench tests were carried out with the aid of different voltage sources, in the laboratories of the IFSC campus Florianópolis.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Fontes%20de%20bancada%203A.jpg" width="520" height="340">

The ADC and CAN network tests were carried out, these results were displayed on the computer by serial communication connecting an arduino to the board's Usart bus.

In the case of PWM, it is necessary to use an oscilloscope, so the figures for some of the tests carried out with the aid of a rheostat are left.

| Duty Cycle 40% Signal | Duty Cycle 40% Values |
| ------ | ------ |
|![Duty Cycle 40% signal](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2040%25%20Signal.jpg)|![Duty Cycle 40% values](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2040%25%20Values.jpg)|

| Duty Cycle 80% | Duty Cycle 80% Values |
| ------ | ------ |
|![Duty Cycle 80% signal](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal.jpg)|![Duty Cycle 80% values](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal.jpg)|

To test the circuit with high current, the resistance in the rheostat was reduced and the duty cycle was maintained:
| Duty Cycle 80% 3A  | Duty Cycle 80% Valores |
| ------ | ------ |
|![Duty 80% Signal (3A)](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Signal%20-%20Low%20resistance,%203A.jpg)|![Duty 80% Values (3A)](https://github.com/ZeniteSolar/MDE22/blob/cb7a627db1aa043f71257f563c45120e6a33601b/Imagens/Duty%20Cycle%2080%25%20Values%20-%20Low%20resistance,%203A.jpg)|

These results were satisfactory, considering that the circuit was not accumulating heat, the PWM was in accordance with what was desired from the configuration, and the values according to the datasheet.

#### Tests with Engine

The next step is the engine test. Bench test setup is done on the vessel, with bench supplies, boards and necessary connections.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Testes%20com%20Motor.jpg" width="420" height="250">

During the motor test, operating logics were implemented to the PWM of the H-bridge:

	H bridge activated by the difference between the potentiometers
	centered start
	Forced center if there is no communication
	Reverse side protection

The last logic was considered interesting considering that the system itself is assembled/disassembled for trips to competitions or testing sites.

The mechanical part can be seen in the figure below. The transmission shaft is coupled to the belt by the disc, and the DC steering motor controls the belt.
At the top of this can be seen the potentiometer used to measure rotation.

<p align="center">
  <img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Motor%20e%20correia.jpg" width="340" height="250">

The engine tests were a success using the bench sources, the operation video is in the Images folder. Due to the file size github cannot run it so it needs to be downloaded. An alternative is to use this [drive link](https://drive.google.com/file/d/12XCSZ1QREOxFkqlZAvMiaMpx_ZMUlxNi/view?usp=sharing).

During the test powered by the 12V battery, the system stopped working. The lack of capacitance for the input voltage caused a failure in one of the half-bridge ICs, after replacing the IC and correcting the values in the schematic the system is safe and operational.

# Considerations

All microcontroller peripherals are functional. All electrical measurements, which were one of the main objectives, showed satisfactory accuracy.

The essential logic of operation is complete, there are safety locks and finally the engine can be controlled smoothly.

Component performance: satisfactory.

Plate manufacturing: poor result, manufacturing caused more work and compromised the robustness of the system. To correctly meet the objective of a more robust system, the PCB ordered must be used.

Below we can see the system assembled with an experimental support — due to the dimensions of the board, a support dedicated to it needs to be manufactured.

On the side, the main power supply module ([MFP19](https://github.com/ZeniteSolar/MFP19)) is shown, which converts the voltage in the battery bank and supplies 18V from the CAN network. Future boat supports are expected to follow this 3D-printed model.

| Assembled System | Support Example |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/Sistema%20Montado.jpg" width="250" height="360">|<img src="https://github.com/ZeniteSolar/MDE22/blob/70da552cb5a2f2a8a49e6934afd4cba5a5405d8f/Imagens/Exemplo%20de%20suporte.jpg" width="250" height="360">|

For comparison, here are screenshots of the previous system.

| Previous System | H Bridge |
| --- | --- |
|<img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Montagem%20do%20sistema%20antigo.jpg" width="250" height="280">|<img src="https://github.com/ZeniteSolar/MDE22/blob/8283bc81c4a9d4ee74052ef4564bf314acc439f0/Imagens/Montagem%20antiga%20PonteH.jpg" width="280" height="210">|

#### Next steps

Some of the next steps for this module and future updates:

- PWM responsive to pilot movements
- H-Bridge design or choice of non-discontinued IC
- Redundancy messages and/or sensors
- Low power mode - Activated by data processing module
- *Remote steering


*The success of this step would make room for a fascinating update: The Autonomous Boat.

# References

Datasheet ATmega328P:
[Microchip, Atmel ATmega328P](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Steering sensor market research:
[Bosch](https://www.bosch-motorsport.com/content/downloads/Raceparts/en-GB/120530059.html),
[CUI Inc](https://www.cuidevices.com/catalog/motion/rotary-encoders),
[TT Electronics](https://www.ttelectronics.com/products/categories/steering-sensors/search-results/)







