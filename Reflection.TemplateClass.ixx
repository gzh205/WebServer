export module Reflection.TemplateClass;
import <string>;
import <unordered_map>;
import Reflection.TypeInfo;
using namespace std;
#define RegF(name) type.fields[#name] = FieldInfo(#name,shared_ptr<string>(&name));
#define RegC(name) type.name=name;type.addr=this;
export namespace Reflection {

	class TemplateClass {
	public:
		TemplateClass() {

		}
		FieldInfo GetField(string field_name) {
			return type.fields[field_name];
		}
		static void Reg() {

		}
	protected:
		TypeInfo type = TypeInfo();
	};
}