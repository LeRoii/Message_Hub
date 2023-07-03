#include "TCPServer.h"

namespace IPSERVER
{
	//静态成员初始化
	TCPServer* TCPServer::m_Object = new TCPServer;
	CALLBACK_FUNC TCPServer::m_Callback_Func = nullptr;
	
	//默认构造函数
	TCPServer::TCPServer()
	{
		
		//读取配置文件中的服务器IP和端口号信息
		Common::GetValueByKeyFromConfig("SERVER_IP", m_ServerIP, IP_LEN);
		Common::GetValueByKeyFromConfig("SERVER_PORT", m_ServerPort, PORT_LEN);

        std::cout<<"从配置文件中读取的 TCP Server IP:"<<m_ServerIP<<std::endl;
        std::cout<<"从配置文件中读取的 TCP Server PORT:"<<m_ServerPort<<std::endl;
		
		TCPServer::m_Callback_Func = nullptr;

        this->m_RunStatus = 0;

        this->m_ListenFD = -1;

        //this->m_ClientList = nullptr;
	}
	
	//构造重载
	/*explicit TCPServer::TCPServer(const char* ServerIP, const char* ServerPort)
	{
		//分配内存
		if(NULL == m_ServerIP)
		{
			m_ServerIP = new char[IP_LEN];
		}
		
		if(NULL == m_ServerPort)
		{
			m_ServerPort = new char[PORT_LEN];
		}
		
		//传入的参数服务器IP为空，则从配置文件中读取服务器IP地址信息,否则取当前参数值
		if(NULL == ServerIP || strlen(ServerIP) == 0)
		{
			//读取配置文件中的服务器IP信息
			Common::GetValueByKeyFromConfig("SERVER_IP", m_ServerIP, IP_LEN);
		}
		else
		{
			strncpy(m_ServerIP, ServerIP, strlen(ServerIP) > IP_LEN-1? IP_LEN-1 : strlen(ServerIP));
		}
		
		//传入的参数服务器端口号为空，则从配置文件中读取服务器端口号信息，否则取当前参数值
		if(NULL == ServerPort || strlen(ServerPort))
		{
			//读取配置文件中的服务器端口号信息
			Common::GetValueByKeyFromConfig("SERVER_PORT", m_ServerPort, PORT_LEN);
		}
		else
		{
			strncpy(m_ServerPort, ServerPort, strlen(ServerPort) > PORT_LEN-1? PORT_LEN-1 : strlen(ServerPort));
		}
		
		
		//初始化成员变量
		p_Receive=0;
		p_Send=0;
		p_Analysis=0;
		
		m_FD=-1;
		
		m_ClientStatus = IPSERVER::e_None;
		
		pthread_mutex_init(&p_mutex_Receive, NULL);
		pthread_mutex_init(&p_mutex_Send, NULL);		
	}
	
	//拷贝重载
	TCPServer::TCPServer(TCPServer& obj)
	{
		//成员复制
		*m_ServerIP = obj.m_ServerIP;
		*m_ServerPort = obj.m_ServerPort;
		
		p_Receive = obj.p_Receive;
		p_Send = obj.p_Send;
		p_Analysis = obj.p_Analysis;
		
		m_FD = obj.m_FD;
		
		m_ClientStatus = obj.m_ClientStatus;
		
		
		pthread_mutex_init(&p_mutex_Receive, NULL);
		pthread_mutex_init(&p_mutex_Send, NULL);	
	}*/
	
	//析构函数，单例析构函数不释放静态对象m_Object。
	TCPServer::~TCPServer()
	{
		
	}
	
	
	//返回单例对象
	TCPServer* TCPServer::GetInstance(void* (*func)(void*))
	{
		if(TCPServer::m_Object == NULL)
		{
			TCPServer::m_Object = new TCPServer;
		}
		
		TCPServer::m_Callback_Func = func;
		
		return TCPServer::m_Object;
	}
	
	//服务器启动
	void TCPServer::Start()
	{
		printf("TCPServer start \n");
		
		int ss,sc;
		struct sockaddr_in  server_addr;
		struct sockaddr_in  client_addr;
		
		int ret;
		void* err;
		
		//创建TCP 流式通信socket
		ss = socket(AF_INET, SOCK_STREAM, 0);
		if(ss < 0)
		{
			printf("TCPServer Create stream socket fail\n");
			printf("%s\n",strerror(errno));
			printf("TCPServer end where create stream socket fail\n");
			return;
		}
		else
		{
			printf("TCPServer Create stream socket succeed\n");
		}
		
		//socket 端口绑定
		bzero(&server_addr, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//inet_pton(AF_INET, m_ServerIP, &server_addr.sin_addr.s_addr);
		server_addr.sin_port = htons(atoi(m_ServerPort));

		//socket端口重用
		int opt = 1;
		if(setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(int)) != 0)
		{
			printf("TCPServer set port reuse fail\n");
			printf("%s\n", strerror(errno));
			printf("TCPServer end where set port reuse fail\n");
			return;
		}
		else
		{
			printf("TCPServer set port reuse succeed\n");
		}

		//服务器地址绑定
		ret = bind(ss, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
		if(ret != 0)
		{
			printf("TCPServer bind server addr to socket fail \n");
			printf("%s\n", strerror(errno));
			printf("TCPServer end where bind server addr to socket fail\n");
			return;
		}
		else
		{
			printf("TCPServer bind server addr to socket succeed \n");
		}

		
		//开始监听 
		this->m_ListenFD = listen(ss, BACK_LOG);
		if(this->m_ListenFD < 0)
		{
			printf("TCPServer listen fail\n");
			printf("%s\n", strerror(errno));
			printf("TCPServer end where start listen fail\n");
			return;
		}
		else
		{
			printf("TCPServer listen succeed\n");
		}
		
        
		IPSERVER::Common::SetStatus(IPSERVER::e_Ready);

        this->m_RunStatus = 1;
		
		//循环处理客户端连接
		while(this->m_RunStatus == 1)
		{
			socklen_t addrLen = sizeof(struct sockaddr);
			
			//accept
			sc = accept(ss, (struct sockaddr*)&client_addr, &addrLen);
			if(sc < 0)
			{
				continue;
			}
			else
			{
				printf("TCPServer accept succeed\n");
			}
			
			//客户端连接成功，创建socket通信对象，并启动通信
			FSocket* f_socket = new FSocket(sc, TCPServer::m_Callback_Func);
			if(NULL != f_socket)
			{
				f_socket->Start();

                //将客户端对象保存到vector中
                this->m_ClientList.push_back(f_socket);
			}
		}


        //关闭所有客户端链接
        for(auto ie = this->m_ClientList.begin() ; ie != this->m_ClientList.end(); ie++)
        {
            if((*ie)->GetClientStatus() != -1)
            {
                (*ie)->Close();
            }
            this->m_ClientList.erase(ie);
        }

        close(this->m_ListenFD);
        this->m_ListenFD = -1;
		printf("TCPServer end\n\n");
		return;
	}
	
	//服务器等待退出
	void TCPServer::Wait()
	{
	}
	
	//获取服务器地址
	const char* TCPServer::GetServerIP()
	{
		return this->m_ServerIP;
	}
	
	//获取服务器端口号
	const char* TCPServer::GetServerPort()
	{
		return this->m_ServerPort;
	}
	
	
	//设置服务器地址
	const int TCPServer::SetServerIP(const char* ipAddress)
	{
		if(NULL != ipAddress)
		{
			strncpy(this->m_ServerIP, ipAddress, strlen(ipAddress) < IP_LEN-1 ? strlen(ipAddress) : IP_LEN-1);
		}
		return 0;
	}
	
	//设置服务器端口号
	const int TCPServer::SetServerPort(const char* port)
	{
		if(NULL != port)
		{
			strncpy(this->m_ServerPort, port, strlen(port) < PORT_LEN-1 ? strlen(port) : PORT_LEN-1);
		}
		return 0;
	}

    
    //设置状态，0-就绪，1-运行服务，-1-退出
    void TCPServer::SetStatus(int status)
    {
        this->m_RunStatus = status;
    }
	

    
    //向客户端发送队列添加发送数据
    void TCPServer::SendData(const IPSERVER::ST_MSG &msg)
    {
        for(auto ie = this->m_ClientList.begin() ; ie != this->m_ClientList.end(); ie++)
        {
            if((*ie)->GetClientStatus() == -1)
            {
                (*ie)->Close();
                this->m_ClientList.erase(ie);
            }
            else
            {
                (*ie)->SEND_PushBack(msg);
            }
        }
    }
	
	
}//end namespace IPSERVER
