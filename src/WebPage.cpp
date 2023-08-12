#include "WebPage.h"

/**
 * @param doc
 * @param config
 * @param jieba
 */
WebPage::WebPage(int docid, 
                 const string & docTitle, 
                 const string & docContent,
                 const string & docSummary) 
: _docid(docid)
, _docTitle(docTitle)
, _docContent(docContent)
, _docSummary(docSummary)
{}

/**
 * @return int
 */
int WebPage::getDocid() {
    return 0;
}

/**
 * @param queryWords
 * @return string
 */
string WebPage::summary(const vector<string> queryWords) {
    return "";
}
