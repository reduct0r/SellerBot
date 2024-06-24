#include "Bot.h"
#include "../States/MainMenu/MainMenuState.h"
#include "../States/Catalog/CatalogState.h"
#include "../States/Category/CategoryState.h"
#include "../States/CheckOut/CheckOutState.h"

Bot::Bot(const std::string& token, std::string connectionString, const std::string& providerToken)
    : dataBase(connectionString), telegramBot(token), currentState(std::make_shared<StartState>(telegramBot)),
    providerToken(providerToken){

    this->inputState = NONE;    // Состояние ввода пользователя

    initializeCommands();       // Инициализация команд для меню бота
    setBotCommands();           // Установка команд для меню бота
    initializeEventHandlers();  // Инициализация обработчиков событий
}

// Проверка сообщения на жизненный срок
int Bot::getMessageAgeInSeconds(TgBot::Message::Ptr message) {
    std::time_t current_time = std::time(nullptr);
    return static_cast<int>(current_time - message->date);
}

// Инициализация команд для меню бота
void Bot::initializeCommands() {
    telegramBot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("menu", [this](TgBot::Message::Ptr message) {
        currentState = std::make_shared<StartState>(telegramBot);
        currentState->handleStart(message);
        });

    telegramBot.getEvents().onCommand("cart", [this](TgBot::Message::Ptr message) {
        this->showCart(message);
        });
}

// Установка команд для меню бота
void Bot::setBotCommands() {
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
}

// Обработчик событий
void Bot::handleCallbackQuery(TgBot::CallbackQuery::Ptr query) {
    if (getMessageAgeInSeconds(query->message) > 60) {
        this->telegramBot.getApi().sendMessage(query->message->chat->id, u8"Данное сообщение устарело. Пожалуйста, попробуйте заново зайти в соотвествующее меню.");
        telegramBot.getApi().answerCallbackQuery(query->id);
        return;
    }
    else if (query->data == "catalog") {
        currentState = std::make_shared<CatalogState>(telegramBot, dataBase);
        currentState->handleStart(query->message);
        telegramBot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data.rfind("category_", 0) == 0) {
        telegramBot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
        std::string category = query->data.substr(9);
        currentState = std::make_shared<CategoryState>(telegramBot, category, dataBase);
        currentState->handleStart(query->message);
    }
    else if (query->data.rfind("back_to_category_", 0) == 0) {
        std::string category = query->data.substr(17);
        currentState = std::make_shared<CategoryState>(telegramBot, category, dataBase);
        currentState->handleStart(query->message);
        telegramBot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data.rfind("add_to_cart_", 0) == 0) {
        std::string productName = query->data.substr(12);
        auto it = std::find_if(dataBase.getProducts().begin(), dataBase.getProducts().end(), [&productName](const Product& product) {
            return product.getName() == productName;
            });

        if (it != dataBase.getProducts().end()) {
            if (it->getAvailableQuantity() <= 0) {
                this->telegramBot.getApi().sendMessage(query->message->chat->id, u8"Извините, товара нет в наличии😥", false, 0, nullptr, "HTML");
            }
            else {
                this->cart.addToCart(*it);
                this->telegramBot.getApi().sendMessage(query->message->chat->id, "<b>" + productName + "</b>" + u8" x 1\nДобавлено в корзину", false, 0, nullptr, "HTML");
            }
        }
        telegramBot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "cart") {
        this->showCart(query->message);
        telegramBot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "clear_cart") {
        telegramBot.getApi().answerCallbackQuery(query->id);
        this->telegramBot.getApi().editMessageText(u8"Корзина очищена", query->message->chat->id, query->message->messageId);
        cart.clearCart();
    }
    else if (query->data == "checkout") {
        currentState = std::make_shared<CheckoutState>(telegramBot, cart, inputState, providerToken);
        currentState->handleStart(query->message);
        telegramBot.getApi().answerCallbackQuery(query->id);
    }
    else {
        currentState->handleMenuQ(query, currentState, dataBase);
    }
}

// Инифиализация обработчиков событий
void Bot::initializeEventHandlers() {
    telegramBot.getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query) {
        handleCallbackQuery(query);         // Обработчик событий по запросу
        });

    telegramBot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        currentState->handleMenu(message);  // Обработчик событий по сообщению
        });
}

// Метод запуска бота
void Bot::run() {

    try {                                                           //  Если внутри блока `try` возникает исключение любого типа, который наследует от `std::exception`, управление передаётся в блок `catch`.
        TgBot::TgLongPoll longPoll(telegramBot);                    //  Создаётся объект longPoll, который отвечает за долгий опрос (long polling) сервера Telegram
        std::cout << "Bot started: Telegram connettion ready\n";
        
        while (true) {                                              // Запускается бесконечный цикл `while`, который непрерывно выполняет метод `start` объекта `longPoll`                                                                                       
            longPoll.start();                                       // Метод `start` обрабатывает новые обновления от сервера Telegram.
        }                                                           // Этот цикл будет работать бесконечно, пока не произойдёт какое-либо исключение или бот не будет принудительно остановлен
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

void Cart::clearCart(){
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

void SetConsoleWidth(int width) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        std::cerr << "Error getting console handle." << std::endl;
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        std::cerr << "Error getting console buffer info." << std::endl;
        return;
    }

    SMALL_RECT& winInfo = csbi.srWindow;
    COORD newSize;
    newSize.X = width;
    newSize.Y = winInfo.Bottom - winInfo.Top + 1;

    if (!SetConsoleScreenBufferSize(hOut, newSize)) {
        std::cerr << "Error setting console buffer size." << std::endl;
        return;
    }

    winInfo.Right = winInfo.Left + width - 1;

    if (!SetConsoleWindowInfo(hOut, TRUE, &winInfo)) {
        std::cerr << "Error setting console window info." << std::endl;
    }
}

