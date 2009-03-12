Readme:
To run the smartPioneerBaseServer component you may need to change the baudrate of the communication line to the pioneer base.

///////////////////////////////////////////////////////////////////////////////////////
Pioneer P3DX (H8S Microcontroller) with AROS 1.13 

we changed the baudrate of our robots to 115200 and the SIP (Server Information Packet) cycle to 25. (details: Pioneer Operations Manual - AROScf) 

Attentions for Player/Stage users: 
Changing the baudrate can cause problems with the connection between Player and your robot (Connection Timeout). It seems that Player connects to the robot by trying different baudrates stored in the bauds-array. As the baudrate 115200 is the fourth rate to try, a timeout occures before the connection is etablished. 

To solve the Problem you have to modify your p2os driver (<Player-Dir>/server/drivers/mixed/p2os/p2os.cc). Simply change the order of the baudrates in the bauds-array (line 544). 
The bauds-array looks like “int bauds[] = {B9600, B38400, B19200, B115200, B57600};” and should look like “int bauds[] = {B115200, B38400, B9600};”

