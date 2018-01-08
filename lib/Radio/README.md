# Radio.h

A simple abstraction of a communication medium, aimed at being able to give a consistent interface for software that needs to send packet data on, but not restricted to radio. Some 'radios' may be filters, encoding data into different formats

## Implementations

### Complete 
N.B. might not be finished.
* KISS transcoder - filter so you can use AX.25 under linux with your radio implementation for testing
* Null Radio - just use the Radio class - everything reports that it can't do anything - override methods for your own implementation
* Loopback Radio - everything you 'transmit' is returned back to you. 

### 'TODO'
* LoRa (via Sandeep Mistry's LoRa class)
* ESP-Now
* Bluetooth/Bluetooth Smart (LE)
* UDP