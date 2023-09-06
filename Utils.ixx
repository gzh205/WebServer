export module Utils;
import <vector>;
import <string>;
import <functional>;
import <algorithm>;
import <iostream>;
import <codecvt>;
import <cstring>;
using namespace std;
export namespace StrUtils {
	//// wstring=>string
	//std::string WString2String(const std::wstring& ws)
	//{
	//	std::string strLocale = setlocale(LC_ALL, "");
	//	const wchar_t* wchSrc = ws.c_str();
	//	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	//	char* chDest = new char[nDestSize];
	//	memset(chDest, 0, nDestSize);
	//	wcstombs(chDest, wchSrc, nDestSize);
	//	std::string strResult = chDest;
	//	delete[] chDest;
	//	setlocale(LC_ALL, strLocale.c_str());
	//	return strResult;
	//}

	//// string => wstring
	//std::wstring String2WString(const std::string& s)
	//{
	//	std::string strLocale = setlocale(LC_ALL, "");
	//	const char* chSrc = s.c_str();
	//	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	//	wchar_t* wchDest = new wchar_t[nDestSize];
	//	wmemset(wchDest, 0, nDestSize);
	//	mbstowcs(wchDest, chSrc, nDestSize);
	//	std::wstring wstrResult = wchDest;
	//	delete[] wchDest;
	//	setlocale(LC_ALL, strLocale.c_str());
	//	return wstrResult;
	//}

	/// <summary>
	/// �ַ����滻
	/// </summary>
	/// <param name="str"></param>
	/// <param name="oldChar"></param>
	/// <param name="newChar"></param>
	/// <returns></returns>
	string ReplaceAll(string str, char oldChar, char newChar) {
		size_t pos = 0;
		while ((pos = str.find(oldChar, pos)) != string::npos) {
			str.replace(pos, 1, 1, newChar);
			pos++;
		}
		return str;
	}
	/// <summary>
	/// �ַ����滻
	/// </summary>
	/// <param name="str"></param>
	/// <param name="oldChar"></param>
	/// <param name="newChar"></param>
	/// <returns></returns>
	wstring ReplaceAll(wstring str, wchar_t oldChar, wchar_t newChar) {
		size_t pos = 0;
		while ((pos = str.find(oldChar, pos)) != string::npos) {
			str.replace(pos, 1, 1, newChar);
			pos++;
		}
		return str;
	}
	/// <summary>
	/// �ַ����ָ�
	/// </summary>
	/// <param name="src">Դ�ַ���</param>
	/// <param name="delim">�ָ��ַ���</param>
	/// <returns></returns>
	vector<string> Split(const string src, const string delim) {
		size_t previous = 0;
		size_t current = src.find(delim);
		vector<std::string> elems;
		while (current != std::string::npos) {
			if (current > previous) {
				elems.emplace_back(src.substr(previous, current - previous));
			}
			previous = current + 1;
			current = src.find(delim, previous);
		}
		if (previous != src.size()) {
			elems.emplace_back(src.substr(previous));
		}
		return elems;
	}
	/// <summary>
	/// �ַ����ָ�
	/// </summary>
	/// <param name="src">Դ�ַ���</param>
	/// <param name="delim">�ָ��ַ���</param>
	/// <returns></returns>
	vector<wstring> SplitW(const wstring src, const wstring delim) {
		size_t previous = 0;
		size_t current = src.find(delim);
		vector<std::wstring> elems;
		while (current != std::string::npos) {
			if (current > previous) {
				elems.emplace_back(src.substr(previous, current - previous));
			}
			previous = current + 1;
			current = src.find(delim, previous);
		}
		if (previous != src.size()) {
			elems.emplace_back(src.substr(previous));
		}
		return elems;
	}
	/// <summary>
	/// ȥ���ַ�����λ�ո�
	/// </summary>
	/// <param name="str">Դ�ַ���</param>
	/// <returns></returns>
	string Trim(string str) {
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ") + 1);
		return str;
	}
	/// <summary>
	/// ȥ���ַ�����β���ɼ��ַ�
	/// </summary>
	/// <param name="str">Դ�ַ���</param>
	/// <returns></returns>
	string Trim2(string str) {
		string::iterator it_begin = str.begin();
		string::iterator it_end = str.end();
		for (; it_begin != str.end(); it_begin++) {
			unsigned char t_ = *it_begin;
			if (t_ != '\t' && t_ != '\n' && t_ != '\r') {
				break;
			}
		}
		for (; it_end != str.begin(); it_end--) {
			unsigned char t_ = *(it_end - 1);
			if (t_ != '\t' && t_ != '\n' && t_ != '\r') {
				break;
			}
		}
		return string(it_begin, it_end);
	}
	/// <summary>
	/// ʹ����XML��trim����
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	string XmlTrim(string str) {
		int it_begin = 0;
		int it_end = str.length();
		bool left_brace = false;
		bool right_brace = false;
		for (int i = 0; i < str.length(); i++) {
			unsigned char t_ = str[i];
			if (left_brace && t_ != '\t' && t_ != '\n' && t_ != '\r') {
				it_begin = i;
				break;
			}
			if (!left_brace && t_ == '>') {
				left_brace = true;
			}
		}
		for (int i=str.length()-1; i>=0; i--) {
			unsigned char t_ = str[i];
			if (right_brace && t_ != '\t' && t_ != '\n' && t_ != '\r') {
				it_end = i;
				break;
			}if (!right_brace && t_ == '<') {
				right_brace = true;
			}
		}
		return str.substr(it_begin, it_end - it_begin + 1);
	}
	/// <summary>
	/// ȥ���ַ�����β����ĸ�������ַ�
	/// </summary>
	/// <param name="str">Դ�ַ���</param>
	/// <returns></returns>
	string Trim3(string str) {
		string::iterator it_begin = str.begin();
		string::iterator it_end = str.end();
		for (; it_begin != str.end(); it_begin++) {
			unsigned char t_ = *it_begin;
			if (t_ >= 'A' && t_ <= 'Z' || t_ >= 'a' && t_ <= 'z' || t_ >= '0' && t_ <= '9') {
				break;
			}
		}
		for (; it_end != str.begin(); it_end--) {
			unsigned char t_ = *(it_end - 1);
			if (t_ >= 'A' && t_ <= 'Z' || t_ >= 'a' && t_ <= 'z' || t_ >= '0' && t_ <= '9') {
				break;
			}
		}
		return string(it_begin, it_end);
	}
	/// <summary>
	/// ��������ı�������
	/// </summary>
	/// <param name="length">�������ĳ���</param>
	/// <returns></returns>
	vector<unsigned char> RandomByteGenerator(int length) {
		srand(length);
		vector<unsigned char> res = vector<unsigned char>();
		for (int i = 0; i < length; i++) {
			res.emplace_back(rand() % 0x100);
		}
		return res;
	}
	/// <summary>
	/// Сдת��д
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	string ToUppor(string src) {
		for (auto i = src.begin(); i != src.end();i++) {
			if (*i >= 97 && *i <= 122) {
				*i = *i & 0xdf;
			}
		}
		return src;
	}
	/// <summary>
	/// ��дתСд
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	string ToLower(string src) {
		for (auto i = src.begin(); i != src.end(); i++) {
			if (*i >= 65 && *i <= 90) {
				*i = *i | 0x20;
			}
		}
		return src;
	}
}