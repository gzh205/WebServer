export module Server.Response;
import Server.Session;
import <string>;
import StringBuilder;
import Server.Config;
import <unordered_map>;
using namespace std;
export namespace Server {
	class Response {
	public:
		Response() {
			resp_code = 200;
			statement = "OK";
			type = "text/html";
		}
		Response(string data, string type) {
			resp_code = 200;
			statement = "OK";
			this->data = data;
			this->type = type;
		}
		Response(unordered_map<string, string> cookie,int resp_code,string statement,string data, string type) {
			if (!cookie.empty()) {
				for (auto ele : cookie) {
					this->cookie[ele.first] = ele.second;
				}
			}
			this->resp_code = resp_code;
			this->statement = statement;
			this->data = data;
			this->type = type;
		}
		string CreateResponseString() {
			StrUtils::TLStringBuilder sb = StrUtils::TLStringBuilder(string("HTTP/1.1 "));
			sb.Append(to_string(resp_code) + " " + statement + "\r\n");
			//填写响应参数
			sb.Append("Server:" + Config::options["server.appname"] + "\r\n");
			sb.Append("Content-Type:" + type + "\r\n");
			string len_ = to_string((unsigned long)data.length());
			sb.Append("Content-Length:" + len_);
			if (cookie.size() > 0) {
				sb.Append("\r\nSet-Cookie:");
				for (auto key : cookie) {
					sb.Append(key.first + "=" + key.second + ";");
				}
			}
			//插入空行，接下来填写响应体
			sb.Append("\r\n\r\n");
			if (data.length() > 0) {
				sb.Append(data);
			}
			return sb.ToString();
		}
		unordered_map<string,string> cookie = unordered_map<string, string>();
		int resp_code;
		string statement;
		string data;
		string type;
	};
}