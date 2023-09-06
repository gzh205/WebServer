export module PF.Core;
//import PF.Cache;
import DBConnector.MySqlConn;
import ThreadPool;
import <iostream>;
import <unordered_map>;
import <string>;
import <mutex>;
import <functional>;
import <fstream>;
import <filesystem>;
import <cctype>;
import Serialize.XMLSerializer;
import Reflection.TemplateClass;
import Utils;
//import PF.Cache;
using namespace std;
export namespace MyDatabase{
	class Connection {
	public:
		static shared_ptr<Connection> GetConnection() {
			call_once(singletonFlag, [&] {
				instance = std::shared_ptr<Connection>(new Connection());
			});
			return instance;
		}
		static shared_ptr<Connection> GetConnection(string k, MySql::Connection v) {
			call_once(singletonFlag, [&] {
				instance = std::shared_ptr<Connection>(new Connection());
				});
			return instance;
		}
		static void Init(string model_folder) {
			for (auto& i : filesystem::directory_iterator(model_folder)) {
				string name = i.path().string();
				int len = name.length();
				//分割文件名
				vector<string> pathes_ = StrUtils::Split(name, "\\");
				string filename = *(pathes_.end() - 1);//获取最后一个元素
				//去除扩展名
				pathes_ = StrUtils::Split(name, ".");
				if (pathes_.size() < 2) {
					continue;
				}
				string extname = *(pathes_.end() - 1);//获取最后一个元素
				if (StrUtils::ToLower(extname) != "ixx") {
					continue;
				}
				//读取文件
				std::ifstream fin(name);
				fin.seekg(0, std::ios::end);
				size_t length = fin.tellg();
				fin.seekg(0, std::ios::beg);
				char* buffer = new char[length];
				fin.read(buffer, length);
				string xml_doc = string(buffer);
				shared_ptr<Serialize::XMLItem> doc_tree = Serialize::XMLSerializer::DeSerialize(xml_doc);
				//获取连接名和表名
				for (auto item : doc_tree->subnodes) {
					if (item->element_type == "table") {
						//MyDatabase::Table table = MyDatabase::Table();
						vector<string> pks = vector<string>();
						vector<string> pro = vector<string>();
						for (auto cols : item->subnodes) {
							if (cols->element_type == "col" && cols->attributes.contains("name")) {
								if (cols->attributes.contains("primary_key") && cols->attributes["primary_key"] == "true") {
									pks.push_back(cols->attributes["name"]);
								}
								else {
									pro.push_back(cols->attributes["name"]);
								}
							}
						}

						break;
					}
				}
			}
		}
		string SelectOne(string conn_name,vector<string> primary_key) {
			return "";
		}
		shared_ptr<Reflection::TemplateClass> SelectSome(string conn_name,string where) {
			shared_ptr<Reflection::TemplateClass> dao_ = shared_ptr<Reflection::TemplateClass>(new Reflection::TemplateClass());

			return dao_;
		}
		bool Insert() {
			return false;
		}
		bool Update() {
			return false;
		}
		bool Delete() {
			return false;
		}
	private:
		static void _cdecl ThreadProc(void* sql) {

		}
		Connection() {
			thread_pool = MultiThread::ThreadPool::CreateInstance(16, ThreadProc);
		}
		static shared_ptr<Connection> instance;
		static once_flag singletonFlag;
		static shared_ptr<MultiThread::ThreadPool> thread_pool;
		// 数据库连接字典
		//static Cache cache;
		//缓存数据
	};
	shared_ptr<Connection> Connection::instance;
	once_flag Connection::singletonFlag;
	shared_ptr<MultiThread::ThreadPool> Connection::thread_pool = MultiThread::ThreadPool::CreateInstance(16, Connection::ThreadProc);
	//Cache Connection::cache = Cache(65536);
}