#include "Bot/Bot.h"
#include "State/State.h"
#include "States/MainMenu/MainMenuState.h"

const std::string token = "7262333796:AAFwzFBdKZ7btr60P0TXt29uMWR4I4gA3Kk";
std::string connectionString = "host=localhost port=5432 dbname=seller_bot user=postgres password=admin connect_timeout=10";

int main() {
    setlocale(LC_ALL, "Russian");

    Bot bot(token);
    bot.run(connectionString);
    return 0;
}
