#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>
#include "StaticString.h"
#include "commandor.h"

void action_help(char* command, Commandor* cmd);
void action_test_params(char* command, Commandor* cmd);

#endif
