#include "WebPage.h"

/**
 * @param doc
 * @param config
 * @param jieba
 */
WebPage::WebPage(int docid, 
                 const string & docUrl,
                 const string & docTitle, 
                 const string & docDescription,
                 const string & docContent)
: _docId(docid)
, _docUrl(docUrl)
, _docTitle(docTitle)
, _docDescription(docDescription)
, _docContent(docContent)
{}

/**
 * @return int
 */
int WebPage::getDocId() const {
    return _docId;
}

/**
 * @param queryWords
 * @return string
 */
string WebPage::summary(const vector<string> queryWords) {
    return "";
}
