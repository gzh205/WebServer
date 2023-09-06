export module Serialize.XMLSerializer;
import <string>;
import <unordered_map>;
import <stack>;
import <memory>;
import Utils;
using namespace std;
export namespace Serialize {
	struct XMLItem {
	public:
		XMLItem() {
		}
		string element_type = "";
		unordered_map<string,string> attributes = unordered_map<string, string>();
		vector<shared_ptr<XMLItem>> subnodes = vector<shared_ptr<XMLItem>>();
		string innerText = "";
	};
	class XMLSerializer {
	public:
		/// <summary>
		/// 反序列化XML文档，不能多线程同时使用一个对象
		/// </summary>
		/// <param name="xml_document"></param>
		/// <returns></returns>
		static shared_ptr<XMLItem> DeSerialize(string xml_document) {
			shared_ptr<XMLItem> result = shared_ptr<XMLItem>(new XMLItem());
			result->element_type = "document";
			result->innerText = xml_document;
			stack<pair<int,shared_ptr<XMLItem>>> item_stack = stack<pair<int,shared_ptr<XMLItem>>>();//保存当前状态，方便深度优先搜索
			stack<pair<int, int>> brackets_pair = stack<pair<int, int>>();//用于配对左右括号和引号等
			unordered_map<string, string> attributes = unordered_map<string, string>();
			string attr_name = "";
			string tock_str = "";
			bool in_bracket = false;//在左右尖括号中
			bool stop = false;
			bool question = false;//XML中的问号
			shared_ptr<XMLItem> temp_ele = result;
			item_stack.push(pair<int,shared_ptr<XMLItem>>(pair<int,shared_ptr<XMLItem>>(0,result)));
			for (int i = 0; i < xml_document.length(); i++) {
				switch (xml_document[i]) {
				case '<': {
					in_bracket = true;
					char t_ = xml_document[i + 1];
					if (t_ == '/') {
						brackets_pair.push(pair<int, int>(++i, End_Angle_bracket));
					}
					else if (t_ == '?') {
						brackets_pair.push(pair<int, int>(++i, Question));
						question = true;
						item_stack.push(pair<int, shared_ptr<XMLItem>>(i, shared_ptr<XMLItem>(new XMLItem())));
					}
					else {
						brackets_pair.push(pair<int, int>(i, Angle_bracket));
						item_stack.push(pair<int, shared_ptr<XMLItem>>(i, shared_ptr<XMLItem>(new XMLItem())));
					}
					/*if (!item_stack.empty()) {
						if (!item_stack.top().second->fixed) {
							shared_ptr<XMLItem> tmp_ = shared_ptr<XMLItem>(new XMLItem());
							item_stack.push(pair<int, shared_ptr<XMLItem>>(i, tmp_));
						}
					}*/
				}break;
				case '/': {
					stop = true;
				}break;
				case '>': {
					in_bracket = false;
					pair<int, int> val_ = brackets_pair.top();
					brackets_pair.pop();
					if (stop) {
						if (val_.second == Quotes) {
							//先处理等于号
							string v_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
							item_stack.top().second->attributes[attr_name] = v_;
						}
						else {
							string v_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
							item_stack.top().second->attributes[v_] = "";
						}
						pair<int, shared_ptr<XMLItem>> tmp_ = item_stack.top();
						item_stack.pop();
						item_stack.top().second->subnodes.push_back(tmp_.second);
						stop = false;
					}
					else if (question) {
						if (val_.second == Quotes) {
							//先处理等于号
							string v_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
							item_stack.top().second->attributes[attr_name] = v_;
						}
						else {
							string v_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
							item_stack.top().second->attributes[v_] = "";
						}
						pair<int, shared_ptr<XMLItem>> tmp_ = item_stack.top();
						item_stack.pop();
						item_stack.top().second->subnodes.push_back(tmp_.second);
						question = false;
					}
					else if (val_.second == End_Angle_bracket) {
						//节点的结束标签
						//先获取标签中的type，并与当前节点比较
						pair<int, shared_ptr<XMLItem>> this_item_ = item_stack.top();
						string t_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
						if (this_item_.second->element_type == t_) {
							//说明这是一个结束符，取出当前的XMLItem和上一个XmlItem，然后将当前节点追加在上一个节点的subnodes中
							//pair<int, shared_ptr<XMLItem>> item_ = item_stack.top();
							item_stack.pop();
							pair<int, shared_ptr<XMLItem>> prev_item_ = item_stack.top();
							prev_item_.second->subnodes.push_back(this_item_.second);
							string inner_txt = xml_document.substr(this_item_.first + 1, val_.first - this_item_.first);
							this_item_.second->innerText = StrUtils::XmlTrim(inner_txt);//文本内容
						}else {
							//标签对不上了
							throw exception("XML格式错误<xxx>和</xxx>无法对应");
						}
					}
					else if (val_.second == Angle_bracket) {
						//节点的起始标签
						shared_ptr<XMLItem> item_t_ = item_stack.top().second;
						string t_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
						item_t_->element_type = t_;
					}
					else if (val_.second == Quotes) {
						//先处理等于号
						string v_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
						item_stack.top().second->attributes[attr_name] = v_;
					}
					else {
						throw exception("XML格式错误<xxx>和</xxx>无法对应");
					}
				}break;
				case '=': {
					pair<int, int> val_ = brackets_pair.top();
					brackets_pair.pop();
					if (in_bracket && val_.second == Space) {
						attr_name = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
						brackets_pair.push(pair<int,int>(i, Quotes));
					}
					else {
						throw exception("XML格式错误");
					}
					
				}break;
				case ' ': {
					pair<int, int> val_ = brackets_pair.top();
					string data_ = StrUtils::Trim2(xml_document.substr(val_.first + 1, i - val_.first - 1));
					brackets_pair.pop();
					if (val_.second == Angle_bracket) {
						item_stack.top().second->element_type = data_;
					}
					else if (val_.second == Quotes) {
						item_stack.top().second->attributes[attr_name] = data_;
					}
					else if (val_.second == Space) {
						item_stack.top().second->attributes[data_] = "";
					}
					else if (val_.second == Question) {
						item_stack.top().second->element_type = data_;
					}
					brackets_pair.push(pair<int, int>(i, Space));
				}break;
				}
			}
			return result;
		}
	private:
		enum pair_type{ Angle_bracket, Parenthesis, Quotes, End_Angle_bracket, Space, Question};
	};
}