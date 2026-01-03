#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

enum class TransactionType{CREDIT, DEBIT};

class Wallet;

class User{
    const int UserId;
    const string UserName;
    shared_ptr<Wallet> UserWallet; // shared ownership because will need in transaction class as weak_ptr

public:
    //User(int UserId, string UserName) : UserId(UserId), UserName(UserName)
    //passing string UserName by value, then copying it into the member. To avoid an extra copy, use UserName(std::move(UserName))
    User(int UserId, string UserName) : UserId(UserId), UserName(std::move(UserName)){ // std used to avoid confusion
        UserWallet = make_shared<Wallet>(this); // shared because will need weak ptr to transaction
        // also since User is the one creating wallet(it will be 1 to 1)
    }

    void getUserDetails() const{
        cout << "User " << UserId << " is " << UserName << endl;
    }

    // Declaration only because these use 'Wallet' methods
    void getUserBalance() const;

    shared_ptr<Wallet> getWallet() const{
        return UserWallet;
    }

    virtual ~User(){ // Virtual because maybe converted to base class in future if different type of user introduced
        cout << "User " << UserId << " deleted" << endl;
    }
};

class Wallet{
    int WalletAmount;
    // const weak_ptr<User> WalletUser; // this will cause many issue.
    // Wallet cannot exist without User
    // User lifetime strictly outlives Wallet
    // You control object creation order
    User* Owner;
    // Also could have simply passed UserId, simpler option

    void creditAmount(int amount){
        WalletAmount += amount;
        cout << "Credited " << amount << ", Final Balance : " << WalletAmount << endl;
    }

    void debitAmount(int amount){
        if(amount > WalletAmount){
            cout << "Insufficient balance" << endl;
            return;
        }
        WalletAmount -= amount;
        cout << "Debited " << amount << ", Final Balance : " << WalletAmount << endl;
    }

public:
    explicit Wallet(User* WalletOwner) : WalletAmount(0), Owner(WalletOwner){};

    int getWalletAmount() const{
       return WalletAmount;
    }

    void execute(TransactionType type, int amount){
        if(type == TransactionType :: CREDIT) creditAmount(amount);
        else if(type == TransactionType :: DEBIT) debitAmount(amount);
        else cout << "Invalid transaction attempt" << endl;
    }

    ~Wallet(){
        cout << "Wallet deleted" << endl;
    }
};

class Transaction{
    const TransactionType type;
    const int amount;
    weak_ptr<Wallet> W;

public:
    Transaction(TransactionType type, int amount, shared_ptr<Wallet> w) : type(type), amount(amount), W(w){
        cout << "Transaction initialised" << endl;
        // W -> execute(amount); // ❌ WRONG
        // weak_ptr does not own the object
        // You cannot dereference it directly
        // weak_ptr may already be expired
        // This will not compile
        if(auto wallet = W.lock()){ 
            wallet -> execute(type, amount);
        }
        else cout << "Wallet doesnot exist" << endl;
    };

    bool isExpired(){
        if(auto wallet = W.lock()) return false;
        else return true;
    }

    ~Transaction(){
        cout << "Transaction deleted" << endl;
    }
};

void User :: getUserBalance() const{
    cout << UserName << " has " << UserWallet -> getWalletAmount() << " in his wallet" << endl;
}

void addTransactionInGlobalLog(vector<shared_ptr<Transaction>>& globalTransactions, shared_ptr<Transaction> t){
    globalTransactions.emplace_back(t);
}

void checkForExpiredTransactions(const vector<shared_ptr<Transaction>>& globalTransactions){
    for(int i = 0; i < globalTransactions.size(); i++){
        if(globalTransactions[i] -> isExpired()) cout << i <<"th transaction expired" << endl;
    }
}

int main() {
    vector<shared_ptr<Transaction>> globalTransactions;

    {
        shared_ptr<User> alice = make_shared<User>(1, "alice");
        shared_ptr<Wallet> walletAlice = alice -> getWallet();

        shared_ptr<Transaction> t1 = make_shared<Transaction>(TransactionType::CREDIT, 10, walletAlice);
        addTransactionInGlobalLog(globalTransactions, t1);
        shared_ptr<Transaction> t2 = make_shared<Transaction>(TransactionType::DEBIT, 30, walletAlice);
        addTransactionInGlobalLog(globalTransactions, t2);
        shared_ptr<Transaction> t3 = make_shared<Transaction>(TransactionType::CREDIT, 30, walletAlice);
        addTransactionInGlobalLog(globalTransactions, t3);
        shared_ptr<Transaction> t4 = make_shared<Transaction>(TransactionType::DEBIT, 30, walletAlice);
        addTransactionInGlobalLog(globalTransactions, t4);

        alice.reset();
        // done alice in a scope because walletAlice will still exist
        // if not doing in scope do walletAlice.reset()
    }

    shared_ptr<User> jenna = make_shared<User>(2, "jenna");
    shared_ptr<Wallet> walletJenna = jenna -> getWallet();

    shared_ptr<Transaction> t5 = make_shared<Transaction>(TransactionType::CREDIT, 10, walletJenna);
    addTransactionInGlobalLog(globalTransactions, t5);
    shared_ptr<Transaction> t6 = make_shared<Transaction>(TransactionType::DEBIT, 30, walletJenna);
    addTransactionInGlobalLog(globalTransactions, t6);
    shared_ptr<Transaction> t7 = make_shared<Transaction>(TransactionType::CREDIT, 30, walletJenna);
    addTransactionInGlobalLog(globalTransactions, t7);
    shared_ptr<Transaction> t8 = make_shared<Transaction>(TransactionType::DEBIT, 30, walletJenna);
    addTransactionInGlobalLog(globalTransactions, t8);

    checkForExpiredTransactions(globalTransactions);
}