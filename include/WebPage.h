#ifndef _WEBPAGE_H
#define _WEBPAGE_H
#include <string>
#include <vector>
#include <map>
using namespace std;

class WebPage {
friend void test5();
friend void test6();
public: 
WebPage(int docid, 
        const string & docUrl,
        const string & docTitle, 
        const string & docDescription,
        const string & docContent
        );
int getDocId() const;
string summary(const vector<string> queryWords);
map<string, int> & getWordsMap(); // 返回_wordsMap
void printWebPage() const 
{
    printf("docid = %d\n"
           "url = %s\n"
           "title = %s\n"
           "description = %s\n"
           "___________________________________________________\n", _docId, _docUrl.c_str(), _docTitle.c_str(), _docDescription.c_str());
}

private:
void clacWordsMap();


private: 
    int _docId;
    string _docUrl;
    string _docTitle;
    string _docDescription;
    string _docContent;
};

#endif //_WEBPAGE_H
