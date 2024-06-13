#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"

class StartState : public TelegramState {
public:
    explicit StartState(TgBot::Bot& bot);
    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;
private:
    TgBot::Bot& bot;
};

class MenuState : public TelegramState {
public:
    explicit MenuState(TgBot::Bot& bot);
    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;
private:
    TgBot::Bot& bot;
};