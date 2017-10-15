# WiLED Protocol

The WiLED Protocol, or "WiLP", is designed to be lightweight and robust. It can be thought of as an application layer protocol - it does not care how a message is physically transmitted and does not implement any methods for doing so, it is up to the developer to connect devices using WiLP via a suitable medium. 

## Features

- Has a maximum length of 20 bytes, with most message types only using 16 bytes. 
- Uses message counters, to prevent duplicated messages or replay attacks (if combined with encryption). 

## Protocol Definition

A WiLP message shall consist of three main sections: a **header**, a **payload**, and a **checksum**. The header shall be 10 bytes, the payload shall be at least 4 bytes and no more than 8 bytes, and the checksum shall be 2 bytes. Hence, in total a WiLP message shall be between 16 and 20 bytes in length. 

### Header

The header shall consist of:

- 1 byte:  Magic Number padding (binary 0b10101010, hex 0xAA)
- 2 bytes: Source Address
- 2 bytes: Destination Address
- 2 bytes: Reset Counter
- 2 bytes: Message Counter
- 1 byte:  Message Type

#### Magic Number

Each message shall begin with the binary number `0b10101010` to distinguish this as a WiLP v1 message. 

#### Source Address

Each device in a WiLED system must have an address, from 1 to 65,535. This is the source address, i.e. the address of the device that this message originated from. 

#### Destination Address

Each device in a WiLED system must have an address, from 1 to 65,535. This is the destination address, i.e. the address of the device that this message is intended for. 

#### Reset Counter

To detect duplicate messages, each transmission includes an incrementing counter. This is broken into two parts, the Reset Counter and the Message Counter. The two numbers are split so that the Reset Counter can be stored in long-term memory but the Message Counter only needs to be stored in short-term RAM. 

The Reset Counter shall be incremented by a device each time it powers on, or each time the Message Counter overflows past 65,535. The Reset Counter for each known device should be stored by any receiving device, and any received message should be validated against the known Reset Counter using these rules: 

- If the received message has a Reset Counter equal to the known Reset Counter for the source address, then it is valid. 
- If a received message has a Reset Counter greater than the known Reset Counter for the source address, then it is valid. Furthermore the known address shall be updated and saved to long-term memory, and the known Message Counter for the source address shall be set to zero. 
- If a received message has a Reset Counter less than the known Reset Counter for the source address, then it is invalid and must be ignored with no further processing or acknowledgement. 

#### Message Counter

The Message Counter shall be incremented by a sending device after each message is transmitted. The Message Counter for each known device should be stored by any receiving device, and any received message should be validated against the known Reset Counter using these rules:

- If the received message has passed Reset Counter validation and the Message Counter is greater than the known Message Counter for the source address, then it is valid. Furthermore, the known Message Counter shall be set to equal the received Message Counter for that source address. 
- If the received message has a Message Counter that is equal to or less than the known Message Counter for the source address, then it is invalid and must be ignored with no further processing or acknowledgement. 

#### Message Type

Several Message Type specifications are defined, using an 8 bit number. The Message Type flag determines the length of the Payload. 

### Payload

The header shall consist of:

- 4 to 8 bytes payload (depending on Message Type)

### Checksum

The checksum shall consist of:

- 2 bytes: CRC16 checksum value

### Diagram

A complete message (in this case of minimum length) can be represented in diagram from like this: 

`
| 1.... | 2.... | 3.... | 4.... | 5.... | 6.... | 7.... | 8.... | 9.... | 10... |
| 0xAA  | Source Addr.  | Dest. Addr.   | Reset Counter | Msg. Counter  | Type  |
| Payload                       | Checksum      |
`

# WiLEDProto class

The `WiLEDProto` class provides an implementation of the WiLED Protocol, to be used for communicating between WiLED devices. 
