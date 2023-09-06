export module RPC.JsonRPC;
import <string>;
import <unordered_map>;
import <memory>;
import "WindowsFuncs.h";
import <shared_mutex>;
import Serialize.JsonSerializer;
import ThreadPool;
import StringBuilder;
using namespace std;
export namespace RPC {
	struct RPCData {
		bool is_new_version = false;
		string name = "";
		vector<string> paras = vector<string>();
		string id = "1";
	};
	struct RPCError {
		RPCError() {
			code = 0;
		}
		RPCError(int code, string message) {
			this->code = code;
			this->message = message;
		}
		int code;
		string message;
	};
	class RPCResult {
	public:
		bool is_new_version = false;
		string result;
		RPC::RPCError error;
		string id = "1";
		string ToString() {
			string res = "{\"jsonrpc\":";
			if (is_new_version) {
				res += "\"2.0\"";
			}
			else {
				res += "\"1.0\"";
			}
			if (error.code == 0) {
				res += string("\"error\":{\"code\":"+error.code)+",\"message\":\""+error.message+"\"}";
			}
			else {
				res += ",\"result\":\"" + result + "\"";
			}
			res += ",\"id\":\"" + id+"\"}";
			return res;
		}
	};
	class JsonRPC {
	public:
		string name;
		JsonRPC() {
		}
		virtual RPCResult Run(RPC::RPCData) = 0;
		void Reg(string name, shared_ptr<JsonRPC> obj) {
			rpc_list[name] = obj;
		}
		shared_ptr<JsonRPC> Get(string name) {
			return rpc_list[name];
		}
		string ThreadProc(string data) {
			//获取请求数据
			shared_ptr<Serialize::JsonItem> item = Serialize::JsonSerializer::DeSerialize(data);
			RPC::RPCData rpc = RPC::RPCData();
			auto rpc_ver = item->values["jsonrpc"];
			if (rpc_ver != nullptr && rpc_ver->name == "2.0") {
				rpc.is_new_version = true;
			}
			auto method_ = item->values["method"];
			if (rpc_ver != nullptr) {
				rpc.name = method_->name;
			}
			auto id_ = item->values["id"];
			if (rpc_ver != nullptr) {
				rpc.id = atoi(id_->name.c_str());
			}
			auto params = item->values["params"];
			for (auto p : params->values) {
				rpc.paras.push_back(p.second->name);
			}
			//调用请求
			RPCResult result = RPCResult();
			try {
				result = rpc_list.at(rpc.name)->Run(rpc);
				result.is_new_version = rpc.is_new_version;
				result.id = rpc.id;
			}
			catch (exception) {
				result.is_new_version = rpc.is_new_version;
				result.id = rpc.id;
				result.error = RPC::RPCError(-32601, "方法不存在或不可见。");
			}
			return result.ToString();
		}
		string GetProcedures() {
			/*StrUtils::TLStringBuilder res = StrUtils::TLStringBuilder(string());
			for (auto s : rpc_list) {

			}*/
			//记得补全代码
		}
		unordered_map <string, shared_ptr<JsonRPC>> rpc_list = unordered_map <string, shared_ptr<JsonRPC>>();
	};
}
//export namespace RPC {
//	struct RPCData {
//		bool is_new_version = false;
//		string name = "";
//		vector<string> paras = vector<string>();
//		string id = "1";
//	};
//	struct RPCError {
//		RPCError() {
//			code = 0;
//		}
//		RPCError(int code, string message) {
//			this->code = code;
//			this->message = message;
//		}
//		int code;
//		string message;
//	};
//	class RPCResult {
//	public:
//		bool is_new_version = false;
//		string result;
//		RPC::RPCError error;
//		string id = "1";
//		string ToString() {
//			string res = "{\"jsonrpc\":";
//			if (is_new_version) {
//				res += "\"2.0\"";
//			}
//			else {
//				res += "\"1.0\"";
//			}
//			if (error.code == 0) {
//				res += string("\"error\":{\"code\":"+error.code)+",\"message\":\""+error.message+"\"}";
//			}
//			else {
//				res += ",\"result\":\"" + result + "\"";
//			}
//			res += ",\"id\":\"" + id+"\"}";
//			return res;
//		}
//	};
//	class JsonRPC {
//	public:
//		string name;
//		JsonRPC() {
//		}
//		virtual RPCResult Run(RPC::RPCData) = 0;
//		static void Reg(string name, shared_ptr<JsonRPC> obj) {
//			rpc_list[name] = obj;
//		}
//		static shared_ptr<JsonRPC> Get(string name) {
//			return rpc_list[name];
//		}
//		void ThreadProc(void* data) {
//			//获取请求
//			mutex_accept.lock();
//			NetFunc::ClientInfo info = server_socket.Accept();
//			mutex_accept.unlock();
//			//获取请求数据
//			shared_ptr<Serialize::JsonItem> item = Serialize::JsonSerializer::DeSerialize(info.data);
//			RPC::RPCData rpc = RPC::RPCData();
//			auto rpc_ver = item->values["jsonrpc"];
//			if (rpc_ver != nullptr && rpc_ver->name == "2.0") {
//				rpc.is_new_version = true;
//			}
//			auto method_ = item->values["method"];
//			if (rpc_ver != nullptr) {
//				rpc.name = method_->name;
//			}
//			auto id_ = item->values["id"];
//			if (rpc_ver != nullptr) {
//				rpc.id = atoi(id_->name.c_str());
//			}
//			auto params = item->values["params"];
//			for (auto p : params->values) {
//				rpc.paras.push_back(p.second->name);
//			}
//			//调用请求
//			RPCResult result = RPCResult();
//			try {
//				result = rpc_list.at(rpc.name)->Run(rpc);
//				result.is_new_version = rpc.is_new_version;
//				result.id = rpc.id;
//			}
//			catch (exception) {
//				result.is_new_version = rpc.is_new_version;
//				result.id = rpc.id;
//				result.error = RPC::RPCError(-32601, "方法不存在或不可见。");
//			}
//			//发送响应
//			server_socket.Send(result.ToString());
//		}
//	private:
//		static bool stop;
//		static unordered_map<string, shared_ptr<JsonRPC>> rpc_list;
//		static shared_ptr<MultiThread::ThreadPool> pool;
//		//static vector<thread> threads;
//		static NetFunc::MySocket server_socket;
//		static shared_mutex mutex_accept;
//	};
//	unordered_map<string, shared_ptr<JsonRPC>> JsonRPC::rpc_list = unordered_map<string, shared_ptr<JsonRPC>>();
//	shared_ptr<MultiThread::ThreadPool> JsonRPC::pool = shared_ptr<MultiThread::ThreadPool>(new MultiThread::ThreadPool(1, 10));
//	//vector<thread> JsonRPC::threads = vector<thread>(threads);
//	bool JsonRPC::stop = false;
//	NetFunc::MySocket JsonRPC::server_socket = NetFunc::MySocket(8089);
//	shared_mutex JsonRPC::mutex_accept;
//}