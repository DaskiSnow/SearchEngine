#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <pthread.h>
#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <mutex>
#include <iostream>
#include <fstream>
using namespace std;

class Configuration
{
public:
    static Configuration * getInstance(const string & path);
    set<string> & getStopWordEngSet();
    set<string> & getStopWordZhSet();
    int port();
    int threadNum();
    string host();
    string user();
    string password();
    string database();
    string ip();
private:
    static void init(const string & path);
    static void destroy();
    static Configuration * _pInstance;
    static once_flag _flag;

private:
    Configuration(const string & path); // 命令行参数传入配置文件的路径
    ~Configuration(); 
    void readStopWordList();
private:
    // 从命令行输入获取_configFilePath
    // mysql的主机地址、用户名、密码、数据库名, 服务器的ip、port、线程数
    int _port;       // 端口号
    int _threadNum;  // 线程数
    string _host;    // mysql主机地址
    string _user;    // mysql用户名
    string _password;   // mysql用户密码
    string _database;   // mysql数据库名
    string _ip;         // ip地址
    string _configFilePath;         // 配置文件存放路径
    string _stopWordEngPath;        // 英文停用词文件路径
    string _stopWordZhPath;         // 中文停用词文件路径
    set<string> _stopWordEngSet;   // 英文停用词
    set<string> _stopWordZhSet;    // 中文停用词
};
#endif
