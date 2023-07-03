/*****************
 * project:IntelligentPerceptionServer
 * source:TCPServer.h
 * author:FEX
 * time:2023-03-24
 * description:
 * TCP通信服务端类定义（单例类）
 * copyright:
 *	服务端软件一直监听，等待客户端连接
 *	客户端请求建立socket连接后，每连接一个创建一个socket通信对象
 * ***************/
 
#ifndef TCPSERVER_H
#define TCPSERVER_H

//定义建立好socket连接处于ESTABLISHED状态的队列的长度，Linux限制最大值为128
#define BACK_LOG (10)

#include "./FSocket.h"

namespace IPSERVER
{
 class TCPServer
 {
	 private:
		static TCPServer* m_Object;
		
		//服务器IP地址
		char m_ServerIP[IP_LEN];
		//服务器端口号
		char m_ServerPort[PORT_LEN];
		
		//默认构造函数
		TCPServer();
		//显示构造重载
		//explicit TCPServer(const char* ip, const char* port);
		
		//拷贝重载禁用
		TCPServer(const TCPServer& obj) = delete;
		//复制重载禁用
		TCPServer& operator=(const TCPServer& obj) = delete;
		
		
		//析构函数
		virtual ~TCPServer();

		
		
		//回调函数，主要用于处理消息
		static CALLBACK_FUNC m_Callback_Func;

        //运行状态，0-就绪，1-运行服务，-1-退出
        int m_RunStatus;

        //监听FD
        int m_ListenFD;

        std::vector<FSocket*> m_ClientList;
	 
	 public:
	 
		//返回单例对象
		static TCPServer* GetInstance(void* (*func)(void*));
		
		//服务器启动
		void Start();
		
		//服务器等待退出
		void Wait();
		
		//获取服务器地址
		const char* GetServerIP();
		
		//获取服务器端口号
		const char* GetServerPort();
		
		
		//设置服务器地址
		const int SetServerIP(const char* ipAddress);
		
		//设置服务器端口号
		const int SetServerPort(const char* port);

        //设置状态，0-就绪，1-运行服务，-1-退出
        void SetStatus(int status);

        //向客户端发送队列添加发送数据
        void SendData(const IPSERVER::ST_MSG &msg);
		
		
	 
 };//end class TCPServer
 
 
}//end namespace IPSERVER


#endif
