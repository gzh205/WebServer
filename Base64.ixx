export module Encryption.Base64;
import <string>;
using namespace std;
export namespace Encryption {
	class Base64 {
	public:
		/// <summary>
		/// 将byte数组转真正的base64字符串
		/// </summary>
		/// <param name="bytes">用string存储的byte数组</param>
		/// <returns>base64字符串</returns>
		static string Encoding(string buffer) {
            string result = "";
            int num = buffer.length();
            int index[4];
            for (int i = 0; i < num / 3 * 3; i += 3) {
                index[0] = buffer[i] >> 2;
                index[1] = (buffer[i] & 0x03) << 4 | buffer[i + 1] >> 4;
                index[2] = (buffer[i + 1] & 0x0f) << 2 | buffer[i + 2] >> 6;
                index[3] = buffer[i + 2] & 0x3f;
                for (int j = 0; j < 4; ++j) {
                    result += base64_code[index[j]];
                }
            }
            if (num % 3 != 0) {
                index[0] = num % 3 == 1 ? buffer[num - 1] >> 2 : buffer[num - 2] >> 2;
                index[1] =
                    num % 3 == 1 ? (buffer[num - 1] & 0x03) << 4 : (buffer[num - 2] & 0x03) << 4 | buffer[num - 1] >> 4;
                index[2] = num % 3 == 1 ? 64 : (buffer[num - 1] & 0x0f) << 2;
                index[3] = 64;
                for (int j = 0; j < 4; ++j) {
                    result += base64_code[index[j]];
                }
            }
            return result;
		}
		/// <summary>
		/// 将base64字符串转byte数组
		/// </summary>
		/// <param name="bytes"></param>
		/// <returns></returns>
		static string Decoding(string bytes) {
            return "";
		}
		static const string base64_code;
	};
	const string Base64::base64_code = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}