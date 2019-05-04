#ifndef STATIC_STRING_CPP
#define STATIC_STRING_CPP

#include "StaticString.h"

template <int B>
char * StaticString<B>::getBuffer()
{
  return this->buffer;
}

template <int B>
unsigned int StaticString<B>::getSize()
{
  return this->size;
}

template <int B>
StaticString<B>::StaticString()
{
  this->size = 0;
  for (int i=0; i < this->max_size; i++)
  {
    this->buffer[i] = '\0';
  }
}

template <int B>
bool StaticString<B>::append(char b)
{
  if (this->size >= this->max_size) return false;
  this->buffer[this->size] = b;
  this->size += 1;
  this->buffer[this->size] = '\0';
  return true;
}

template <int B>
bool StaticString<B>::append(char *s, int size)
{
  if (this->size + size >= this->max_size) return false;
  for (int i=0; i< size; i++)
  {
    this->buffer[this->size] = s[i];
    this->size += 1;
  }
  return true;
}

template <int B>
void StaticString<B>::clear()
{
  this->size = 0;
}

template <int B>
char StaticString<B>::at(unsigned int pos)
{
  return this->buffer[pos];
}

#endif
