export module Server.Session;
import <unordered_map>;
import <string>;
//import <ctime>;
import <chrono>;
import "WindowsFuncs.h";
import LRUList;
import StrUtils.RandomStringGenerate;
using namespace std;
export namespace Server {
	struct SessionInfo {
		SessionInfo() {
		}
		string sessionid = "";
		chrono::system_clock::time_point create_time = chrono::system_clock::now();
		unordered_map<string, string> user_vars = unordered_map<string, string>();
	};
	class Session {
	public:
		Session() {
		}
		Tools::LRUList<string, SessionInfo> lru_list = Tools::LRUList<string, SessionInfo>(65536);
		SessionInfo operator[](string& key) {
			SessionInfo info = lru_list.Use(key);
			if (info.sessionid.empty()) {
				//创建一个session
				SessionInfo info_ = SessionInfo();
				string tmp = StrUtils::RandomStringGenerate(20).random_string;
				string t = WinOpts::GetCurTime();
				info.sessionid = tmp + t;
				lru_list.Add(info.sessionid, info);
			}
			return info;
		}
	};
}