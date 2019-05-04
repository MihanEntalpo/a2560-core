#ifndef COMMANDOR_CPP
#define COMMANDOR_CPP

#include "commandor.h"
#include "StaticString.h"
#include "StaticString.cpp"
#include "DummyPort.h"

// Если нужны отладочные сообщения нужно написать DEBUG_COMMANDOR 1
#define DEBUG_COMMANDOR 0

#define OUT_PORT Serial

#if DEBUG_COMMANDOR

  #define DBG_PORT Serial

#else

  DummyPort DBG_PORT;
  
#endif

Commandor::Commandor() {
  Serial.print(F("Commandor constructor called\n"));
  this->cmdBuf.clear();
  this->isCommandMode = false;
  this->lineByteNumber = false;
  this->cmdParamsCount = 0;
  for (int i = 0; i < COMMAND_MAX_PARAMS; i++)
  {
    this->cmdParams[i].clear();
  }
  
}

bool Commandor::processSerialByte(process_unused_byte callback)
{
  char newByte;
  if (COMMANDOR_PORT.available() > 0)
  {
    newByte = COMMANDOR_PORT.read();
    if (newByte != -1)
    {
      char c = newByte;
      DBG_PORT.print(F("      BYTE: `"));
      DBG_PORT.print(c);
      DBG_PORT.print(F("`"));
      DBG_PORT.print(F(", lineByteNumber:"));
      DBG_PORT.print(lineByteNumber);
      DBG_PORT.print(F(", buffer_size before="));
      DBG_PORT.print(cmdBuf.getSize());
      DBG_PORT.print(F(" (max_size="));
      DBG_PORT.print(cmdBuf.max_size);
      DBG_PORT.println(F(")"));
      if (lineByteNumber == 0)
      {
        DBG_PORT.println(F("        It's first byte"));
        if (c == '<')
        {
          DBG_PORT.println(F("          First byte is <"));
          DBG_PORT.println(F("          Clearing buffer and switching to command mode"));
          cmdBuf.clear();
          isCommandMode = true;
          lineByteNumber += 1;
          return false;
        }
        lineByteNumber += 1;
      }
      else 
      {
        DBG_PORT.println(F("        It's more than first byte"));
        if (isCommandMode)
        {
          DBG_PORT.println(F("          We are in command mode"));
          if (c == '>')
          {
            DBG_PORT.println(F("            Found closing bracket"));
            int found_command = -1;
            for (int i=0; i< command_count; i++)
            {
              if (command_items[i].name.length() <= cmdBuf.getSize())
              {
                boolean is_equal = true;
                for (int j=0; j < command_items[i].name.length(); j++)
                {
                  if (command_items[i].name.charAt(j) != cmdBuf[j])
                  {
                    is_equal = false;
                    break;
                  }
                }
                if (is_equal) 
                {
                  if (command_items[i].name.length() < cmdBuf.getSize())
                  {
                    if (cmdBuf[command_items[i].name.length()] == ' ')
                    {
                      found_command = i;
                      break;
                    }
                  }
                  else
                  {
                    found_command = i;
                    break;
                  }
                }
              }
            }
            if (found_command == -1)
            {
              OUT_PORT.print(F("<error command_not_found ")); 
              OUT_PORT.print(cmdBuf.getBuffer());
              OUT_PORT.println(F(">"));
            }
            else
            {
              if (!this->readParams(found_command))
              {
                OUT_PORT.println(F("<error too_many_params>"));
              }

              DBG_PORT.println(F("              Params readed successfull"));

              DBG_PORT.print(F("              Command found: "));
              DBG_PORT.println(command_items[found_command].name);
              DBG_PORT.print(F("              Params parsed: "));
              DBG_PORT.print(cmdParamsCount);
              DBG_PORT.print(F(", params: "));
              for (int p = 0; p < cmdParamsCount; p ++)
              {
                DBG_PORT.print(F("["));
                DBG_PORT.print(p);
                DBG_PORT.print(F(","));
                DBG_PORT.print((int)&(cmdParams[p]), HEX);
                DBG_PORT.print(F(","));
                DBG_PORT.print((int)cmdParams[p].getBuffer(), HEX);
                DBG_PORT.print(F("] `"));
                DBG_PORT.print(cmdParams[p].getBuffer());
                DBG_PORT.print(F("`"));
                if (p + 1 < cmdParamsCount)
                {
                  DBG_PORT.print(F(", "));
                }
              }
              DBG_PORT.println(F("\n              Calling..."));
              
              command_items[found_command].func(command_items[found_command].name.c_str(), this);
              cmdBuf.clear();
              cmdParamsCount = 0;
            }
            lineByteNumber = 0;
            isCommandMode = false;
          }
          else
          {
            DBG_PORT.print(F("            Adding byte to buffer: "));
            DBG_PORT.print(c);
            DBG_PORT.print(F(" ("));
            DBG_PORT.print(c, DEC);
            DBG_PORT.print(F(")\n"));
            
            if (!cmdBuf.append(c))
            {
              OUT_PORT.print(F("<error command_buffer_exceeded max_size "));
              OUT_PORT.print(cmdBuf.max_size);
              OUT_PORT.print(F(">"));
              
              isCommandMode = false;
              cmdBuf.clear();
              lineByteNumber = 0;
              return true;
            }

            DBG_PORT.print(F("            pbuf = `"));
            DBG_PORT.print(cmdBuf.getBuffer());
            DBG_PORT.print(F("`, pbuf bytes = ["));
            for (int ii=0; ii<cmdBuf.getSize(); ii++)
            {
              DBG_PORT.print((int)cmdBuf[ii]);
              DBG_PORT.print(F(", "));
            }
            DBG_PORT.print(F("]\n"));
            
          }
        }
        else if (c == '\n')
        {
          DBG_PORT.println(F("            Found newline character, resetting bytNumInLine to zero")); 
          lineByteNumber = 0;
          return true;
        }
        else
        {
          return true;
        }
      }

      lineByteNumber += 1;

      if (!isCommandMode)
      {
        if (c == '\n')
        {
          DBG_PORT.println(F("          Resetting byteNumInLine"));
          lineByteNumber = 0;
        }
      }
      
      return true;
    }
  }
  return false;  
}

bool Commandor::readParams(unsigned int action_num)
{
  DBG_PORT.println(F("  Commandor->readParams"));
  
  int command_length = command_items[action_num].name.length();
  cmdParamsCount = 0;
  if (cmdBuf.getSize() == command_length || cmdBuf.getSize() == command_length + 1)
  {
    return true;
  }
  else
  {
    int c = 0;
    bool escaped = false;
    cmdParams[c].clear();
    for (int i = command_length + 1; i < cmdBuf.getSize(); i++)
    {
      char b = cmdBuf[i];
      
      DBG_PORT.print(F("      Byte: `"));
      DBG_PORT.print(b);
      DBG_PORT.println(F("`"));
      
      if (b == '\\')
      {
        DBG_PORT.println(F("        Byte is a slash"));
        if (!escaped) 
        { 
          DBG_PORT.println(F("        We're not in 'escaped' mode, switching to it"));
          escaped = true;
          continue;
        }
      }
      
      if (b == ' ' && !escaped)
      {
        DBG_PORT.println(F("        byte is SPACE and it's not escaped"));
        if (cmdParams[c].getSize() > 0)
        {
          if (c + 1 >= COMMAND_MAX_PARAMS)
          {
            for (int i = c + 1; i >= 0; i --)
            {
              if (cmdParams[i].getSize()  > 0)
              {
                cmdParamsCount = i + 1;
                return false;
              }
            }
            return false;
          }
          c += 1;
          cmdParams[c].clear();
          DBG_PORT.print(F("          current param ["));
          DBG_PORT.print(c);
          DBG_PORT.print(F("] isn't empty (`"));
          DBG_PORT.print(cmdParams[c-1].getBuffer());
          DBG_PORT.print(F("`), increasing c to "));          
          DBG_PORT.print(c);
          DBG_PORT.println(F(", and continuing to next iteration"));
          continue;    
        }
        else
        {
          DBG_PORT.println(F("          current param is empty, skipping this space"));
          continue;
        }
      }

      cmdParams[c].append(b);

      DBG_PORT.print(F("      Appending byte to current param ["));
      DBG_PORT.print(c);
      DBG_PORT.print(F("], result ["));
      DBG_PORT.print((int)cmdParams[c].getBuffer(), HEX);
      DBG_PORT.print(F("]: \n      "));
      DBG_PORT.println(cmdParams[c].getBuffer());
      
      escaped = false;
    }

    for (int i = c + 1; i >= 0; i --)
    {
      if (cmdParams[i].getSize()  > 0)
      {
        cmdParamsCount = i + 1;
        break;
      }
    }
  }
  return true;
}


#endif
