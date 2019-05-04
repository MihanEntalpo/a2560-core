#ifndef COMMANDOR_H
#define COMMANDOR_H

#define MAX_COMMANDS 64
#define COMMAND_BUFFER_SIZE 300
#define COMMAND_PARAM_BUFFER_SIZE 64
#define COMMAND_MAX_PARAMS 4

#define COMMANDOR_PORT Serial

#include <Arduino.h>
#include <string.h>
#include "DummyPort.h"
#include "StaticString.h"
#include "StaticString.cpp"

class Commandor;

typedef void (*command_action)(char*, AbstractStaticString*, AbstractStaticString *, unsigned int);
typedef void (*commandor_action)(char*, Commandor*);
typedef void (*process_unused_byte)(char *);

struct CommandItem {
  public:
    String name;
    String description;
    commandor_action func;
};

class Commandor {

  public:

    /**
     * Количество команд к коммандоре
     */
    int command_count;
    /**
     * Набор заданных команд
     */
    CommandItem command_items[MAX_COMMANDS];
    /**
     * Мы в коммандном режиме?
     */
    bool isCommandMode;
    /**
     * Номер байта в строке
     */
    unsigned int lineByteNumber;
    /**
     * Буффер команды
     */
    StaticString<COMMAND_BUFFER_SIZE> cmdBuf;
    /**
     * Буфферы параметров команды
     */
    StaticString<COMMAND_PARAM_BUFFER_SIZE> cmdParams[COMMAND_MAX_PARAMS+1];

    unsigned int cmdParamsCount;

    /**
     * Конструктор
     */
    Commandor();

    boolean processSerialByte(process_unused_byte callback);

    bool readParams(unsigned int action_num);
};



#endif
