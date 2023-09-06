export module Tools;
#pragma once
import <vector>;
import <string>;
import <iostream>;
import <algorithm>;
using namespace std;

export namespace BigEdion {
	/// <summary>
	/// //������ת�ֽڣ�0���ֽڣ�1���ֽ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	unsigned char* Short2Byte(unsigned short int a) {
		unsigned char* res = new unsigned char[2];
		unsigned char a_l = a % 0x100;
		unsigned char a_h = a / 0x100;
		res[0] = a_h;
		res[1] = a_l;
		return res;
	}
	/// <summary>
	/// //������ת�ֽڣ�0���ֽڣ�1���ֽ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	vector<unsigned char> Short2Vec(unsigned short int a) {
		unsigned char a_l = a % 0x100;
		unsigned char a_h = a / 0x100;
		return {a_h,a_l};
	}
	/// <summary>
	/// �ֽ�ת������
	/// </summary>
	/// <returns></returns>
	unsigned short int Bytes2ShortInt(char* data) {
		return data[0] * 0x100 + data[1];
	}
	/// <summary>
	/// �ֽ�ת������
	/// </summary>
	/// <returns></returns>
	unsigned short int Bytes2ShortInt(vector<unsigned char> data) {
		return data[0] * 0x100 + data[1];
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ4</param>
	/// <returns></returns>
	int MyString2Int4(string data) {
		return data[0] * 0x1000000 + data[1] * 0x10000 + data[2] * 0x100 + data[3] ;
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ4</param>
	/// <returns></returns>
	int MyString2Int2(string data) {
		return data[0] * 0x100 + data[1];
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ4</param>
	/// <returns></returns>
	string MyInt2String(int data) {
		char* a = (char*)&data;
		return { a[3], a[2], a[1], a[0]};
	}
}
export namespace LittleEdion {
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ4</param>
	/// <returns></returns>
	int MyString2Int(string data) {
		unsigned int res = 0;
		for (int i = data.length() - 1; i >= 0;i--) {
			res = (res << 8) + (unsigned char)data[i];
		}
		return res;
	}
	uint64_t MyString2Int64(string data) {
		unsigned int res = 0;
		for (int i = data.length() - 1; i >= 0; i--) {
			res = (res << 8) + (unsigned char)data[i];
		}
		return res;
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ4</param>
	/// <returns></returns>
	int MyString2Int4(string data) {
		return data[0] + data[1] * 0x100 + data[2] * 0x10000 + data[3] * 0x1000000;
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <param name="data">����Ϊ3</param>
	/// <returns></returns>
	int MyString2Int3(string data) {
		return data[0] + data[1] * 0x100 + data[2] * 0x10000;
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <returns></returns>
	string MyInt2String(int length) {
		char _int1 = length % 0x100;
		char _int2 = length / 0x100 % 0x10000;
		char _int3 = length / 0x10000 % 0x1000000;
		return { _int1,_int2,_int3 };
	}
	/// <summary>
	/// ����ת����MySql���ݰ�ͷ���ĳ��ȸ�ʽ
	/// </summary>
	/// <returns></returns>
	string MyInt4String(int length) {
		char* a = (char*) & length;
		return { a[0], a[1] ,a[2], a[3]};
	}
	/// <summary>
	/// �ֽ�ת������
	/// </summary>
	/// <returns></returns>
	unsigned short int Bytes2ShortInt(char* data) {
		return data[1] * 0x100 + data[0];
	}
	/// <summary>
	/// //������ת�ֽڣ�0���ֽڣ�1���ֽ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	vector<unsigned char> Short2Vec(unsigned short int a) {
		unsigned char a_l = a % 0x100;
		unsigned char a_h = a / 0x100;
		return { a_l,a_h };
	}
	/// <summary>
	/// //������ת�ֽڣ�1���ֽڣ�0���ֽ�
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	vector<unsigned char> Int2Byte(unsigned int a) {
		unsigned char a_1 = a % 0x100;
		unsigned char a_2 = a / 0x100 % 0x10000;
		unsigned char a_3 = a / 0x10000 % 0x1000000;
		unsigned char a_4 = a / 0x1000000;
		return {a_1,a_2,a_3,a_4};
	}
	/// <summary>
	/// ��ת����
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	unsigned char* Reverse(const unsigned char* a,int len) {
		unsigned char* res = new unsigned char[len];
		for (int i = 0; i < len; i++) {
			res[len - 1] = a[i];
		}
		return res;
	}
}
export namespace Language {
	vector<unsigned char> zh_CN = { 0x00, 0x02, 0x08, 0x04 };//�й�
	vector<unsigned char> en_US = { 0x00, 0x00, 0x04, 0x09 };//����

}
/// <summary>
/// G=general�����еĹ��߶�������
/// </summary>
export namespace VectorPots {
	template<typename T> vector<T> AppendVector(vector<T> dst, vector<T> src) {
		dst.insert(dst.end(), src.begin(), src.end());
		return dst;
	}
	template<typename T> vector<T> ReverseVector(vector<T> input) {
		vector<unsigned char> res = vector<unsigned char>();
		reverse_copy(begin(input), end(input), back_inserter(res));
		return res;
	}
	/// <summary>
	/// �ַ���ת�ֽ����飬���ǲ��ܰ�������
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	vector<unsigned char> UcsString2Bytes(string data) {
		vector<unsigned char> res = vector<unsigned char>();
		for (char c : data) {
			res.emplace_back((unsigned char)c);
			res.emplace_back(0x00);
		}
		return res;
	}
	/// <summary>
	/// �ַ���ת�ֽ����飬���ǲ��ܰ�������
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	vector<unsigned char> String2Bytes(string data) {
		vector<unsigned char> res = vector<unsigned char>();
		for (char c : data) {
			res.emplace_back((unsigned char)c);
		}
		return res;
	}
	vector<unsigned char> Byte2Vec(unsigned char* data,int length) {
		vector<unsigned char> res = vector<unsigned char>(length);
		for (int i = 0; i < length; i++) {
			res.emplace_back(data[i]);
		}
		return res;
	}
	vector<unsigned char> Byte2Vec(char* data, int length) {
		vector<unsigned char> res = vector<unsigned char>(length);
		for (int i = 0; i < length; i++) {
			res.emplace_back((unsigned char)data[i]);
		}
		return res;
	}
}
/*template <typename T> class Node {
	public:
		Node(int length) {
			this->arr = new T[length];
		}
		T* arr;
		int length;
		Node<T>* next_node;
		/*		void set(int index,T value) {
			this->arr[index] = value;
		}
		T get(int index) {
			return this->arr[index];
		}
		int Length() {
			return this->length;
		}
		void append(T element) {

		}
		void append(Node<T> elements) {

		}
	};
	template <typename T> class ArrayList {
	public:
		/// <summary>
		/// ����һ��ָ�����ȵ�����
		/// </summary>
		/// <param name="length">ÿ���ڵ�ĳ���</param>
		ArrayList(int length) {
			this->start_node = new Node<T>(length);
			this->block_len = length;
			this->end_node = this->start_node;
		}
		/// <summary>
		/// ����һ���ɱ䳤���飬������ʵ�֣������е�ÿ���ڵ����50��Ԫ��
		/// </summary>
		ArrayList() {
			this->start_node = new Node<T>(50);
			this->block_len = 50;
			this->end_node = this->start_node;
		}
		T operator[](int i) {
			int block_idx = i / this->block_len;
			if (block_idx < this->length) {
				int offset = i % block_len;
				Node<T>* cur_node = start_node;
				if (block_idx == this->length - 1) {
					if (offset < cur_node->length) {
						return this->end_node[offset];
					}Node<T>* cur_node = start_node;
				}
				else {
					for (int i = 0; i < block_idx; i++) {
						cur_node = cur_node->next_node;
					}
					if (offset < cur_node->length) {
						return cur_node[offset];
					}
				}
			}
			throw new exception("����Խ��");
		}
		T* data() {
			int total_length = this->length * this->block_len - 1 + this->end_node->length;
			T* res = new T[total_length];
			int index = 0;
			Node<T>* cur_node = start_node;
			for (int i = 0; i < this->length - 1; i++) {
				memcpy_s(res + i * this->block_len, this->block_len, cur_node, this->block_len);
				cur_node = cur_node->next_node;
			}

		}
	private:
		Node<T>* start_node;
		Node<T>* end_node;
		int length;
		int block_len;
	}; */