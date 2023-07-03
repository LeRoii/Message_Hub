#include "FSocket.h"

namespace IPSERVER
{
	//显式构造函数
	FSocket::FSocket(const int fd, void* (*func)(void*))
	{
		this->m_FD = fd;
		
		//设置当前设备状态为无效
		IPSERVER::Common::SetStatus(IPSERVER::e_None);
		
		//客户端连接状态,0-normal
		this->m_ClientStatus = 0;
		
		//回调函数
		this->m_Callback_Func = func;
		
	}
	
	//拷贝构造函数
	/*FSocket::FSocket(FSocket& object)
	{
		this->m_FD = object->m_FD;
		
		this->m_SocketStatus = object->m_SocketStatus;
	}*/
	
	//析构函数
	FSocket::~FSocket()
	{
        if(this->m_FD > 0)
        {
            close(this->m_FD);
            this->m_FD = -1;
        }
	}
	
	
	//服务器启动
	void FSocket::Start()
	{
		//IPSERVER::Common::SetStatus(IPSERVER::e_Ready);
	
		
		//使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
		//创建接收消息的线程
		r_thread = std::thread(&FSocket::OnReceive, this);
		r_thread.detach();//主线程与子线程分离，保证主线程结束不影响子线程，确保子线程在后台运行
		printf("create Receive thread succeed\n");
		
		
		//创建分析消息的线程
		a_thread = std::thread(&FSocket::OnAnalysis, this);
		a_thread.detach();//主线程与子线程分离，保证主线程结束不影响子线程，确保子线程在后台运行
		printf("create Analysis thread succeed\n");
		
		
		//创建发送消息的线程
		s_thread = std::thread(&FSocket::OnSend, this);
		s_thread.detach();
		printf("create Send thread succeed\n");
		
	}
	
	//服务器等待退出
	void FSocket::Wait()
	{
		/*//阻塞接收数据线程，等待线程结束，使用detach之后不应再使用join
		r_thread.join();
		
		//阻塞发送数据线程，等待线程结束
		s_thread.join();
		
		//阻塞分析线程，等待线程结束
		a_thread.join();
		*/

		
		//关闭socket连接
		if(this->m_FD >= 0)
		{
			close(this->m_FD);
			this->m_FD=-1;
		}
	}
			
	
	
	//获取FD信息
	const int FSocket::GetFD()const
	{
		return this->m_FD;
	}
	
	
	
	//获取接收消息的vector
	const std::vector<IPSERVER::ST_MSG>& FSocket::GetRECV_Vector()
	{
		return this->m_ReceiveMsg;
	}
	
	//获取发送消息的vector
	const std::vector<IPSERVER::ST_MSG>& FSocket::GetSEND_Vector()
	{
	 	return this->m_SendMsg;
	}
	
	
	//发送消息向量添加数据
	const int FSocket::SEND_PushBack(const IPSERVER::ST_MSG& msg)
	{		
        //如果当前互斥量没有被锁定，且获取锁成功
        if(this->m_RecvMutex.try_lock())
        {
			this->m_SendMsg.push_back(msg);
            this->m_RecvMutex.unlock();
		
		    return 0;
        }   
        else//获取锁失败
        {
            return -1;
        }
		
		return 0;
	}
	
	//接收消息向量添加数据 0：成功  -1：失败
	const int FSocket::RECV_PushBack(const IPSERVER::ST_MSG& msg)
	{
        //如果当前互斥量没有被锁定，且获取锁成功
        if(this->m_RecvMutex.try_lock())
        {
		    this->m_ReceiveMsg.push_back(msg);
            this->m_RecvMutex.unlock();
		
		    return 0;
        }   
        else//获取锁失败
        {
            return -1;
        }
	}
	
	//发送消息向量删除数据
	const int FSocket::SEND_Erase(vector<IPSERVER::ST_MSG>::iterator& iter)
	{
		if(this->m_SendMutex.try_lock())
		{
			this->m_SendMsg.erase(iter);
			
			this->m_SendMutex.unlock();
		
			return 0;
		}
		else
		{
			return -1;
		}
	}
	
	//接收消息向量删除数据
	const int FSocket::RECV_Erase(vector<IPSERVER::ST_MSG>::iterator& iter)
	{
        if(this->m_RecvMutex.try_lock())
        {
            this->m_ReceiveMsg.erase(iter);

            this->m_RecvMutex.unlock();
            return 0;
        }
        else
        {
            return -1;
        }
	}
	
	
	//设置客户端状态
	const int FSocket::SetClientStatus(const int status)
	{
		this->m_ClientStatus = status;
		return 0;
	}
	
	//获取客户端状态
	const int FSocket::GetClientStatus()
	{
		return this->m_ClientStatus;
	}
	
	
	
	//回调函数，当底层其他操作执行后，如果需要向客户端回馈消息，则回调当前函数
	// int FSocket::AddItemToRecvVector(const IPSERVER::ST_MSG& msg)
	// {
	// 	RECV_PushBack(msg);
	// 	return 0;
	// }
	
	//从客户端接收数据执行体
	void FSocket::OnReceive()
	{
		printf("OnReceive thread start!\n");
		if(this->m_ClientStatus < 0 || this->m_FD < 3)
		{
			printf("OnReceive thread exit\n");
			return;
		}
		
		fd_set readfd;
		struct timeval tv;
		
		int64_t ret = -1;
		
		
		//当前服务器状态是未停止
		while(this->m_ClientStatus >= 0)
		{
			FD_CLR(m_FD, &readfd);
			FD_ZERO(&readfd);
			FD_SET(m_FD, &readfd);

			//设定2s超时
			tv.tv_sec = 0;
			tv.tv_usec = 50000;
		
			ret = select(m_FD+1, &readfd, NULL, NULL, &tv);
			
			//select通道发生错误，退出循环
			if(-1 >= ret)
			{
				printf("socket channel abnomal where OnReceive thread!\n");
                this->m_ClientStatus = -1;
				break;
			}
			//select超时，continue执行
			if(0 == ret)
			{
				//printf("select timeout where OnReceive thread!\n");
				continue;
			}
			
			//判断当前读取数据的通道中有数据进入
			//if(FD_ISSET(m_FD, &readfd))
			{
                uint8_t rcv_msg[MSG_LEN] = {0};
				
				ret = read(m_FD, rcv_msg ,MSG_LEN);
				int64_t read_Len = ret;
				//ret = Common::Read(this->m_FD, rcv_msg, MSG_LEN, &read_Len, 2000);
				
				//接收消息内容打印
				printf("Receive messages %ld bytes where OnReceive thread:\n", ret);
				
				if(-1 >= ret)
				{
					printf("read data failed from client where OnReceive thread that read len is %ld\n", ret);
					
					//设定服务器退出状态
					m_ClientStatus = -1;
                    break;
				}
				else if(0 == ret)
				{
					//设定服务器退出状态
					m_ClientStatus = -1;
					printf("client disconnected where OnReceive thread that read len is 0!!!\n");
					break;
				}
				else 
				{
					//打印收到的消息
					for(int j=0;j<ret;j++)
					{
						printf("[%02x]", rcv_msg[j]);
					}
					
					printf("\n");
							
                    ST_MSG msg;
					//当前读取的数据量大于消息头+消息尾
					if(read_Len > sizeof(msg.st_head)+sizeof(msg.st_tail))
					{
						//复制头数据
						memcpy((void*)&(msg.st_head), rcv_msg,sizeof(msg.st_head));
						
						printf("msg.st_head.st_len:%lld\n", msg.st_head.st_len);
						printf("msg.st_head.st_commond:%lld\n", msg.st_head.st_command);

						//当前接收的消息满足一个消息结构（消息长度判断）
						if(sizeof(msg.st_head)+msg.st_head.st_len+sizeof(msg.st_tail) <= read_Len)
						{
							msg.st_msg = new uint8_t[msg.st_head.st_len];

							memcpy(msg.st_msg, rcv_msg+sizeof(msg.st_head), msg.st_head.st_len);

							memcpy((void*)&(msg.st_tail), rcv_msg+sizeof(msg.st_head)+msg.st_head.st_len, sizeof(msg.st_tail));

							
							
							printf("\n\n从客户端接收到的数据，校验正确\n\n");
							//向接收消息的vector中添加一条数据信息
							this->RECV_PushBack(msg);		
						}
						else//接收到的消息并不满足一个消息结构长度
						{
							printf("read data succeed from client but OnReceive thread that read len is %ld\n", ret);
							continue;
						}
					}
					else
					{
						printf("current read buffer don`t adapt frame\n"
							"read_Len:%ld \tsizeof(msg.st_head)+sizeof(msg.st_tail):%ld",
							read_Len, sizeof(msg.st_head)+sizeof(msg.st_tail));
					}				    
				}
			}
		}
		
		printf("OnReceive thread exit\n");
	}
	
	//向客户端发送数据执行体
	void FSocket::OnSend()
	{
		printf("start OnSend thread!\n");
		
	 	if(m_ClientStatus < 0 || m_FD < 3)
	 	{
	 		printf("OnSend thread exit\n");
	 		return;
	 	}
		
	 	//当前服务器状态是未停止
	 	while(m_ClientStatus >= 0)
	 	{
	 		//当前发送消息的vector不为空
	 		while(!this->m_SendMsg.empty())
	 		{
	 			if(this->m_FD < 0 || this->m_ClientStatus < 0)
	 			{
	 				break;
	 			}
	 			//取发送数据vector中的首个元素
	 			vector<IPSERVER::ST_MSG>::iterator iter = this->m_SendMsg.begin();
			
				//开始发送指令消息
				ST_MSG s_msg = this->m_SendMsg[0];
				
				
				uint64_t wLen = 0;
				const uint64_t s_len = s_msg.st_head.st_len+sizeof(s_msg.st_head)+sizeof(s_msg.st_tail);
				
				//发送buffer整理
				uint8_t s_buffer[s_len] = {0};
				memcpy(s_buffer, (uint8_t*)&s_msg.st_head, sizeof(s_msg.st_head));
				memcpy(s_buffer+sizeof(s_msg.st_head), s_msg.st_msg, s_msg.st_head.st_len);
				memcpy(s_buffer+sizeof(s_msg.st_head)+s_msg.st_head.st_len, (uint8_t*)&s_msg.st_tail, sizeof(s_msg.st_tail));
				
				
				//执行发送数据，超时设置为2000ms
				int64_t ret  = Common::Write(this->m_FD, s_buffer, s_len, &wLen, 2000);
				
				if(ret < 0)
				{
					printf("where OnSend thread, send messages to client failed because of %s\n", strerror(errno));
				}
				else if(wLen != s_len)
				{
					printf("where OnSend thread, send messages to client succeed, but write length %d, not %ld\n", wLen, s_len);
				}
				else
				{
					printf("where OnSend thread, send messages to client succeed\n");
				}
				
				//发送消息内容打印
				printf("where OnSend thread, Send messages %d bytes:\n",wLen);
				for(int j=0;j<wLen;j++)
				{
				  printf("[%02x]", s_buffer[j]);
				}
				printf("\n");
	 			
				
				
	 			//从发送数据的vector向量中删除首元素
	 			this->SEND_Erase(iter);
				
	 		}
	 	}

		
	 	printf("OnSend thread exit\n");		
	}
	
	//解析数据执行体
	void FSocket::OnAnalysis()
	{
		printf("start OnAnalysis thread!\n");
		
		if(this->m_ClientStatus < 0 || this->m_FD < 3)
		{
			printf("where OnAnalysis thread, Analysis messages thread exit\n");
			return;
		}
		
		//当前服务器状态是未停止
		while(this->m_ClientStatus >= 0)
		{
			//接收数据的vector有未处理的消息
			while(!this->m_ReceiveMsg.empty())
			{
				if(this->m_ClientStatus < 0)
				{
					break;
				}
				
				//取接收数据vector中的首个元素，begin函数取vector首个元素地址，
				//end函数取vector末尾元素地址下一地址（此位置不存储任何元素）
				vector<IPSERVER::ST_MSG>::iterator iter = this->m_ReceiveMsg.begin();
				
				if(this->m_Callback_Func != NULL && this->m_Callback_Func != nullptr)
				{
					//调用回调函数
					ST_MSG *s_msg = (ST_MSG*)m_Callback_Func(&this->m_ReceiveMsg[0]);
					//直接发送数据到客户端
					if(s_msg != NULL && s_msg != nullptr)
					{
						//向发送消息的队列中添加一个发送对象
						this->SEND_PushBack(*s_msg);
						
						
						/*uint64_t wLen = 0;
						const uint64_t s_len = s_msg->st_head.st_len+sizeof(s_msg->st_head)+sizeof(s_msg->st_tail);
						
						//执行发送数据，超时设置为2000ms
						int64_t ret  = Common::Write(this->m_FD, (uint8_t*)(s_msg), s_len, &wLen, 2000);
						
						if(ret < 0)
						{
							printf("where OnAnalysis thread, send messages to client failed because of %s\n", strerror(errno));
						}
						else if(wLen != s_len)
						{
							printf("where OnAnalysis thread, send messages to client succeed, but write length %d, not %ld\n", wLen, s_len);
						}
						else
						{
							printf("where OnAnalysis thread, send messages to client succeed\n");
						}
						
						
						//发送消息内容打印
						printf("where OnAnalysis thread, Send messages %d bytes:\n",wLen);
						for(int j=0;j<wLen;j++)
						{
						  printf("[%02x]", ((uint8_t*)(s_msg))[j]);
						}
						printf("\n");*/
						
						
					}
					else
					{
						printf("where OnAnalysis thread, execute Callback_Func return NULL\n");
					}
				}
				else
				{
					printf("where OnAnalysis thread, The socket server define callback function is NULL\n");
				}
				
				
				//从接收数据的vector向量中删除首元素
				this->RECV_Erase(iter);
			}
		}
		
		printf("OnAnalysis thread exit\n");
	}

    
    //关闭
    void FSocket::Close()
    {
        this->m_ClientStatus = -1;

        //等待100毫秒
        usleep(100000);
        close(this->m_FD);
        this->m_FD = -1;
    }
	
}//end namespace IPSERVER
