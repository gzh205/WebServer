export module Server.Command;
import <iostream>;
import <unordered_map>;
import <functional>;
using namespace std;
export namespace Server {
	class Command {
	public:
		Command() {
		}
		void Run() {
			string c;
			while (true) {
				cin >> c;
				if (c == "exit") {
					break;
				}
				else if(c == "help") {
					cout << "exit:ÍË³ö³ÌÐò\nhelp:°ïÖú\n";
				}
			}
		}
	};
}