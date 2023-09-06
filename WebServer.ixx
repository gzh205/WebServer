export module Server.WebServer;
import "WindowsFuncs.h";
import ThreadPool;
import <memory>;
import <mutex>;
import <shared_mutex>;
import Server.Request;
import Utils;
import Server.Session;
import <filesystem>;
import Server.Config;
import Utils.Timer;
import Server.Response;
import <unordered_map>;
import <string>;
import Server.WebHandle;
import FileSystem;
import <iostream>;
import UserLogin;
import UserReg;
using namespace std;
export namespace Server {
	class WebServer {
	public:
		WebServer() {
			//��ȡcontent type
			content_type = Config::ReadContent("content_type.txt");
			//��ʼ��Config
			Config::Init("config.xml");
			//����sessoin��ʱ���
			session_interval = atoi(Config::options["server.session_store_time"].c_str());
			//thread pool
			string threadnum = StrUtils::Trim2(Config::options["server.threadnum"]);
			this->pool = shared_ptr<MultiThread::ThreadPool>(new MultiThread::ThreadPool(atoi(threadnum.c_str())));
			//����socket����
			string port = StrUtils::Trim2(Config::options["server.port"]);
			this->server_socket = shared_ptr<NetFunc::MySocket>(new NetFunc::MySocket(atoi(port.c_str())));
		}
		void SocketReader() {
			//��ȡ����
			while (true) {
				NetFunc::ClientInfo* info = server_socket->Accept();
				NetFunc::MySocket::CloseConnection(info->ClientSocket);
				pool->submit(&WebServer::ThreadProc, info);
			}
		}
		static shared_ptr<WebServer> GetInstance() {
			call_once(singletonFlag, [] {
				instance = std::shared_ptr<WebServer>(new WebServer());
			//ע��web��������
			WebHandle::web_handle["/userlogin"] = (WebHandle*)new Controller::UserLogin();
			WebHandle::web_handle["/userreg"] = (WebHandle*)new Controller::UserReg();
			//����web�����߳�
			instance->http_handle_thread = thread(std::mem_fn(&WebServer::SocketReader), instance);
			//����һ����ʱ����ÿ��20�����Ƿ��г�ʱ��session
			Utils::Timer timer = Utils::Timer();
			timer.start(atoi(Config::options["server.session_store_time"].c_str()), bind([]() {
				chrono::system_clock::time_point now = chrono::system_clock::now();
			chrono::seconds interval = std::chrono::seconds(session_interval);
			auto dat_list_ = WebServer::instance->sessions.lru_list.data_list;
			for (auto dat : dat_list_) {
				if (now - dat.second.create_time > interval) {
					WebServer::instance->sessions.lru_list.Delete(dat.first);
				}
			}
				}));
				});
			return instance;
		}
		static void ThreadProc(void* data) {
			shared_ptr<NetFunc::ClientInfo> info = std::shared_ptr<NetFunc::ClientInfo>(static_cast<NetFunc::ClientInfo*>(data));
			if (info->data.length() != 0) {
				//�������
				Server::Request req = Server::Request(info->data);
				//�������
				cout << req.method + " " + req.url + "\n";
				//����session
				Server::SessionInfo sess_info = WebServer::instance->sessions[req.cookie["sessionid"]];
				//�������ļ�ϵͳ������Ҳ���˵���������һ������
				//����url
				string root_path = Server::Config::options["server.rootpath"];
				string real_path = string();
				if (req.url == "/") {
					//����url����������ҳ
					string index_ = Server::Config::options["server.index"];
					real_path = root_path + '\\' + StrUtils::ReplaceAll(index_, '/', '\\');
				}
				else {
					string req_path_ = req.url;
					real_path = root_path + '\\' + StrUtils::ReplaceAll(req_path_, '/', '\\');
				}
				Server::Response resp = Server::Response();
				//�����ļ�ϵͳ
				if (filesystem::exists(real_path)) {
					//�ļ����ڣ���ô�ͷ�������ļ�
					vector<string> ext_name_ = StrUtils::Split(real_path,".");
					string ext_name = StrUtils::ToLower(ext_name_[ext_name_.size() - 1]);
					resp.type = content_type[ext_name];
					resp.cookie["sessionid"] = sess_info.sessionid;
					//��ȡ�ļ�
					resp.data = File::FileStream::Read(real_path);
				}
				else {
					//�ļ������ڣ����ŵ��ú���
					try {
						resp = WebHandle::web_handle.at(req.url)->Handle(req);
						resp.cookie["sessionid"] = sess_info.sessionid;
					}
					catch (exception) {
						//����Ҳ�����ڣ�����404 NOT FOUND
						resp.resp_code = 404;
						resp.statement = "Not Found";
					}
				}
				instance->server_socket->ServerSend(info->ClientSocket,resp.CreateResponseString());
				//NetFunc::MySocket::CloseConnection(info->ClientSocket);
			}
			return;
		}
		thread http_handle_thread;
		Session sessions;
		static once_flag singletonFlag;
		static shared_ptr<WebServer> instance;
		//shared_ptr<MultiThread::ThreadPool> thread_pool = MultiThread::ThreadPool::CreateInstance(65536, WebServer::ThreadProc);
		shared_ptr<MultiThread::ThreadPool> pool;
		shared_ptr<NetFunc::MySocket> server_socket;
		static int session_interval;
		static unordered_map<string, string> content_type;
	};
	int Server::WebServer::session_interval = 0;
	once_flag Server::WebServer::singletonFlag;
	shared_ptr<Server::WebServer> Server::WebServer::instance;
	unordered_map<string, string> Server::WebServer::content_type = unordered_map<string, string>();
}