#ifndef STATIC_STRING_H
#define STATIC_STRING_H

class AbstractStaticString {
  public:
    virtual bool append(char b);
    virtual bool append(char *s, int size);
    virtual char operator [](int i) const;
    virtual char at(unsigned int pos);
    virtual void clear();
    virtual unsigned int getSize();
    virtual char* getBuffer();
};


template <int B>
class StaticString : public AbstractStaticString {

  protected:
    char buffer[B];
    unsigned int size;
    
  public:
    unsigned const int max_size = B - 1;
    
    char* getBuffer();
    unsigned int getSize();
    StaticString();
    bool append(char b);
    bool append(char *s, int size);
    void clear();
    char at(unsigned int pos);
    char operator [](int i) const {return i < B ? this->buffer[i] : '\0';}


  
};


#endif
