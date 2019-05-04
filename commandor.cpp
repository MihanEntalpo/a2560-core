#ifndef COMMANDOR_CPP
#define COMMANDOR_CPP

#include "commandor.h"
#include "StaticString.h"
#include "StaticString.cpp"
#include "DummyPort.h"

#define DEBUG_COMMANDOR 1

#if DEBUG_COMMANDOR

#define DBG_PORT Serial

#else
  
#define DBG_PORT DummyPort
  
DummyPort dummy_port;

#endif

boolean Commandor::firstByteWasSpecial;
int Commandor::byteNumInLine;
StaticString<1024> Commandor::pbuf;
StaticString<128> Commandor::params[10];
unsigned int Commandor::paramsCount;

void Commandor::init() {
  Serial.print("Commandor::init()\n");
  Commandor::pbuf.clear();
  Commandor::firstByteWasSpecial = false;
  Commandor::byteNumInLine = 0;
  Commandor::paramsCount = 0;
}

Commandor::Commandor() {
  Serial.print("Commandor constructor called\n");
  this->cmdBuf.clear();
  this->isCommandMode = false;
  this->lineByteNumber = false;
  this->cmdParamsCount = 0;
  for (int i = 0; i < COMMAND_MAX_PARAMS; i++)
  {
    this->cmdParams[0].clear();
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
      DBG_PORT.print("      BYTE: `");
      DBG_PORT.print(c);
      DBG_PORT.print("`");
      DBG_PORT.print(", lineByteNumber:");
      DBG_PORT.print(lineByteNumber);
      DBG_PORT.print(", buffer_size before=");
      DBG_PORT.print(cmdBuf.getSize());
      DBG_PORT.print(" (max_size=");
      DBG_PORT.print(cmdBuf.max_size);
      DBG_PORT.println(")");
      if (lineByteNumber == 0)
      {
        DBG_PORT.println("        It's first byte");
        if (c == '<')
        {
          DBG_PORT.println("          First byte is <");
          DBG_PORT.println("          Clearing buffer and switching to command mode");
          cmdBuf.clear();
          isCommandMode = true;
          lineByteNumber += 1;
          return false;
        }
        lineByteNumber += 1;
      }
      else 
      {
        DBG_PORT.println("        It's more than first byte");
        if (isCommandMode)
        {
          DBG_PORT.println("          We are in command mode");
          if (c == '>')
          {
            DBG_PORT.println("            Found closing bracket");
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
              DBG_PORT.print("              ERROR! COMMAND '"); 
              DBG_PORT.print(cmdBuf.getBuffer());
              DBG_PORT.println("' not found!");
            }
            else
            {
              this->readParams(found_command);

              DBG_PORT.println("              Params readed successfull");

              DBG_PORT.print("              Command found: ");
              DBG_PORT.println(command_items[found_command].name);
              DBG_PORT.print("              Params parsed: ");
              DBG_PORT.print(cmdParamsCount);
              DBG_PORT.print(", params: ");
              for (int p = 0; p < cmdParamsCount; p ++)
              {
                DBG_PORT.print("[");
                DBG_PORT.print(p);
                DBG_PORT.print(",");
                DBG_PORT.print((int)&(cmdParams[p]), HEX);
                DBG_PORT.print(",");
                DBG_PORT.print((int)(&(cmdParams) + p), HEX);
                DBG_PORT.print("] `");
                DBG_PORT.print(cmdParams[p].getBuffer());
                DBG_PORT.print("`");
                if (p + 1 < cmdParamsCount)
                {
                  DBG_PORT.print(", ");
                }
              }
              DBG_PORT.println("\n              Calling...");
              
              command_items[found_command].func(command_items[found_command].name.c_str(), this);
              cmdBuf.clear();
              cmdParamsCount = 0;
            }
            lineByteNumber = 0;
            isCommandMode = false;
          }
          else
          {
            DBG_PORT.print("            Adding byte to buffer: ");
            DBG_PORT.print(c);
            DBG_PORT.print(" (");
            DBG_PORT.print(c, DEC);
            DBG_PORT.print(")\n");
            
            if (!cmdBuf.append(c))
            {
              DBG_PORT.println("PBUF SIZE EXCEEDED, FORCED TO RESET COMMAND MODE");
              isCommandMode = false;
              cmdBuf.clear();
              lineByteNumber = 0;
              return true;
            }

            DBG_PORT.print("            pbuf = `");
            DBG_PORT.print(cmdBuf.getBuffer());
            DBG_PORT.print("`, pbuf bytes = [");
            for (int ii=0; ii<cmdBuf.getSize(); ii++)
            {
              DBG_PORT.print((int)cmdBuf[ii]);
              DBG_PORT.print(", ");
            }
            DBG_PORT.print("]\n");
            
          }
        }
        else if (c == '\n')
        {
          DBG_PORT.println("            Found newline character, resetting bytNumInLine to zero"); 
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
          DBG_PORT.println("          Resetting byteNumInLine");
          lineByteNumber = 0;
        }
      }
      
      return true;
    }
  }
  return false;  
}

void Commandor::readParams(unsigned int action_num)
{
  DBG_PORT.println("  Commandor->readParams");
  
  int command_length = command_items[action_num].name.length();
  cmdParamsCount = 0;
  if (cmdBuf.getSize() == command_length || cmdBuf.getSize() == command_length + 1)
  {
    return;
  }
  else
  {
    int c = 0;
    bool escaped = false;
    cmdParams[c].clear();
    for (int i = command_length + 1; i < cmdBuf.getSize(); i++)
    {
      char b = cmdBuf[i];
      
      DBG_PORT.print("      Byte: `");
      DBG_PORT.print(b);
      DBG_PORT.println("`");
      
      if (b == '\\')
      {
        DBG_PORT.println("        Byte is a slash");
        if (!escaped) 
        { 
          DBG_PORT.println("        We're not in 'escaped' mode, switching to it");
          escaped = true;
          continue;
        }
      }
      
      if (b == ' ' && !escaped)
      {
        DBG_PORT.println("        byte is SPACE and it's not escaped");
        if (cmdParams[c].getSize() > 0)
        {
          c += 1;
          cmdParams[c].clear();
          DBG_PORT.print("          current param isn't empty (`");
          DBG_PORT.print(cmdParams[c-1].getBuffer());
          DBG_PORT.print("`), increasing c to ");          
          DBG_PORT.print(c);
          DBG_PORT.println(", and continuing to next iteration");
          continue;    
        }
        else
        {
          DBG_PORT.println("          current param is empty, skipping this space");
          continue;
        }
      }

      cmdParams[c].append(b);

      DBG_PORT.print("      Appending byte to current param, result:");
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
}

void Commandor::parseParams(int commandNum)
{
  DBG_PORT.println("  Commandor::parseParams");
  
  int command_length = Commandor::command_names[commandNum].length();
  Commandor::paramsCount = 0;
  if (Commandor::pbuf.getSize() == command_length || Commandor::pbuf.getSize() == command_length + 1)
  {
    return;
  }
  else
  {
    int c = 0;
    bool escaped = false;
    Commandor::params[c].clear();
    for (int i = command_length + 1; i < Commandor::pbuf.getSize(); i++)
    {
      char b = Commandor::pbuf[i];
      
      DBG_PORT.print("      Byte: `");
      DBG_PORT.print(b);
      DBG_PORT.println("`");
      
      if (b == '\\')
      {
        DBG_PORT.println("        Byte is a slash");
        if (!escaped) 
        { 
          DBG_PORT.println("        We're not in 'escaped' mode, switching to it");
          escaped = true;
          continue;
        }
      }
      
      if (b == ' ' && !escaped)
      {
        DBG_PORT.println("        byte is SPACE and it's not escaped");
        if (Commandor::params[c].getSize() > 0)
        {
          c += 1;
          Commandor::params[c].clear();
          DBG_PORT.print("          current param isn't empty (`");
          DBG_PORT.print(Commandor::params[c-1].getBuffer());
          DBG_PORT.print("`), increasing c to ");          
          DBG_PORT.print(c);
          DBG_PORT.println(", and continuing to next iteration");
          continue;    
        }
        else
        {
          DBG_PORT.println("          current param is empty, skipping this space");
          continue;
        }
      }

      Commandor::params[c].append(b);

      DBG_PORT.print("      Appending byte to current param, result:");
      DBG_PORT.println(Commandor::params[c].getBuffer());
      
      escaped = false;
    }

    for (int i = c + 1; i >= 0; i --)
    {
      if (Commandor::params[i].getSize()  > 0)
      {
        Commandor::paramsCount = i + 1;
        break;
      }
    }
  }
}

boolean Commandor::hasNewByte(int *newByte)
{
  if (COMMANDOR_PORT.available() > 0)
  {
    *newByte = COMMANDOR_PORT.read();
    if (*newByte != -1)
    {
      char c = *newByte;
      DBG_PORT.print("      BYTE: `");
      DBG_PORT.print(c);
      DBG_PORT.print("`");
      DBG_PORT.print(", byteNumInLine:");
      DBG_PORT.print(Commandor::byteNumInLine);
      DBG_PORT.print(", buffer_size before=");
      DBG_PORT.print(Commandor::pbuf.getSize());
      DBG_PORT.print(" (max_size=");
      DBG_PORT.print(Commandor::pbuf.max_size);
      DBG_PORT.println(")");
      if (Commandor::byteNumInLine == 0)
      {
        DBG_PORT.println("        It's first byte");
        if (c == '<')
        {
          DBG_PORT.println("          First byte is <");
          DBG_PORT.println("          Clearing buffer and switching to command mode");
          Commandor::pbuf.clear();
          Commandor::firstByteWasSpecial = true;
          Commandor::byteNumInLine += 1;
          return false;
        }
        Commandor::byteNumInLine += 1;
      }
      else 
      {
        DBG_PORT.println("        It's more than first byte");
        if (Commandor::firstByteWasSpecial)
        {
          DBG_PORT.println("          We are in command mode");
          if (c == '>')
          {
            DBG_PORT.println("            Found closing bracket");
            int found_command = -1;
            for (int i=0; i< Commandor::commands; i++)
            {
              if (Commandor::command_names[i].length() <= Commandor::pbuf.getSize())
              {
                boolean is_equal = true;
                for (int j=0; j < Commandor::command_names[i].length(); j++)
                {
                  if (Commandor::command_names[i].charAt(j) != Commandor::pbuf[j])
                  {
                    is_equal = false;
                    break;
                  }
                }
                if (is_equal) 
                {
                  if (Commandor::command_names[i].length() < Commandor::pbuf.getSize())
                  {
                    if (Commandor::pbuf[Commandor::command_names[i].length()] == ' ')
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
              DBG_PORT.print("              ERROR! COMMAND '"); 
              DBG_PORT.print(Commandor::pbuf.getBuffer());
              DBG_PORT.println("' not found!");
            }
            else
            {
              Commandor::parseParams(found_command);
                      
              DBG_PORT.print("              Command found: ");
              DBG_PORT.println(Commandor::command_names[found_command]);
              DBG_PORT.print("              Params parsed: ");
              DBG_PORT.print(Commandor::paramsCount);
              DBG_PORT.print(", params: ");
              for (int p = 0; p < Commandor::paramsCount; p ++)
              {
                DBG_PORT.print("[");
                DBG_PORT.print(p);
                DBG_PORT.print(",");
                DBG_PORT.print((int)&(Commandor::params[p]), HEX);
                DBG_PORT.print(",");
                DBG_PORT.print((int)(&(Commandor::params) + p), HEX);
                DBG_PORT.print("] `");
                DBG_PORT.print(Commandor::params[p].getBuffer());
                DBG_PORT.print("`");
                if (p + 1 < Commandor::paramsCount)
                {
                  DBG_PORT.print(", ");
                }
              }
              DBG_PORT.println("\n              Calling...");
              
              Commandor::command_actions[found_command](
                 Commandor::command_names[found_command].c_str(), 
                 &Commandor::pbuf, 
                 Commandor::params,
                 Commandor::paramsCount
              );
              Commandor::pbuf.clear();
            }
            Commandor::byteNumInLine = 0;
            Commandor::firstByteWasSpecial = false;
          }
          else
          {
            DBG_PORT.print("            Adding byte to buffer: ");
            DBG_PORT.print(c);
            DBG_PORT.print(" (");
            DBG_PORT.print(c, DEC);
            DBG_PORT.print(")\n");
            
            if (!Commandor::pbuf.append(c))
            {
              DBG_PORT.println("PBUF SIZE EXCEEDED, FORCED TO RESET COMMAND MODE");
              Commandor::firstByteWasSpecial = false;
              Commandor::pbuf.clear();
              Commandor::byteNumInLine = 0;
              return true;
            }

            DBG_PORT.print("            pbuf = `");
            DBG_PORT.print(Commandor::pbuf.getBuffer());
            DBG_PORT.print("`, pbuf bytes = [");
            for (int ii=0; ii<Commandor::pbuf.getSize(); ii++)
            {
              DBG_PORT.print((int)Commandor::pbuf[ii]);
              DBG_PORT.print(", ");
            }
            DBG_PORT.print("]\n");
            
          }
        }
        else if (c == '\n')
        {
          DBG_PORT.println("            Found newline character, resetting bytNumInLine to zero"); 
          Commandor::byteNumInLine = 0;
          return true;
        }
        else
        {
          return true;
        }
      }

      Commandor::byteNumInLine += 1;

      if (!Commandor::firstByteWasSpecial)
      {
        if (c == '\n')
        {
          DBG_PORT.println("          Resetting byteNumInLine");
          Commandor::byteNumInLine = 0;
        }
      }
      
      return true;
    }
  }
  return false;
}

#endif
