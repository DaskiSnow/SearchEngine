#include "WebPageQuery.h"
#include "WebPage.h"

/**
 * WebPageQuery implementation
 */


WebPageQuery::WebPageQuery(MYSQL * db, WordSegmentation & jieba) 
: _db(db)
, _jieba(jieba)
, _docNum(getDocNum())
{
    
    // _pageLib的读取
    // _invertIndexTable的读取
    
    
}

WebPageQuery::~WebPageQuery()
{
    cout << "~WebPageQuery()" << endl;
}

// 功能: 获取文章总数 
unsigned long long WebPageQuery::getDocNum()
{
    char sql[1024] = {0};
    sprintf(sql, "select count(*) from webPage");   
    int qret = mysql_query(_db, sql);
    if(qret != 0)
    {
        cerr << "Error:" << mysql_error(_db) << endl;
        exit(1);
    }

    unsigned long long docNum = 0;
    MYSQL_RES * result = mysql_store_result(_db);
    MYSQL_ROW row;
    if((row = mysql_fetch_row(result)) != NULL)
    {
        docNum = stoull(row[0]);
    }
    
    // 释放res
    mysql_free_result(result);

    return docNum;
}

// ok
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
        // 必然不存在, 插入(移动语义)
        /* _pageLib.insert({row[0],{stoi(row[0]), row[1], row[2], row[3], row[4]}}); */

        // 插入到_pages
        _pages.push_back({stoi(row[0]), row[1], row[2], row[3], row[4]});

    }

    // 释放res
    mysql_free_result(result);
}

// ok
void WebPageQuery::addInvertIndex(string word)
{
    // 该word已经存在
    if(_invertIndexTable.find(word) != _invertIndexTable.end())
    {
        return;
    }

    // 将invertedListd的对应记录读取进来
    char sql[1024] = {0};
    sprintf(sql, "select wId, word, docId, frequency, weight from invertedList where word = '%s'", word.c_str());
    cout << sql << endl;
    // char * sql = "select wId, word, docId, frequency, weight from invertedList";
    int qret = mysql_query(_db, sql);
    if(qret != 0)
    {
        cerr << "Error:" << mysql_error(_db) << endl;
        exit(1);
    }

    // 倒排索引库存储格式:
    // unordered_map<word, vector<wId, word, docId, frequency, weight>>
    MYSQL_RES * result = mysql_store_result(_db);
    MYSQL_ROW row;

    // 读取并存储每一行为倒排索引的格式
    while((row = mysql_fetch_row(result)) != NULL)
    {
        cout <<"查询好结果了, 进入fetch_row循环" << endl; 
        unordered_map<string,vector<vector<string>>>::iterator vec = _invertIndexTable.find(row[1]);

        if(vec != _invertIndexTable.end())
        {
            cout << "存在插入" << endl;
            // word已经存在, 插入
            vector<string> tempVec;  
            tempVec.push_back(row[0]);
            tempVec.push_back(row[1]);
            tempVec.push_back(row[2]);
            tempVec.push_back(row[3]);
            tempVec.push_back(row[4]);
            vec->second.push_back(std::move(tempVec));
        }
        else
        {
            cout << "不存在插入" << endl;
            // word不存在, 创建并插入
            vector<string> tempVec;  
            tempVec.push_back(row[0]);
            tempVec.push_back(row[1]);
            tempVec.push_back(row[2]);
            tempVec.push_back(row[3]);
            tempVec.push_back(row[4]);
            vector<vector<string>> tempVec2;
            tempVec2.push_back(std::move(tempVec));
            _invertIndexTable.insert({row[1], std::move(tempVec2)});
        }
    }
    cout << "循环结束" << endl;

    // 释放res
    mysql_free_result(result);
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
/**
 * @param str
 * @return string
 */
string WebPageQuery::doQuery(const string & str) 
{
    // 1. 分词: 得到N个关键词(ok)
    // 2. 计算每文章N个关键词的权重系数w', 并组成其特征向量
    // 2.1 根据关键词查询倒排索引库, 获取resultVec,(ok) 
    //     存储为unmap<string, vec<vec<string>>> resVecs
    // 2.2 获取每个关键字的(N个)章频, 存储为map<string, ull> df;(ok)
    // 2.3 获取每关键字每文章的词频, 存储为map<string, pair<int, ull>>(ok)
    // 2.3 docId取交集, 得到一个set<int>(ok)
    // 2.4 计算每(交集)文章每关键字, 计算w'.(ok)
    //     所有文章各自的特征向量存储为map<int, vec<double>>
    // 3.1 计算基准向量Base, 存储为vec<double> 
    // 3.2 计算余弦相似度, 存储为map<int, double>
    // 3.3 加入到优先级队列
    // 4. 取队列中前10个, 读取为vec<int> pre_output
    // 5. 遍历pre_output, 根据docId查询_pageLib, 获取对应WebPage
    //    存储为vec<WebPage>
    // 6. 将所有WebPage转化为JSON格式
    //    存储为string output;
    // 7. 返回output;

    // 1. 分词
    vector<string> keywords = splitStrToWords(str);

    // 2. 添加所需的倒排索引记录
    for(const auto & word : keywords)
    {
        addInvertIndex(word);
    }

    // 3. 根据已读取的倒排索引库记录, 获取交集后的docIds(string类型)
    set<string> innerDocIds = getInnerDocId();

    // 4. 计算基准向量
    vector<double> baseVec = getBaseVec(keywords);

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
    priority_queue<WebPage,vector<WebPage>, Compare> retPages(c);
    for(const auto & page : _pages)
    {
        retPages.push(page);
    }

    int cnt = 0;
    while(cnt < 10)
    {
        retPages.top().printWebPage();        
        retPages.pop();
        cnt++;
    }

    // 7. 转成JSON格式

    return "haha";
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
        cerr << "calcDotProduct: mismatched length!" << endl;
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
    for(const auto & word : words)
    {
        auto it = _invertIndexTable.find(word);
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

bool WebPageQuery::executeQuery(const vector<string> & queryWords, 
                                vector<pair<int,vector<double>>> &resultVec) 
{

}

/**
 * @param queryWords
 * @return vector<double>
 */
    vector<double> 
WebPageQuery::getQueryWordsWeightVector(vector<string> & queryWords) 
{

}

/**
 * @param docIdVec
 * @param queryWords
 * @return string
 */
string WebPageQuery::createJson(vector<int> & docIdVec, 
                                const vector<string> & queryWords) 
{
    return "";
}

/**
 * @return string
 */
string WebPageQuery::returnNoAnswer() 
{
    return "";
}
