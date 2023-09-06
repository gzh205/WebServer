export module UserReg;
import Server.WebHandle;
import DBConnector.MySqlConn;
import <string>;
import <format>;
import DBConnector.ResultSet;
using namespace std;
export namespace Controller {
	class UserReg : Server::WebHandle {
	public:
		UserReg() {
		}
		Server::Response Handle(Server::Request& req) {
			string email = req.param_data["email"];
			string password = req.param_data["password"];
			MySql::Connection conn = MySql::Connection("43.143.104.120", 3306, "root", "gzh19961206", "WebServer");
			//查询数据库
			string returnval = "";
			try {
				conn.Open();
				string db_info = conn.SelectDatabase("student_info");
				pair<bool, string> result = conn.ExecNonQuery("insert into users(email,password) values('" + email + "','" + password + "')");
				if (!result.first) {
					conn.Close();
					string error_msg = format("{{\"error\":\"{}\"; }}", result.second);
					return Server::Response(error_msg, "application / json");
				}
				conn.Close();
				returnval = "{\"result\":\"注册成功\"}";
			}
			catch (exception e) {
				conn.Close();
				string error_msg = format("{{\"error\":\"{}\"; }}", e.what());
				return Server::Response(error_msg, "application / json");
			}
			cout << "response " + returnval << endl;
			return Server::Response(returnval, "application/json");
		}
	};
}