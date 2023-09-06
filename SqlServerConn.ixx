export module DBConnector.SqlServerConn;
#pragma once
import Tools;
import Utils;
import "WindowsFuncs.h";
import <memory>;
import <vector>;
using namespace std;

export namespace SqlServer {
	class PreLoginToken {
	public:
		unsigned char TokenType;
		vector<unsigned char> TokenPosition;
		vector<unsigned char> TokenLeng;
		vector<unsigned char> GetVector() {
			vector<unsigned char> result = vector<unsigned char>();
			result.emplace_back(this->TokenType);
			result.emplace_back(this->TokenPosition[0]);
			result.emplace_back(this->TokenPosition[1]);
			result.emplace_back(this->TokenLeng[0]);
			result.emplace_back(this->TokenLeng[1]);
			return result;
		}
		/// <summary>
		/// 初始化预登陆Token，参考MS-TDS 2.2.6.4 PreLogin报文
		/// </summary>
		/// <param name="TokenType">类型</param>
		/// <param name="TokenPosition">位置</param>
		/// <param name="TokenLength">长度</param>
		PreLoginToken(unsigned char TokenType, vector<unsigned char> TokenPosition, vector<unsigned char> TokenLeng) {
			this->TokenType = TokenType;
			this->TokenPosition = TokenPosition;
			this->TokenLeng = TokenLeng;
		}
		~PreLoginToken() {
			this->TokenPosition.clear();
			this->TokenLeng.clear();
			delete& TokenType;
			delete& TokenPosition;
			delete& TokenLeng;
		}
	};
	/// <summary>
	/// 数据库连接对象，用于创建数据库连接并用该对象执行SQL语句
	/// </summary>
	class Connection {
	public:
		string ip;
		int port;
		string username;
		string password;
		string hostname;
		string appname;//app名称
		string initial_catalog;//默认数据库
		vector<unsigned char> LCID;
		vector<unsigned char> PID;
		vector<unsigned char> TDSVersion;//TDS版本
		vector<unsigned char> ClientProgVer;//接口库的版本，如ODBC或OLEDB
		vector<unsigned char> ClientTimeZone;//客户端时区
		vector<unsigned char> ClientMac;//客户端MAC
		string instname = "MSSQLServer";
		vector<unsigned char> GUID_CONNID;
		vector<unsigned char> ACTIVITYID;
		shared_ptr<NetFunc::MySocket> socket;
		string server_version;
		/// <summary>
		/// 构造函数
		/// </summary>
		[[deprecated("应该使用带参数的构造函数")]]
		Connection() {
			this->PID = VectorPots::Byte2Vec(WinOpts::GetPID(), 2);
			this->TDSVersion = { 0x02, 0x00, 0x09, 0x72 };//tds版本为2.0.9.114
			this->ClientProgVer = { 0x00, 0x00, 0x00, 0x07 };//接口库版本为7
			this->appname = "website";
			this->ClientTimeZone = { 0xc0,0x03,0x00,0x00 };//推测是东八区，因为示例中1e0是西八区
			this->ClientMac = VectorPots::Byte2Vec(WinOpts::GetClientMac(), 6);
			this->GUID_CONNID = StrUtils::RandomByteGenerator(16);
			this->ACTIVITYID = StrUtils::RandomByteGenerator(20);

		}
		/// <summary>
		/// 构造函数
		/// </summary>
		/// <param name="ip">ip地址</param>
		/// <param name="port">端口号，默认为1433</param>
		/// <param name="username">用户名</param>
		/// <param name="password">密码</param>
		/// <param name="initial_catalog">默认数据库</param>
		Connection(string ip, int port, string username, string password, string initial_catalog) {
			this->PID = VectorPots::Byte2Vec(WinOpts::GetPID(), 2);
			this->TDSVersion = { 0x02, 0x00, 0x09, 0x72 };//tds版本为2.0.9.72
			this->ClientProgVer = { 0x00, 0x00, 0x00, 0x07 };//接口库版本为7
			this->ClientTimeZone = { 0xc0,0x03,0x00,0x00 };
			this->ClientMac = VectorPots::Byte2Vec(WinOpts::GetClientMac(), 6);
			this->ip = ip;
			this->port = port;
			this->username = username;
			this->password = password;
			this->initial_catalog = initial_catalog;
			this->appname = "website";
			this->GUID_CONNID = StrUtils::RandomByteGenerator(16);
			this->ACTIVITYID = StrUtils::RandomByteGenerator(20);
		}
		/// <summary>
		/// 设置通信的语言，防止乱码
		/// </summary>
		void SetLanguage(vector<unsigned char> language) {
			this->LCID = language;
		}
		void SetHostName(string name) {
			this->hostname = name;
		}
		/// <summary>
		/// 设置客户端时区,时区编码参考RFC3339
		/// </summary>
		/// <param name="TimeZone">TimeZone编码</param>
		void SetTimeZone(vector<unsigned char> TimeZone) {
			this->ClientTimeZone = TimeZone;
		}

		//预登陆请求
		void PreLogin() {
			shared_ptr<NetFunc::TDSHeader> tds_header = shared_ptr<NetFunc::TDSHeader>(new NetFunc::TDSHeader());
			tds_header->Type = 0x12;//login报文类型为0x10
			tds_header->Status = 0x01;//
			tds_header->PacketID = 0x01;
			tds_header->WINDOW = 0;
			// tds头部创建完成，接下来创建tds数据部分
			vector<vector<unsigned char>> data = vector<vector<unsigned char>>();
			data.push_back({ 0x04, 0x08, 0x11, 0xb8, 0, 0 });//VERSION
			data.push_back({ 2 });//ENCRYPTION: 0表示不加密，1表示加密，2表示加密不可用，3表示需要加密
			vector<unsigned char> _inst = { 0 };
			if (this->instname != "MSSQLServer") {
				_inst = VectorPots::String2Bytes(this->instname);
			}
			data.emplace_back(_inst);//INSTOPT
			data.emplace_back(WinOpts::GetThreadId());//THREADID
			data.push_back({ 0 });//MARS
			data.emplace_back(VectorPots::AppendVector(this->GUID_CONNID, this->ACTIVITYID));//TraceID是由16字节的GUID_CONNID和20字节的ACTIVITYID拼接而成
			data.push_back({ 0x01 });//FedAuthRequired;如果客户端的 PRELOGIN 请求消息 包含 FEDAUTHREQUIRED 选项，客户端必须将0x01指定为 B_FEDAUTHREQUIRED值。
			unsigned short int data_begin_idx = 5 * data.size() + 1;//数据偏移最后+1因为要算上group结束符0xFF
			vector<shared_ptr<PreLoginToken>> position = vector<shared_ptr<PreLoginToken>>();
			int _i = 0;
			for (auto data_i : data) {
				int _len = data_i.size();
				position.emplace_back(shared_ptr<PreLoginToken>(new PreLoginToken((unsigned char)_i++, BigEdion::Short2Vec(data_begin_idx), BigEdion::Short2Vec((unsigned short int)_len))));
				data_begin_idx += _len;
			}
			//组装报文
			vector<unsigned char> _tds_data = vector<unsigned char>();
			for (shared_ptr<PreLoginToken> pos : position) {
				_tds_data = VectorPots::AppendVector(_tds_data, pos->GetVector());
			}
			_tds_data.emplace_back((unsigned char)0xff);//group 结束符0xff
			for (vector<unsigned char> d : data) {
				_tds_data = VectorPots::AppendVector(_tds_data, d);
			}
			vector<unsigned char> tds_data = CreateTDS(tds_header, _tds_data);
			this->socket = make_shared<NetFunc::MySocket>(this->ip.data(), this->port);
			socket->Open();
			socket->Send(tds_data);
			vector<unsigned char> _data = vector<unsigned char>();// socket->Resv();
			pair<shared_ptr<NetFunc::TDSHeader>, unordered_map<string, vector<unsigned char>>> _res = this->ReadTDS(_data);
			if (_res.first->Type == 4 && _res.first->Length == _data.size()) {
				vector<unsigned char> version = _res.second["VERSION"];
				//this->server_version = "" + (int)version[0] + "." + (int)version[1] + "." + (int)BigEdion::Bytes2ShortInt({ version[2], })
			}
			socket->Close();
		}
		//登录
		void Login() {
			shared_ptr<NetFunc::TDSHeader> tds_header = shared_ptr<NetFunc::TDSHeader>(new NetFunc::TDSHeader());
			tds_header->Type = 0x10;//login报文类型为0x10
			tds_header->Status = 0x01;//
			tds_header->PacketID = 0x01;
			tds_header->WINDOW = 0;
			// tds头部创建完成，下面创建tds数据部分。tds数据部分又包含login的头部，所以首先创建login头部
			vector<unsigned char> TDS_version = this->TDSVersion;
			vector<unsigned char> packet_size = LittleEdion::Int2Byte(0x1000);//包的大小为4096
			vector<unsigned char> client_prog_ver = this->ClientProgVer;
			vector<unsigned char> client_pid = this->PID;
			vector<unsigned char> connection_id = { 0, 0, 0, 0 };
			unsigned char option_flags1 = 0xe0;
			unsigned char option_flags2 = 0x03;
			unsigned char type_flags = 0x00;
			unsigned char option_flags3 = 0x00;
			vector<unsigned char> client_time_zone = this->ClientTimeZone;//客户端时区编码，参考RFC3339
			vector<unsigned char> client_LCID = VectorPots::ReverseVector(Language::zh_CN);//客户端LCID值，参考RFC1766
			//先构造数据，再根据data构建offset
			vector<unsigned char> host_name = VectorPots::UcsString2Bytes(this->hostname);
			vector<unsigned char> user_name = VectorPots::UcsString2Bytes(this->username);
			vector<unsigned char> password = VectorPots::UcsString2Bytes(this->password);
			vector<unsigned char> appname = VectorPots::UcsString2Bytes(this->appname);
			vector<unsigned char> database = VectorPots::UcsString2Bytes(this->initial_catalog);
			vector<unsigned char> clt_int_name = VectorPots::UcsString2Bytes("ODBC");
			//计算数据的位置
			unsigned short int host_name_addr = 0x5e;
			unsigned short int user_name_addr = host_name_addr + host_name.size();
			unsigned short int password_addr = user_name_addr + user_name.size();
			unsigned short int appname_addr = password_addr + password.size();
			unsigned short int database_addr = appname_addr + appname.size();
			unsigned short int clt_int_name_addr = database_addr + database.size();
			//OffsetLength，所有字段长度都为2
			//ib开头是起始地址，cch开头表示长度
			vector<unsigned char> ib_host_name = LittleEdion::Short2Vec(host_name_addr);//客户端计算机名称
			vector<unsigned char> cch_host_name = LittleEdion::Short2Vec((unsigned short int)host_name.size() / 2);
			vector<unsigned char> ib_user_name = LittleEdion::Short2Vec(user_name_addr);//用户标识，例如sqlserver默认的sa用户
			vector<unsigned char> cch_user_name = LittleEdion::Short2Vec((unsigned short int)user_name.size() / 2);
			vector<unsigned char> ib_password = LittleEdion::Short2Vec(password_addr);//客户端提供的密码
			vector<unsigned char> cch_password = LittleEdion::Short2Vec((unsigned short int)password.size() / 2);
			vector<unsigned char> ib_app_name = LittleEdion::Short2Vec(appname_addr);//客户端应用程序名称
			vector<unsigned char> cch_app_name = LittleEdion::Short2Vec((unsigned short int)appname.size() / 2);
			vector<unsigned char> ib_server_name = { 0x80, 0 };//服务器名称
			vector<unsigned char> cch_server_name = { 0, 0 };
			vector<unsigned char> ib_unused = { 0x80, 0 };//保留字段
			vector<unsigned char> cb_unused = { 0, 0 };
			vector<unsigned char> ib_clt_int_name = LittleEdion::Short2Vec(clt_int_name_addr);//接口库名称,指向报文末尾的"ODBC"
			vector<unsigned char> cch_clt_int_name = LittleEdion::Short2Vec((unsigned short int)clt_int_name.size() / 2);
			vector<unsigned char> ib_language = { 0x80, 0 };;//初始语言
			vector<unsigned char> cch_language = { 0, 0 };
			vector<unsigned char> ib_database = LittleEdion::Short2Vec(database_addr);//初始数据库
			vector<unsigned char> cch_database = LittleEdion::Short2Vec((unsigned short int)database.size() / 2);
			vector<unsigned char> client_ID = this->ClientMac;//客户端ID是MAC地址
			vector<unsigned char> ib_SSPI = { 0x80, 0 };//SSPI data
			vector<unsigned char> cb_SSPI = { 0, 0 };
			vector<unsigned char> ib_atch_DBfile = { 0x80, 0 };//要在连接期间附加的数据库的文件名
			vector<unsigned char> cch_atch_DBfile = { 0, 0 };
			vector<unsigned char> ib_change_password = { 0x80, 0 };//指定登录的新密码
			vector<unsigned char> cch_change_password = { 0, 0 };
			vector<unsigned char> cbSSPILong = { 0, 0, 0, 0 };//用于大型SSPI cbSSPI==USHORT_MAX
			//创建数组
			vector<unsigned char> data = vector<unsigned char>();
			vector<unsigned char> length = LittleEdion::Int2Byte(93 + host_name.size() + user_name.size() + password.size() + appname.size() + database.size());
			data = VectorPots::AppendVector(data, length);
			data = VectorPots::AppendVector(data, TDS_version);
			data = VectorPots::AppendVector(data, packet_size);
			data = VectorPots::AppendVector(data, client_prog_ver);
			data = VectorPots::AppendVector(data, client_pid);
			data = VectorPots::AppendVector(data, connection_id);
			data.emplace_back(option_flags1);
			data.emplace_back(option_flags2);
			data.emplace_back(type_flags);
			data.emplace_back(option_flags3);
			data = VectorPots::AppendVector(data, client_time_zone);
			data = VectorPots::AppendVector(data, client_LCID);
			//OffsetLength
			data = VectorPots::AppendVector(data, ib_host_name);
			data = VectorPots::AppendVector(data, cch_host_name);
			data = VectorPots::AppendVector(data, ib_user_name);
			data = VectorPots::AppendVector(data, cch_user_name);
			data = VectorPots::AppendVector(data, ib_password);
			data = VectorPots::AppendVector(data, cch_password);
			data = VectorPots::AppendVector(data, ib_app_name);
			data = VectorPots::AppendVector(data, cch_app_name);
			data = VectorPots::AppendVector(data, ib_server_name);
			data = VectorPots::AppendVector(data, cch_server_name);
			data = VectorPots::AppendVector(data, ib_unused);
			data = VectorPots::AppendVector(data, cb_unused);
			data = VectorPots::AppendVector(data, ib_clt_int_name);
			data = VectorPots::AppendVector(data, cch_clt_int_name);
			data = VectorPots::AppendVector(data, ib_language);
			data = VectorPots::AppendVector(data, cch_language);
			data = VectorPots::AppendVector(data, ib_database);
			data = VectorPots::AppendVector(data, cch_database);
			data = VectorPots::AppendVector(data, client_ID);
			data = VectorPots::AppendVector(data, ib_SSPI);
			data = VectorPots::AppendVector(data, cb_SSPI);
			data = VectorPots::AppendVector(data, ib_atch_DBfile);
			data = VectorPots::AppendVector(data, cch_atch_DBfile);
			data = VectorPots::AppendVector(data, ib_change_password);
			data = VectorPots::AppendVector(data, cch_change_password);
			data = VectorPots::AppendVector(data, cbSSPILong);
			vector<unsigned char> tds_msg = this->CreateTDS(tds_header, data);
			socket->Open();
			socket->Send(tds_msg);
			//pair<NetFunc::TDSHeader, vector<unsigned char>> received_data = socket->TDSResv();
			socket->Close();
			unsigned char TokenType = data[0];

		}
		//SQL命令
		void SqlCommand() {

		}
		//数据批量操作
		void BulkOperation() {

		}
		//远程过程调用
		void RPC() {

		}
		//提醒
		void Attention() {

		}
	private:
		//创建TDS报文
		//一个报文最长为2^24-1个字节，如果长度过长，则需要设置PacketID分组发送
		vector<unsigned char> CreateTDS(shared_ptr<NetFunc::TDSHeader> tdshead, vector<unsigned char> info) {
			//创建报文头部
			/*	Type: （1字节，unsigned char）标识当前TDS数据包的类型。
				值 描述 数据
				1 sql命令或批处理 有
				2 登陆（TDS4.2） 有
				3 RPC 有
				4 服务端对客户端的数据响应 有
				5 Unused ⽆
				6 提醒信号 有
				7 批量数据操作 有
				8-13 Unused. ⽆
				14 事务管理请求 有
				15-16 Unused ⽆
				17 SSPI消息 有
				18 预登陆 有
			*/
			unsigned short int length = info.size() + 8;
			vector<unsigned char> header = vector<unsigned char>(8);
			unsigned char* Length = BigEdion::Short2Byte(length);
			header[0] = tdshead->Type;
			header[1] = tdshead->Status;
			header[2] = Length[0];
			header[3] = Length[1];
			header[4] = this->PID[0];
			header[5] = this->PID[1];
			header[6] = tdshead->PacketID;
			header[7] = tdshead->WINDOW;
			// 报文头部创建完成，接下来创建报文内容
			// 创建完整报文数组，并将头部和数据依次插入
			vector<unsigned char> res = VectorPots::AppendVector<unsigned char>(header, info);
			return res;
		}
		/// <summary>
		/// 读取TDS报文，提取二进制报文头部
		/// </summary>
		/// <returns></returns>
		pair<shared_ptr<NetFunc::TDSHeader>, unordered_map<string, vector<unsigned char>>> ReadTDS(vector<unsigned char> tds_data) {
			shared_ptr<NetFunc::TDSHeader> tds_header = shared_ptr<NetFunc::TDSHeader>();
			tds_header->Type = tds_data[0];
			tds_header->Status = tds_data[1];
			tds_header->Length = BigEdion::Bytes2ShortInt({ tds_data[2],tds_data[3] });
			tds_header->SPID = BigEdion::Bytes2ShortInt({ tds_data[4],tds_data[5] });
			tds_header->PacketID = tds_data[6];
			tds_header->WINDOW = tds_data[7];
			unordered_map<string, vector<unsigned char>> prelogin_msg = unordered_map<string, vector<unsigned char>>();
			switch (tds_header->Type) {
			case 0x04: {
				//PreLogin的请求响应				
				for (int i = 8; i < tds_data.size(); i += 5) {
					if (tds_data[i] == 0xFF) {
						break;
					}
					else {
						unsigned char _type = tds_data[i];
						unsigned short _pos = BigEdion::Bytes2ShortInt({ tds_data[i + 1] ,tds_data[i + 2] });
						unsigned short _len = BigEdion::Bytes2ShortInt({ tds_data[i + 3] ,tds_data[i + 4] });
						vector<unsigned char> _data = vector<unsigned char>();
						_data.assign(tds_data.begin() + _pos + 8, tds_data.begin() + _pos + _len + 8);
						switch (_type) {
						case 0: {
							prelogin_msg["VERSION"] = _data;
						}break;
						case 1: {
							prelogin_msg["ENCRYPTION"] = _data;
						}break;
						case 2: {
							prelogin_msg["INSTOPT"] = _data;
						}break;
						case 3: {
							prelogin_msg["THREADID"] = _data;
						}break;
						case 4: {
							prelogin_msg["MARS"] = _data;
						}break;
						case 5: {
							prelogin_msg["TRACEID"] = _data;
						}break;
						}
					}
				}
			}
			}
			return pair<shared_ptr<NetFunc::TDSHeader>, unordered_map<string, vector<unsigned char>>>();//tds_header, prelogin_msg
		}
	};
};
