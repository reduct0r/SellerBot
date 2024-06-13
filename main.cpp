#include "Bot/Bot.h"
#include "State/State.h"
#include "States/MainMenu/MainMenuState.h"
const std::string token = "7262333796:AAFwzFBdKZ7btr60P0TXt29uMWR4I4gA3Kk";

int main() {
    Bot bot(token);
    bot.run();
    return 0;
}