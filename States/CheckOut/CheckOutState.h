#pragma once
#include "../../Bot/Bot.h"

class CheckoutState : public TelegramState {
public:
    explicit CheckoutState(TgBot::Bot& bot, Cart& cart, InputState& inputState);

    void handleStart(TgBot::Message::Ptr message) override;
    void handleMenu(TgBot::Message::Ptr message) override;
    void handleMenuQ(TgBot::CallbackQuery::Ptr query) override;
    void confirmOrder(TgBot::Message::Ptr message);

private:
    bool geoPoint = 0;
    Cart& cart;
    TgBot::Bot& bot;
    InputState inputState;
    ShippingInfo shipInfo;
};