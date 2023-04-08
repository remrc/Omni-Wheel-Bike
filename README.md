# Omni-Wheel-Bike

Arduino nano, Nidec 24H motors, MPU6050, 3S 1000 mAh LiPo battery.

Balancing controller can be tuned remotely over bluetooth.

Example (change K1):

Send p+ (or p+p+p+p+p+p+p+) for increase K1.

Send p- (or p-p-p-p-p-p-p-) for decrease K1.

Remote control over Joy BT Commander app.

<img src="/pictures/bike.jpg" alt="Omni wheel bike"/>
<img src="/pictures/schematic.png" alt="Schematic"/>

About schematic:

Battery: 3S1P LiPo (11.1V). 

Buzzer: any 5V active buzzer.

Transistor: 2N2222 or similar.

First connect to controller over bluetooth. You will see a message that you need to calibrate the balancing point. Send c+ from serial monitor. This activate calibrating procedure. Set the bike to balancing point. Hold still when the bike does not fall to either side. Send c- from serial monitor. This will write the offsets to the EEPROM. After calibrating, the bike will begin to balance.
 
More about this:

https://youtu.be/-fPYXWaVcOw

