export module Encryption.SHA256;
import <string>;
import <vector>;
import <stdint.h>;
import <iomanip>;
import <iostream>;
using namespace std;
export namespace Encryption {
	class SHA256 {
	public:
		bool encrypt(const vector<uint8_t>& message, vector<uint8_t>* _digest) {
			if (!message.empty() && _digest)
			{
				//! �ı�Ԥ����
				std::vector<uint8_t> message = message;
				preprocessing(&message);

				//! ���ı��ֽ��������64Byte��С�����ݿ�
				std::vector<std::vector<uint8_t>> chunks;
				breakTextInto64ByteChunks(message, &chunks);

				//! ��64Byte��С�����ݿ飬�����64��4Byte��С���֡�Ȼ�����ѭ��������
				std::vector<uint32_t> message_digest(initial_message_digest_); // ��ʼ����ϢժҪ

				std::vector<uint32_t> words;
				for (const auto& chunk : chunks)
				{
					structureWords(chunk, &words);
					transform(words, &message_digest);
				}

				//! ��ȡ���ս��
				produceFinalHashValue(message_digest, _digest);

				return true;
			}
			else
			{
				return false;
			}
		}
		string getHexMessageDigest(const std::string& message) {
			if (!message.empty())
			{
				std::vector<uint8_t> __message;
				for (auto it = message.begin(); it != message.end(); ++it)
				{
					__message.push_back(static_cast<uint8_t>(*it));
				}

				std::vector<uint8_t> digest;
				encrypt(__message, &digest);

				std::ostringstream o_s;
				o_s << std::hex << std::setiosflags(std::ios::uppercase);
				for (auto it = digest.begin(); it != digest.end(); ++it)
				{
					o_s << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(*it);
				}

				return o_s.str();
			}
			else
			{
				return "";
			}
		}
	protected:
		/// SHA256�㷨�ж����6���߼����� ///
		inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) const {
			return (x & y) ^ ((~x) & z);
		}
		inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) const {
			return (x & y) ^ (x & z) ^ (y & z);
		}
		inline uint32_t big_sigma0(uint32_t x) const {
			return (x >> 2 | x << 30) ^ (x >> 13 | x << 19) ^ (x >> 22 | x << 10);
		}
		inline uint32_t big_sigma1(uint32_t x) const {
			return (x >> 6 | x << 26) ^ (x >> 11 | x << 21) ^ (x >> 25 | x << 7);
		}
		inline uint32_t small_sigma0(uint32_t x) const {
			return (x >> 7 | x << 25) ^ (x >> 18 | x << 14) ^ (x >> 3);
		}
		inline uint32_t small_sigma1(uint32_t x) const {
			return (x >> 17 | x << 15) ^ (x >> 19 | x << 13) ^ (x >> 10);
		}
		/** @brief: SHA256�㷨��������Ϣ��Ԥ�������������������ء��͡����ӳ���ֵ��
		����������: �ڱ���ĩβ������䣬�Ȳ���һ������Ϊ1��Ȼ�󶼲�0��ֱ�����������512ȡģ��������448����Ҫע����ǣ���Ϣ���������䡣
		���ӳ���ֵ: ��һ��64λ����������ʾԭʼ��Ϣ�����ǰ����Ϣ���ĳ��ȣ������䲹���Ѿ�����������������Ϣ���档
		@param[in][out] _message: ���������Ϣ
		@return: �Ƿ�ɹ�
		*/
		bool preprocessing(vector<uint8_t>* _message) const {
			if (_message)
			{
				const uint64_t original_bit_size = _message->size() * 8;

				//! ����������
				size_t remainder = _message->size() % 64;
				if (remainder < 56)
				{
					_message->push_back(0x80); // ox80 == 10000000
					for (size_t i = 1; i < 56 - remainder; ++i)
					{
						_message->push_back(0x00);
					}
				}
				else if (remainder == 56)
				{
					_message->push_back(0x80);
					for (size_t i = 1; i < 64; ++i)
					{
						_message->push_back(0x00);
					}
				}
				else
				{
					_message->push_back(0x80);
					for (size_t i = 1; i < 64 - remainder + 56; ++i)
					{
						_message->push_back(0x00);
					}
				}

				//! ����ԭʼ�ı��ĳ���ֵ
				for (int i = 1; i <= 8; ++i)
				{
					uint8_t c = static_cast<uint8_t>(original_bit_size >> (64 - 8 * i));
					_message->push_back(c);
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		/** @brief: ����Ϣ�ֽ��������64Byte��С�����ݿ�
		@param[in] message: ������Ϣ������Ϊ64Byte�ı���
		@param[out] _chunks: ������ݿ�
		@return: �Ƿ�ɹ�
		*/
		bool breakTextInto64ByteChunks(const vector<uint8_t>& message, vector<vector<uint8_t>>* _chunks) const {
			if (_chunks && 0 == message.size() % 64)
			{
				_chunks->clear(); // ������buffer

				size_t quotient = message.size() / 64;
				for (size_t i = 0; i < quotient; ++i)
				{
					std::vector<uint8_t> temp(message.begin() + i * 64, message.begin() + (i + 1) * 64);
					_chunks->push_back(temp);
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		/** @brief: ��64Byte��С�����ݿ飬�����64��4Byte��С���֡�
		�����㷨��ǰ16����ֱ�������ݿ�ֽ�õ���������������µ�����ʽ�õ���
				W[t] = small_sigma1(W[t-2]) + W[t-7] + small_sigma0(W[t-15]) + W[t-16]
		@param[in] chunk: �������ݿ飬��СΪ64Byte
		@param[out] _words: �����
		@return: �Ƿ�ɹ�
		*/
		bool structureWords(const vector<uint8_t>& chunk, vector<uint32_t>* _words) const {
			if (_words && 64 == chunk.size())
			{
				_words->resize(64);

				auto& words = *_words;
				for (int i = 0; i < 16; ++i)
				{
					words[i] = (static_cast<uint32_t>(chunk[i * 4]) << 24)
						| (static_cast<uint32_t>(chunk[i * 4 + 1]) << 16)
						| (static_cast<uint32_t>(chunk[i * 4 + 2]) << 8)
						| static_cast<uint32_t>(chunk[i * 4 + 3]);
				}
				for (int i = 16; i < 64; ++i)
				{
					words[i] = small_sigma1(words[i - 2])
						+ words[i - 7]
						+ small_sigma0(words[i - 15])
						+ words[i - 16];
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		/** @breif: ����64��4Byte��С���֣�����64��ѭ������
		@param[in] words: 64��4Byte��С����
		@param[in][out] _message_digest: ��ϢժҪ
		@return: �Ƿ�ɹ�
		*/
		bool transform(const vector<uint32_t>& words, vector<uint32_t>* _message_digest) const {
			if (_message_digest && 8 == _message_digest->size() && 64 == words.size())
			{
				std::vector<uint32_t> d = *_message_digest;

				for (int i = 0; i < 64; ++i)
				{
					uint32_t temp1 = d[7] + big_sigma1(d[4]) + ch(d[4], d[5], d[6]) + add_constant_[i] + words[i];
					uint32_t temp2 = big_sigma0(d[0]) + maj(d[0], d[1], d[2]);

					d[7] = d[6];
					d[6] = d[5];
					d[5] = d[4];
					d[4] = d[3] + temp1;
					d[3] = d[2];
					d[2] = d[1];
					d[1] = d[0];
					d[0] = temp1 + temp2;
				}

				for (int i = 0; i < 8; ++i)
				{
					(*_message_digest)[i] += d[i];
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		/** @brief: ������յĹ�ϣֵ������ָ�ƣ�
		@param[in] input: ����Ϊ32bit�Ĺ�ϣֵ
		@param[out] _output: ����Ϊ8bit�Ĺ�ϣֵ
		@return: �Ƿ�ɹ�
		*/
		bool produceFinalHashValue(const std::vector<uint32_t>& input, vector<uint8_t>* _output) const {
			if (_output)
			{
				_output->clear();

				for (auto it = input.begin(); it != input.end(); ++it)
				{
					for (int i = 0; i < 4; i++)
					{
						_output->push_back(static_cast<uint8_t>((*it) >> (24 - 8 * i)));
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	private:
		static vector<uint32_t> initial_message_digest_;// ��SHA256�㷨�еĳ�ʼ��ϢժҪ����Щ�����Ƕ���Ȼ����ǰ8��������ƽ������С������ȡǰ32bit������
		static vector<uint32_t> add_constant_; // ��SHA256�㷨�У��õ�64����������Щ�����Ƕ���Ȼ����ǰ64����������������С������ȡǰ32bit������
	};
	vector<uint32_t> SHA256::initial_message_digest_ = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372,
		0xa54ff53a, 0x510e527f, 0x9b05688c,
		0x1f83d9ab, 0x5be0cd19
	};
	vector<uint32_t> SHA256::add_constant_ = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};
}