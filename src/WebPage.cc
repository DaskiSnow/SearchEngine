#include "WebPage.h"

WebPage::WebPage(int docid, 
                 const string & docUrl,
                 const string & docTitle, 
                 const string & docDescription,
                 const string & docContent)
: _docId(docid)
, _docUrl(docUrl)
, _docTitle(docTitle)
, _docContent(docContent)
{
    _docDescription = getDes(docDescription, _docContent);
}

int WebPage::getDocId() const 
{
    return _docId;
}

string WebPage::getDocUrl() const
{
    return _docUrl;
}

string WebPage::getDocTitle() const
{
    return _docTitle;
}

string WebPage::getDocDescription() const
{
    return _docDescription;
}

string WebPage::getDocContent() const
{
    return _docContent;
}

// 功能: 处理长摘要和空摘要
string WebPage::getDes(const string & s1, const string & s2) 
{
    string sumStr = s1 + s2;
    if(sumStr.size() > 0)
    {
        size_t idx =  sumStr.find("。");
        if(idx == string::npos)
        {
            return "";
        }
        return string(sumStr.substr(0, idx + 3));
    }
    return "";
}

