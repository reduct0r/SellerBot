#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"
#include "../../Product/Product.h"

class CatalogState : public TelegramState {
public:
    explicit CatalogState(TgBot::Bot& bot, DataBase& dataBase);
    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) override;
    void handleMenu(TgBot::Message::Ptr message) override;
private:
    TgBot::Bot& bot;
    std::vector<Product> products;
    std::vector<std::string> categories;
};