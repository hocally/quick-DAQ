# Quick-DAQ

This was a far-too quick build of a simple DAQ system to support suspension analysis on a Baja SAE car (Specifically the John's Hopkins University team, go Blue Jays!). It consists of the following components:
- An Arduino Uno
- An Adafruit datalogging shield
- A protoshield housing a simple RC frontend with an FC ~20 kHz
- A hammond (?) enclosure with a waterproof gasket  

The electrical build took about 2 and a half hours and is certainly not my finest work. I think it will do the trick given the project requirements. The following pinout and most of the TODO's refer to the first article (SN: 1). 

### Pinout
- USB Cable  
    - Hopefully pretty obvious, used to re-flash device firmware. Current logging session (if in progress) is stopped briefely while device is forced under reset to connect to UART
- Hole #1 (from left): Pot 1  
    - Blue: Ground
    - Purple: 5V reference
    - Green: Signal
- Hole #2 (from left): Pot 2  
    - Black: Ground
    - Grey: 5V reference
    - White: Signal
- Hole #3 (from left): Pot 3  
    - Brown: Ground
    - Orange: 5V reference
    - Red: Signal
- Hole #4 (from left): Pot 4  
    - Green: Ground
    - Orange: 5V reference
    - Yellow: Signal

### Open TODOs:
- Once basic functionality and correct pinout is confirmed, all flying leads should be adhered to the analog filter protoboard. Hot glue would probably work fine for this, the important thing is to strain relieve the wire connections, so the more glue the merrier
    - The importance of this cannot be emphasized enough: when in it's current state the box should be treated with gentle care as to not fracture any of the direct solder connections. Only once it's fully hot glued or otherwise potted should it go anywhere near the vehicle
- Final connectors should be crimped onto the wires coming out of the box. The current connectors are probably unsuitable for chassis useage.
- The harness connecting the shock pots to the needs to be constructed. The pinouts for such harnesses should reference the above pinout
- There is currently no provision for non-USB power on the board. Henry O'C has a few ideas, but the easiest would likely be to just run a 9V battery to the VIN and GND pins of the board through a gap in the USB connector hole
- The USB cable can currently be "removed" but it may end up making the box more robust to just keep it permanently mounted to the box and simply ziptie/tape off the connecotor to reduce the likelyhood if water/dirt ingress
- The shock pots should be calibrated using the following procedure. The following procedure should be repeated on each corner in sucession
    1. Remove the shock to allow the linkage to move freely
    2. Connect the board to a laptop and open the serial monitor
    3. Find the data corresponding to the shock you are calibrating (ordering of the shocks is documented at the top of the stream and in the data file, however it goes FL displacement, FL voltage, FR displacement, FR voltage, RL displacement, RL voltage, RR displacement, RR voltage)
    4. Bring the suspension to full bump
    5. Observe the ***VOLTAGE*** of the shock under calibration and adjust the SPXX_TOT_VOLTAGE to this number
    6. Set SPXX_TOT_DISPLACEMENT to 0.0 inches
    7. Against a tape measure or height gauge, being the suspension to full droop. Record the delta between full bump and full droop as SPXX_BOT_DISPLACEMENT
    8. Observe the ***VOLTAGE*** of the shock under calibration and adjust the SPXX_BOT_VOLTAGE to this number
    9. Once these values are saved, reprogram the board with the new calibration and ensure coherent results (0 displacement at TOT, linearliy increasing to SPXX_BOT_DISPLACEMENT at BOT)
    10. Repeat steps 1-9 on all other corners

## Operating Instructions
1. Ensure all connectors are mated (potentiometers)
2. Ensure battery is charged
3. Turn on/connect battery and observe solid green and blinking red light
4. Drive! Logging is taking place
5. Once done with test, simply disconnect power, wait a minimum of 5 seconds, then remove SD card.
6. Connect to SD card and retrieve your design-tent winning data!
    - The file naming convention is sequential, so the latest run is the highest number

*From Minneapolis with love <3*