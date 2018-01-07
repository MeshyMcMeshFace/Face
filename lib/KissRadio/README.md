# Kiss Radio

A pretend radio that expects KISS format packets from the host, and send the packets on to the real radio.
It will also read packets from the real radio and convert them into KISS format for the host.

Caveats:
* It only responds to node '0'
* It will attempt to pass on KISS configuration upstream. 
* The upstream implementation is under not obligation to honor attempts at KISS configuration
* H/W Specific KISS configuration is not implemented at the time of writing.
* It would be nice to be able to set frequency etc. with the H/W configuration settings though!