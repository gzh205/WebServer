export module Serialize.JsonSerializer;
import <string>;
import <vector>;
import <stack>;
import <memory>;
import <unordered_map>;
import <iterator>;
import Utils;
using namespace std;
export namespace Serialize {
	class JsonItem {
	public:
		string name;
		unordered_map<string, shared_ptr<JsonItem>> values = unordered_map<string, shared_ptr<JsonItem>>();
		bool isArray = false;
	};
	class JsonObject:JsonItem {
	public:
		JsonObject() {
			isArray = false;
		}
	};
	class JsonArray:JsonItem {
	public:
		JsonArray() {
			isArray = true;
		}
	};
	class JsonSerializer {
	public:
		static shared_ptr<JsonItem> DeSerialize(string json_document) {
			stack<pair<int,JsonOperators>> ops_stack = stack<pair<int,JsonOperators>>();
			stack<shared_ptr<JsonItem>> item_stack = stack<shared_ptr<JsonItem>>();
			stack<string> key = stack<string>();
			stack<int> arr_num = stack<int>();
			for (int i = 0; i < json_document.length(); i++) {		
				switch (json_document[i]) {
				case ':': {
					auto ops = ops_stack.top();
					ops_stack.pop();
					key.push(StrUtils::Trim3(json_document.substr(ops.first + 1,i - ops.first - 1)));
					ops_stack.push(pair<int, JsonOperators>(i, Colon));
				}break;
				case ',': {
					auto ops = ops_stack.top();
					if (ops.second == End_Curly_bracket) {
						continue;
					}
					else if (ops.second == End_Square_bracket) {
						continue;
					}
					ops_stack.pop();
					if (ops.second == Colon) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.top()->values[key.top()] = foo_;
						key.pop();
					}
					else if (ops.second == Square_bracket || ops.second == Comma) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.push(foo_);
						arr_num.top() += 1;
					}
					ops_stack.push(pair<int, JsonOperators>(i, Comma));
				}break;
				case '{': {
					ops_stack.push(pair<int, JsonOperators>(i, Curly_bracket));
					item_stack.push(shared_ptr<JsonItem>((JsonItem*)new JsonObject()));
					arr_num.push(0);
				}break;
				case '[': {
					ops_stack.push(pair<int, JsonOperators>(i, Square_bracket));
					item_stack.push(shared_ptr<JsonItem>((JsonItem*)new JsonArray()));
				}break;
				case '}': {
					auto ops = ops_stack.top();
					if (ops.second == End_Curly_bracket) {
						continue;
					}
					else if (ops.second == End_Square_bracket) {
						continue;
					}
					ops_stack.pop();
					if (ops.second == Colon) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.top()->values[key.top()] = foo_;
						key.pop();
					}
					else if (ops.second == Square_bracket || ops.second == Comma) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.push(foo_);
						arr_num.top() += 1;
					}
					if (item_stack.size() > 1) {
						auto foo_ = item_stack.top();
						item_stack.pop();
						item_stack.top()->values[key.top()] = foo_;
						key.pop();
					}
					ops_stack.push(pair<int, JsonOperators>(i, End_Curly_bracket));
				}break;
				case ']': {
					auto ops = ops_stack.top();
					if (ops.second == End_Curly_bracket) {
						continue;
					}
					else if (ops.second == End_Square_bracket) {
						continue;
					}
					ops_stack.pop();
					if (ops.second == Colon) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.top()->values[key.top()] = foo_;
						key.pop();
					}
					else if (ops.second == Square_bracket || ops.second == Comma) {
						auto foo_ = shared_ptr<JsonItem>((JsonItem*)new JsonObject());
						foo_->name = StrUtils::Trim3(json_document.substr(ops.first + 1, i - ops.first - 1));
						item_stack.push(foo_);
						arr_num.top() += 1;
					}
					int num_ = arr_num.top();
					arr_num.pop();
					vector<shared_ptr<JsonItem>> arr_ = vector<shared_ptr<JsonItem>>();
					for (int j = 0; j < num_; j++) {
						arr_.push_back(item_stack.top());
						item_stack.pop();
					}
					for (int j = 0; j < num_; j++) {
						item_stack.top()->values[string("" + j)] = arr_[j];
					}
					auto bar_ = ops_stack.top();
					ops_stack.push(pair<int, JsonOperators>(i, End_Square_bracket));
				}break;
				}
			}
			return item_stack.top();
		}
	private:
		enum JsonOperators {
			Square_bracket, Curly_bracket, Comma, Colon, End_Square_bracket, End_Curly_bracket
		};
	};
}