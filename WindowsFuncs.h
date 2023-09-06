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
	/// 获取当前进程的PID值
	/// </summary>
	/// <returns></returns>
	unsigned char* GetPID();
	/// <summary>
	/// 获取进程ID
	/// </summary>
	/// <returns></returns>
	unsigned char* GetClientMac();
	/// <summary>
	/// 获取客户端名称
	/// </summary>
	/// <returns></returns>
	unsigned char* GetClientName();
	/// <summary>
	/// 获取线程ID
	/// </summary>
	/// <returns></returns>
	std::vector<unsigned char> GetThreadId();
	std::string GetStrThreadId();
	std::string GetStrPId();
	std::string GetCurTime();
}
namespace NetFunc {
	/// <summary>
	/// TDS报文头部的结构体
	/// 详情参考https://blog.csdn.net/u014608280/article/details/80776703
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
		/// 弃用的构造函数
		/// </summary>
		MySocket();
		/// <summary>
		/// 创建一个"服务器"SOCKET
		/// </summary>
		/// <param name="port">服务绑定的端口号</param>
		MySocket(int port);
		/// <summary>
		/// 创建一个客户端SOCKET
		/// 使用Open和Close打开和关闭连接
		/// 使用Send和TDSResv等函数发送和接收数据
		/// 注意：Resv和TDSResv已经弃用，请使用Resv2
		/// </summary>
		/// <param name="ip">服务器ip</param>
		/// <param name="port">服务器端口号</param>
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