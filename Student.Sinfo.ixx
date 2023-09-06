export module Dao.Student;
import Reflection.TemplateClass;
import "micros.h";
import Reflection.TypeInfo;
import <string>;
using namespace std;
export namespace Dao::Student {
	class Sinfo :Reflection::TemplateClass {
	public:
		Sinfo() {
			RegC(Sinfo);
			RegF(sno);
			RegF(sname);
			RegF(birth);
			RegF(info);
		}
		static void Register() {

		}
		string sno;
		string sname;
		string birth;
		string info;
	};
}