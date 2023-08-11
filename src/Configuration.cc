#include "Configuration.h"
#include "json.hpp"
Configuration * Configuration::getInstance(const string & path)
{
    call_once(_flag, &init, path);
    return _pInstance;
}



void Configuration::init(const string & path)
{
    if(_pInstance == nullptr)                                            
    {
        _pInstance = new Configuration(path);
        atexit(destroy);
    }
}

void Configuration::destroy()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

void Configuration::readStopWordList()
{
    // 读取英文停用词
    ifstream ifs(_stopWordEngPath);
    if(!ifs)
    {
        fprintf(stderr, "open \"\%s\"failed!\n", _stopWordEngPath.c_str());
        return ;
    }
    string word;
    while(ifs >> word)
    {
        _stopWordEngSet.insert(word);
    }
    ifs.close();

    // 读取中文停用词
    ifs.open(_stopWordZhPath);
    if(!ifs)
    {
        fprintf(stderr, "open \"\%s\"failed!\n", _stopWordZhPath.c_str());
        return ;
    }
    while(ifs >> word)
    {
        _stopWordZhSet.insert(word);
    }
    ifs.close();
}

Configuration::~Configuration()
{}

// 参数来自于命令行的输入
Configuration::Configuration(const string & path)
{
    // 配置文件路径
    _configFilePath = path;

    // 解析JSON配置文件
    ifstream configFile(path);
    if(!configFile)
    {
        cout << "打开配置文件失败" << endl;
        exit(1);
    }
    nlohmann::json data;
    configFile >> data;

    // 下面编译器可能会报错, 但能正常运行, 忽略它
    _host = data["mysql"]["host"];
    _user = data["mysql"]["user"];
    _password = data["mysql"]["password"];
    _database = data["mysql"]["database"];
    _ip = data["server"]["ip"];
    _port = data["server"]["port"];
    _threadNum = data["server"]["threadNum"];
    _stopWordEngPath = data["extra"]["stopWordEngPath"];
    _stopWordZhPath = data["extra"]["stopWordZhPath"];
    readStopWordList();    
    configFile.close();
}

set<string> & Configuration::getStopWordEngSet()
{
    return _stopWordEngSet;
}

set<string> & Configuration::getStopWordZhSet()
{
    return _stopWordZhSet;
}

int Configuration::port()
{
    return _port;
}

int Configuration::threadNum()
{
    return _threadNum;
}

string Configuration::host()
{
    return _host;
}

string Configuration::user()
{
    return _user;
}

string Configuration::password()
{
    return _password;
}

string Configuration::database()
{
    return _database;
}

string Configuration::ip()
{
    return _ip;
}

Configuration * Configuration::_pInstance = nullptr;                           
once_flag Configuration::_flag;
