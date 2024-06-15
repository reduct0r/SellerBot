#pragma once
#include <tgbot/tgbot.h>
#include "../State/State.h"
#include "../Product/Product.h"

enum InputState {
    NONE,
    MANUAL_INPUT_MAIN_ADDRESS,
    MANUAL_INPUT_SUB_ADDRESS,
    MANUAL_INPUT_COMMENT,
    GEOLOCATION_INPUT
};

struct ShippingInfo
{
    std::string mainAddress;
    std::string subAddress;
    std::string shippingComment;
};

class Cart
{
public:
    std::vector<Product> listOfProducts;
    double sumOfCart;
    std::string sumOfCartS;

    Cart();
    Cart(std::vector<Product> listOfProducts, double sumOfCart);
    void clearCart(TgBot::Message::Ptr message);
    void addToCart(const Product& product);
};

class Bot {
public:
    Bot(const std::string& token);
    void run(std::string connectionString);
    void showCart(TgBot::Message::Ptr message);

private:
    TgBot::Bot telegramBot;
    std::shared_ptr<TelegramState> currentState;
    Cart cart;  // корзина
    InputState inputState;
    std::vector<Product> products;
};
