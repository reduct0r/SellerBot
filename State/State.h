#pragma once
#include <tgbot/tgbot.h>
#include <stack>
#include "../DataBase/DataBase.h"

// Основной виртуальный родительский класс состояния
class TelegramState {
public:
    virtual void handleStart(TgBot::Message::Ptr message) = 0;  // Обработка при старте состояния
    virtual void handleMenu(TgBot::Message::Ptr message) = 0;   // Обработка состояния по сообщению
    virtual void handleMenuQ(TgBot::CallbackQuery::Ptr query,   // Обработка состояния по запросу
        std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) = 0;
};
