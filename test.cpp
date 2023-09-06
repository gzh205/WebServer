// MyProgram.cpp
//#include "WindowsFuncs.h"
import Server.WebServer;
import Server.WebServer;
import <iostream>;
import <fstream>;
import Serialize.XMLSerializer;
import DBConnector.IConn;
import DBConnector.MySqlConn;
import Server.Command;
//import Tools;
using namespace std;
int main()
{
	auto inst = Server::WebServer::GetInstance();
	Server::Command cmd = Server::Command();
	cmd.Run();
	//NetFunc::MySocket* socket = new NetFunc::MySocket(8088);
	return 0;
	//string a = string({0x01, 0x02, 0x03});
	//int res = LittleEdion::MyString2Int(a);
	//cout << res;
	//return 0;
}
