export module DBConnector.ResultSet;
import <vector>;
import <string>;
import <memory>;
using namespace std;
export namespace DBConnector {
	/// <summary>
/// 该对象用与存储一行元素
/// </summary>
	class RowSet {
	public:
		RowSet(vector<string> data) {
			data_vec = data;
		}
		string operator[](int index) {
			return data_vec[index];
		}
		string Get(int index) {
			return data_vec[index];
		}
		void Put(string& t) {
			data_vec.push_back(t);
		}
	private:
		vector<string> data_vec;
	};
	class ColSet {
	public:
		ColSet(vector<string> data) {
			data_vec = data;
		}
		string operator[](int index) {
			return data_vec[index];
		}
		string Get(int index) {
			return data_vec[index];
		}
		void Put(string t) {
			data_vec.push_back(string(t));
		}
	private:
		vector<string> data_vec;
	};
	/// <summary>
	/// 你可以修改ResultSet中的元素，但是不能这么做，否则会出现BUG。
	/// </summary>
	class ResultSet {
	public:
		bool error = false;
		string error_message;
		RowSet operator[](int index) {
			if (Length <= index) {
				throw exception("索引越界");
			}
			vector<string> d = vector<string>();
			for (auto c : data_matrix) {
				d.push_back(string(c[index]));
			}
			return RowSet(d);
		}
		ColSet operator[](string col) {
			int i = 0;
			for (auto c : column_name) {
				if (c == col)
					break;
				i++;
			}
			return ColSet(data_matrix[i]);
		}
		bool isEmpry() {
			if (data_matrix.size() == 0) {
				return true;
			}
			else {
				return false;
			}
		}
		/// <summary>
		/// 获取列数
		/// </summary>
		int ColLen() {
			return column_name.size();
		}
		/// <summary>
		/// 获取行数
		/// </summary>
		int RowLen() {
			return Length;
		}
		/// <summary>
		/// 获取列名
		/// </summary>
		vector<string> GetCols() {
			return column_name;
		}
		/// <summary>
		/// 插入列名
		/// </summary>
		/// <param name="column_name">列名数组</param>
		void SetColName(vector<string> column_name) {
			this->column_name = column_name;
			for (auto s : column_name) {
				data_matrix.push_back(vector<string>());
			}
		}
		/// <summary>
		/// 插入一行元素
		/// </summary>
		/// <param name="r">一行元素的对象</param>
		void Put(RowSet r) {
			for (int i = 0; i < column_name.size(); i++) {
				data_matrix[i].push_back(r.Get(i));
			}
			Length++;
		}
		vector<string> column_name;
		vector<vector<string>> data_matrix = vector<vector<string>>();
		int Length;
	};
}