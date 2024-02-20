#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"
#include "CandlestickPlotter.h"
#include "Utility.h"
#include "MovingAveragePlotter.h"
#include <iomanip>

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}


void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}


void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }

    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }  
    // }    
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;

    //show the candlesticks
    std::string product;
    std::string orderTypeStr;
    OrderBookType orderType;

    // Prompt user for product and order type
    std::cout << "Enter product (e.g., ETH/BTC) and type (bid/ask), separated by a comma: ";
    std::getline(std::cin, product, ',');
    std::getline(std::cin, orderTypeStr);

    // Convert order type string to OrderBookType
    if (orderTypeStr == "bid") 
    {
        orderType = OrderBookType::bid;
    } 
    else if (orderTypeStr == "ask") 
    {
        orderType = OrderBookType::ask;
    } 
    else 
    {
        std::cout << "Invalid order type entered." << std::endl;
        return;
    }

    // Compute moving averages for the past 10 minutes
    std::vector<double> movingAverages = orderBook.computeMovingAverages(product, orderType, currentTime.substr(0, 16));

    // Determine the number of moving averages to display (up to 10)
    int numMovingAveragesToShow = std::min(static_cast<int>(movingAverages.size()), 10);

    std::vector<double> lastMovingAverages(movingAverages.end() - numMovingAveragesToShow, movingAverages.end()); // Get the last 10 moving averages
    std::vector<std::string> timestamps; // Vector to store the corresponding timestamps

    // Collect timestamps for the last 10 moving averages
    for (int i = numMovingAveragesToShow - 1; i >= 0; --i) {
        timestamps.push_back(subtractMinutes(currentTime, i));
    }

    // Plot the moving averages using the MovingAveragePlotter
    if (!lastMovingAverages.empty()) {
        //Print the title for the Moving Averages plot
        std::cout << "\n\n";
        std::cout << std::string(108, '-') << std::endl;
        std::string title = "Moving Average for " + orderTypeStr + " on " + product + " of the Last 10 Mins";
        int padding = (108 - title.length()) / 2;  // Padding on each side
        std::cout << std::setw(padding + title.length()) << title << std::endl;

        //Plotting Function
        plotMovingAverages(lastMovingAverages, timestamps);
        std::cout << "\n\n";
    } else {
        std::cout << "No moving average data available for " << product << std::endl;
    }

    // Compute candlestick data for all timestamps up to the current time
    auto candlesticks = orderBook.computeCandlesticks(product, orderType, currentTime);

    // Determine the number of candlesticks to display (up to 5)
    int numCandlesticksToShow = std::min(static_cast<int>(candlesticks.size()), 5);

    std::vector<Candlestick> lastCandlesticks;  // Vector to store the last five candlesticks

    if (!candlesticks.empty()) {
        for (int i = numCandlesticksToShow - 1; i >= 0; --i) {
            const Candlestick& candle = candlesticks[candlesticks.size() - 1 - i];
            lastCandlesticks.push_back(candle);  // Collect the last five candlesticks for plotting
        }

        //Print the title for the Candlestick plot
        std::cout << std::string(65, '-') << std::endl;
        std::string title = "Candlesticks for " + orderTypeStr + " on " + product + " of the Last 5 Timestamps";
        int padding = (65 - title.length()) / 2;  // Padding on each side
        std::cout << std::setw(padding + title.length()) << title << std::endl;

        //plot the candlesticks
        plotCandlesticks(lastCandlesticks);

    } else {
        std::cout << "No candlestick data available for " << product << std::endl;
    }

}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }       
}
