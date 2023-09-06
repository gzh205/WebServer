export module StringBuilder;
import <list>;
import <string>;
using namespace std;
export namespace StrUtils {
    /* �ַ���ƴ���� */
    template <typename chr>
    class TLStringBuilder {
    private:
        typedef std::basic_string<chr> string_t;
        typedef std::list<string_t> container_t; // Reasons not to use vector below.
        //typename��־string_t::size_type��һ�����ͣ���������
        typedef typename string_t::size_type size_type; // Reuse the size type in the string.
        container_t _container;
        size_type _total_size;
        void append(const string_t& src)
        {
            _container.push_back(src);
            //+=��ζ��_total_size�����ʼ��
            _total_size += src.length();
        }
        // No copy constructor, no assignment.
        //TLStringBuilder(const TLStringBuilder&);
        //TLStringBuilder& operator= (const TLStringBuilder&);
    public:
        TLStringBuilder(const string_t& src)
        {
            if (!src.empty()) {
                _container.push_back(src);
            }
            _total_size = src.length();
        }

        TLStringBuilder()
        {
            _total_size = 0;
        }

        ~TLStringBuilder()
        {
            _container.clear();
            std::list<string_t>().swap(_container);
            _total_size = 0;
        }
        // TODO: Constructor that takes an array of strings.
        //֧����ʽ���
        TLStringBuilder& Append(const string_t& src) {
            append(src);
            return *this; // allow chaining.
        }

        //��ģ���ڲ�ģ�庯���Ķ��巽ʽ
        // This one lets you add any STL container to the string builder.
        template<class inputIterator>
        TLStringBuilder& Add(const inputIterator& first, const inputIterator& afterLast) {
            // std::for_each and a lambda look like overkill here.
            // <b>Not</b> using std::copy, since we want to update m_totalSize too.
            for (inputIterator f = first; f != afterLast; ++f) {
                append(*f);
            }
            return *this; // allow chaining.
        }

        TLStringBuilder& AppendLine(const string_t& src) {
            //10��ascii�룬��ʾ����
            //�����Ƕ���һ�����飬������Ԫ�� { 10, 0 }
            static chr lineFeed[]{ 10, 0 }; // C++ 11
            _container.push_back(src + lineFeed);
            _total_size += 1 + src.size();
            return *this; // allow chaining.
        }
        TLStringBuilder& AppendLine() {
            static chr lineFeed[]{ 10, 0 };
            _container.push_back(lineFeed);
            _total_size++;
            return *this; // allow chaining.
        }

        // TODO: AppendFormat implementation. Not relevant for the article.

        // Like C# StringBuilder.ToString()
        // Note the use of reserve() to avoid reallocations.
        string_t ToString() const {
            string_t result;

            // The whole point of the exercise!
            // If the container has a lot of strings, reallocation (each time the result grows) will take a serious toll,
            // both in performance and chances of failure.
            // I measured (in code I cannot publish) fractions of a second using 'reserve', and almost two minutes using +=.
            //����ÿ��׷���ַ�����Ҫ�����ڴ�
            result.reserve(_total_size + 1);
            // result = std::accumulate(m_Data.begin(), m_Data.end(), result); // This would lose the advantage of 'reserve'
            /*
                ע�⣺�˴���iter������Ϊʲô��const_iterator������iterator
                ������ΪToString()��const�����ˣ�c++��������ֹ�ں������޸�������
            */
            for(auto iter = _container.begin(); iter != _container.end(); ++iter) {
                result += *iter;
            }
            return result;
        }
    };
};