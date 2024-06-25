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
        price->label = u8"Заказ";
        price->amount = this->cart.sumOfCart * 100; // цена в копейках

        std::vector<TgBot::LabeledPrice::Ptr> prices;
        prices.push_back(price);

        std::map<std::string, std::pair<int, double>> productMap; // <название товара, <количество, общая стоимость>>

        for (const auto& product : this->cart.listOfProducts) {
            auto& productEntry = productMap[product.getName()];
            productEntry.first += 1;                    // Увеличиваем количество
            productEntry.second += product.getPrice();  // Прибавляем к общей сумме
            this->cart.sumOfCart += product.getPrice();
        }
        // Формируем детали корзины на основе подсчитанной информации
        std::string cartDetails;
        cartDetails = u8"Ваша корзина: \n";
        for (const auto& entry : productMap) {
            std::string str = std::to_string(entry.second.second);
            cartDetails += entry.first + " x" + std::to_string(entry.second.first) + " - " +
                str.erase(str.find_last_not_of('0') + 2, std::string::npos) + u8" руб\n";
        }
        cartDetails += u8"\nИтого: " + this->cart.sumOfCartS + u8" руб";

        if (!this->geoPoint) {
            bot.getApi().sendInvoice(
                query->message->chat->id,
                u8"Оплата заказа на сумму " + this->cart.sumOfCartS + u8" руб.",
                cartDetails,
                u8"unique_payload_for_this_transaction",
                providerToken,
                u8"RUB",
                prices,
                u8"",
                this->cart.listOfProducts[0].getImageUrl(),
                100, 100, 100, 1, 1, 1, 1
            );
        }
        else {
            bot.getApi().sendInvoice(
                query->message->chat->id,
                u8"Оплата заказа на сумму " + this->cart.sumOfCartS + u8" руб.",
                cartDetails,
                u8"unique_payload_for_this_transaction",
                providerToken,
                u8"RUB",
                prices,
                u8"",
                this->cart.listOfProducts[0].getImageUrl(),
                100, 100, 100, 1, 1, 1
            );
        }
    }
    catch (const TgBot::TgException& e) {
        std::cerr << u8"Error in handleBuyStars: " << e.what() << std::endl;
    }
}

void CheckoutState::handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase) {
    if (query->data == "manual_input") {
        bot.getApi().sendMessage(query->message->chat->id, u8"Введите комментарий для службы доставки. \n Уточните важные моменты, не оставляйте личные данные");
        inputState = MANUAL_INPUT_COMMENT;
        this->geoPoint = 0;
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
        handleBuy(query);
        bot.getApi().editMessageText(u8"Заказ отменен.", query->message->chat->id, query->message->messageId);
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<StartState>(bot);
    }
    else if (query->data == "confirm_order_yes") {
        bot.getApi().editMessageText(u8"Ваш заказ подтвержден!\nВыполните оплату", query->message->chat->id, query->message->messageId);

        // Вызов метода для обработки покупки звёзд
        handleBuy(query);
        // Подтверждение заказа и обновление базы данных
        for (const auto& item : cart.listOfProducts) {
            dataBase.confirmOrder(item.getName());
        }

        inputState = NONE;
        this->cart.clearCart();
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<StartState>(bot);
    }
}

// Функция для обработки обновлений с проверкой успешного платежа
void CheckoutState::handleUpdate(TgBot::Update::Ptr update) {
    if (update->preCheckoutQuery) {
        bot.getApi().answerPreCheckoutQuery(update->preCheckoutQuery->id, true, u8"Подтверждаю!");
    }
    else if (update->message && update->message->successfulPayment) {
        std::cout << u8"Payment successful! Payload: " << update->message->successfulPayment->invoicePayload << std::endl;
        // Здесь можно добавить дальнейшее действие после успешного платежа, например, сообщение пользователю
        bot.getApi().sendMessage(update->message->chat->id, u8"Ваш платеж успешно завершен! Спасибо за покупку.");
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
        bot.getApi().sendMessage(message->chat->id, u8"Введите комментарий для службы доставки. \nУточните важные моменты, не оставляйте личные данные");
        inputState = MANUAL_INPUT_COMMENT;
        break;
    case MANUAL_INPUT_COMMENT:
        shipInfo.shippingComment = message->text;
        inputState = NONE;
        confirmOrder(message);
        break;
    case GEOLOCATION_INPUT:
        if (message->location) {
            bot.getApi().sendMessage(message->chat->id, u8"Введите суб-адрес (дом, квартира, этаж):");
            //inputState = MANUAL_INPUT_SUB_ADDRESS;
            inputState = MANUAL_INPUT_SUB_ADDRESS;
        }
        break;
    default:
        break;
    }
}

void CheckoutState::confirmOrder(TgBot::Message::Ptr message) {
    std::string confirmationMessage;
    if (this->geoPoint) {
        confirmationMessage = u8"(Геометка) " + this->shipInfo.subAddress;
    }
    else {
        confirmationMessage = "";
    }
    confirmationMessage += u8"\nВаш комментарий: " + shipInfo.shippingComment + u8"\nПродолжить оформление заказа?";

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
