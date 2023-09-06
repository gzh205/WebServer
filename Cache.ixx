export module PF.Cache;
import LRUList;
import <memory>;
import <unordered_map>;
import <string>;
import <cctype>;
import PF.Table;
import DBConnector.IConn;
import Utils;
import DBConnector.MySqlConn;
import ThreadPool;
import <mutex>;
import <shared_mutex>;
import StringBuilder;
import DBConnector.ResultSet;
using namespace std;
export namespace MyDatabase {
	/// <summary>
	/// ���ݳ־û���ܵĻ���
	/// </summary>
	class Cache {
	public:
		Cache() {
			key_list = Tools::LRUList<string, pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>>(65536);
		}
		Cache(int size) {
			key_list = Tools::LRUList<string, pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>>(size);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="key"></param>
		/// <param name="pks"></param>
		/// <returns></returns>
		shared_ptr<unordered_map<string, string>> Get(string& key,vector<string>& pks) {
			string cache_get_str = key + "::";
			string table_key_str = "";
			for (string pk : pks) {
				table_key_str.append(pk + ',');
			}
			cache_get_str += table_key_str;
			mutex_lru.lock_shared();
			auto res = key_list.Use(cache_get_str);
			mutex_lru.unlock_shared();
			shared_ptr<unordered_map<string, string>> result = nullptr;
			auto dat_ = res.first->table_map[table_key_str];
			if (dat_ == nullptr) {
				//���������û�и�����
				//���û�оͶ�ȡ���ݿⲢ������뻺��
				//�ȶ�ȡ���ݿ�
				string sql = "select ";
				StrUtils::TLStringBuilder sb = StrUtils::TLStringBuilder(sql);
				auto all_cols_ = res.first->columns;
				string last_ = *(all_cols_.end() - 1);
				for (string s : all_cols_) {
					sb.Append(s);
					if (s == last_)
						sb.Append(",");
				}
				sb.Append(" from ");
				sb.Append(res.first->table_name);
				sb.Append(" where ");
				auto pk_cols_ = res.first->primary_keys;
				last_ = *(pk_cols_.end() - 1);
				for (int i = 0; i < pk_cols_.size();i++) {
					sb.Append(pk_cols_[i] + "=\"" + pks[i] + "\"");
				}
				sb.Append(";");
				res.second->Open();
				DBConnector::ResultSet res_set;
				try {
					res_set = res.second->ExecuteReader(sql);
				}
				catch (exception e) {
					cout << e.what();
				}
				res.second->Close();
				if (res_set.RowLen() > 0) {
					DBConnector::RowSet row = res_set[0];
					shared_ptr<unordered_map<string, string>> map_ = shared_ptr<unordered_map<string, string>>();
					int idx_ = 0;
					for (string col : res_set.column_name) {
						(*map_)[col] = *row.Get(idx_++);
					}
					//���뻺��
					mutex_lru.lock();
					res.first->table_map[key] = map_;
					mutex_lru.unlock();
				}
			}
			else {
				result = (*res.first).table_map[key];
			}
			return result;
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="key"></param>
		/// <param name="set"></param>
		void Set(string key,unordered_map<string, string> set,int flag) {
			switch (flag) {
			case 0: {
				//insert
				//�Ȳ�����߸��ǻ��棬Ȼ��д�����ݿ�

			}break;
			case 1: {
				//update
			}break;
			case 2: {
				//delete
			}break;
			}
		}
		/// <summary>
		/// ���һ�����ݿ�����
		/// </summary>
		/// <param name="key"></param>
		/// <param name="ip"></param>
		/// <param name="port"></param>
		/// <param name="username"></param>
		/// <param name="pwd"></param>
		/// <param name="driver"></param>
		/// <param name="pk"></param>
		/// <param name="cols"></param>
		void AddConn(string key,string ip,string port,string username, string pwd,string driver,vector<string> pk,vector<string> cols) {
			DBConnector::IConn* conn = nullptr;
			if (StrUtils::ToLower(driver) == "mysql") {
				conn = new MySql::Connection(ip, atoi(port.c_str()), username, pwd, "SqlReader");
			}
			else if (StrUtils::ToLower(driver) == "sqlserver") {
				//���Ҳûʵ����
			}
			else {
				throw exception("��δʵ�ֳ���mysql��sqlserver��������ݶ�ȡ��ʽ");
			}
			key_list.Add(key,pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>(shared_ptr<Table>(new Table(pk,cols)), shared_ptr<DBConnector::IConn>(conn)));
		}
		static void ThreadProc(void* sql) {

		}
	private:
		Tools::LRUList<string, pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>> key_list;
		//unordered_map<string, pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>> conn_map = unordered_map<string, pair<shared_ptr<Table>, shared_ptr<DBConnector::IConn>>>();
		shared_ptr<MultiThread::ThreadPool> thread_pool = MultiThread::ThreadPool::CreateInstance(16, Cache::ThreadProc);
		shared_mutex mutex_lru;
		//mutable std::shared_mutex mutex_lru;
		//mutable std::shared_mutex mutex_table;
	};
}