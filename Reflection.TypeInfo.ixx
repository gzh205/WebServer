export module Reflection.TypeInfo;
import <string>;
import <unordered_map>;
import <memory>;
import <mutex>;
using namespace std;
export namespace Reflection {
	class FieldInfo {
	public:
		FieldInfo() {}
		FieldInfo(string name, shared_ptr<string> addr) {
			this->name = name;
			this->addr = addr;
		}
		string name;
		shared_ptr<string> addr;
	};
	class TypeInfo {
	public:
		void* addr;
		string name;
		unordered_map<string, FieldInfo> fields = unordered_map<string, FieldInfo>();
		static void Init() {
			call_once(singletonFlag, [&] {
				type_map = unordered_map<string, TypeInfo>();
			});
		}
		static unordered_map<string, TypeInfo> type_map;
	private:
		static once_flag singletonFlag;
	};
	unordered_map<string, TypeInfo> TypeInfo::type_map;
	once_flag TypeInfo::singletonFlag;
}