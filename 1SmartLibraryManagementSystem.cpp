#include <iostream>
#include <vector>
#include <memory>
using namespace std;

enum class MediaType{BOOK, DVD};

class Media{
    protected:
        const string title;

    public:    
        Media(string title): title(title){}

        // const void func() -> This says: "The value this function returns is constant." (Since it returns void, this does nothing).
        // void func() const -> This says: "This function is read-only for the object."

        virtual void displayInfo() const = 0;

        virtual ~Media(){
            cout << "Destructor called" << endl;
        }
};

class Book : public Media{ // If made protected/private, Won't be accessible to Library class
    private:
        const string author;

    public:
        Book(string title, string author) : Media(title), author(author){}

        void displayInfo() const override{
            cout << "The author of book " << title << " is " << author << endl;
        }
};

class Dvd : public Media{
    private:
        const int duration;

    public:
        Dvd(string title, int duration) : Media(title), duration(duration){}

        void displayInfo() const override{
            cout << "The duration of dvd " << title << " is " << duration << endl;
        }
};

class Library{
    vector<unique_ptr<Media>> mediaList;
    
    public:
        Library() = default;

        void addMedia(std::unique_ptr<Media> m){
            mediaList.emplace_back(move(m)); // use emplace_back instead of push_back
            // keep note of how the move operator is used
        }

        void const showAll(void){
            for(const auto& media : mediaList){
                media -> displayInfo();
            }
        }
};


int main() {
    Library myLibrary;

    auto b1 = make_unique<Book>("C++ with Gemini", "Singla");
    // Same as unique_ptr<Book> b1 = make_unique<Book>("C++ with Gemini", "Singla")
    auto d1 = make_unique<Dvd>("C++ with ChatGpt", 120);
    // Same as unique_ptr<Media> b1 = make_unique<Dvd>("C++ with ChatGpt", 120) -> preffered one

    myLibrary.addMedia(move(b1));// keep note of how the move operator is used, same to be used in constructor
    myLibrary.addMedia(move(d1));
}