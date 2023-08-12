#include "WebPageQuery.h"
#include "WebPage.h"

/**
 * WebPageQuery implementation
 */


WebPageQuery::WebPageQuery(MYSQL * db) 
: _db(db)
, _jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
{
    // _pageLib的读取
    // _invertIndexTable的读取
    
    
}

WebPageQuery::~WebPageQuery()
{
    cout << "~WebPageQuery()" << endl;
}

void WebPageQuery::readPageLib(string docId)
{
    // 将WebPage整个表读取进来
    char * sql = "select docId, url, title, description from webPage";
    int utf8;
    utf8=mysql_query(_db,"set character_set_results=utf8");
    assert(utf8==0);
    int qret = mysql_query(_db, sql);
    if(qret != 0)
    {
        cerr << "Error:" << mysql_error(_db) << endl;
        exit(1);
    }

    MYSQL_RES * result = mysql_store_result(_db);
    MYSQL_ROW row;
    mysql_free_result(result);
    while((row = mysql_fetch_row(result)) != NULL)
    {
        
    }
}

void WebPageQuery::readInvertIndexTable(string word)
{
    // 将invertedList整个表读取进来
    char sql[1024] = {0};
    sprintf(sql, "select wId, word, docId, frequency, weight from invertedList where word = '%s'", word.c_str());
    cout << sql << endl;
    // char * sql = "select wId, word, docId, frequency, weight from invertedList";
    int utf8;
    utf8=mysql_query(_db,"set character_set_results=utf8"); // 否则中文乱码
    assert(utf8==0);
    int qret = mysql_query(_db, sql);
    if(qret != 0)
    {
        cerr << "Error:" << mysql_error(_db) << endl;
        exit(1);
    }

    // 倒排索引库存储格式:
    // unordered_map<word, vector<wId, word, docId, frequency, weight>>
    MYSQL_RES * result = mysql_store_result(_db);
    MYSQL_ROW row;

    // 读取并存储每一行为倒排索引的格式
    while((row = mysql_fetch_row(result)) != NULL)
    {
    cout <<"查询好结果了, 进入fetch_row循环" << endl; 
        unordered_map<string,vector<vector<string>>>::iterator vec 
            = _invertIndexTable.find(row[1]);

        if(vec != _invertIndexTable.end())
        {
            cout << "存在插入" << endl;
            // word已经存在, 插入
            vector<string> tempVec;  
            tempVec.push_back(row[0]);
            tempVec.push_back(row[1]);
            tempVec.push_back(row[2]);
            tempVec.push_back(row[3]);
            tempVec.push_back(row[4]);
            vec->second.push_back(std::move(tempVec));
        }
        else
        {
            cout << "不存在插入" << endl;
            // word不存在, 创建并插入
            vector<string> tempVec;  
            tempVec.push_back(row[0]);
            tempVec.push_back(row[1]);
            tempVec.push_back(row[2]);
            tempVec.push_back(row[3]);
            tempVec.push_back(row[4]);
            vector<vector<string>> tempVec2;
            tempVec2.push_back(std::move(tempVec));
            _invertIndexTable.insert({row[1], std::move(tempVec2)});
        }
    }
    cout << "循环结束" << endl;

    // 释放res
    mysql_free_result(result);
}

/**
 * @param str
 * @return string
 */
string WebPageQuery::doQuery(const string & str) 
{
    // 1. 分词: 得到N个关键词(ok)
    // 2. 计算每文章N个关键词的权重系数w', 并组成其特征向量
    // 2.1 根据关键词查询倒排索引库, 获取N个resultVec, 
    //     存储为map<string, map<...>> resVecs
    // 2.2 获取每个关键字的(N个)章频, 存储为map<string, ull> df;
    // 2.3 获取每关键字每文章的词频, 存储为map<string, pair<int, ull>>
    // 2.3 docId取交集, 得到一个set<int>
    // 2.4 计算每(交集)文章每关键字, 计算w'.
    //     所有文章各自的特征向量存储为map<int, vec<double>>
    // 3.1 计算基准向量Base, 存储为vec<double> 
    // 3.2 计算余弦相似度, 存储为map<int, double>
    // 3.3 加入到优先级队列
    // 4. 取队列中前10个, 读取为vec<int> pre_output
    // 5. 遍历pre_output, 根据docId查询_pageLib, 获取对应WebPage
    //    存储为vec<WebPage>
    // 6. 将所有WebPage转化为JSON格式
    //    存储为string output;
    // 7. 返回output;
    return "";
}

// 功能：对输入的查询语句, 进行分词
// 输入: 查询语句
// 输出: 分词后的结果
vector<string> WebPageQuery::splitStrToWords(string & str)
{
    vector<string> words;
    char * saveptr;
    char pendingWords[1024] = {0}; 
    vector<string> res;

    // 1. 使用jieba进行分词
    _jieba.CutForSearch(str, words);
    string splitWords = limonp::Join(words.begin(), words.end(), "/");

    // 2. 将结果进行切分, 并存储到vector<string> res中
    strcpy(pendingWords, splitWords.c_str());
    const char * token = strtok_r(pendingWords, "/", &saveptr);
    res.push_back(token);
    while(token != NULL)
    {
        token = strtok_r(NULL, "/", &saveptr);
        if(token)
        {
            res.push_back(token);
        }
    }

    // 3. 返回结果
    return res;
}

bool WebPageQuery::executeQuery(const vector<string> & queryWords, 
                                vector<pair<int,vector<double>>> &resultVec) 
{

}

/**
 * @param queryWords
 * @return vector<double>
 */
    vector<double> 
WebPageQuery::getQueryWordsWeightVector(vector<string> & queryWords) 
{

}

/**
 * @param docIdVec
 * @param queryWords
 * @return string
 */
string WebPageQuery::createJson(vector<int> & docIdVec, 
                                const vector<string> & queryWords) 
{
    return "";
}

/**
 * @return string
 */
string WebPageQuery::returnNoAnswer() 
{
    return "";
}
