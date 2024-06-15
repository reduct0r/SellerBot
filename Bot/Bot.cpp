#include "Bot.h"
#include <iostream>
#include <map>
#include "../States/MainMenu/MainMenuState.h"
#include "../States/Catalog/CatalogState.h"
#include "../States/Category/CategoryState.h"
#include "../States/CheckOut/CheckOutState.h"
#include "../DataBase/DataBase.h"

// Примерный список категорий
std::vector<std::string> categories = { "Printers", "Scanners" };

int getMessageAgeInSeconds(TgBot::Message::Ptr message) {
    std::time_t current_time = std::time(nullptr);
    return static_cast<int>(current_time - message->date);
}

Bot::Bot(const std::string& token) : telegramBot(token), currentState(std::make_shared<StartState>(telegramBot)) {
    
    this->inputState = NONE;
    // Инициализация команд для меню бота
    telegramBot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("menu", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("cart", [this](TgBot::Message::Ptr message) {
        this->showCart(message);
        });

    // Установка команд для меню бота
    std::vector<TgBot::BotCommand::Ptr> commands;
    auto commandStart = std::make_shared<TgBot::BotCommand>();
    commandStart->command = "start";
    commandStart->description = u8"Начать";

    auto commandMenu = std::make_shared<TgBot::BotCommand>();
    commandMenu->command = "menu";
    commandMenu->description = u8"Меню";

    auto commandCart = std::make_shared<TgBot::BotCommand>();
    commandCart->command = "cart";
    commandCart->description = u8"Моя корзина 🛒";

    commands.push_back(commandStart);
    commands.push_back(commandMenu);
    commands.push_back(commandCart);

    telegramBot.getApi().setMyCommands(commands);

    // Обработка callback-запросов
    telegramBot.getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query) {

        if (getMessageAgeInSeconds(query->message) > 60) { // Если сообщение старше 60 секунд
            this->telegramBot.getApi().sendMessage(query->message->chat->id, u8"Данное сообщение устарело. Пожалуйста, попробуйте заново зайти в соотвествующее меню.");
            telegramBot.getApi().answerCallbackQuery(query->id);
            return; // Пропускаем старый callback-запрос
        }
        else if (query->data == "catalog") { // список категорий
            currentState = std::make_shared<CatalogState>(telegramBot, categories, products); 
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data.rfind("category_", 0) == 0) { // список товаров в категории
            telegramBot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
            std::string category = query->data.substr(9); // Получаем название категории
            currentState = std::make_shared<CategoryState>(telegramBot, category, products);
            currentState->handleStart(query->message);    
        }
        else if (query->data.rfind("back_to_category_", 0) == 0) { // кнопка назад из товара
            std::string category = query->data.substr(17);
            currentState = std::make_shared<CategoryState>(telegramBot, category, products);
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data.rfind("add_to_cart_", 0) == 0) { // добавить в корзину
            std::string productName = query->data.substr(12); // Получаем имя продукта
            auto it = std::find_if(products.begin(), products.end(), [&productName](const Product& product) {
                return product.getName() == productName;
                });

            if (it != products.end()) {
                if (it->getAvailableQuantity() <= 0) {
                    this->telegramBot.getApi().sendMessage(query->message->chat->id,
                        u8"Извините, товара нет в наличии😥", false, 0, nullptr, "HTML");
                }
                else {
                    this->cart.addToCart(*it);
                    this->telegramBot.getApi().sendMessage(query->message->chat->id, "<b>" + productName + "</b>" + u8" x 1\nДобавлено в корзину", false, 0, nullptr, "HTML");
                }
            }
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data == "back_to_catalog") { // назад из товаров
            telegramBot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
            currentState = std::make_shared<CatalogState>(telegramBot, categories, products);
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data == "cart") { // моя корзина
            this->showCart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data == "clear_cart") { // очистить корзину
            telegramBot.getApi().answerCallbackQuery(query->id);
            this->telegramBot.getApi().editMessageText(u8"Корзина очищена", query->message->chat->id, query->message->messageId);
            cart.clearCart(query->message);
        }
        else if (query->data == "checkout") {
            currentState = std::make_shared<CheckoutState>(telegramBot, cart, inputState);
            currentState->handleStart(query->message);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data == "confirm_order_yes") {
            inputState = NONE;
            telegramBot.getApi().editMessageText(u8"Ваш заказ подтвержден!\nВас уведомят о статусе заказа.", query->message->chat->id, query->message->messageId);
            currentState = std::make_shared<StartState>(telegramBot);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else if (query->data == "confirm_order_no") {
            inputState = NONE;
            telegramBot.getApi().editMessageText(u8"Заказ отменен.", query->message->chat->id, query->message->messageId);
            currentState = std::make_shared<StartState>(telegramBot);
            telegramBot.getApi().answerCallbackQuery(query->id);
        }
        else {
            currentState->handleMenuQ(query); // Обработка запросов в соответствующих меню
        }
    });

    telegramBot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
         currentState->handleMenu(message);
    });
}

void Bot::run(std::string connectionString) {
    try {
        TgBot::TgLongPoll longPoll(telegramBot);
        std::cout << "Bot started: Telegram connettion ready\n";
        
        this->products = fetchProductsFromDb(connectionString);

        while (true) {
            longPoll.start();
        }
    }
    catch (std::exception& e) {
        printf("Bot stopped with error: %s\n", e.what());
    }
}

// Cart
Cart::Cart(std::vector<Product> listOfProducts, double sumOfCart) :
    listOfProducts(listOfProducts), sumOfCart(sumOfCart) {
    this->sumOfCartS = std::to_string(this->sumOfCart).erase(std::to_string(this->sumOfCart).find_last_not_of('0') + 2, std::string::npos);
}

Cart::Cart() {
    this->sumOfCart = 0;
    this->sumOfCartS = "";
}

void Cart::addToCart(const Product& product) {
    this->listOfProducts.push_back(product);
}

void Cart::clearCart(TgBot::Message::Ptr message){
    this->listOfProducts.clear();
    this->sumOfCart = 0;
}

void Bot::showCart(TgBot::Message::Ptr message) {
    this->cart.sumOfCart = 0;
    // Создаем хранилище для подсчета товаров
    std::map<std::string, std::pair<int, double>> productMap; // <название товара, <количество, общая стоимость>>

    for (const auto& product : this->cart.listOfProducts) {
        auto& productEntry = productMap[product.getName()];
        productEntry.first += 1; // Увеличиваем количество
        productEntry.second += product.getPrice(); // Прибавляем к общей сумме
        this->cart.sumOfCart += product.getPrice();
    }
    this->cart.sumOfCartS = std::to_string(this->cart.sumOfCart).erase(std::to_string(this->cart.sumOfCart).find_last_not_of('0') + 2, std::string::npos);
    // Формируем детали корзины на основе подсчитанной информации
    std::string cartDetails;
    cartDetails = u8"<i>Ваша корзина:</i>\n";
    for (const auto& entry : productMap) {
        std::string str = std::to_string(entry.second.second);
        cartDetails += entry.first + " x" + std::to_string(entry.second.first) + " - " +
            str.erase(str.find_last_not_of('0') + 2, std::string::npos) + u8" руб\n";
    }
    cartDetails += u8"\n<b>Итого:</b> " + this->cart.sumOfCartS + u8" руб";

    if (productMap.empty()) {
        cartDetails = u8"<i>Ваша корзина пуста.</i>";
        this->telegramBot.getApi().sendMessage(message->chat->id, cartDetails, false, 0, nullptr, "HTML");
    }
    else {
        // Создаем инлайн-клавиатуру
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

        // Кнопка "Очистить корзину"
        TgBot::InlineKeyboardButton::Ptr clearButton(new TgBot::InlineKeyboardButton);
        clearButton->text = u8"🗑️ Очистить корзину";
        clearButton->callbackData = "clear_cart";
        keyboard->inlineKeyboard.push_back({ clearButton });

        // Кнопка "Оформить заказ"
        TgBot::InlineKeyboardButton::Ptr orderButton(new TgBot::InlineKeyboardButton);
        orderButton->text = u8"🛍️ Оформить заказ";
        orderButton->callbackData = "checkout";
        keyboard->inlineKeyboard.push_back({ orderButton });

        // Отправляем сообщение с инлайн-клавиатурой
        this->telegramBot.getApi().sendMessage(message->chat->id, cartDetails, false, 0, keyboard, "HTML");
    }
}

