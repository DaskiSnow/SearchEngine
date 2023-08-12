#include <func.hh>
/* #include "Configuration.h" */
#include "WebPageQuery.h"
#include <mysql/mysql.h>
using namespace std;

// 配置文件的测试ok
//void test0(const string path)
//{
//    Configuration * pConfig = Configuration::getInstance(path);
//    cout << pConfig->port() << endl;
//    cout << pConfig->threadNum() << endl;
//    cout << pConfig->host() << endl;
//    cout << pConfig->user() << endl;
//    cout << pConfig->password() << endl;
//    cout << pConfig->database() << endl;
//    cout << pConfig->ip() << endl;
//    set<string> eng = pConfig->getStopWordEngSet();
//    set<string> zh = pConfig->getStopWordZhSet();
//    for(const auto & str : eng)
//    {
//        cout << str << endl;
//    }
//    cout << "黄金分割线" << endl;
//    for(const auto & str : zh)
//    {
//        cout << str << endl;
//    }
//}

// WebPageQuery的分词测试
//void test1()
//{
//    MYSQL * db = mysql_init(NULL);
//    vector<string> res;
//    WebPageQuery wpq(db);
//    string queryWords = "初雪是个大笨蛋";
//    res = wpq.splitStrToWords(queryWords);
//    for(const auto & word : res)
//    {
//        cout << word << endl;
//    }
//}

// WebPageQuery的数据库查询测试
/* unordered_map<string, vector<vector<string>>> test2() */
//void test2()
//{
//    // 初始化MYSQ句柄
//    MYSQL * db = mysql_init(NULL);
//    char * host = "localhost";
//    char * user = "root";
//    char * password = "1688";
//    char * database = "webData";
//    MYSQL * ret = mysql_real_connect(db, host, user, password, database, 0, NULL, 0);
//    if(ret == NULL)
//    {
//        cerr << "Error:" << mysql_error(db) << endl;
//        exit(1);
//    }
//
//    // 将invertedList整个表读取进来
//    char * sql = "select wId, word, docId, frequency, weight from invertedList";
//    int utf8;
//    utf8=mysql_query(db,"set character_set_results=utf8");
//    assert(utf8==0);
//    int qret = mysql_query(db, sql);
//    if(qret != 0)
//    {
//        cerr << "Error:" << mysql_error(db) << endl;
//        exit(1);
//    }
//
//    // 倒排索引库:unordered_map<word, vector<wId, word, docId, frequency, weight>>
//    unordered_map<string, vector<vector<string>>> _invertIndexTable;  
//    MYSQL_RES * result = mysql_store_result(db);
//    MYSQL_ROW row;
//    int countNum = 0;
//
//    // 读取并存储每一行为倒排索引的格式
//    while((row = mysql_fetch_row(result)) != NULL)
//    {
//        unordered_map<string,vector<vector<string>>>::iterator vec 
//            = _invertIndexTable.find(row[1]);
//
//        if(vec != _invertIndexTable.end())
//        {
//            // word已经存在, 插入
//            vector<string> tempVec;  
//            tempVec.push_back(row[0]);
//            tempVec.push_back(row[1]);
//            tempVec.push_back(row[2]);
//            tempVec.push_back(row[3]);
//            tempVec.push_back(row[4]);
//            vec->second.push_back(std::move(tempVec));
//        }
//        else
//        {
//            // word不存在, 创建并插入
//            vector<string> tempVec;  
//            tempVec.push_back(row[0]);
//            tempVec.push_back(row[1]);
//            tempVec.push_back(row[2]);
//            tempVec.push_back(row[3]);
//            tempVec.push_back(row[4]);
//            vector<vector<string>> tempVec2;
//            tempVec2.push_back(std::move(tempVec));
//            _invertIndexTable.insert({row[1], std::move(tempVec2)});
//        }
//    }
//
//    // 释放res
//    mysql_free_result(result);
//
//    // 关闭连接
//    mysql_close(db);
//
//    // 读取测试
//    //auto tempVec = _invertIndexTable.find("新民");
//    //if(tempVec == _invertIndexTable.end())
//    //{
//    //    cout << "找不到" << endl;
//    //}
//    //else
//    //{
//    //    for(auto vec1 : tempVec->second)
//    //    {
//    //        for(auto item : vec1)
//    //        {
//    //            cout << item << endl;
//    //        }
//    //        cout << "同word的下一条" << endl;
//    //    }
//    //    cout << "找到了" << endl;
//    //}
//
//    // 移动语义返回
//    return ;
//}

// 成员函数readInvertIndexTable的测试(ok)
void test3()
{
    // 初始化MYSQ句柄
    MYSQL * db = mysql_init(NULL);
    char * host = "localhost";
    char * user = "root";
    char * password = "1688";
    char * database = "webData";
    MYSQL * ret = mysql_real_connect(db, host, user, password, database, 0, NULL, 0);
    if(ret == NULL)
    {
        cerr << "Error:" << mysql_error(db) << endl;
        exit(1);
    }


    WebPageQuery wpq(db);    
    wpq.readInvertIndexTable("18");
    auto tempVec = wpq._invertIndexTable.find("11");

    if(tempVec == wpq._invertIndexTable.end())
    {
        cout << "找不到" << endl;
    }
    else
    {
        int num = 0;
        for(auto vec1 : tempVec->second)
        {
            for(auto item : vec1)
            {
                cout << item << endl;
            }
            cout << "_________________" << endl;
            num++;
        }
        cout << num << endl;
    }
}

void test4()
{
    // 初始化MYSQ句柄
    MYSQL * db = mysql_init(NULL);
    char * host = "localhost";
    char * user = "root";
    char * password = "1688";
    char * database = "webData";
    MYSQL * ret = mysql_real_connect(db, host, user, password, database, 0, NULL, 0);
    if(ret == NULL)
    {
        cerr << "Error:" << mysql_error(db) << endl;
        exit(1);
    }
    mysql_query(db,"set names 'utf8'");
    mysql_query(db,"select * from invertedList where word = '龟'");
    MYSQL_RES * result = mysql_store_result(db);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(result)) != NULL)
    {
        cout << "有东西"<< endl;
    }
    cout << "结束循环" << endl;
}

int main(int argc, char* argv[])
{
    test4();
    return 0;
}

