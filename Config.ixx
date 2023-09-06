export module Server.Config;
import <string>;
import <unordered_map>;
import <fstream>;
import Serialize.XMLSerializer;
import Utils;
using namespace std;
export namespace Server {
	class Config {
	public:
		static void Init(string filepath) {
			ifstream ifs(filepath);
			string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
			shared_ptr<Serialize::XMLItem> xml_reader = Serialize::XMLSerializer().DeSerialize(content);
			ifs.close();
			shared_ptr<Serialize::XMLItem> settings = xml_reader->subnodes[0];
			for (auto o : settings->subnodes) {
				for (auto p : o->subnodes) {
					options[o->element_type + '.' + p->element_type] = StrUtils::Trim2(p->innerText);
				}
			}
		}
		static unordered_map<string, string> ReadContent(string filepath) {
			ifstream ifs(filepath);
			unordered_map<string, string> res = unordered_map<string, string>();
			char buf_[1024] = { 0 };
			while (ifs.getline(buf_,1024)) {
				vector<string> foo_ = StrUtils::Split(string(buf_),"=");
				if (foo_.size() == 2)
					res[StrUtils::Trim2(foo_[0])] = StrUtils::Trim2(foo_[1]);
			}
			ifs.close();
			return res;
		}
		static unordered_map<string,string> options;
	};
	unordered_map<string, string> Config::options = unordered_map<string, string>();
}