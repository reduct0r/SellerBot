#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"
#include "../../Product/Product.h"
#include <vector>


class CategoryState : public TelegramState {
public:
    CategoryState(TgBot::Bot& bot, const std::string& category, const std::vector<Product>& products);

    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;

private:
    TgBot::Bot& bot;
    std::string category;
    std::vector<Product> products;
};

