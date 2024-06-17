#pragma once
#include <iostream>
#include <map>
#include <tgbot/tgbot.h>
#include "../State/State.h"
#include "../Product/Product.h"
#include "../DataBase/DataBase.h"

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
    Bot(const std::string& token, std::string connectionString);
    void run();
    void showCart(TgBot::Message::Ptr message);

private:
    DataBase dataBase;
    TgBot::Bot telegramBot;
    std::shared_ptr<TelegramState> currentState;
    Cart cart;  // �������
    InputState inputState;

    // ������ ��� ������������� ����������� � ��������� �������.
    int getMessageAgeInSeconds(TgBot::Message::Ptr message);  // ����� ��� ��������� �������� ��������� � ��������
    void initializeCommands();  // ������������� ������
    void setBotCommands();  // ���������� ������� ���a
    void handleCallbackQuery(TgBot::CallbackQuery::Ptr query);  // ��������� callback-��������
    void initializeEventHandlers();  // ������������� ������������ �������
};

void SetConsoleWidth(int width);