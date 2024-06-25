#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"
#include "../../Product/Product.h"
#include <vector>

class CategoryState : public TelegramState {

public:
    CategoryState(TgBot::Bot& bot, const std::string& category, DataBase& dataBase);

    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) override;
    void handleMenu(TgBot::Message::Ptr message) override;

private:
    bool waitingForKeyword = 0;
    TgBot::Bot& bot;
    std::string category;
    std::vector<Product> products;
    void sortProducts(bool ascending);
    void handleSearch(TgBot::Message::Ptr message);

};

