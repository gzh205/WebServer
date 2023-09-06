export module Server.WebHandle;
import <map>;
import <string>;
import <functional>;
import <iostream>;
import Server.Request;
import Server.Response;
using namespace std;
export namespace Server {

	class WebHandle {
	public:
		static map<string, WebHandle*> web_handle;
		virtual Response Handle(Request& req) = 0;
	};
	map<string, WebHandle*> WebHandle::web_handle = map<string, WebHandle*>();
}