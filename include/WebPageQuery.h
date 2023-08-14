#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H
#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include <queue>
#include <functional>
#include <utility>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <mysql/mysql.h>
#include "WebPage.h"
#include "cppjieba/Jieba.hpp"
using namespace std;
using WordSegmentation = cppjieba::Jieba;

const char* const DICT_PATH = "/home/daskisnow/SearchEngine/data/dict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/daskisnow/SearchEngine/data/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/daskisnow/SearchEngine/data/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/daskisnow/SearchEngine/data/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/daskisnow/SearchEngine/data/dict/stop_words.utf8";

class WebPageQuery {
    friend void test3();
    friend void test5();
    friend void test6();
public: 
    WebPageQuery(MYSQL * db, WordSegmentation & jieba);
    ~WebPageQuery();
    string doQuery(const string & str);

public:
    vector<double> getQueryWordsWeightVector(vector<string> & queryWords);
    bool executeQuery(const vector<string> & queryWords, 
                      vector<pair<int,vector<double>>> & resultVec);
    string createJson(vector<int> & docIdVec, 
                      const vector<string> & queryWords);
    string returnNoAnswer();
    vector<string> splitStrToWords(const string & str);
    void addInvertIndex(string word);
    void addPage(string docId);
    vector<double> getDocVec(const string & docId, 
                             const vector<string> & words);
    set<string> getInnerDocId();
    vector<double> getBaseVec(const vector<string> & words);
    unsigned long long getDocNum();
    bool cmpDocVec(const vector<double> & baseVec, 
                    const vector<double> & vecA, 
                    const vector<double> & vecB);
    double calcCosSim(const vector<double> & vecA, 
                      const vector<double> & vecB);
    double calcDotProduct(const vector<double> & vecA, 
                          const vector<double> & vecB); // 计算向量的内积
    double calcMagnitude(const vector<double> & vec);  // 计算向量的模
    bool cmpWebPage(const map<string, vector<double>> & docVecs, 
                    const vector<double> & baseVec,
                    const WebPage & pageA, 
                    const WebPage & pageB);
private: 
    MYSQL * _db;
    WordSegmentation & _jieba;
    unsigned long long _docNum;
    vector<WebPage> _pages; // 需要转成json返回的网页集合
    unordered_map<string, WebPage> _pageLib; // 存储所需要的WebPage(表示docId的string)
    unordered_map<int, pair<int, int>> _offsetLib;  
    unordered_map<string, vector<vector<string>>> _invertIndexTable; // 存储所需要的倒排索引表记录
};
#endif //_WEBPAGEQUERY_H
