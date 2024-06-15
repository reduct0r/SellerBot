#include "Bot.h"
#include "../States/MainMenu/MainMenuState.h"
#include <iostream>
#include "../States/Catalog/CatalogState.h"
#include "../States/Category/CategoryState.h"
#include <map>


// Примерный список продуктов
std::vector<Product> products = {
    Product("Printer1", "Printers", 100.0, 10, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg"),
    Product("Scanner1", "Scanners", 150.0, 5, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg"),
    Product("Printer2", "Printers", 200.0, 8, u8"Описание", u8"Характеристики", u8"Доставка", "https://ir.ozone.ru/s3/multimedia-9/6693430473.jpg")
};

// Примерный список категорий
std::vector<std::string> categories = { "Printers", "Scanners" };

int getMessageAgeInSeconds(TgBot::Message::Ptr message) {
    std::time_t current_time = std::time(nullptr);
    return static_cast<int>(current_time - message->date);
}

Bot::Bot(const std::string& token) : telegramBot(token), currentState(std::make_shared<StartState>(telegramBot)) {

    // Инициализация команд для меню бота
    telegramBot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("menu", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);                   // сброс стейта
        currentState->handleMenu(message);
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
    commandMenu->description = u8"Показать главное меню";

    auto commandCart = std::make_shared<TgBot::BotCommand>();
    commandCart->command = "cart";
    commandCart->description = u8"🛒 Просмотреть корзину";

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
                this->cart.addToCart(*it);
                this->telegramBot.getApi().sendMessage(query->message->chat->id, "<b>" + productName + "</b>" + u8" x 1\nДобавлено в корзину", false, 0, nullptr, "HTML");
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
        else if (query->data == "checkout") { // оформление заказа
            

        }

        else {
            currentState->handleMenuQ(query); // Обработка запросов в соответствующих меню
        }

        });
}

void Bot::run() {
    try {
        TgBot::TgLongPoll longPoll(telegramBot);
        std::cout << "Bot started: Telegram connettion ready\n";
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
    listOfProducts(listOfProducts), sumOfCart(sumOfCart) {}

Cart::Cart() {
    this->sumOfCart = 0;
}

void Cart::addToCart(const Product& product) {
    this->listOfProducts.push_back(product);
}

void Cart::clearCart(TgBot::Message::Ptr message){
    this->listOfProducts.clear();
}

void Bot::showCart(TgBot::Message::Ptr message) {

    // Создаем хранилище для подсчета товаров
    std::map<std::string, std::pair<int, double>> productMap; // <название товара, <количество, общая стоимость>>

    for (const auto& product : this->cart.listOfProducts) {
        auto& productEntry = productMap[product.getName()];
        productEntry.first += 1; // Увеличиваем количество
        productEntry.second += product.getPrice(); // Прибавляем к общей сумме
        this->cart.sumOfCart += product.getPrice();
    }

    // Формируем детали корзины на основе подсчитанной информации
    std::string cartDetails;
    cartDetails = u8"<i>Ваша корзина:</i>\n";
    for (const auto& entry : productMap) {
        std::string str = std::to_string(entry.second.second);
        cartDetails += entry.first + " x" + std::to_string(entry.second.first) + " - " +
            str.erase(str.find_last_not_of('0') + 2, std::string::npos) + u8" руб\n";
    }
    cartDetails += u8"\n<b>Итого:</b> " + std::to_string(this->cart.sumOfCart).erase(std::to_string(this->cart.sumOfCart).find_last_not_of('0') + 2, std::string::npos) +
        u8" руб";

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

