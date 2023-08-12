#include <func.hh>
#include "Configuration.h"
#include "cppjieba/Jieba.hpp"
using namespace std;

// 配置文件的测试ok
void test0(const string path)
{
    Configuration * pConfig = Configuration::getInstance(path);
    cout << pConfig->port() << endl;
    cout << pConfig->threadNum() << endl;
    cout << pConfig->host() << endl;
    cout << pConfig->user() << endl;
    cout << pConfig->password() << endl;
    cout << pConfig->database() << endl;
    cout << pConfig->ip() << endl;
    set<string> eng = pConfig->getStopWordEngSet();
    set<string> zh = pConfig->getStopWordZhSet();
    for(const auto & str : eng)
    {
        cout << str << endl;
    }
    cout << "黄金分割线" << endl;
    for(const auto & str : zh)
    {
        cout << str << endl;
    }
}

// cppjieba的使用
void test1()
{
    const char* const DICT_PATH = "/home/daskisnow/SearchEngine/data/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "/home/daskisnow/SearchEngine/data/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "/home/daskisnow/SearchEngine/data/dict/user.dict.utf8";
    const char* const IDF_PATH = "/home/daskisnow/SearchEngine/data/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "/home/daskisnow/SearchEngine/data/dict/stop_words.utf8";
     cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
  vector<string> words;
  vector<cppjieba::Word> jiebawords;
  string s;
  string result;

  s = "他来到了网易杭研大厦";
  cout << s << endl;
  cout << "[demo] Cut With HMM" << endl;
  jieba.Cut(s, words, true);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] Cut Without HMM " << endl;
  jieba.Cut(s, words, false);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

   s = "我来到北京清华大学";
  cout << s << endl;
  cout << "[demo] CutAll" << endl;
  jieba.CutAll(s, words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  //----------------------------------
  s = "小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
  cout << s << endl;
  cout << "[demo] CutForSearch" << endl;
  jieba.CutForSearch(s, words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;


  cout << "[demo] Insert User Word" << endl;
  jieba.Cut("男默女泪", words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;
  jieba.InsertUserWord("男默女泪");
  jieba.Cut("男默女泪", words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] CutForSearch Word With Offset" << endl;
  jieba.CutForSearch(s, jiebawords, true);
  cout << jiebawords << endl;

  cout << "[demo] Lookup Tag for Single Token" << endl;
  const int DemoTokenMaxLen = 32;
  char DemoTokens[][DemoTokenMaxLen] = {"拖拉机", "CEO", "123", "。"};
  vector<pair<string, string> > LookupTagres(sizeof(DemoTokens) / DemoTokenMaxLen);
  vector<pair<string, string> >::iterator it;
  for (it = LookupTagres.begin(); it != LookupTagres.end(); it++) {
	it->first = DemoTokens[it - LookupTagres.begin()];
	it->second = jieba.LookupTag(it->first);
  }
  cout << LookupTagres << endl;

  cout << "[demo] Tagging" << endl;
  vector<pair<string, string> > tagres;
  s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";
  jieba.Tag(s, tagres);
  cout << s << endl;
  cout << tagres << endl;

  cout << "[demo] Keyword Extraction" << endl;
  const size_t topk = 5;
  vector<cppjieba::KeywordExtractor::Word> keywordres;
  jieba.extractor.Extract(s, keywordres, topk);
  cout << s << endl;
  cout << keywordres << endl;
}

int main(int argc, char* argv[])
{
    test1();
    return 0;
}

