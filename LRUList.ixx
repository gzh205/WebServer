export module LRUList;
import <list>;
import <string>;
import <vector>;
using namespace std;
export namespace Tools {
	template <typename K, typename V>
	class LRUList {
	public:
		LRUList() {
			this->max_size = 65536;
		}
		LRUList(int maxsize) {
			this->max_size = maxsize;
		}
		list<pair<K, V>> data_list = list<pair<K, V>>();
		int max_size;
		/// <summary>
		/// ����һ��Ԫ��
		/// </summary>
		/// <param name="data"></param>
		/// <returns>����ɾ����һ��Ԫ��</returns>
		void Add(K& key,V data) {
			data_list.push_front(pair<K,V>(key,data));
			if (data_list.size() >= max_size) {
				data_list.pop_back();
			}
		}
		/// <summary>
		/// ʹ��һ��Ԫ�أ�ʹ�ú�Ὣ��ŵ�linklist�Ŀ�ͷ
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		V Use(K& key) {
			for (auto it = data_list.begin(); it != data_list.end(); it++) {
				if (key == (*it).first) {
					V data_ = (*it).second;
					data_list.erase(it);
					data_list.push_front(pair<K,V>(key, data_));
					return data_;
				}
			}
			return V();
		}
		/// <summary>
		/// ɾ��һ��Ԫ��
		/// </summary>
		/// <param name="data"></param>
		void Delete(K& key) {
			for (auto it = data_list.begin(); it != data_list.end(); it++) {
				if (key == (*it).first) {
					data_list.erase(it);
				}
			}
		}
		/// <summary>
		/// ����һЩԪ��
		/// </summary>
		/// <param name="data"></param>
		/// <returns>�����Ѿ�ɾ����Ԫ��</returns>
		void AddSome(vector<pair<K,V>>& data) {
			data_list.insert(data_list.begin(), data.begin(), data.end());
			if (data_list.size() >= max_size) {
				int size = data.size();
				for (auto i = data_list.rbegin(); i != data_list.rend() || size > 0; i++) {
					data_list.erase(i);
					size--;
				}
			}
		}
	};
}