#ifndef _WEBPAGE_H
#define _WEBPAGE_H
#include <string>
#include <vector>
#include <map>
using namespace std;

class WebPage {
public: 
WebPage(int docid, 
        const string & docUrl,
        const string & docTitle, 
        const string & docDescription,
        const string & docContent
        );
int getDocId() const;
string getDocUrl() const;
string getDocTitle() const;
string getDocDescription() const;
string getDocContent() const;
string getDes(const string &s1, const string &s2);
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
