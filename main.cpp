#include "Bot/Bot.h"
#include "State/State.h"
#include "States/MainMenu/MainMenuState.h"

const std::string token = "7262333796:AAFwzFBdKZ7btr60P0TXt29uMWR4I4gA3Kk";
const std::string providerToken = "401643678:TEST:4ea52708-6b35-492a-a670-2b1e034bd4e3";
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
