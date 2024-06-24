#include "Bot/Bot.h"
#include "State/State.h"
#include "States/MainMenu/MainMenuState.h"

const std::string token = "7352338666:AAFb-3pidYMDq5bZ4TeRqDFaZFJYLHxzalo";
const std::string providerToken = "401643678:TEST:d3fd1b40-5343-4ded-86df-d2b2f20eae93";
std::string connectionString = "host=localhost port=5432 dbname=seller_bot user=postgres password=admin connect_timeout=10";

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleWidth(130);           // Установить ширину консоли в 120 символов
    std::cout << R"(
      ___           ___           ___       ___       ___           ___                    ___           ___                   
     /  /\         /  /\         /  /\     /  /\     /  /\         /  /\                  /  /\         /  /\          ___     
    /  /::\       /  /::\       /  /:/    /  /:/    /  /::\       /  /::\                /  /::\       /  /::\        /__/\    
   /__/:/\:\     /  /:/\:\     /  /:/    /  /:/    /  /:/\:\     /  /:/\:\              /  /:/\:\     /  /:/\:\       \  \:\   
  _\_ \:\ \:\   /  /::\ \:\   /  /:/    /  /:/    /  /::\ \:\   /  /::\ \:\            /  /::\ \:\   /  /:/  \:\       \__\:\  
 /__/\ \:\ \:\ /__/:/\:\ \:\ /__/:/    /__/:/    /__/:/\:\ \:\ /__/:/\:\_\:\          /__/:/\:\_\:| /__/:/ \__\:\      /  /::\ 
 \  \:\ \:\_\/ \  \:\ \:\_\/ \  \:\    \  \:\    \  \:\ \:\_\/ \__\/~|::\/:/          \  \:\ \:\/:/ \  \:\ /  /:/     /  /:/\:\
  \  \:\_\:\    \  \:\ \:\    \  \:\    \  \:\    \  \:\ \:\      |  |:|::/            \  \:\_\::/   \  \:\  /:/     /  /:/__\/
   \  \:\/:/     \  \:\_\/     \  \:\    \  \:\    \  \:\_\/      |  |:|\/              \  \:\/:/     \  \:\/:/     /__/:/     
    \  \::/       \  \:\        \  \:\    \  \:\    \  \:\        |__|:|~                \__\::/       \  \::/      \__\/      
     \__\/         \__\/         \__\/     \__\/     \__\/         \__\|                     ~~         \__\/                  
    )" << std::endl;


    Bot bot(token, connectionString, providerToken);
    bot.run();                          // Запуск бота
    return 0;
}
