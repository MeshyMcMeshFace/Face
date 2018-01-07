#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>
#include <stdlib.h>

#include "RadioDefs.h"

class RadioClass {
public:
/* getMTU()
 * returns Maximum size (bytes) of the packet we can send. 
 */
virtual size_t getMTU() { return 0; }

/* getAddressLength()
 * returns the maximum size in bytes of the length of the address.
 * returns 0 on no address.
 */
virtual size_t getAddressLength() { return 0; }

/* getAddress()
 * Copies the address of the radio into buffer
 * returns false if no address, or buffer = NULL.
 */
virtual bool   getAddress(uint8_t *buffer) { return false; }

/* canRecv()
 * Check if we can Recv data.
 * return false if no data is waiting.
 */
virtual bool canRecv() { return false; }

/* recv()
 * Get the data waiting in the queue.
 * Set pointers to null (apart from buffer) if you don't care aboute them
 * buffer MUST BE at least getMTU() bytes long.
 * return false for no packet.
 */
virtual bool recv(uint8_t *src, 
                    uint8_t *dest, 
                    uint8_t *buffer, 
                    size_t *len,
                    int *rssi,
                    float *snr) { return false; }

/* canSend()
 * check if there's enough queue space to send.
 * return false, if there's no space
 */
virtual bool canSend() { return false; }

/* Send()
 * put the data in the send queue.
 * returns false if we cannot send.
 */
virtual bool Send(uint8_t *dest, uint8_t *buffer, size_t len) { return false; }

/* setValue()
 * sets radio-specific setting to specific value
 * returns true on error/unsupported value 
 */
virtual bool setValue(uint32_t key, uint32_t value) { return true; }

/* getValue()
 * gets radio-specific setting to specific value
 * returns true on error/unsupported value
 */
virtual bool getValue(uint32_t key, uint32_t *value) { return true; }

/* Tick()
 * A function, called regularly to poll devices for those without interrupt driven RX.
 */
virtual void Tick() { }

};

#endif