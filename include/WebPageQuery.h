#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <utility>
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
public: 
    WebPageQuery(MYSQL * db);
    ~WebPageQuery();
    string doQuery(const string & str);

public:
    vector<double> getQueryWordsWeightVector(vector<string> & queryWords);
    bool executeQuery(const vector<string> & queryWords, 
                      vector<pair<int,vector<double>>> & resultVec);
    string createJson(vector<int> & docIdVec, const vector<string> & queryWords);
    string returnNoAnswer();
    vector<string> splitStrToWords(string & str);
    void readInvertIndexTable(string word);
    void readPageLib();

private: 
    MYSQL * _db;
    WordSegmentation _jieba;
    unordered_map<int, WebPage> _pageLib;
    unordered_map<int, pair<int, int>> _offsetLib;
    unordered_map<string, vector<vector<string>>> _invertIndexTable;
};
#endif //_WEBPAGEQUERY_H
