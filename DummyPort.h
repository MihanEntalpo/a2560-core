#ifndef DUMMY_PORT_H
#define DUMMY_PORT_H

class DummyPort
{
   public:
    void print() {}
    void print(const char* s) {}
    void print(int16_t s) {}
    void print(int16_t s, int16_t f) {}
    void flush() {}
    void println() {}
    void println(const char* s) {}
    void println(int s) {}
  
    
};

#endif
