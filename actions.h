#ifndef COMMANDOR_H
#define COMMANDOR_H

#include <Arduino.h>
#include <string.h>
#include "StaticString.h"
#include "commandor.h"

class Commandor;

void action_help(char *command, AbstractStaticString *full_command, AbstractStaticString *params, unsigned int paramsCount);
void action_help_d(char* command, Commandor* cmd);


#endif
