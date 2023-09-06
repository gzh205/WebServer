//#include <iphlpapi.h>
#ifndef M_ws2tcpip_H
#define M_ws2tcpip_H
#include <ws2tcpip.h>
#endif
#include "WindowsFuncs.h"
#ifndef M_VECTOR
#define M_VECTOR
#include <vector>
#endif
#ifndef M_SOCKET2_H
#define M_SOCKET2_H
#include <winsock2.h>
#include <time.h>
#endif
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"onecore.lib")
import Utils;
import Tools;
const unsigned long IP_LOCALHOST = 0x0100007F;
//�����127.0.0.1����С��ģʽ�洢
unsigned char* WinOpts::GetPID() {
	unsigned short int pid = GetCurrentProcessId();
	unsigned char* res = new unsigned char[2];
	unsigned char a_l = pid % 0x100;
	unsigned char a_h = pid / 0x100;
	res[0] = a_h;
	res[1] = a_l;
	return res;
}

unsigned char* WinOpts::GetClientMac()
{
	unsigned char macAddr[6] = { 0 };
	//DWORD status = NO_ERROR;
	//ULONG size = 0;
	//MIB_IPADDRTABLE* ip_table = NULL;
	//status = GetIpAddrTable(ip_table, &size, 0);
	//if (status == ERROR_INSUFFICIENT_BUFFER) {
	//	ip_table = (MIB_IPADDRTABLE*)malloc(size);
	//	status = GetIpAddrTable(ip_table, &size, 0);
	//	if (status == NO_ERROR) {
	//		for (int i = 0; i < ip_table->dwNumEntries; ++i)
	//		{
	//			if (IP_LOCALHOST != ip_table->table[i].dwAddr)
	//			{
	//				//print_ip(ip_table->table[i].dwAddr);
	//				MIB_IFROW iInfo;
	//				BYTE byMAC[6] = { 0, 0, 0, 0, 0, 0 };
	//				memset(&iInfo, 0, sizeof(MIB_IFROW));
	//				iInfo.dwIndex = ip_table->table[i].dwIndex;
	//				GetIfEntry(&iInfo);
	//				if (MIB_IF_TYPE_ETHERNET == iInfo.dwType)
	//				{
	//					memcpy(&byMAC, iInfo.bPhysAddr, iInfo.dwPhysAddrLen);
	//					for (int j = 0; j < 6; j++) {
	//						macAddr[j] = (unsigned char)byMAC[j];
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else {
	//		throw std::exception("Unable to read IP table from system\n");
	//	}
	//	free(ip_table);
	//}
	//else {
	//	throw std::exception("Unable to determine number of entries in IP table\n");
	//}
	return macAddr;
}

unsigned char* WinOpts::GetClientName()
{
	return nullptr;
}

std::vector<unsigned char> WinOpts::GetThreadId()
{
	unsigned int idx = GetCurrentThreadId();
	unsigned char a_1 = idx % 0x100;
	unsigned char a_2 = idx % 0x10000 / 0x100;
	unsigned char a_3 = idx % 0x1000000 / 0x10000;
	unsigned char a_4 = idx / 0x1000000;
	return { a_4,a_3,a_2,a_1 };
}

std::string WinOpts::GetStrThreadId()
{
	unsigned int idx = GetCurrentThreadId();
	return std::to_string(idx);
}

std::string WinOpts::GetStrPId()
{
	unsigned int idx = GetCurrentProcessId();
	return std::to_string(idx);
}

/// <summary>
/// ��ȡ��ǰʱ��
/// </summary>
/// <returns></returns>
std::string WinOpts::GetCurTime()
{
	std::time_t now = std::time(nullptr);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", std::localtime(&now));
	return std::string(timestamp, strlen(timestamp));
}

NetFunc::MySocket::MySocket()
{
}

NetFunc::MySocket::MySocket(int port)
{
	WSADATA wsaData = WSADATA();
	//��ʼ���׽��ֿ�
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		throw std::exception("��Socket��ʧ��");
	}
	int wsa_res = WSAGetLastError();
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock == INVALID_SOCKET) {
		int res = WSAGetLastError();
		throw std::exception("�����׽���ʧ��"+ res);
	}
	SOCKADDR_IN* addrserv = new SOCKADDR_IN();
	addrserv->sin_family = AF_INET;//����ΪTCPЭ��
	addrserv->sin_port = htons((uint16_t)port);//���ö˿�
	addrserv->sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//����ip
	this->address = addrserv;
	int RetVal = bind(this->sock, (sockaddr*)addrserv, sizeof(sockaddr));//���׽��ֺ�IP���˿�
	if (RetVal == SOCKET_ERROR)
	{
		int a = WSAGetLastError();
		throw std::exception("�׽��ְ�ʧ��,�������="+a);
	}
	RetVal = listen(this->sock, 2);
	if (RetVal == SOCKET_ERROR)
	{
		throw std::exception("�׽��ּ���ʧ��");
	}
	mode = "server";
}

NetFunc::MySocket::MySocket(std::string ip,int port)
{
	WSADATA wsaData = WSADATA();
	//��ʼ���׽��ֿ�
	WORD wVersionRequested = MAKEWORD(2, 2);//�汾��
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		throw std::exception("��Socket��ʧ��,�������"+ WSAGetLastError());
	}
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock == INVALID_SOCKET) {
		int _what = WSAGetLastError();
		throw std::exception("�����׽���ʧ��,�������" + _what);
	}
	int timeout = 200;
	if (SOCKET_ERROR == setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int))) {
		int _what = WSAGetLastError();
		throw std::exception("���ý��ճ�ʱʧ��,�������" + _what);
	}
	//struct linger {
	//	u_short l_onoff;
	//	u_short l_linger;
	//};
	//linger m_sLinger;
	//m_sLinger.l_onoff = 1;//(��closesocket()����,���ǻ�������û������ϵ�ʱ��������)
	//// ���m_sLinger.l_onoff=0;���ܺ�2.)������ͬ;
	//m_sLinger.l_linger = 5;//(��������ʱ��Ϊ5��)
	//if (SOCKET_ERROR == setsockopt(this->sock, SOL_SOCKET, SO_LINGER, (const char*)&m_sLinger, sizeof(linger))) {
	//	int _what = WSAGetLastError();
	//	throw std::exception("���ùر����Ӻ��ͻ��������������ݶ���ʧ��,�������" + _what);
	//}
	/*unsigned long ul = 1;
	if (SOCKET_ERROR == ioctlsocket(this->sock, FIONBIO, (unsigned long*)&ul)) {
		int _what = WSAGetLastError();
		throw std::exception("���÷�����ģʽʧ��,�������" + _what);
	}*/
	this->ip = ip;
	this->port = port;
	sockaddr_in* clientaddr = new sockaddr_in();
	clientaddr->sin_family = AF_INET;
	clientaddr->sin_port = htons((uint16_t)port);
	clientaddr->sin_addr.S_un.S_addr = inet_addr(ip.data());
	this->address = clientaddr;
	mode = "client";
	unsigned long arg_ = 0;
	if (SOCKET_ERROR == ioctlsocket(this->sock, FIONBIO, &arg_)) {
		int _what = WSAGetLastError();
		throw std::exception("����keepaliveʧ��,�������" + _what);
	}
}

NetFunc::MySocket::~MySocket()
{
	WSACleanup();
	closesocket(this->sock);
}

NetFunc::ClientInfo* NetFunc::MySocket::Accept()
{
	NetFunc::ClientInfo* result = new ClientInfo();
	result->ClientSocket = accept(this->sock, (SOCKADDR*)&result->ClientAddr, &result->ClientAddrLen);
	if (result->ClientSocket == INVALID_SOCKET) {
		int err_code = WSAGetLastError();
		closesocket(this->sock);
		WSACleanup();
		throw std::exception("���տͻ�������ʧ�ܣ��������:"+ err_code);
	}
	result->data = "";
	while (true)
	{
		char* ReceiveBuff = new char[BUFSIZ];
		int RetVal = recv(result->ClientSocket, ReceiveBuff, BUFSIZ, 0);
		if (RetVal == SOCKET_ERROR)
		{
			throw std::exception("��������ʧ��");
		}
		if (RetVal == 0 || RetVal == -1) {
			break;
		}
		//int length = strlen(ReceiveBuff);
		if (RetVal < BUFSIZ) {
			result->data += std::string(ReceiveBuff, RetVal);
			break;
		}
		else {
			result->data += std::string(ReceiveBuff, BUFSIZ);
		}
	}
	return result;
}

void NetFunc::MySocket::Send(std::vector<unsigned char> data)
{
	send(this->sock, (const char*)data.data(), data.size(), 0);
}

void NetFunc::MySocket::Send(std::string data)
{
	send(this->sock, (const char*)data.data(), data.length(), 0);
}

void NetFunc::MySocket::ServerSend(SOCKET socket,std::string data)
{
	int d = send(socket, (const char*)data.data(), data.length(), 0);
	int a = WSAGetLastError();
	int b = 0;
}

void NetFunc::MySocket::CloseConnection(SOCKET info)
{
	shutdown(info, SD_RECEIVE);
}

void NetFunc::MySocket::ResetWsa()
{
	WSADATA wsaData = WSADATA();
	//��ʼ���׽��ֿ�
	WORD wVersionRequested = MAKEWORD(2, 2);//�汾��
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		throw std::exception("��Socket��ʧ��,�������" + WSAGetLastError());
	}
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock == INVALID_SOCKET) {
		int _what = WSAGetLastError();
		throw std::exception("�����׽���ʧ��,�������" + _what);
	}
	int timeout = 200;
	if (SOCKET_ERROR == setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int))) {
		int _what = WSAGetLastError();
		throw std::exception("���ý��ճ�ʱʧ��,�������" + _what);
	}
	sockaddr_in* clientaddr = new sockaddr_in();
	clientaddr->sin_family = AF_INET;
	clientaddr->sin_port = htons((uint16_t)port);
	clientaddr->sin_addr.S_un.S_addr = inet_addr(ip.data());
	this->address = clientaddr;
	mode = "client";
	unsigned long arg_ = 0;
	if (SOCKET_ERROR == ioctlsocket(this->sock, FIONBIO, &arg_)) {
		int _what = WSAGetLastError();
		throw std::exception("����keepaliveʧ��,�������" + _what);
	}
}

void NetFunc::MySocket::Open()
{
	int Retval = connect(this->sock, (sockaddr*)this->address, sizeof(SOCKADDR_IN));
	if (Retval == SOCKET_ERROR) {
		ResetWsa();
	}
}

void NetFunc::MySocket::Close()
{
	closesocket(this->sock);
}

bool NetFunc::MySocket::isConnected()
{
	return false;
}

std::vector<unsigned char> NetFunc::MySocket::Resv()
{
	//����ͷ��
	int RetVal = 0;
	std::vector<unsigned char> data = std::vector<unsigned char>();
	do {
		char* package_header = new char[2048];
		RetVal = recv(this->sock, package_header, 2048, 0);
		data = VectorPots::AppendVector(data,VectorPots::Byte2Vec(package_header, RetVal));
	} while (RetVal > 0);
	return data;
}

std::string NetFunc::MySocket::MySQLResv()
{
	//����ͷ��
	int RetVal = 0;
	std::string data = "";
	while (true) {
		char* package_header = new char[2048];
		RetVal = recv(this->sock, package_header, 2048, 0);
		if (RetVal <= 0) {
			break;
		}
		data.append(package_header, RetVal);
	}
	return data;
}

std::string NetFunc::MySocket::Resv2()
{
	//����ͷ��
	int RetVal = 0;
	std::string data = "";
	bool continue_resv = true;
	while(continue_resv) {
		char* package_header = new char[2048];
		RetVal = recv(this->sock, package_header, 2048, 0);
		if (RetVal <= 0) {
			break;
		}
		else if (RetVal < 2048) {
			continue_resv = false;
		}
		data.append(package_header, RetVal);
	}
	return data;
}

std::pair<NetFunc::TDSHeader,std::vector<unsigned char>> NetFunc::MySocket::TDSResv()
{
	//����ͷ��
	char* package_header = new char[8];
	unsigned char RetVal = 0;
	while (RetVal <= 0) {
		unsigned char RetVal = recv(this->sock, package_header, 8, 0);
		if (RetVal == SOCKET_ERROR)
		{
			throw std::exception("��������ʧ��");
		}
	}
	NetFunc::TDSHeader header = NetFunc::TDSHeader();
	header.Type = package_header[0];
	header.Status = package_header[1];
	header.Length = BigEdion::Bytes2ShortInt(package_header + 2);
	header.SPID = BigEdion::Bytes2ShortInt(package_header + 4);
	header.PacketID = package_header[6];
	header.WINDOW = package_header[7];
	//���ձ���
	int package_len = header.Length - 8;
	package_header = new char[package_len];
	RetVal = recv(this->sock, package_header, package_len, 0);
	if (RetVal == SOCKET_ERROR)
	{
		throw std::exception("��������ʧ��");
	}
	std::vector<unsigned char> data = VectorPots::Byte2Vec(package_header, header.Length - 8);
	return std::pair<NetFunc::TDSHeader, std::vector<unsigned char>>(header, data);
}