#include "CategoryState.h"
#include "../MainMenu/MainMenuState.h"

CategoryState::CategoryState(TgBot::Bot& bot, const std::string& category, const std::vector<Product>& products)
    : bot(bot), category(category), products(products) { }

void CategoryState::handleStart(TgBot::Message::Ptr message) {
    auto chatId = message->chat->id;
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    for (const auto& product : products) {

        if (product.getCategory() == category) {

            std::vector<TgBot::InlineKeyboardButton::Ptr> row;
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text = product.getName();
            button->callbackData = "product_" + product.getName(); // ���������� ������������� ��� ��������
            row.push_back(button);
            keyboard->inlineKeyboard.push_back(row);
        }
    }

    bot.getApi().sendMessage(chatId, u8"�������� � ���������: " + category, false, 0, keyboard);
}

void CategoryState::handleMenuQ(TgBot::CallbackQuery::Ptr query)
{
    if (query->data.rfind(u8"product_", 0) == 0) {
        std::string productName = query->data.substr(8); // �������� ��� ��������
        auto it = std::find_if(products.begin(), products.end(), [&productName](const Product& product) {
            return product.getName() == productName;
            });

        if (it != products.end()) {
            const Product& product = *it;
            TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
            std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

            TgBot::InlineKeyboardButton::Ptr addToCartButton(new TgBot::InlineKeyboardButton);
            addToCartButton->text = u8"�������� � �������";
            addToCartButton->callbackData = "add_to_cart_" + productName;
            buttons.push_back(addToCartButton);

            TgBot::InlineKeyboardButton::Ptr backButton(new TgBot::InlineKeyboardButton);
            backButton->text = u8"�����";
            backButton->callbackData = "back_to_category_" + product.getCategory();
            buttons.push_back(backButton);

            keyboard->inlineKeyboard.push_back(buttons);

            // �������� ��������� � �����������, ���������, ��������� � �����
            bot.getApi().sendPhoto(query->message->chat->id, product.getImageUrl(),
                product.getName() + "\n" + product.getDescription() + u8"\n����: " + std::to_string(product.getPrice()), 0, keyboard);

            bot.getApi().answerCallbackQuery(query->id);
        }
    }


}

void CategoryState::handleMenu(TgBot::Message::Ptr message) {

}

