/*****************
 * project:IntelligentPerceptionServer
 * source:FSocket.h
 * author:FEX
 * time:2023-03-24
 * description:
 * TCP通信socket类定义（客户端和服务器端建立连接后的socket）
 * copyright:
 *	线程使用c++11 thread技术
 *	包括接收客户端消息、解析收到的消息两个独立线程
 *	只包括接收消息vector
 *  一个状态机标志，表示当前服务器执行状态
 *  一个互斥锁控制接收消息存放的vector
 * ***************/
 
#ifndef FSOCKET_H
#define FSOCKET_H
 
#include "../common/Common.h"


//#include <thread>

using namespace std;
  
namespace IPSERVER
{
	//定义服务器和客户端建立连接后的socket类
	class FSocket
	{
		private:
			//socket连接建立后的FD
			int m_FD;
			
			//接收客户端消息的vector
			std::vector<IPSERVER::ST_MSG>	m_ReceiveMsg;
			
			//发送消息的vector
			std::vector<IPSERVER::ST_MSG>	m_SendMsg;
			
			
			//互斥锁
			//接收数据互斥锁，主要是接收数据线程和分析数据线程使用
			std::mutex m_RecvMutex;
			//发送数据互斥锁，主要是发送数据线程和分析数据线程使用
			std::mutex m_SendMutex;
			
			//客户端状态
			int m_ClientStatus;//0-正常，-1-断开
			
			
			//使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
			//从客户端接收数据执行体
			void OnReceive();
			
			//向客户端发送数据执行体
			void OnSend();
			
			//解析数据执行体
			void OnAnalysis();
			
			std::thread r_thread;
			std::thread s_thread;
			std::thread a_thread;
			
			
		
			//回调函数
			CALLBACK_FUNC m_Callback_Func;
			
		public:
		
			//显式构造函数
			explicit FSocket(const int fd, void* (*func)(void*));
			//拷贝构造函数
			FSocket(const FSocket& object) = delete;
			//复制运算符重载
			FSocket& operator=(const FSocket& object) = delete;
			//析构函数
			~FSocket();
			
			//启动
			void Start();
			
			//等待退出
			void Wait();

            //关闭
            void Close();
			
			
			//获取socket通信的FD信息
			const int GetFD()const;
			
			//获取接收消息的vector
			const std::vector<IPSERVER::ST_MSG>& GetRECV_Vector();
			
			//获取发送消息的vector
			const std::vector<IPSERVER::ST_MSG>& GetSEND_Vector();
			
			
			//设置客户端状态
			const int SetClientStatus(const int status);
			
			//获取客户端状态
			const int GetClientStatus();
			
			
			//发送消息向量添加数据
			const int SEND_PushBack(const IPSERVER::ST_MSG& msg);
			
			//接收消息向量添加数据
			const int RECV_PushBack(const IPSERVER::ST_MSG& msg);
			
			
			//发送消息向量删除数据
			const int SEND_Erase(std::vector<IPSERVER::ST_MSG>::iterator& iter);
			
			//接收消息向量删除数据
			const int RECV_Erase(std::vector<IPSERVER::ST_MSG>::iterator& iter);

            //回调函数，当底层其他操作执行后，如果需要向客户端回馈消息，则回调当前函数
	        //int FSocket::AddItemToRecvVector(const IPSERVER::ST_MSG& msg);
			
		
	};//end class FSocket
	
}//end namespace IPSERVER
  
 
 
#endif 
