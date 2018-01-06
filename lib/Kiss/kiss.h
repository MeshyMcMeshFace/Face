
#ifndef KISS_H
#define KISS_H

#include <Arduino.h>

#define KISS_STATUS_LEN (32)

typedef struct kiss_buff_s {
    uint8_t *data;
    size_t len;
    size_t p;
    bool ready;
} kiss_buff_t;

class KissClass : public Stream {
public:
    KissClass() { /* do nothing */ };
    int begin(size_t mtu=256,
                uint8_t frameEnd  = 0xC0,
                uint8_t frameEscape=0xDB,
                uint8_t frameEndTransposed=0xDC,
                uint8_t frameEscapeTransposed=0xDD);
    void end();

    // From Stream
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush() {/* No-OP */}
    // From Print
    virtual size_t write(uint8_t ch);
    virtual size_t write(const uint8_t *buffer, size_t size);

    size_t getMtu();
    bool isOkay();

    char *getStatus();

private:
    kiss_buff_t *allocBuffer(int size);
    void freeBuffer(kiss_buff_t *buff);
    int resetBuffer(kiss_buff_t *buff);

    int transformToKiss();

    void processPacket();

    Stream* radio;

    kiss_buff_t *inp,*out,*tmp;

    size_t mtu;
    uint8_t frameEnd;
    uint8_t frameEscape;
    uint8_t frameEndTransposed;
    uint8_t frameEscapeTransposed;
    
    bool status_changed;
    char status[KISS_STATUS_LEN];
};

extern KissClass Kiss;
#endif