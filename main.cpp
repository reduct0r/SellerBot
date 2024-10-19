#include "Bot/Bot.h"
#include "State/State.h"
#include "States/MainMenu/MainMenuState.h"

const std::string token = "";
const std::string providerToken = "";
std::string connectionString = "host=localhost port=5432 dbname=seller_bot user=postgres password=admin connect_timeout=10";

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleWidth(130);           // Óñòàíîâèòü øèðèíó êîíñîëè â 120 ñèìâîëîâ
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
    bot.run();                          // Çàïóñê áîòà
    return 0;
}
