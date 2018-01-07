#ifndef KISSRADIO_H
#define KISSRADIO_H

#include "Radio.h"

class KissRadioClass : public RadioClass {
private:
RadioClass *chain;
uint8_t    *buff;
size_t      buffLen;
size_t      mtu;

public:
KissRadioClass(RadioClass *chain) { 
    this->chain  = chain;
    this->buff = nullptr;
    this->mtu    = 0;
    if(this->chain && this->chain->getMTU()) {
        this->mtu = chain->getMTU() *2 +2;
        this->buff = new uint8_t[this->mtu];
        }
    if(! this->buff)
        this->mtu = 0; // we can assume a MTU of zero is an initialisation error.
    }
~KissRadioClass() {
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
virtual size_t getAddressLength() { return chain->getAddressLength(); }

/* getAddress()
 * Copies the address of the radio into buffer
 * returns false if no address, or buffer = NULL.
 */
virtual bool   getAddress(uint8_t *buffer) { return chain->getAddress(buffer); }

/* canRecv()
 * Check if we can Recv data.
 * return false if no data is waiting.
 */
virtual bool canRecv() { return chain->canRecv(); }

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
    bool retFlag = chain->recv(src,dest,this->buff,&this->buffLen,rssi,snr);
    if(retFlag)
    {
        // From Radio to Kiss Packet format.
        TransformToKiss(buffer,len); // transform this->buff to buffer.
    }
    return retFlag;
}

/* canSend()
 * check if there's enough queue space to send.
 * return false, if there's no space
 */
virtual bool canSend() { return chain->canSend(); }

/* Send()
 * put the data in the send queue.
 * returns false if we cannot send.
 */
virtual bool Send(uint8_t *dest, uint8_t *buffer, size_t len) { 
    if(!TransformFromKiss(buffer,len))
        return false;   
    return chain->Send(dest,this->buff,this->buffLen);
 }

/* setValue()
 * sets radio-specific setting to specific value
 * returns true on error/unsupported value 
 */
virtual bool setValue(uint32_t key, uint32_t value) { return chain->setValue(key,value); }

/* getValue()
 * gets radio-specific setting to specific value
 * returns true on error/unsupported value
 */
virtual bool getValue(uint32_t key, uint32_t *value) { return chain->getValue(key,value); }


/* Tick()
 * A function, called regularly to poll devices for those without interrupt driven RX.
 */
virtual void Tick() { chain->Tick(); }

private:
    void TransformToKiss(uint8_t *buffer, size_t *len);
    bool TransformFromKiss(uint8_t *buffer, size_t len);
};

#endif