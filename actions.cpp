#ifndef ACTIONS_CPP
#define ACTIONS_CPP

#include "actions.h"

void action_help(char *command, AbstractStaticString *full_command, AbstractStaticString *params, unsigned int paramsCount)
{
  Serial.println("*************");
  Serial.println("* Help info *");
  Serial.println("*************");
  Serial.print  (" Params passed to Help: ");
  
  for (int p = 0; p < paramsCount; p ++)
  {
    Serial.print(" [");
    Serial.print(p);
    Serial.print(",");
    Serial.print((int)(params + p), HEX);
    Serial.print(",");
    
    Serial.print("] `");
    Serial.print(p == 0 ? params[p].getBuffer() : "unknown");
    Serial.print("`");
    if (p + 1 < paramsCount)
    {
      Serial.print(", ");
    }
  }

  Serial.println();
}
  
void action_help_d(char* command, Commandor* cmd)
{
  Serial.println("*********************");
  Serial.println("* Help info DYNAMIC *");
  Serial.println("*********************");

  for (int p = 0; p < cmd->cmdParamsCount; p ++)
  {
    Serial.print(" [");
    Serial.print(p);
    Serial.print(",");
    Serial.print((int)&(cmd->cmdParams[p]), HEX);
    Serial.print(",");
    Serial.print((int)&(cmd->cmdParams + p), HEX);
    Serial.print("] `");
    Serial.print(cmd->cmdParams[p].getBuffer());
    Serial.print("`");
    if (p + 1 < paramsCount)
    {
      Serial.print(", ");
    }
  }

  Serial.println();
}

#endif
