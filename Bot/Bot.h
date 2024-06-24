#pragma once
#include <iostream>
#include <map>
#include <tgbot/tgbot.h>
#include "../State/State.h"
#include "../Product/Product.h"
#include "../DataBase/DataBase.h"

// Состояние ввода данных для доставки
enum InputState {
    NONE,
    MANUAL_INPUT_MAIN_ADDRESS,
    MANUAL_INPUT_SUB_ADDRESS,
    MANUAL_INPUT_COMMENT,
    GEOLOCATION_INPUT
};

// Структура данных для доставки
struct ShippingInfo
{
    std::string mainAddress;
    std::string subAddress;
    std::string shippingComment;
};

// Корзина
class Cart
{
public:
    std::vector<Product> listOfProducts;
    double sumOfCart;
    std::string sumOfCartS; // Сумма корзина типа строка

    Cart();
    Cart(std::vector<Product> listOfProducts, double sumOfCart);
    void clearCart(TgBot::Message::Ptr message); // Очистить корзину
    void addToCart(const Product& product);      // Добавить в корзину
};

class Bot {
public:
    Bot(const std::string& token, std::string connectionString, const std::string& providerToken);
    void run();
    void showCart(TgBot::Message::Ptr message);  // Показать корзину

private:
    const std::string providerToken;

    DataBase dataBase;
    TgBot::Bot telegramBot;
    std::shared_ptr<TelegramState> currentState;
    Cart cart;  // корзина
    InputState inputState;

    // Методы для инициализации компонентов и обработки событий.
    int getMessageAgeInSeconds(TgBot::Message::Ptr message);    // Метод для получения возраста сообщения в секундах
    void initializeCommands();                                  // Инициализация команд
    void setBotCommands();                                      // Установить команды ботa
    void handleCallbackQuery(TgBot::CallbackQuery::Ptr query);  // Обработка callback-запросов
    void initializeEventHandlers();                             // Инициализация обработчиков событий
};

void SetConsoleWidth(int width); // Установка ширины консоли