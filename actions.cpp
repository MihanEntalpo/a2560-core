#ifndef ACTIONS_CPP
#define ACTIONS_CPP

#include "actions.h"

class Commandor;

void action_help(char* command, Commandor* cmd)
{
  Serial.println(F("*************"));
  Serial.println(F("* Help info *"));
  Serial.println(F("*************"));
  Serial.println(F("Commands:"));
  for (int i=0; i< cmd->command_count; i++)
  {
    Serial.print("Name: `");
    Serial.print(cmd->command_items[i]->name));
    Serial.print("`\n");
    Serial.println("Description:");
    
  }
}

void action_test_params(char* command, Commandor* cmd)
{
  Serial.println(F("***************************************"));
  Serial.println(F("* Test of command and params parsing: *"));
  Serial.println(F("***************************************"));

  Serial.println(F("Your command string:\n```\n"));
  Serial.println(cmd->cmdBuf.getBuffer());
  Serial.println(F("```"));
  
  Serial.print(F("\nCommand: "));
  Serial.println(command);

  Serial.print(F("\nParameters count: "));
  Serial.println(cmd->cmdParamsCount);

  Serial.println(F("Parameters:"));
  
  for (int p = 0; p < cmd->cmdParamsCount; p ++)
  {
    Serial.print(p+1);
    Serial.print(F(") `"));
    Serial.print(cmd->cmdParams[p].getBuffer());
    Serial.println(F("`"));    
  }

  Serial.println();
}

#endif
