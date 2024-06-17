#pragma once
#include <tgbot/tgbot.h>
#include <stack>
#include "../DataBase/DataBase.h"

class TelegramState {
public:
    virtual void handleStart(TgBot::Message::Ptr message) = 0;
    virtual void handleMenu(TgBot::Message::Ptr message) = 0;
    virtual void handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) = 0;
};
