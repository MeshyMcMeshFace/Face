#ifndef LOOPBACKRADIO_H
#define LOOPBACKRADIO_H

#include "Radio.h"
#include "string.h"

class LoopbackRadioClass : public RadioClass {
private:
uint8_t    *buff;
size_t      buffLen;
size_t      mtu;
bool        isReady;
public:
LoopbackRadioClass(size_t mtu = 256) { 
    this->buff = nullptr;
    this->mtu = mtu;
    this->buff = new uint8_t[this->mtu];
    this->isReady = false;
    }
~LoopbackRadioClass() {
    // free temp buffer.
    if(this->buff)
        delete this->buff;
}
/* getMTU()
 * returns Maximum size (bytes) of the packet we can send. 
 * if zero, then something has gone badly wrong.
 */
virtual size_t getMTU() { return this->mtu; }

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
virtual bool canRecv() { return isReady; }

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
                    float *snr) { 
    // nothing sent, nothing to recv
    if(!canRecv())
        return false;
    
    // ignore src/dest, just copy buffer+len
    memcpy(buffer,this->buff,this->mtu);
    *len = this->buffLen;
    // perfect reception
    *rssi = 0;
    *snr = 100;
    return true;
}

/* canSend()
 * check if there's enough queue space to send.
 * return false, if there's no space
 */
// can't send if you've not recv'd
virtual bool canSend() { return ! this->isReady; }

/* Send()
 * put the data in the send queue.
 * returns false if we cannot send.
 */
virtual bool Send(uint8_t *dest, uint8_t *buffer, size_t len) { 
    if(!canSend())
        return false;
    // avoid buffer overflow.
    if(len >= this->mtu)
        return false;
    memset(this->buff,0,this->mtu);
    memcpy(this->buff,buffer,len);
    this->buffLen = len;
    return true;
 }

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
virtual void Tick() { /* no-op */ }

};

#endif