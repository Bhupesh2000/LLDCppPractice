// The Scenario:
// You are building a system for a multinational corporation that generates legal documents. The system must support two regions: Japan and UK.

// Requirements:
// Families of Objects (Abstract Factory):
//  Each region has a different Header and Footer.
//  UK: UKHeader, UKFooter (Format: "Date: DD/MM/YYYY").
//  Japan: JapanHeader, JapanFooter (Format: "Date: YYYY/MM/DD").
// Global Settings (Singleton):
//  Create a DocumentSettings Singleton that stores the companyName and the currentRegion.
// Dependency Injection:
//  Create a DocumentGenerator class. It should not know which region it is working for.
//  It should be injected with a DocumentFactory via its constructor.
// The Process:
//  The DocumentGenerator has a method void generate() that creates a header, creates a footer, and prints them to the console along with the company name from the Singleton.


#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>


class IHeader{public: virtual std::string header() = 0; virtual ~IHeader() = default;};
class IFooter{public: virtual std::string footer() = 0; virtual ~IFooter() = default;};

class UKHeader : public IHeader{
public:
    std::string header() override{
        return "UK header";
    }
};
class JapanHeader : public IHeader{
public:
    std::string header() override{
        return "Japan header";
    }
};
class UKFooter : public IFooter{
public:
    std::string footer() override{
        return "UK footer";
    }
};
class JapanFooter : public IFooter{
public:
    std::string footer() override{
        return "Japan footer";
    }
};

class Template{
public:
    virtual std::unique_ptr<IHeader> createHeader() = 0;
    virtual std::unique_ptr<IFooter> createFooter() = 0;

    virtual ~Template() = default; 
};

class UKTemplate : public Template{
public:
    std::unique_ptr<IHeader> createHeader() override{
        return std::make_unique<UKHeader>();
    }
    std::unique_ptr<IFooter> createFooter() override{
        return std::make_unique<UKFooter>();
    }
};

class JapanTemplate : public Template{
public:
    std::unique_ptr<IHeader> createHeader() override{
        return std::make_unique<JapanHeader>();
    }
    std::unique_ptr<IFooter> createFooter() override{
        return std::make_unique<JapanFooter>();
    }
};


class DocumentSettings{
    std::unordered_map<std::string, std::string> companyDetails;

    DocumentSettings(){
        std::cout << "Loading documentation settings\n";
    }
public:
    DocumentSettings(const DocumentSettings&) = delete;
    void operator=(const DocumentSettings&) = delete;

    static DocumentSettings& getInstance(){
        static DocumentSettings instance;
        return instance;
    }

    void addCompanyDetails(const std::string& CompanyName, const std::string& Region){
        companyDetails[CompanyName] = Region;
    }

    std::string getCompanyDetails(const std::string& CompanyName){
        if(companyDetails.count(CompanyName) != 0) return companyDetails[CompanyName];
        else{
            std::cout << "Invalid company\n";
            return ""; 
        }
    }
};

class DocumentGenerator{
    const std::string companyName;
    std::shared_ptr<Template> document;

public:
    DocumentGenerator(const std::string companyName, std::shared_ptr<Template> doc) : companyName(companyName), document(doc){}

    void generate(){
        DocumentSettings& docSetting = DocumentSettings::getInstance();
        std::cout << companyName << " - " << docSetting.getCompanyDetails(companyName) <<":";

        auto docHeader = document -> createHeader();
        auto docFooter = document -> createFooter();

        std::string headerStr = docHeader -> header();
        std::string footerStr = docFooter -> footer();

        std::cout << headerStr << ", " << footerStr << "\n";
    }
};

int main() {

    std::shared_ptr<Template> UKDoc = std::make_shared<UKTemplate>();

    std::unique_ptr<DocumentGenerator> generator1 = std::make_unique<DocumentGenerator>("Company 1", UKDoc);
    generator1 -> generate();

    return 0;
}