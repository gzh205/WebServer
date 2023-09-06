export module Server.Request;
import <string>;
import <unordered_map>;
import <vector>;
import Utils;
import Serialize.JsonSerializer;
using namespace std;
export namespace Server {

	class Request {
	public:
		Request(string request_data) {
			//请求解析
			vector<string> request_datas_ = StrUtils::Split(request_data, "\n");
			//先读取第一行的请求头部
			int idx = 1;
			vector<string> method_url_version = StrUtils::Split(StrUtils::Trim2(request_datas_[0]), " ");
			if (method_url_version.size() != 3) {
				throw exception("错误,请求报文格式错误");
			}
			method = StrUtils::ToUppor(method_url_version[0]);
			url = method_url_version[1];
			version = method_url_version[2];
			//再读取后的请求参数
			for (; idx < request_datas_.size(); idx++) {
				if (request_datas_[idx].empty() || request_datas_[idx] == "\r") {
					idx++;
					break;
				}
				else {
					vector<string> foo_ = StrUtils::Split(request_datas_[idx], ":");
					if (foo_.size() != 2) {
						continue;
					}
					param[StrUtils::Trim2(foo_[0])] = StrUtils::Trim2(foo_[1]);
				}
			}
			//读取POST参数(如果有)
			if (method == "POST") {
				string bar_ = StrUtils::Trim2(request_datas_[idx]);
				if (!bar_.empty()) {
					// 将POST请求参数当做JSON处理(浏览器必须发送JSON参数)
					shared_ptr<Serialize::JsonItem> json = Serialize::JsonSerializer::DeSerialize(bar_);
					for (auto& pair : (*json.get()).values)
						param_data[pair.first] = pair.second.get()->name;
				}
			}
			else if (method == "GET") {
				vector<string> url_paras = StrUtils::Split(url, "?");
				if (url_paras.size() == 2) {
					url = StrUtils::Trim(url_paras[0]);
					vector<string> paras_ = StrUtils::Split(url_paras[1], "&");
					for (string c : paras_) {
						vector<string> baz_ = StrUtils::Split(c, "=");
						if (baz_.size() == 2) {
							param_data[StrUtils::Trim2(baz_[0])] = StrUtils::Trim2(baz_[1]);
						}
					}
				}
			}
			//处理Cookie
			vector<string> cookies = StrUtils::Split(param["cookie"], ";");
			if (cookie.size() > 0) {
				for (string k : cookies) {
					vector<string> k_v = StrUtils::Split(k, "=");
					if (k_v.size() == 2) {
						cookie[StrUtils::Trim(k_v[0])] = StrUtils::Trim(k_v[1]);
					}
				}
			}
		}
		string method;
		string url;
		string version;
		unordered_map<string, string> param = unordered_map<string, string>();
		//如果是GET请求，这里用于存储URL后的请求参数
		unordered_map<string, string> param_data = unordered_map<string, string>();
		unordered_map<string, string> cookie = unordered_map<string, string>();

	};
}