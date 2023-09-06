export module DBConnector.ResultSet;
import <vector>;
import <string>;
import <memory>;
using namespace std;
export namespace DBConnector {
	/// <summary>
/// �ö�������洢һ��Ԫ��
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
	/// ������޸�ResultSet�е�Ԫ�أ����ǲ�����ô������������BUG��
	/// </summary>
	class ResultSet {
	public:
		bool error = false;
		string error_message;
		RowSet operator[](int index) {
			if (Length <= index) {
				throw exception("����Խ��");
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
		/// ��ȡ����
		/// </summary>
		int ColLen() {
			return column_name.size();
		}
		/// <summary>
		/// ��ȡ����
		/// </summary>
		int RowLen() {
			return Length;
		}
		/// <summary>
		/// ��ȡ����
		/// </summary>
		vector<string> GetCols() {
			return column_name;
		}
		/// <summary>
		/// ��������
		/// </summary>
		/// <param name="column_name">��������</param>
		void SetColName(vector<string> column_name) {
			this->column_name = column_name;
			for (auto s : column_name) {
				data_matrix.push_back(vector<string>());
			}
		}
		/// <summary>
		/// ����һ��Ԫ��
		/// </summary>
		/// <param name="r">һ��Ԫ�صĶ���</param>
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