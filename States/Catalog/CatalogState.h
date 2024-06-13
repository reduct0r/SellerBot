#pragma once
#include "../../State/State.h"
#include "../../Bot/Bot.h"
#include "../../Product/Product.h"

class CatalogState : public TelegramState {
public:
    explicit CatalogState(TgBot::Bot& bot, const std::vector<Product>& products);
    explicit CatalogState(TgBot::Bot& bot, const std::vector<std::string>& categories, const std::vector<Product>& products);
    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;
private:
    TgBot::Bot& bot;
    std::vector<Product> products;
    std::vector<std::string> categories;
};