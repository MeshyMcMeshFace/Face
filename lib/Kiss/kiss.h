
#ifdef KISS_H
#define KISS_H

#include <Arduino.h>

typedef struct kiss_buff_s {
    char *data;
    int len;
    bool ready;
} kiss_buff_t;

class KissClass : public Stream {
public:
    KissClass() { /* do nothing */ };
    int begin(int mtu=256,
                dataTag = 0,
                frameEnd  = 0xC0,
                frameEscape=0xDB,
                frameEndTransposed=0xDC,
                frameEscapeTransposed=0xDD);
    void end();

    // From Stream
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual bool flush();
    // From Print
    virtual size_t write(uint8_t ch);

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
    uint8_t dataTag;

    char status[32];
};

KissClass Kiss;
#endif