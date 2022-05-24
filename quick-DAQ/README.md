# Quick-DAQ

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
- Mechanical  
    - Once basic functionality and correct pinout is confirmed, all flying leads should be adhered to the analog filter protoboard. Hot glue would probably work fine for this, the important thing is to strain relieve the wire connections, so the more glue the merrier
    - Final connectors should be crimped onto the wires coming out of the s
    - The harness connecting the shock pots to the needs to be constructed. The pinouts for such harnesses should reference the above pinout