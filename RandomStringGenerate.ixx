export module StrUtils.RandomStringGenerate;
import <string>;
//import <ctime>;
import <random>;
using namespace std;
export namespace StrUtils {
	class RandomStringGenerate {
	public:
		RandomStringGenerate(const int len) {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            const int charsetSize = sizeof(charset) - 1; // Exclude null terminator
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, charsetSize - 1);

            std::string result(len, '\0');
            for (int i = 0; i < len; ++i) {
                result[i] = charset[dist(gen)];
            }

            random_string = result;
		}
		string random_string = "";
	};
}