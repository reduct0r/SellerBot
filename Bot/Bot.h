#pragma once
#include <tgbot/tgbot.h>
#include "../State/State.h"

class Bot {
public:
    Bot(const std::string& token);
    void run();
private:
    TgBot::Bot telegramBot;
    std::shared_ptr<TelegramState> currentState;
};