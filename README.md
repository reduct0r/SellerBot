# Telegram Bot Initialization and Launch Guide

Welcome to the guide for initializing and launching a Telegram bot using the `tgbot-cpp` library. This README will walk you through the steps required to set up commands, event handlers, and various states to ensure your bot operates seamlessly.

## Initialization and Bot Launch

To initialize and launch a Telegram bot, you need to create an instance of `Bot` with a token obtained from BotFather. The constructor of the `Bot` class sets up commands and event handlers.

### Step-by-Step Setup

#### 1. Command and Event Handler Configuration
During initialization, handlers for various commands such as "/start", "/menu", and "/cart" are set up. These commands allow users to:
- `/start`: Begin interaction with the bot.
- `/menu`: View the product menu.
- `/cart`: Check the contents of their cart.

#### 2. Command Menu Setup
Commands visible to the user in the Telegram interface are set for the bot. For instance, the commands "Start", "Menu", and "My Cart" are provided. This improves user navigation and enhances the overall experience.

#### 3. Bot Launch
After configuring all commands and handlers, the bot starts its operation cycle using the `longPoll` function. This function continuously polls the Telegram server for new messages, enabling the bot to function in real-time and respond to every user interaction promptly.

### Handling Incoming Messages

#### 1. Message and Callback Query Handling
When the bot receives new messages or callback queries, they are processed through appropriate functions. For example:
- Upon receiving the "/start" command, the bot's internal state is updated, and an initial menu is sent to the user.
- Clicking a menu button sends a callback query, which the bot processes to perform the relevant action (e.g., adding an item to the cart).

#### 2. Dynamic State Management
The functionality of the bot is managed through changeable states, simplifying the handling of complex interaction scenarios. Different states correspond to different parts of the user experience:
- The `StartState` handles the initial menu display when the `/start` command is issued.
- Another state manages the product catalog display.

### Responding to User Messages

The bot supports various response scenarios to user commands and callback queries. When entering a category state, the bot loads the product list and displays the appropriate menu based on incoming commands. For instance:
- Pressing "My Cart" lets the user see their cart contents with a message.

Responses to messages occur within the current state, ensuring cohesive user interaction.

# Using the State Pattern in a Chatbot

In this project, we utilize the State Pattern to manage various states of the user interface and logic for a Telegram chatbot. Each state, such as "Start," "Catalog," "Category," and "Checkout," is implemented as a separate class that inherits from the base class `TelegramState`. This approach allows for easy addition of new states and modification of message handling logic without altering the main bot class.

## States and Their Implementation

### TelegramState

`TelegramState` is a virtual class from which the following states are derived. It contains virtual methods for handling state transitions, menu processing through messages, and requests.

### StartState

`StartState` is the main menu state, which is active by default when first interacting with the bot. This state can direct the user to the product catalog, the shopping cart, and provide technical support information.

### CatalogState

`CatalogState` is the state for the catalog, active when the user is browsing the product catalog. It shows existing product categories and directs the user to the selected category. From this state, the user can return to `StartState` or proceed to `CategoryState`.

### CategoryState

`CategoryState` is the state for product categories, active when the user is within a product category. It displays products, provides detailed information, allows adding products to the cart, and sorts products. It checks for product availability, preventing users from adding out-of-stock items to the cart. From this state, the user can return to `CatalogState` or move to `CheckoutState`.

### CheckoutState

`CheckoutState` is the order processing state, activated when the user presses the "Checkout" button in the cart. It offers options to enter the delivery address manually or via a geo-tag, and add a comment for the delivery service. After entering these details, it sends a confirmation or cancellation message for the order. Upon completing the checkout, it transitions back to `StartState`.

## Working with PostgreSQL Database

### Database Structure for Storing Product Information

In the `products` table, data about the products that the bot will display to users is stored. Each field in the table contains relevant information:

| column_name | data_type          |
|-------------|--------------------|
| price       | integer            |
| specs       | jsonb              |
| delivery    | integer            |
| amount      | integer            |
| link        | character varying  |
| category    | character varying  |
| description | character varying  |
| name        | character varying  |

### Setting Up Connection to the Database Using libpqxx

We use the `libpqxx` library to interface with PostgreSQL and the `JsonCpp` library for parsing JSON objects.

### Database Queries: Create, Read, Update

#### Initializing Database Connection and Setup

```cpp
if (conn.is_open()) {
    std::cout << "DataBase started: DataBase connection ready\n";
    pqxx::work txn(conn);
```

Here, a connection to the database is established using the connection string `connectionString`. If the connection opens successfully, a `txn` object is created to execute queries within a single transaction.

#### Executing SQL Query

```cpp
std::string query = "SELECT name, category, price, amount, description, specs, delivery, link FROM products";
pqxx::result result = txn.exec(query);
```

The query selects columns `name`, `category`, `price`, `amount`, `description`, `specs`, `delivery`, and `link` from the `products` table. The query result is stored in the `result` object.

#### Processing Query Results

```cpp
for (const auto& row : result) {
    // Extract and process each row’s data
    // Parse JSON from the specs column
}
```

Each row of the result is processed in a `for` loop. Data from each column is extracted and converted to appropriate variables. The JSON from the `specs` column is then parsed. If parsing is successful, a string of product specifications is formed.

#### Handling Exceptions

```cpp
catch (const std::exception& e) {
    std::cerr << "DataBase error: " << e.what() << std::endl;
}
```

If any exceptions occur while working with the database, they are caught and printed to the standard error stream.

#### Returning a List of Products as `vector<Products>`

```cpp
return products;
```

This snippet indicates that the function returns a list of products contained in a `vector<Products>`.
