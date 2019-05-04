#include "StaticString.h"
#include "commandor.h"
#include "actions.h"

Commandor commandor;

/********************************************************\
 * Инициализация комманд в модуле Commander
 */


// Задаём список функций и запускаем инициализацию коммандера
void init_commandor_actions()
{
  // Задаём инициализацию
  commandor.command_count = 2;
  commandor.command_items[0] = {
    F("help"),
    F("Display this help"),
    action_help
  };
  commandor.command_items[1] = {
    F("test_params"),
    F("Show, how parameters are parsed\nTakes all parameters and outputs it"),
    action_test_params
  };
}

/**
 * Конец инициализации комманд в модуле Commander
 *******************************************************/

int last_millis = 0;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  last_millis = millis();

  Serial.println(F("Starting..."));

  init_commandor_actions();

  Serial.println(F("Enter <command> or other data"));
  
}

void process_byte_unparsed_by_commandor(char* byte)
{
  
}

void loop() {

  int newByte;

  commandor.processSerialByte(process_byte_unparsed_by_commandor);
  
}
