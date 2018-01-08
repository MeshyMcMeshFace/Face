# Loopback Radio

A pretend radio that echos what has been sent to it.

## Caveats
* It will only ever store one packet to send.
* Send one packet, and it will fail sending until you Recv it back.
* It will gleefully ignore any attempts to configure it. 
* Small MTUs might cause problems. MTU of zero is considered an error, and operation is undefined.
