#include "WebPageQuery.h"
#include "WebPage.h"

/**
 * WebPageQuery implementation
 */


WebPageQuery::WebPageQuery(MYSQL * db, WordSegmentation & jieba, mutex & m) 
: _db(db)
, _jieba(jieba)
, _mutex(m)
, _docNum(getDocNum())
{}

WebPageQuery::~WebPageQuery()
{
    /* cout << "~WebPageQuery()" << endl; */
}

// 功能: 获取文章总数 
unsigned long long WebPageQuery::getDocNum()
{
    unsigned long long docNum = 0;
    char sql[1024] = {0};
    sprintf(sql, "select count(*) from webPage");   
    {
        lock_guard<mutex> lock(_mutex); // 互斥取结果

        int qret = mysql_query(_db, sql);
        if(qret != 0)
        {
            cerr << "Error:" << mysql_error(_db) << endl;
            exit(1);
        }

        MYSQL_RES * result = mysql_store_result(_db);
        MYSQL_ROW row;
        if((row = mysql_fetch_row(result)) != NULL)
        {
            docNum = stoull(row[0]);
        }

        // 释放res
        mysql_free_result(result);

    }

    return docNum;
}

// 功能: 查找网页库, 添加所需要的网页库记录
void WebPageQuery::addPage(string docId)
{
    // 该docId已经存在
    //if(_pageLib.find(docId) != _pageLib.end())
    //{
    //    cout << "docId已经存在addPage直接返回" << endl;
    //    return;
    //}

    // 将WebPage的对应记录读取进来
    char sql[1024] = {0};
    sprintf(sql, "select docId, url, title, description, content from webPage where docId = '%s'", docId.c_str());
    {
        lock_guard<mutex> lock(_mutex); // 互斥取结果
        int qret = mysql_query(_db, sql);
        if(qret != 0)
        {
            cerr << "Error:" << mysql_error(_db) << endl;
            exit(1);
        }

        MYSQL_RES * result = mysql_store_result(_db);
        MYSQL_ROW row;
        while((row = mysql_fetch_row(result)) != NULL)
        {
            // 插入到_pages
            _pages.push_back({stoi(row[0]), row[1], row[2], row[3], row[4]});
        }

        // 释放res
        mysql_free_result(result);
    }
}

// 功能: 查找倒排索引库, 并添加所需要的记录
int WebPageQuery::addInvertIndex(string word)
{
    /* cout << "index添加:" << word <<endl; */
    // 该word已经存在
    if(_invertIndexTable.find(word) != _invertIndexTable.end())
    {
        return 0;
    }

    // 将invertedListd的对应记录读取进来
    char sql[1024] = {0};
    sprintf(sql, "select wId, word, docId, frequency, weight from invertedList where word = '%s'", word.c_str());

    // char * sql = "select wId, word, docId, frequency, weight from invertedList";
    {
        lock_guard<mutex> lock(_mutex); // 互斥取结果
        int qret = mysql_query(_db, sql);
        if(qret != 0)
        {
            cerr << "Error:" << mysql_error(_db) << endl;
            exit(1);
        }

        // 倒排索引库存储格式:
        MYSQL_RES * result = mysql_store_result(_db);
        MYSQL_ROW row;
        if(mysql_num_rows(result) == 0)
        {
            return -1;
        }

        // 读取并存储每一行为倒排索引的格式
        while((row = mysql_fetch_row(result)) != NULL)
        {
            unordered_map<string,vector<vector<string>>>::iterator vec = _invertIndexTable.find(word);

            if(vec != _invertIndexTable.end())
            {
                // word已经存在, 插入
                vector<string> tempVec;  
                tempVec.push_back(row[0]);
                tempVec.push_back(word);
                tempVec.push_back(row[2]);
                tempVec.push_back(row[3]);
                tempVec.push_back(row[4]);
                vec->second.push_back(std::move(tempVec));
            }
            else
            {
                // word不存在, 创建并插入
                vector<string> tempVec;  
                tempVec.push_back(row[0]);
                tempVec.push_back(word);
                tempVec.push_back(row[2]);
                tempVec.push_back(row[3]);
                tempVec.push_back(row[4]);
                vector<vector<string>> tempVec2;
                tempVec2.push_back(std::move(tempVec));
                _invertIndexTable.insert({word, std::move(tempVec2)});
            }
        }

        // 释放res
        mysql_free_result(result);
    }
    return 0;
}

struct Compare
{
    bool operator()(const WebPage & pageA, const WebPage & pageB)
    {
        return pwpq->cmpWebPage(*pdocVecs,*pbaseVec, pageA, pageB);
    }
    WebPageQuery * pwpq;
    map<string, vector<double>> * pdocVecs;
    vector<double> * pbaseVec;
};

string WebPageQuery::doQuery(const string & str) 
{
    // 1. 分词
    vector<string> keywords = splitStrToWords(str);
    //for(size_t i = 0; i < keywords.size(); ++i)
    //{
    //    cout << keywords[i] << endl;
    //    cout << "----------------------" << endl;
    //}
    // 2. 添加所需的倒排索引记录
    for(size_t i = 0; i < keywords.size();)
    {
        if(addInvertIndex(keywords[i]) == -1)
        {
            keywords.erase(keywords.begin() + i);
        }
        else
        {
            ++i;
        }
    }
    /* cout << "添加索引后" << endl; */
    //for(auto it : _invertIndexTable)
    //{
    //    cout << it.first << endl;
    //}
    /* cout << "------------------------" << endl; */
    // 3. 根据已读取的倒排索引库记录, 获取交集后的docIds(string类型)
    set<string> innerDocIds = getInnerDocId();

    // 4. 计算基准向量
    vector<double> baseVec = getBaseVec(keywords);
    /* cout << "baseVec.len = " << baseVec.size() << endl; */

    // 5. 获取待对比的所有文章向量, 存储为docVecs
    map<string, vector<double>> docVecs; // map<docId, 文章向量>
    for(const auto & docId : innerDocIds)
    {
        addPage(docId);
        docVecs.insert({docId, getDocVec(docId, keywords)});
    }

    // 6. 根据余弦相似度算法, 将可能需要的WebPage以优先级队列的方式进行存储
    Compare c;
    c.pwpq = this;
    c.pdocVecs = &docVecs;
    c.pbaseVec = &baseVec;
    priority_queue<WebPage,vector<WebPage>, Compare> priorityPages(c);
    for(const auto & page : _pages)
    {
        priorityPages.push(page);
    }

    // 7. 取前10个相似度最高的网页, 存储为retVec
    vector<WebPage> retPagesVec;
    size_t cnt = 0;
    while(cnt < 10)
    {
        if(cnt >= priorityPages.size()) break;
        retPagesVec.push_back(priorityPages.top());
        //priorityPages.top().printWebPage();        
        priorityPages.pop();
        cnt++;
    }

    // 8. 将retVec中的内容转成JSON格式的字符串, 并返回
    return toJsonStr(retPagesVec);
}

// 功能: 将WebPage转化成JSON格式的字符串 
string WebPageQuery::toJsonStr(const vector<WebPage> & retPagesVec) const 
{
    json j;
    int cnt = 0;
    for(const auto & page : retPagesVec)
    {
        string cntStr = to_string(cnt);
        j[cntStr]["docUrl"] = page.getDocUrl();
        j[cntStr]["docTitle"] = page.getDocTitle();
        j[cntStr]["docDescription"] = page.getDocDescription();
        cnt++;
    }
    return j.dump();
}

// 功能: 用于比较两个WebPage
bool WebPageQuery::cmpWebPage(const map<string, vector<double>> & docVecs, 
                              const vector<double> & baseVec, 
                              const WebPage & pageA, 
                              const WebPage & pageB)
{
    int docIdA = pageA.getDocId();
    int docIdB = pageB.getDocId();
    vector<double> vecA = docVecs.find(to_string(docIdA))->second;
    vector<double> vecB = docVecs.find(to_string(docIdB))->second;
    return cmpDocVec(baseVec, vecA, vecB);
}

// 功能: 用于比较两个向量(降序、余弦相似度算法)
bool WebPageQuery::cmpDocVec(const vector<double> & baseVec, 
                             const vector<double> & vecA, 
                             const vector<double> & vecB) 
{
    double cosSimA = calcCosSim(baseVec, vecA);
    double cosSimB = calcCosSim(baseVec, vecB);

    // 比较余弦相似度
    return cosSimA < cosSimB;

}

// 功能: 比较两个向量的余弦相似度
// 返回: 余弦相似度的值
double WebPageQuery::calcCosSim(const vector<double> & vecA, const vector<double> & vecB)
{
    double dotProduct = calcDotProduct(vecA, vecB);
    double magnitudeA = calcMagnitude(vecA);
    double magnitudeB = calcMagnitude(vecB);
    double cosSim = dotProduct / (magnitudeA * magnitudeB);

    return cosSim;
}

// 功能: 计算两个向量的内积
double WebPageQuery::calcDotProduct(const vector<double> & vecA, const vector<double> & vecB)
{
    if(vecA.size() != vecB.size())
    {
        /* cerr << "calcDotProduct: mismatched length!" << endl; */
        return -1;
    }

    double dotProduct = 0;
    for(size_t i = 0; i < vecA.size(); ++i)
    {
        dotProduct += vecA[i] * vecB[i];
    }

    return dotProduct; 
}

// 功能: 计算向量的模
double WebPageQuery::calcMagnitude(const vector<double> & vec)
{
    double square_sum = 0;
    for(const auto & num : vec)
    {
        square_sum += num * num;
    }
    double magnitude = sqrt(square_sum);
    return magnitude;

}
vector<double> WebPageQuery::getBaseVec(const vector<string> & words)
{
    // 1. 获取每个word的DF(TF=1), 并计算其IDF
    vector<double> baseVec;
    /* cout << "getBaseVec" << endl; */
    for(const auto & word : words)
    {
        auto it = _invertIndexTable.find(word);
        if(it == _invertIndexTable.end())
        {
            /* cout << "没有这个词："<< word << endl; */
            break;
        }
        unsigned long long df = it->second.size();      // 获取word的DF
        double idf = log2((double)_docNum / (df + 1));  // 计算word的IDF
        baseVec.push_back(idf);  // 用baseVec暂存其w(即IDF, w = TF * DF)
    }

    // 2. 计算全部idf的欧几里得范数sqrtSum
    double sum = 0;
    for(const auto & idf : baseVec)
    {
        sum += idf * idf;
    }
    double sqrtSum = sqrt(sum);

    // 3. 对baseVec执行归一化
    for(auto & idf : baseVec)
    {
        idf = idf / sqrtSum;
    }

    // 4. 返回baseVec
    return baseVec;

}

// 功能: 获取交集后的docid集合
// 返回: docid集合
set<string> WebPageQuery::getInnerDocId()
{
    // 取交集处理
    set<string> interDocIds;    // docId的交集
    for(const auto & p : _invertIndexTable)
    {
        // 新词的新集合
        set<string> tempDocIds; 
        for(const auto & item : p.second)
        {
            tempDocIds.insert(item[2]);    
        }

        if(interDocIds.size() == 0)
        {
            // 首个集合先全部加入到interDocIds中
            interDocIds = std::move(tempDocIds); 
        }
        else
        {
            // 非首个集合则与interDocIds取交集
            set<string> tempinterDocIds;
            set_intersection(interDocIds.begin(), interDocIds.end(), tempDocIds.begin(), tempDocIds.end(), 
                             inserter(tempinterDocIds, tempinterDocIds.begin()));
            // 置换interDocIds
            interDocIds = std::move(tempinterDocIds);
        }
    }
    return interDocIds;
}

// 根据文章Id和切分的关键词, 获取该篇文章的向量
vector<double> WebPageQuery::getDocVec(const string & docId, const vector<string> & words)
{
    vector<double> res;
    for(const auto & keyWord : words)
    {
        auto it = _invertIndexTable.find(keyWord);
        /* cout << "size = " << it->second.size() << endl; */
        if(it == _invertIndexTable.end())
        {
            // 某个关键词不存在, 无法生成文章向量
            static vector<double> nondata;
            return nondata;
        }
        int num = 0;
        for(const auto & item : it->second)
        {
            num++;
            if(docId == item[2])
            {
                /* cout << "添加了一次double" << endl; */
                res.push_back(stod(item[4]));   // 把weight添加到向量中
            }
        }
        /* cout << "该词的倒排索引记录检查了"  << num << endl; */
    }
    /* cout << "某文章向量长度=" << res.size() << endl; */
    return res;
}

// 功能：对输入的查询语句, 进行分词
// 输入: 查询语句
// 输出: 分词后的结果
vector<string> WebPageQuery::splitStrToWords(const string & str)
{
    vector<string> words;
    char * saveptr;
    char pendingWords[1024] = {0}; 
    vector<string> res;

    // 1. 使用jieba进行分词
    _jieba.CutForSearch(str, words);
    string splitWords = limonp::Join(words.begin(), words.end(), "/");

    // 2. 将结果进行切分, 并存储到vector<string> res中
    strcpy(pendingWords, splitWords.c_str());
    const char * token = strtok_r(pendingWords, "/", &saveptr);
    res.push_back(token);
    while(token != NULL)
    {
        token = strtok_r(NULL, "/", &saveptr);
        if(token)
        {
            res.push_back(token);
        }
    }

    // 3. 返回结果
    return res;
}

string WebPageQuery::returnNoAnswer() 
{
    return "";
}
