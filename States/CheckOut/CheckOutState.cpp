#include "CheckOutState.h"
#include "../MainMenu/MainMenuState.h"
//#include <jcon/json.h>

CheckoutState::CheckoutState(TgBot::Bot& bot, Cart& cart, InputState& inputState) : bot(bot), cart(cart), inputState(inputState){}

void CheckoutState::handleStart(TgBot::Message::Ptr message){
    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    TgBot::InlineKeyboardButton::Ptr manualButton = std::make_shared<TgBot::InlineKeyboardButton>();
    manualButton->text = u8"���� �������";
    manualButton->callbackData = "manual_input";
    row.push_back(manualButton);

    TgBot::InlineKeyboardButton::Ptr geolocationButton = std::make_shared<TgBot::InlineKeyboardButton>();
    geolocationButton->text = u8"���������� ��������";
    geolocationButton->callbackData = "geolocation_input";
    row.push_back(geolocationButton);

    keyboard->inlineKeyboard.push_back(row);
    bot.getApi().sendMessage(message->chat->id, u8"�������� ������ ����� ������:", false, 0, keyboard);
}

void CheckoutState::handleMenuQ(TgBot::CallbackQuery::Ptr query, std::shared_ptr<TelegramState>& currentState, DataBase& dataBase)
{
    if (query->data == "manual_input") {
        bot.getApi().sendMessage(query->message->chat->id, u8"������� �������� ����� (������, �����, �����):");
        inputState = MANUAL_INPUT_MAIN_ADDRESS;
        bot.getApi().answerCallbackQuery(query->id);
}
    else if (query->data == "geolocation_input") {
        bot.getApi().sendMessage(query->message->chat->id, u8"���������� ��������:");
        this->geoPoint = 1;
        inputState = GEOLOCATION_INPUT;
        bot.getApi().answerCallbackQuery(query->id);
    }   
    else if (query->data == "confirm_order_no") {
        inputState = NONE;
        bot.getApi().editMessageText(u8"����� �������.", query->message->chat->id, query->message->messageId);
        bot.getApi().answerCallbackQuery(query->id);
        currentState = std::make_shared<StartState>(bot);
    }
    else if (query->data == "confirm_order_yes") {
        inputState = NONE;
        bot.getApi().editMessageText(u8"��� ����� �����������!\n��� �������� � ������� ������.", query->message->chat->id, query->message->messageId);
        bot.getApi().answerCallbackQuery(query->id);

        // ������������� ������ � ���������� ���� ������
        for (const auto& item : cart.listOfProducts) {
            dataBase.confirmOrder(item.getName());
        }

        currentState = std::make_shared<StartState>(bot);
    }
}

void CheckoutState::handleMenu(TgBot::Message::Ptr message)
{
    switch (inputState) {
    case MANUAL_INPUT_MAIN_ADDRESS:
        shipInfo.mainAddress = message->text;
        bot.getApi().sendMessage(message->chat->id, u8"������� ���-����� (���, ��������, ����):");
        inputState = MANUAL_INPUT_SUB_ADDRESS;
        break;
    case MANUAL_INPUT_SUB_ADDRESS:
        shipInfo.subAddress = message->text;
        bot.getApi().sendMessage(message->chat->id, u8"������� ����������� ��� ������ ��������:");
        inputState = MANUAL_INPUT_COMMENT;
        break;
    case MANUAL_INPUT_COMMENT:
        shipInfo.shippingComment = message->text;
        inputState = NONE;
        confirmOrder(message);
        break;
    case GEOLOCATION_INPUT:
        if (message->location) {
            bot.getApi().sendMessage(message->chat->id, u8"�������� ��������. ������� ���-����� (���, ��������, ����):");
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
        address = u8"(��������), " + shipInfo.subAddress;
    }
    else {
        address = shipInfo.mainAddress + ", " + shipInfo.subAddress;
    }
    std::string confirmationMessage = u8"��� �����: " + address + u8"\n�����������: " + shipInfo.shippingComment + u8"\n����������� �����?";

    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;

    TgBot::InlineKeyboardButton::Ptr yesButton = std::make_shared<TgBot::InlineKeyboardButton>();
    yesButton->text = u8"��";
    yesButton->callbackData = "confirm_order_yes";
    row.push_back(yesButton);

    TgBot::InlineKeyboardButton::Ptr noButton = std::make_shared<TgBot::InlineKeyboardButton>();
    noButton->text = u8"���";
    noButton->callbackData = "confirm_order_no";
    row.push_back(noButton);

    keyboard->inlineKeyboard.push_back(row);
    bot.getApi().sendMessage(message->chat->id, confirmationMessage, false, 0, keyboard);
}

