#include <func.hh>
#include "Configuration.h"
using namespace std;

// 配置文件的测试ok
void test0(const string path)
{
    Configuration * pConfig = Configuration::getInstance(path);
    cout << pConfig->port() << endl;
    cout << pConfig->threadNum() << endl;
    cout << pConfig->host() << endl;
    cout << pConfig->user() << endl;
    cout << pConfig->password() << endl;
    cout << pConfig->database() << endl;
    cout << pConfig->ip() << endl;
    set<string> eng = pConfig->getStopWordEngSet();
    set<string> zh = pConfig->getStopWordZhSet();
    for(const auto & str : eng)
    {
        cout << str << endl;
    }
    cout << "黄金分割线" << endl;
    for(const auto & str : zh)
    {
        cout << str << endl;
    }
}
int main(int argc, char* argv[])
{
    return 0;
}

