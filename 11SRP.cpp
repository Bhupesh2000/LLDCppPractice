// "A class should have one, and only one, reason to change."

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

class Trade{
    const double price;
    const int quantity;
    const string symbol;

public:
    Trade(double price, int qty, string symbol) : price(price),  quantity(qty), symbol(symbol){};   

    double getPrice() const {return price;}
    int getQuantity() const {return quantity;}
    string getSymbol() const {return symbol;}

    ~Trade() = default;
};

class ITradeFormatter{
public:
    virtual string format(const Trade& trade) const = 0;
    virtual ~ITradeFormatter() = default;
};

class JsonFormatter : public ITradeFormatter{
public:
    string format(const Trade& trade){
        string s = "Formatted to Json";
        return s;
    }
};

class XMLFormatter : public ITradeFormatter{
public:
    string format(const Trade& trade){
        string s = "Formatted to XML";
        return s;
    }
};

class ITradeSave{
public:
    virtual void save(const Trade& trade) const = 0;
    virtual ~ITradeSave() = default;
};

class SaveToFile : public ITradeSave{
public:
    void save(const Trade& trade){
        cout << "Saved to file" << endl;
    }
};

class SaveToDB : public ITradeSave{
public:
    void save(const Trade& trade){
        cout << "Saved to db" << endl;
    }
};

class TradeLogger{
    vector<unique_ptr<ITradeFormatter>> formatters;
    vector<unique_ptr<ITradeSave>> savers;
public:
    TradeLogger() = default;

    void addFormatter(unique_ptr<ITradeFormatter> f){
        formatters.push_back(move(f));
    }

    void addSaver(unique_ptr<ITradeSave> s){
        savers.push_back(move(s));
    }

    void Log(const Trade& t){
        for(auto& f : formatters){
            string output = f -> format(t);
            for(auto& s : savers){
                s -> save(t);
            }
        }
    }
};

int main() {

    

    return 0;
}