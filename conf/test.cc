#include <func.hh>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

void test()
{
    // create an empty structure (null)
json j;

// add a number that is stored as double (note the implicit conversion of j to an object)
j["pi"] = 3.141;

// add a Boolean that is stored as bool
j["happy"] = true;

// add a string that is stored as std::string
j["name"] = "Niels";

// add another null object by passing nullptr
j["nothing"] = nullptr;

// add an object inside the object
j["answer"]["everything"] = 42;

// add an array that is stored as std::vector (using an initializer list)
j["list"] = { 1, 0, 2 };

// add another object (using an initializer list of pairs)
j["object"] = { {"currency", "USD"}, {"value", 42.99} };

cout << j.dump() << endl;
ofstream ofs("test.json");
ofs << j.dump() << endl;
ofs.close();
}

void test2()
{
    ifstream ifs("config.json");
    json j; 
    ifs >> j;
    cout << j["mysql"]["host"] << endl; 
}

int main()
{
    test2();
    return 0;
}
