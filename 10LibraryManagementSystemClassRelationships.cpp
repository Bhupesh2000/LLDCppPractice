#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

class Page{
    string content;
public:
    Page() = default;
    void setContent(string content){
        this -> content = content;
    }
    ~Page() = default;
};

class Book{ // Can be made abstract class in future based on different types
    const string title;
    vector<unique_ptr<Page>> Pages; // Composition
public:
    explicit Book(string title) : title(std::move(title)){};

    void addPage(unique_ptr<Page> p){
        Pages.emplace_back(std::move(p));
    }

    virtual ~Book() = default;
};

class Library{
    const string Name;
    vector<shared_ptr<Book>> Books; // Aggregation
public:
    explicit Library(string Name) : Name(move(Name)){};

    void addBook(shared_ptr<Book> b){
        Books.emplace_back(b);
    }

    ~Library() = default;
};

int main() {

    

    return 0;
}