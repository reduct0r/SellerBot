#pragma once
#include "../../Bot/Bot.h"

class CheckoutState : public TelegramState {
public:
    explicit CheckoutState(TgBot::Bot& bot, Cart& cart, InputState& inputState, const std::string& providerToken);

    void handleStart(TgBot::Message::Ptr message) override;
    void handleBuy(TgBot::CallbackQuery::Ptr query);
    void handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) override;
    void handleUpdate(TgBot::Update::Ptr update);
    void handleMenu(TgBot::Message::Ptr message) override;
    void confirmOrder(TgBot::Message::Ptr message);

private:
    const std::string providerToken;
    bool geoPoint = 0;
    Cart& cart;
    TgBot::Bot& bot;
    InputState inputState;
    ShippingInfo shipInfo;
};