export module PF.Table;
import <unordered_map>;
import <string>;
import <memory>;
using namespace std;
export namespace MyDatabase {
    class Table {
    public:
        Table(vector<string> pk,vector<string>pro) {
            primary_keys = pk;
            columns = pro;
        }
        string table_name;
        vector<string> primary_keys;
        vector<string> columns;
        unordered_map<string, shared_ptr<unordered_map<string, string>>> table_map = unordered_map<string, shared_ptr<unordered_map<string, string>>>();
        shared_ptr<unordered_map<string, string>> operator[](vector<string>& primary_keys) {
            string real_index = "";
            for (string c : primary_keys) {
                real_index.append(c+',');
            }
            return table_map[real_index];
        }
        void Insert(unordered_map<string, string>& data) {
            //获取主键的值，用逗号隔开
            string pk_val_ = "";
            for (string c : primary_keys) {
                pk_val_.append(data[c] + ',');
            }
            //设置其他属性
            shared_ptr<unordered_map<string, string>> map = shared_ptr<unordered_map<string, string>>(new unordered_map<string, string>());
            for (string c : columns) {
                (*map)[c] = data[c];
            }
            table_map[pk_val_] = map;
        }
    };
}