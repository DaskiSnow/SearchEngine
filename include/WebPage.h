#ifndef _WEBPAGE_H
#define _WEBPAGE_H
#include <string>
#include <vector>
#include <map>
using namespace std;

class WebPage {
public: 
WebPage(int docid, 
        const string & docTitle, 
        const string & docContent,
        const string & docSummary
        );
int getDocid();
string summary(const vector<string> queryWords);
map<string, int> & getWordsMap(); // 返回_wordsMap

private:
void clacWordsMap();


private: 
    int _docid;
    string _docTitle;
    string _docContent;
    string _docSummary;
};

#endif //_WEBPAGE_H
