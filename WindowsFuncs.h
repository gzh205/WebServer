#pragma once
#ifndef M_winsock2_H
#define M_winsock2_H
#include <winsock2.h>
#endif
#ifndef M_vector
#define M_vector
#include <vector>
#endif
#ifndef M_unordered_map
#define M_unordered_map
#include <unordered_map>
#endif
#ifndef M_string
#define M_string
#include <string>
#endif
#include <memory>
namespace WinOpts {
	/// <summary>
	/// ��ȡ��ǰ���̵�PIDֵ
	/// </summary>
	/// <returns></returns>
	unsigned char* GetPID();
	/// <summary>
	/// ��ȡ����ID
	/// </summary>
	/// <returns></returns>
	unsigned char* GetClientMac();
	/// <summary>
	/// ��ȡ�ͻ�������
	/// </summary>
	/// <returns></returns>
	unsigned char* GetClientName();
	/// <summary>
	/// ��ȡ�߳�ID
	/// </summary>
	/// <returns></returns>
	std::vector<unsigned char> GetThreadId();
	std::string GetStrThreadId();
	std::string GetStrPId();
	std::string GetCurTime();
}
namespace NetFunc {
	/// <summary>
	/// TDS����ͷ���Ľṹ��
	/// ����ο�https://blog.csdn.net/u014608280/article/details/80776703
	/// </summary>
	struct TDSHeader {
		unsigned char Type = 1;
		unsigned char Status = 0;
		unsigned short int Length = 0;
		unsigned short int SPID = 0;
		unsigned char PacketID = 0;
		unsigned char WINDOW = 0;
	};
	struct ClientInfo {
		SOCKET ClientSocket = 0;
		SOCKADDR_IN ClientAddr = SOCKADDR_IN();
		int ClientAddrLen = sizeof(ClientAddr);
		std::string data = "";
	};
	class MySocket {
	public:
		/// <summary>
		/// ���õĹ��캯��
		/// </summary>
		MySocket();
		/// <summary>
		/// ����һ��"������"SOCKET
		/// </summary>
		/// <param name="port">����󶨵Ķ˿ں�</param>
		MySocket(int port);
		/// <summary>
		/// ����һ���ͻ���SOCKET
		/// ʹ��Open��Close�򿪺͹ر�����
		/// ʹ��Send��TDSResv�Ⱥ������ͺͽ�������
		/// ע�⣺Resv��TDSResv�Ѿ����ã���ʹ��Resv2
		/// </summary>
		/// <param name="ip">������ip</param>
		/// <param name="port">�������˿ں�</param>
		MySocket(std::string ip, int port);
		~MySocket();
		NetFunc::ClientInfo* Accept();
		void Send(std::vector<unsigned char> data);
		void Send(std::string data);
		void Open();
		void Close();
		bool isConnected();
		[[deprecated]]
		std::pair<TDSHeader, std::vector<unsigned char>> TDSResv();
		int max_length = 2048;
		std::string mode = "";
		[[deprecated]]
		std::vector<unsigned char> Resv();
		std::string Resv2();
		std::string MySQLResv();
		void ServerSend(SOCKET sock, std::string data);
		static void CloseConnection(SOCKET info);
	private:
		SOCKET sock = NULL;
		std::string ip;
		int port;
		SOCKADDR_IN* address;
		void ResetWsa();
	};
}