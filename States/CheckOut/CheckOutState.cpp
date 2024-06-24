#include "CheckOutState.h"
#include "../MainMenu/MainMenuState.h"

CheckoutState::CheckoutState(TgBot::Bot& bot, Cart& cart, InputState& inputState, const std::string& providerToken)
    : bot(bot), cart(cart), inputState(inputState), providerToken(providerToken){}

void CheckoutState::handleStart(TgBot::Message::Ptr message){
    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    TgBot::InlineKeyboardButton::Ptr manualButton = std::make_shared<TgBot::InlineKeyboardButton>();
    manualButton->text = u8"Ввод вручную";
    manualButton->callbackData = "manual_input";
    row.push_back(manualButton);

    TgBot::InlineKeyboardButton::Ptr geolocationButton = std::make_shared<TgBot::InlineKeyboardButton>();
    geolocationButton->text = u8"Прикрепить геометку";
    geolocationButton->callbackData = "geolocation_input";
    row.push_back(geolocationButton);

    keyboard->inlineKeyboard.push_back(row);
    bot.getApi().sendMessage(message->chat->id, u8"Выберите способ ввода адреса:", false, 0, keyboard);
}

void CheckoutState::handleBuy(TgBot::CallbackQuery::Ptr query) {
    try {
        TgBot::LabeledPrice::Ptr price(new TgBot::LabeledPrice());
        price->label = u8"5 Telegram Stars";
        price->amount = 10000; // цена в копейках

        std::vector<TgBot::LabeledPrice::Ptr> prices;
        prices.push_back(price);

        bot.getApi().sendInvoice(
            query->message->chat->id,
            u8"Покупка Telegram Stars",
            u8"Вы собираетесь купить 5 Telegram Stars",
            u8"unique_payload_for_this_transaction",
            providerToken,
            u8"RUB",
            prices,
            u8"",  // Фото URL (оставляем пустым, если фото нет)
            u8"",  // Фото размера (оставляем пустым, если фото нет)
            60   // Время ожидания в секундах перед завершением платежа
        );
    }
    catch (const TgBot::TgException& e) {
        std::cerr << u8"Error in handleBuyStars: " << e.what() << std::endl;
    }
}

void CheckoutState::handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) {
    if (query->data == "manual_input") {
        bot.getApi().sendMessage(query->message->chat->id, u8"Введите основной адрес (страна, город, улица):");
        inputState = MANUAL_INPUT_MAIN_ADDRESS;
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "geolocation_input") {
        bot.getApi().sendMessage(query->message->chat->id, u8"Прикрепите геометку:");
        this->geoPoint = 1;
        inputState = GEOLOCATION_INPUT;
        bot.getApi().answerCallbackQuery(query->id);
    }
    else if (query->data == "confirm_order_no") {
        inputState = NONE;
        bot.getApi().editMessageText(u8"Заказ отменен.", query->message->chat->id, query->message->messageId);
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<StartState>(bot);
    }
    else if (query->data == "confirm_order_yes") {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;

        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text = u8"Купить 5 звезд за 100 рублей";
        button->callbackData = u8"buy_stars";
        row.push_back(button);
        keyboard->inlineKeyboard.push_back(row);

        bot.getApi().editMessageText(u8"Ваш заказ подтвержден!\nВыполните оплату", query->message->chat->id, query->message->messageId);

        // Вызов метода для обработки покупки звёзд
        handleBuy(query);
        // Подтверждение заказа и обновление базы данных
        for (const auto& item : cart.listOfProducts) {
            dataBase.confirmOrder(item.getName());
        }


        inputState = NONE;
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<StartState>(bot);
    }
}

void CheckoutState::handleMenu(TgBot::Message::Ptr message)
{
    switch (inputState) {
    case MANUAL_INPUT_MAIN_ADDRESS:
        shipInfo.mainAddress = message->text;
        bot.getApi().sendMessage(message->chat->id, u8"Введите суб-адрес (дом, квартира, этаж):");
        inputState = MANUAL_INPUT_SUB_ADDRESS;
        break;
    case MANUAL_INPUT_SUB_ADDRESS:
        shipInfo.subAddress = message->text;
        bot.getApi().sendMessage(message->chat->id, u8"Введите комментарий для службы доставки:");
        inputState = MANUAL_INPUT_COMMENT;
        break;
    case MANUAL_INPUT_COMMENT:
        shipInfo.shippingComment = message->text;
        inputState = NONE;
        confirmOrder(message);
        break;
    case GEOLOCATION_INPUT:
        if (message->location) {
            bot.getApi().sendMessage(message->chat->id, u8"Геометка получена. Введите суб-адрес (дом, квартира, этаж):");
            inputState = MANUAL_INPUT_SUB_ADDRESS;
        }
        break;
    default:
        break;
    }
}

void CheckoutState::confirmOrder(TgBot::Message::Ptr message) {
    std::string address;
    if (this->geoPoint) {
        address = u8"(Геометка), " + shipInfo.subAddress;
    }
    else {
        address = shipInfo.mainAddress + ", " + shipInfo.subAddress;
    }
    std::string confirmationMessage = u8"Ваш адрес: " + address + u8"\nКомментарий: " + shipInfo.shippingComment + u8"\nПодтвердить заказ?";

    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    TgBot::InlineKeyboardButton::Ptr yesButton = std::make_shared<TgBot::InlineKeyboardButton>();
    yesButton->text = u8"Да";
    yesButton->callbackData = "confirm_order_yes";
    row.push_back(yesButton);

    TgBot::InlineKeyboardButton::Ptr noButton = std::make_shared<TgBot::InlineKeyboardButton>();
    noButton->text = u8"Нет";
    noButton->callbackData = "confirm_order_no";
    row.push_back(noButton);

    keyboard->inlineKeyboard.push_back(row);
    bot.getApi().sendMessage(message->chat->id, confirmationMessage, false, 0, keyboard);
}

