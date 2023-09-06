export module StringBuilder;
import <list>;
import <string>;
using namespace std;
export namespace StrUtils {
    /* 字符串拼接类 */
    template <typename chr>
    class TLStringBuilder {
    private:
        typedef std::basic_string<chr> string_t;
        typedef std::list<string_t> container_t; // Reasons not to use vector below.
        //typename标志string_t::size_type是一个类型，并非属性
        typedef typename string_t::size_type size_type; // Reuse the size type in the string.
        container_t _container;
        size_type _total_size;
        void append(const string_t& src)
        {
            _container.push_back(src);
            //+=意味着_total_size必须初始化
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
        //支持链式编程
        TLStringBuilder& Append(const string_t& src) {
            append(src);
            return *this; // allow chaining.
        }

        //类模板内部模板函数的定义方式
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
            //10是ascii码，表示换行
            //这里是定义一个数组，有两个元素 { 10, 0 }
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
            //避免每次追加字符串都要分配内存
            result.reserve(_total_size + 1);
            // result = std::accumulate(m_Data.begin(), m_Data.end(), result); // This would lose the advantage of 'reserve'
            /*
                注意：此处的iter的类型为什么是const_iterator而不是iterator
                这是因为ToString()被const修饰了，c++编译器禁止在函数内修改类属性
            */
            for(auto iter = _container.begin(); iter != _container.end(); ++iter) {
                result += *iter;
            }
            return result;
        }
    };
};