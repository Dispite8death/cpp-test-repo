#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <limits>
#include <map>
#include <cmath>
#include <chrono>

using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

struct Document {
    unsigned int id;
    double relevance;

    bool operator < (const Document& rhs) const {
        return  std::tie(relevance, id) > std::tie(rhs.relevance, rhs.id);
    }
};

class DocumentSearcher
{
private:
    const set<string> STOP_WORDS;
    const unsigned int COUNT_DOC;
    const map<string, map<unsigned int, double>> DICTIONARY_DOC;

    const vector<string> readTextDoc() const
    {
        string buffer;
        vector<string> result;

        string line;
        getline(cin, line);

        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    result.push_back(buffer);
                    buffer = "";
                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            result.push_back(buffer);
            buffer = "";
        }

        return result;
    }

    const vector<string> readTextDoc(const string& line) const
    {
        string buffer;
        vector<string> result;

        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    result.push_back(buffer);
                    buffer = "";
                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            result.push_back(buffer);
            buffer = "";
        }

        return result;
    }

    const set<string> readStopWords() const
    {
        string buffer;
        set<string> result;

        string line;
        getline(cin, line);
        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    if (result.count(buffer) == 0)
                    {
                        result.insert(buffer);
                        buffer = "";
                    }
                    
                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            if (result.count(buffer) == 0)
            {
                result.insert(buffer);
                buffer = "";
            }
        }

        return result;
    }

    const set<string> readStopWords(const string& line) const
    {
        string buffer;
        set<string> result;

        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    if (result.count(buffer) == 0)
                    {
                        result.insert(buffer);
                        buffer = "";
                    }

                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            if (result.count(buffer) == 0)
            {
                result.insert(buffer);
                buffer = "";
            }
        }

        return result;
    }

    unsigned int readCountDoc() const
    {
        unsigned int countDoc;
        cin >> countDoc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return countDoc;
    }

    const map<string, map<unsigned int, double>> createDcList() const
    {
        map<string, map<unsigned int, double>> result;

        double TF;
        for (unsigned int id = 0; id < COUNT_DOC; id++)
        {
            const vector<string> text = findstop(readTextDoc()); // Зона TF
            for (string word : text)
            {
                if (result[word].count(id) > 0) continue;
                TF = static_cast<double>(count(text.begin(), text.end(), word)) / static_cast<double>(text.size()); // Преобразование unsigned _int64 в double возможна потеря данных C4244
                result[word][id] = TF;
            }
        }
        return result;
    }

    const map<string, map<unsigned int, double>> createDcList(const vector<string>& lines_document) const
    {
        map<string, map<unsigned int, double>> result;

        double TF;
        for (unsigned int id = 0; id < COUNT_DOC; id++)
        {
            const vector<string> text = findstop(readTextDoc(lines_document[id]));
            for (string word : text)
            {
                if (result[word].count(id) > 0) continue;
                TF = static_cast<double>(count(text.begin(), text.end(), word)) / static_cast<double>(text.size()); // test
                result[word][id] = TF;
            }
        }
        return result;
    }

    const vector<string> findstop(const vector<string>& listquery) const
    {
        if (listquery.size() == 0 || STOP_WORDS.size() == 0) return listquery;
        vector<string> result;
        for (string el : listquery)
        {
            if (STOP_WORDS.count(el) == 0)
            {
                result.push_back(el);
            }
        }

        return result;
    }

    static void PrintResult(const set<Document>& result) {
        unsigned int step = 0;
        for (Document el : result)
        {
            if (step < MAX_RESULT_DOCUMENT_COUNT)
            {
                cout << "{ document_id = "s << el.id << ", " << "relevance = "s << el.relevance << " }"s << endl;
                step++;
            }
            else
            {
                break;
            }
        }
    }

public:
    DocumentSearcher() : STOP_WORDS(readStopWords()), COUNT_DOC(readCountDoc()), DICTIONARY_DOC(createDcList())
    {

    }

    DocumentSearcher(const string& stop_line, const int& countD, vector<string> line_docs) : STOP_WORDS(readStopWords(stop_line)), COUNT_DOC(countD), DICTIONARY_DOC(createDcList(line_docs))
    {

    }

    void ReadQuery()
    {
        vector<string> query = findstop(readTextDoc());

        set<string> find;
        set<string> nofind;

        for (string el : query)
        {
            if (el[0] == '-')
            {
                if (STOP_WORDS.count(el.substr(1, el.length() - 1)) == 0)
                {
                    nofind.insert(el.substr(1, el.length() - 1));
                }
            }
            else
            {
                find.insert(el);
            }
        }
        if (find.empty()) return;
        
        
        map<unsigned int, double> result;
        for (string find_word : find)
        {
            if (DICTIONARY_DOC.count(find_word) > 0 && DICTIONARY_DOC.count(find_word) != COUNT_DOC)
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    double logword = (log(static_cast<double>(COUNT_DOC) / static_cast<double>(DICTIONARY_DOC.at(find_word).size())));
                    if (result.count(find_doc.first) > 0)
                    {
                        result[find_doc.first] += find_doc.second * logword;
                    }
                    else
                    {
                        result[find_doc.first] = find_doc.second * logword;

                    }
                }
            }
            else if (DICTIONARY_DOC.count(find_word) > 0 && DICTIONARY_DOC.count(find_word) == COUNT_DOC)
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    result[find_doc.first] = 0;
                }
            }
        }

        for (string nofind_word : nofind)
        {
            if (DICTIONARY_DOC.count(nofind_word) == 0) continue;

            for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(nofind_word))
            {
                if (result.count(find_doc.first) > 0)
                {
                    result.erase(find_doc.first);
                }
            }
        }
        set<Document> inputResult;
        for (pair<unsigned int, double> el : result)
        {
            Document dc = { static_cast<unsigned int>(el.first), static_cast<double>(el.second) }; //Требует сужающего преобразования C4838
            inputResult.insert(dc);
        }

        PrintResult(inputResult);
    }

    void ReadQuery(string line_query)
    {
        vector<string> query = findstop(readTextDoc(line_query));

        set<string> find;
        set<string> nofind;

        for (string el : query)
        {
            if (el[0] == '-')
            {
                if (STOP_WORDS.count(el.substr(1, el.length() - 1)) == 0)
                {
                    nofind.insert(el.substr(1, el.length() - 1));
                }
            }
            else
            {
                find.insert(el);
            }
        }

        map<unsigned int, double> result;

        for (string find_word : find)
        {
            if (DICTIONARY_DOC.count(find_word) > 0 && DICTIONARY_DOC.count(find_word) != COUNT_DOC)
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    double logword = (log(static_cast<double>(COUNT_DOC) / static_cast<double>(DICTIONARY_DOC.at(find_word).size())));
                    if (result.count(find_doc.first) > 0)
                    {
                        result[find_doc.first] += find_doc.second * logword;
                    }
                    else
                    {
                        result[find_doc.first] = find_doc.second * logword;

                    }
                }
            }
            else if (DICTIONARY_DOC.count(find_word) > 0 && DICTIONARY_DOC.count(find_word) == COUNT_DOC)
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    result[find_doc.first] = 0;
                }
            }
        }

        for (string nofind_word : nofind)
        {
            if (DICTIONARY_DOC.count(nofind_word) == 0) continue;

            for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(nofind_word))
            {
                if (result.count(find_doc.first) > 0)
                {
                    result.erase(find_doc.first);
                }
            }
        }

        set<Document> inputResult;

        for (pair<unsigned int, double> el : result)
        {
            Document dc = { static_cast<unsigned int>(el.first), static_cast<double>(el.second) };
            inputResult.insert(dc);
        }

        PrintResult(inputResult);
    }
};

class SearchServer
{
private:
    string stop_input;
    int count_doc;
    vector<string> textDocList;
    string query;
public:
    void SetStopWords(string line)
    {
        stop_input = line;
    }

    void AddDocument(int document_id, const string& document)
    {
        if (document_id < 0) return;
        textDocList.push_back(document);
    }

    vector<Document> FindTopDocuments(const string& raw_query) const
    {
        vector<Document> rr;
        sort(rr.begin(), rr.end(), [] (const Document& rhs, const Document& lhs){
           return lhs.relevance > rhs.relevance; 
        });
        if (raw_query.length() > 0) return { };
        else return { };
    }
};

SearchServer CreateSearchServer()
{
    SearchServer search_server;
    string stopline; getline(cin, stopline); search_server.SetStopWords(stopline);
    
    int document_count;
    cin >> document_count;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int document_id = 0; document_id < document_count; ++document_id)
    {
        string line_doc;
        getline(cin, line_doc);
        search_server.AddDocument(document_id, line_doc);
    }
    
    return search_server;
}

int main()
{
    setlocale(0, "");
    DocumentSearcher sr;
    sr.ReadQuery();
    //Для теста времени
    /*chrono::steady_clock::time_point start = chrono::steady_clock::now();
    DocumentSearcher sr("is are was a an in the with near at"s, 3, { "a colorful parrot with green wings and red tail is lost"s, "a grey hound with black ears is found at the railway station"s, "a white cat with long furry tail is found near the red square"s });
    sr.ReadQuery("white cat long tail");
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << endl;*/
}
