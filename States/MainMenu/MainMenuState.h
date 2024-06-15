#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"

class StartState : public TelegramState {
public:
    explicit StartState(TgBot::Bot& bot);
    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;
    void handleMenuQ(TgBot::CallbackQuery::Ptr query) override;
private:
    TgBot::Bot& bot;
};
