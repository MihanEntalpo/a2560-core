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

class Commandor;

typedef void (*command_action)(char*, AbstractStaticString*, AbstractStaticString *, unsigned int);
typedef void (*commandor_action)(char*, Commandor*);
typedef void (*process_unused_byte)(char *);

struct CommandItem {
  public:
    String name;
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
    StaticString<COMMAND_PARAM_BUFFER_SIZE> cmdParams[COMMAND_MAX_PARAMS];

    unsigned int cmdParamsCount;

    /**
     * Конструктор
     */
    Commandor();

    boolean processSerialByte(process_unused_byte callback);

    void readParams(unsigned int action_num);
    
    // ***********************************
    // Дальше - статические поля и методы:
    // ***********************************
    static int commands;
    static String command_names[MAX_COMMANDS];
    static command_action command_actions[MAX_COMMANDS];

    //static char port_buffer[2048];
    //static int port_buffer_size;
    static StaticString<1024> pbuf;
    static StaticString<128> params[10];
    static unsigned int paramsCount;

    static void init();

    static boolean hasNewByte(int *newByte);

    static boolean firstByteWasSpecial;
    static int byteNumInLine;

    static void parseParams(int commandNum);
};



#endif
