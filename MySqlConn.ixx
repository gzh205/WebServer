export module DBConnector.MySqlConn;
import <vector>;
import <string>;
import <sstream>;
import "WindowsFuncs.h";
import <algorithm>;
import <map>;
import Tools;
import Utils;
import Encryption.SHA1;
import StringBuilder;
import DBConnector.ResultSet;
import DBConnector.IConn;
using namespace std;
export namespace MySql {
	class Connection : public DBConnector::IConn {
	public:
		string version;
		string protocal;
		int threadID;
		string auth_plugin_name = "mysql_native_password";
		string charset;
		string server_charset;
		unordered_map<string, bool> options = unordered_map<string, bool>();
		unordered_map<string, bool> status = unordered_map<string, bool>();
		unordered_map<string, string> params = unordered_map<string, string>();
		vector<string> keys = vector<string>();
		Connection() {
			throw exception("������ʹ��Ĭ�Ϲ��캯��");
		}
		~Connection() {
			client_socket = nullptr;
		}
		/// <summary>
		/// ���캯��
		/// </summary>
		/// <param name="ip">IP��ַ</param>
		/// <param name="port">�˿ں�</param>
		/// <param name="username">�û���</param>
		/// <param name="password">����</param>
		/// <param name="client_name">�ͻ�������</param>
		Connection(string ip, int port, string username, string password, string client_name) {
			//�Ƚ���Ԥ����
			//������Ӳ���
			this->params.emplace("_os", "Windows");
			keys.push_back("_os");
			this->params.emplace("_client_name", client_name);
			keys.push_back("_client_name");
			this->params.emplace("_pid", WinOpts::GetStrPId());
			keys.push_back("_pid");
			this->params.emplace("_thread", WinOpts::GetStrThreadId());
			keys.push_back("_thread");
			this->params.emplace("_platform", "AMD64");
			keys.push_back("_platform");
			this->params.emplace("_client_version", "3.2.3");
			keys.push_back("_client_version");
			this->params.emplace("_server_host", ip);
			keys.push_back("_server_host");
			StrUtils::TLStringBuilder<char> para_sb = StrUtils::TLStringBuilder<char>();
			for (auto _k : this->keys) {
				string k = _k;
				string v = this->params[_k];
				char _l_k = k.length();
				char _l_v = v.length();
				para_sb.Append(_l_k + k);
				para_sb.Append(_l_v + v);
			}
			this->para = para_sb.ToString();
			//���������SHA1
			this->PreEncrpy(password);
			//��ʼ��socket
			this->client_socket = shared_ptr<NetFunc::MySocket>(new NetFunc::MySocket(ip, port));
			//���ݿ��¼
			StrUtils::TLStringBuilder<char> sb = StrUtils::TLStringBuilder<char>();
			string capabilities = { (char)0x85,(char)0xa6,(char)0xff,(char)0x80 };
			sb.Append(capabilities);
			string MAX_Packet = { (char)0,(char)0,(char)0,(char)0x40 };
			sb.Append(MAX_Packet);
			string character_set = { 0x21 };//���뷽ʽ
			sb.Append(character_set);
			string filler = string(23, (char)0);
			sb.Append(filler);
			//string username �û���
			string _end_flag = { 0 };
			sb.Append(username + _end_flag);
			this->pwd_insert_idx = 38 + username.length();
			string auth_response_length = { 20 };//��Ϊ������SHA1���ܺ�����ģ���SHA1���ȹ̶�Ϊ20
			sb.Append(auth_response_length);
			string pwd = string(20, 0);
			sb.Append(pwd);
			sb.Append(this->auth_plugin_name);
			sb.Append({ 0 });
			string _para_len = { (char)(this->para.length()) };
			sb.Append(_para_len);
			sb.Append(this->para);
			string _raw_data = sb.ToString();
			this->conn_data = CreateMySqlPackage(_raw_data)[0];
			this->conn_data[3] = 1;
		}
		/// <summary>
		/// ������
		/// </summary>
		void Open() override {
			//����TCP���Ӳ�����Server Greeting���ݰ�
			//ֱ�Ӷ�ȡ�������
			this->client_socket->Open();
			string _data = this->client_socket->Resv2();
			int retry_times = 100;
			for (int i = 0; i < 100; i++) {
				if (_data.length() < 4) {
					_data = this->client_socket->Resv2();
				}
				else {
					break;
				}
			}
			if (_data.length() > 4) {
				char* _data_bin = _data.data();
				int _length = _data.length();
				int _cursor = 4;
				this->protocal = _data_bin[_cursor];
				_cursor += 1;
				int _t_len = strlen(_data_bin + _cursor);
				this->version = _data.substr(_cursor, _t_len);
				_cursor += _t_len + 1;
				this->threadID = LittleEdion::MyString2Int4(_data.substr(_cursor, 4));
				_cursor += 4;
				string auth_plugin_data_part_1 = _data.substr(_cursor, 8);
				_cursor += 9;//��Ϊ�����һ����Զ��0�Ľ�����
				string capability_flags_1 = _data.substr(_cursor, 2);
				_cursor += 2;
				char charset = _data[_cursor];
				_cursor += 1;
				string status_flags = _data.substr(_cursor, 2);
				_cursor += 2;
				string capability_flags_2 = _data.substr(_cursor, 2);
				_cursor += 2;
				char auth_plugin_data_len = _data[_cursor];
				_cursor += 11; //auth_plugin_data_len���10���ֽڵ�reserved
				int _auth_2_len = auth_plugin_data_len - 8;
				string auth_plugin_data_part_2 = _data.substr(_cursor, _auth_2_len - 1);//_auth_2_len-1����Ϊauth_plugin_data_part_2���һ���ֽ�Ϊ0
				_cursor += _auth_2_len;
				this->auth_plugin_name = _data.substr(_cursor, strlen(_data_bin + _cursor));
				this->SetServerOptions(capability_flags_1, capability_flags_2, charset);
				//������ɵ�¼���ݿⱨ��
				StrUtils::TLStringBuilder<char> sb = StrUtils::TLStringBuilder<char>();
				string pwd = this->PwdEncrpy(auth_plugin_data_part_1 + auth_plugin_data_part_2);
				this->conn_data.replace(this->pwd_insert_idx, 20, pwd);
				this->client_socket->Send(this->conn_data);
				//�ȴ�����OK����
				string resp_dat = this->client_socket->Resv2();
				DBConnector::ResultSet set = DBConnector::ResultSet();
				unordered_map<string, string> res_map = this->ReadMySqlPackage(resp_dat, set);
				if (res_map["code"] == "error") {
					throw exception(res_map["data"].c_str());
				}
			}
			else {
				throw exception("δ�յ���������Ӧ");
			}
		}
		/// <summary>
		/// �ر�����
		/// </summary>
		void Close() override {
			string cmd_ = string({ 0x01 });
			string data_ = this->CreateMySqlPackage(cmd_)[0];
			this->client_socket->Send(data_);
			this->client_socket->Close();
		}
		/// <summary>
		/// ִ��SQL���ݣ������ؽ��
		/// ����true�����ѯ����ִ�У�false�����ѯʧ��
		/// ���������Ϣ����GetLastError
		/// ��Ҫ��ѯ���ȳ���255�ֽڵ�����,��Ϊ��ֱ�ӽ�int<lenenc>����int<1>������
		/// </summary>
		/// <param name="sql">sql���</param>
		/// /// <returns>�Ƿ�ɹ�</returns>
		pair<bool, string> ExecNonQuery(string sql) override {
			string _sql = string({ 3 }) + sql;
			vector<string> cmd = this->CreateMySqlPackage(_sql);
			StrUtils::TLStringBuilder sb = StrUtils::TLStringBuilder(string(""));
			for (string c : cmd) {
				sb.Append(c);
			}
			this->client_socket->Send(sb.ToString());
			string resv_data = this->client_socket->Resv2();
			DBConnector::ResultSet set = DBConnector::ResultSet();
			unordered_map<string, string> result_map = this->ReadMySqlPackage(resv_data, set);
			string dat_;
			bool b_res_ = true;
			if (result_map["code"] == "ok") {
				dat_ = "Ӱ������:" + result_map["affected_rows"];
			}
			else if (result_map["code"] == "error") {
				b_res_ = false;
				dat_ = result_map["data"];
			}
			return pair<bool, string>(b_res_, dat_);
		}
		/// <summary>
		/// ִ�в�ѯ������һ����
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		DBConnector::ResultSet ExecuteReader(string sql) override {
			DBConnector::ResultSet res = DBConnector::ResultSet();
			string _sql = string({ 3 }) + sql;
			vector<string> cmd = this->CreateMySqlPackage(_sql);
			StrUtils::TLStringBuilder sb = StrUtils::TLStringBuilder(string(""));
			for (string c : cmd) {
				sb.Append(c);
			}
			this->client_socket->Send(sb.ToString());
			while (true) {
				string resv_data = this->client_socket->Resv2();
				unordered_map<string, string> result_map = this->ReadMySqlPackage(resv_data, res);
				//if (result_map["code"] == "text resultset") {
				//	if ((unsigned char)result_map["index"][0] != cmd.size()) {
				//		throw exception("���ص����ݰ���Ŵ���");
				//	}
				//}
				if (result_map["code"] == "error") {
					res.error_message = result_map["data"];
					res.error = true;
					this->last_error = result_map["data"];
				}
				if (result_map["finish"] == "true") {
					break;
				}
			}
			return res;
		}
		/// <summary>
		/// ������һ�β�ѯ�Ĵ�����Ϣ
		/// </summary>
		/// <returns></returns>
		string GetLastError() {
			return this->last_error;
		}
		/// <summary>
		/// ѡ�����ݿ�
		/// </summary>
		string SelectDatabase(string database) {
			string select_ = string({ 2 }) + database;
			string data_ = this->CreateMySqlPackage(select_)[0];
			this->client_socket->Send(data_);
			string res = this->client_socket->Resv2();
			return res;
		}
		static inline vector<string> Capabilities_Flags{ "CLIENT_LONG_PASSWORD", "CLIENT_FOUND_ROWS", "CLIENT_LONG_FLAG",
		"CLIENT_CONNECT_WITH_DB","CLIENT_NO_SCHEMA","CLIENT_COMPRESS","CLIENT_ODBC","CLIENT_LOCAL_FILES","CLIENT_IGNORE_SPACE",
		"CLIENT_PROTOCOL_41","CLIENT_INTERACTIVE","CLIENT_SSL","CLIENT_IGNORE_SIGPIPE","CLIENT_TRANSACTIONS","CLIENT_RESERVED",
		"CLIENT_RESERVED2","CLIENT_MULTI_STATEMENTS","CLIENT_MULTI_RESULTS","CLIENT_PS_MULTI_RESULTS","CLIENT_PLUGIN_AUTH",
		"CLIENT_CONNECT_ATTRS","CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA","CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS","CLIENT_SESSION_TRACK",
		"CLIENT_DEPRECATE_EOF","CLIENT_OPTIONAL_RESULTSET_METADATA","CLIENT_ZSTD_COMPRESSION_ALGORITHM","CLIENT_QUERY_ATTRIBUTES",
		"MULTI_FACTOR_AUTHENTICATION","CLIENT_CAPABILITY_EXTENSION","CLIENT_SSL_VERIFY_SERVER_CERT","CLIENT_REMEMBER_OPTIONS" };
		static inline vector<string> Status_Flags{ "SERVER_STATUS_IN_TRANS","SERVER_STATUS_AUTOCOMMIT","SERVER_MORE_RESULTS_EXISTS",
			"SERVER_QUERY_NO_GOOD_INDEX_USED","SERVER_QUERY_NO_INDEX_USED","SERVER_STATUS_CURSOR_EXISTS","SERVER_STATUS_LAST_ROW_SENT",
			"SERVER_STATUS_DB_DROPPED","SERVER_STATUS_NO_BACKSLASH_ESCAPES","SERVER_STATUS_METADATA_CHANGED","SERVER_QUERY_WAS_SLOW",
			"SERVER_PS_OUT_PARAMS","SERVER_STATUS_IN_TRANS_READONLY","SERVER_SESSION_STATE_CHANGED" };
		//�ο�mysql�ĵ�https://dev.mysql.com/doc/dev/mysql-server/latest/group__group__cs__capabilities__flags.html
		string last_error;
	private:
		string conn_data;
		int pwd_insert_idx;
		string pwd1;
		string pwd2;
		string para;
		void PreEncrpy(string pwd) {
			this->pwd1 = Encryption::SHA1(pwd).finalize().raw_data();
			this->pwd2 = Encryption::SHA1(pwd1).finalize().raw_data();
		}
		/// <summary>
		/// MySql��������㷨
		/// </summary>
		/// <param name="random_data">��������������20λ�����</param>
		/// <param name="pwd">���ݿ��˺Ŷ�Ӧ������</param>
		/// <returns></returns>
		string PwdEncrpy(string random_data) {
			//SHA1( password ) XOR SHA1( "20-bytes random data from server" <concat> SHA1( SHA1( password ) ) )
			string sha1 = Encryption::SHA1(random_data + pwd2).finalize().raw_data();
			for (int i = 0; i < 20; i++) {
				sha1[i] = sha1[i] ^ pwd1[i];
			}
			return sha1;
		}
		void SetServerStatus(string status) {
			unsigned int _capbility = LittleEdion::MyString2Int(status);
			int _idx = 0;
			for (string s : Connection::Capabilities_Flags) {
				unsigned int _t = (1 << _idx) & _capbility;
				this->options.emplace(s, _t != 0);
				_idx += 1;
			}
		}
		void SetServerOptions(string capability_flags_1, string capability_flags_2, char charset) {
			switch (charset) {
			case  0xc0: {
				this->server_charset = "utf8_unicode_ci";
			}break;
			default: {
				this->server_charset = "unknown";
			}break;
			}
			unsigned int _capbility = BigEdion::MyString2Int4({ capability_flags_2[1],capability_flags_2[0],capability_flags_1[1], capability_flags_1[0] });
			int _idx = 0;
			for (string s : Connection::Capabilities_Flags) {
				unsigned int _t = (1 << _idx) & _capbility;
				this->options.emplace(s, _t != 0);
				_idx += 1;
			}
		}
		string GetServerOptions() {
			unsigned int _capbility = 0;
			int _idx = 0;
			for (string s : Connection::Capabilities_Flags) {
				if (this->options[s]) {
					_capbility += (1 << _idx);
				}
				_idx += 1;
			}
			string res = BigEdion::MyInt2String(_capbility);
			reverse(res.begin(), res.end());
			return res;
		}
		shared_ptr<NetFunc::MySocket> client_socket;
		vector<string> CreateMySqlPackage(string& data) {
			//ÿ�����ݰ��Ĵ�С���Ϊ2^24�ֽ�
			int max_length = 0x1000000 - 1;
			vector<string> packages = vector<string>();
			string tmp = data;
			string _id = { 0 };
			int _data_len = data.length();
			while (true) {
				if (_data_len < max_length) {
					string _len = LittleEdion::MyInt2String(tmp.length());
					packages.push_back(_len + _id + tmp);
					break;
				}
				else {
					char _FF = 0xFF;
					string _header = { _FF,_FF,_FF, 0 };
					string _t = tmp.substr(0, max_length);
					tmp = tmp.substr(max_length, tmp.length() - max_length);
					packages.push_back(_header + _t);
				}
			}
			return packages;
		}
		/// <summary>
		/// �÷���ר�����ڶ�ȡTextResult�е���
		/// </summary>
		/// <param name="data_"></param>
		/// <returns></returns>
		vector<string> ReadColPackage(string data_) {
			switch (data_[0]) {
				case 0xFE:{//����һ��EOF��
					vector<string> result = vector<string>();
					result.push_back("PACKAGE_EOF");
					return result;
				}default: {//�������ݰ�(column)
					vector<string> result = vector<string>();
					int idx = 0;
					//����Catalog,Database,Table,Origin Table,Name,Origin Name,��������ֻ��Name
					for(int i=0;i<6;i++) {
						int field_len = (unsigned char)data_[idx];
						idx += 1;
						result.push_back(data_.substr(idx, field_len));
						idx += field_len;
					}
					return result;
				}
			}
		}
		/// <summary>
		/// �÷���ר�����ڶ�ȡTextResult�е���
		/// </summary>
		/// <param name="data_"></param>
		/// <returns></returns>
		vector<string> ReadRowPackage(string data_,int col_num) {
			switch ((unsigned char)data_[0]) {
			case 0xFE: {//����һ��EOF��
				vector<string> result = vector<string>();
				result.push_back("PACKAGE_EOF");
				return result;
			}default: {//�������ݰ�(row)
				vector<string> result = vector<string>();
				int idx = 0;
				for (int i = 0; i < col_num; i++) {
					int field_len = (unsigned char)data_[idx];
					idx += 1;
					result.push_back(data_.substr(idx, field_len));
					idx += field_len;
				}
				return result;
			}
			}
		}
		/// <summary>
		/// ����EOF��
		/// </summary>
		/// <param name="data"></param>
		/// <param name="idx"></param>
		/// <returns></returns>
		int SkipEofPackage(string data,int idx) {
			int sub_package_len = LittleEdion::MyString2Int3(data.substr(idx, 3));
			idx += 3;
			int sub_package_num = (unsigned char)data[idx];
			idx += 1;
			idx += sub_package_len;
			return idx;
		}
		/// <summary>
		/// ��ȡmysql��Ӧ����
		/// </summary>
		/// <param name="data"></param>
		/// <param name="set"></param>
		/// <returns></returns>
		unordered_map<string, string> ReadMySqlPackage(string& data, DBConnector::ResultSet& set) throw(exception) {
			unordered_map<string, string> result = unordered_map<string, string>();
			int package_len = LittleEdion::MyString2Int3(data.substr(0, 3));
			result["length"] = data.substr(0, 3);
			result["index"] = string({ data[3] });
			result["next_idx"] = string({ data[3] });
			result["finish"] = "true";
			switch ((unsigned char)data[4]) {
			case 0xFF: {
				//�����ˣ�����������ʹ�����Ϣ
				result["code"] = "error";
				char* szDst = new char[5];
				_itoa((int)LittleEdion::MyString2Int(data.substr(5, 2)), szDst, 10);
				string err_cod = "error_code:" + string(szDst) + "\nmessage:" + string(data.substr(6));
				result["data"] = err_cod;
			}break;
			case 0:
			case 0xFE: {
				//OK
				int idx = 5;
				result["code"] = "ok";
				result["affected_rows"] = GetLenenc(data, idx);
				result["last_insert_id"] = GetLenenc(data, idx);
				string tmp = data.substr(idx, 2);
				result["status_flags"] = tmp;
				this->SetServerStatus(tmp);
				idx += 2;
				if (this->options["CLIENT_PROTOCOL_41"]) {
					result["warnings"] = data.substr(idx, 2);
					idx += 2;
				}
				else if (this->options["CLIENT_TRANSACTIONS"]) {
					result["warnings"] = "";
				}
				else {
					result["status_flags"] = "";
					result["warnings"] = "";
				}
				if (this->options["CLIENT_SESSION_TRACK"]) {
					int len_ = LittleEdion::MyString2Int(this->GetLenenc(data, idx));
					if (len_ == 0) {
						result["info"] = "";
						idx -= 1;
					}
					else {
						result["info"] = data.substr(idx, len_);
					}
				}
				if (this->status["SERVER_SESSION_STATE_CHANGED"]) {
					int len_ = LittleEdion::MyString2Int(this->GetLenenc(data, idx));
					if (len_ != 0) {
						result["session_state_info"] = data.substr(idx, len_);
						idx -= 1;
					}
					else {
						result["session_state_info"] = "";
					}
				}
				if (idx >= data.length()) {
					result["info"] = "";
					break;
				}
				else {
					result["info"] = data.substr(idx);
				}
			}break;
			case 2: {
				//use database;
				result["code"] = "use database";
				result["data"] = data.substr(5);
			}
			default: {
				if (LittleEdion::MyString2Int3(result["length"]) == 1) {
					result["finish"] = "false";
					set = DBConnector::ResultSet();
					//Text Resultset
					//mysql�����ĵ�:https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_com_query_response_text_resultset.html
					int idx = 4;
					int column_num = LittleEdion::MyString2Int(data.substr(idx, package_len));
					idx += package_len;
					//��ȡ����Ϣ
					vector<string> col_name = vector<string>();
					for (int k = 0; k < column_num; k++) {
						int sub_package_len = LittleEdion::MyString2Int3(data.substr(idx, 3));
						idx += 3;
						int sub_package_num = (unsigned char)data[idx];
						idx += 1;
						vector<string> package_info = ReadColPackage(data.substr(idx, sub_package_len));
						if (package_info[0] == "PACKAGE_EOF") {
							break;
						}
						if (package_info.size() > 1) {
							col_name.push_back(package_info[package_info.size() - 2]);
						}
						idx += sub_package_len;
					}
					set.SetColName(col_name);
					//EOF��
					idx = SkipEofPackage(data, idx);
					//��ȡ����Ϣ
					while (idx<data.length()) {
						int sub_package_len = LittleEdion::MyString2Int3(data.substr(idx, 3));
						idx += 3;
						int sub_package_num = (unsigned char)data[idx];
						idx += 1;
						vector<string> package_info = ReadRowPackage(data.substr(idx, sub_package_len), column_num);
						if (package_info[0] == "PACKAGE_EOF") {
							break;
						}
						set.Put(DBConnector::RowSet(package_info));
						idx += sub_package_len;
					}
					result["finish"] = "true";
				}
				else {
					throw exception("δ֪�ı��ĸ�ʽ");
				}
			}break;
			}
			return result;
		}
		string GetLenenc(string& data, int& len) {
			char buf_ = data[len];
			string intstr = string({ buf_ });
			len += 1;
			if ((unsigned char)buf_ >= 0xFC) {
				if (buf_ == 0xFC) {
					intstr = data.substr(len + 1, 2);
					len += 2;
				}
				else if (buf_ == 0xFD) {
					intstr = data.substr(len + 1, 3);
					len += 3;
				}
				else if (buf_ == 0xFE) {
					intstr = data.substr(len + 1, 8);
					len += 8;
				}
				else {
					throw exception("Э���ʽ����");
				}
			}
			return intstr;
		}
	};
}