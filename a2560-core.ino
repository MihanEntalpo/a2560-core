#include "StaticString.h"
#include "commandor.h"
#include "actions.h"

Commandor commandor;

/**
 * Определения функций из actions.h
 */
void action_help(char* command, AbstractStaticString* full_command, AbstractStaticString *params, unsigned int paramsCount);
void action_help_d(char* command, Commandor* cmd);

/********************************************************\
 * Инициализация комманд в модуле Commander
 */

// Количество команд
int Commandor::commands = 1;

// Список имён команд
String Commandor::command_names[] = {"help"};
// Декларация списка функций
command_action Commandor::command_actions[MAX_COMMANDS];

// Задаём список функций и запускаем инициализацию коммандера
void init_commandor_actions()
{
  // Задаём функции
  Commandor::command_actions[0] = &action_help;

  // Задаём инициализацию
  Commandor::init();

  commandor.command_count = 1;
  commandor.command_items[0] = {
    "help",
    action_help_d
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

  Serial.println("Starting...");

  init_commandor_actions();

  Serial.println("Enter <command> or other data");
  
}

void process_byte_unparsed_by_commandor(char* byte)
{
  
}

void loop() {

  int newByte;

  commandor.processSerialByte(process_byte_unparsed_by_commandor);
  
}
