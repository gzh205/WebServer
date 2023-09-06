export module DBConnector.IConn;
import <string>;
import DBConnector.ResultSet;
using namespace std;
export namespace DBConnector {
	class IConn {
	public:
		virtual void Open() = 0;
		virtual void Close() = 0;
		virtual pair<bool, string> ExecNonQuery(string sql) = 0;
		virtual DBConnector::ResultSet ExecuteReader(string sql) = 0;
		virtual string GetLastError() = 0;
		virtual string SelectDatabase(string database) = 0;
	};
}