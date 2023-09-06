import <string>;
import <iostream>;
import <cstdio>;
import <filesystem>;
import <locale>;
import <codecvt>;
import <sstream>;
export module FileSystem;
using namespace std;
export namespace File {
    class FileStream {
    public:
        static std::string Read(string path) {
            FILE* fp = fopen(path.data(), "rb");
            if (fp != nullptr) {
                fseek(fp, 0, SEEK_END);
                long fileSize = ftell(fp);
                rewind(fp);
                char* buffer = new char[fileSize];
                fread(buffer, sizeof(char), fileSize, fp);
                fclose(fp);
                return string(buffer,fileSize);
                [=](int a) { a += 1; return a; };
            }
            else {
                return "";
            }
        }

        static void Write(const std::string path,const std::string& data) {
            FILE* fp = fopen(path.data(), "wb");
            if (fp != nullptr) {
                fwrite(data.data(), sizeof(char), data.size(), fp);
                fclose(fp);
            }
        }
    };
}