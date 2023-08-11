#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <utility>
#include "WebPage.h"
using namespace std;

class WebPageQuery {
public: 
WebPageQuery();
string doQuery(const string & str);
    
private:
vector<double> getQueryWordsWeightVector(vector<string> & queryWords);
bool executeQuery(const vector<string> & queryWords, 
                  vector<pair<int,vector<double>>> & resultVec);
string createJson(vector<int> & docIdVec, const vector<string> & queryWords);
string returnNoAnswer();

private: 
    WordSegmentation _jieba;
    unordered_map<int, WebPage> _pageLib;
    unordered_map<int, pair<int, int>> _offsetLib;
    unordered_map<int, set<pair<int, double>>> _invertIndexTable;
};
#endif //_WEBPAGEQUERY_H
