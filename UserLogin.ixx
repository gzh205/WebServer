export module UserLogin;
import Server.WebHandle;
import DBConnector.MySqlConn;
import <string>;
import <format>;
import DBConnector.ResultSet;
using namespace std;
export namespace Controller {
	class UserLogin : Server::WebHandle {
	public:
		UserLogin() {
		}
		Server::Response Handle(Server::Request& req) {
			string email = req.param_data["email"];
			string password = req.param_data["password"];
			MySql::Connection conn = MySql::Connection("43.143.104.120", 3306, "root", "gzh19961206", "WebServer");
			//²éÑ¯Êý¾Ý¿â
			string returnval = "";
			try {
				conn.Open();
				string sb_info = conn.SelectDatabase("student_info");
				DBConnector::ResultSet result = conn.ExecuteReader("select email,password from users where email='" + email + "'; ");
				if (result.error) {
					conn.Close();
					string error_msg = format("{{\"error\":\"{}\"; }}", result.error_message);
					return Server::Response(error_msg, "application / json");
				}
				string pwd = result[0][1];
				conn.Close();
				if (pwd == password) {
					returnval = "{\"result\":\"success\"}";
				}
				else {
					returnval = "{\"result\":\"error\"}";
				}
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