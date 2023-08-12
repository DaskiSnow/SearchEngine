#include "WebPageQuery.h"

/**
 * WebPageQuery implementation
 */


WebPageQuery::WebPageQuery(MYSQL * db) 
: _db(db)
, _jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
{}

/**
 * @param str
 * @return string
 */
string WebPageQuery::doQuery(const string & str) 
{
    // 1. 分词: 得到N个关键词
    // 2. 计算每文章N个关键词的权重系数w', 并组成其特征向量
    // 2.1 根据关键词查询倒排索引库, 获取N个resultVec, 
    //     存储为map<string, vec<...>> resVecs
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
