#pragma once
#include <tgbot/tgbot.h>
#include "../State/State.h"
#include "../Product/Product.h"

class Cart
{
public:
    std::vector<Product> listOfProducts;
    double sumOfCart;

    Cart();
    Cart(std::vector<Product> listOfProducts, double sumOfCart);
    void clearCart(TgBot::Message::Ptr message);
    void addToCart(const Product& product);
};

class Bot {
public:
    Bot(const std::string& token);
    void run();

    void showCart(TgBot::Message::Ptr message);


private:
    TgBot::Bot telegramBot;
    std::shared_ptr<TelegramState> currentState;
    Cart cart;  // корзина
};

struct ShippingInfo
{
    std::string mainAddress;
    std::string subAddress;
    std::string shippingComment;
};