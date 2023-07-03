/*****************
 * project:IntelligentPerceptionServer
 * source:UDPClient.h
 * author:FEX
 * time:2023-05-23
 * description:
 * UDP通信服务端类实现
 * copyright:
 *	
 *	
 * ***************/


#include "./UDPClient.h"
#include <iostream>
#include <cstdint>


namespace IPSERVER
{

    //构造函数
    UDPClient::UDPClient()
    {
		//读取配置文件中的服务器IP和端口号信息
		Common::GetValueByKeyFromConfig("UDPServer_IP", m_ServerIP, IP_LEN);
		Common::GetValueByKeyFromConfig("UDPServer_PORT", m_ServerPort, PORT_LEN);
    }


    //析构函数
    UDPClient::~UDPClient()
    {


    }



    //初始化
    int UDPClient::Init()
    {
        //创建socket
        this->m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(this->m_Socket < 0)
        {
            std::cout<<"创建UDP Client Socket failed!"<<std::endl;

            return -1;
        }
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        addr.sin_addr.s_addr = INADDR_ANY;
        // addr.sin_addr.s_addr = inet_addr("192.168.1.187");
        int ret = bind(this->m_Socket, (struct sockaddr*)&addr, sizeof(addr));
        if(ret == -1)
        {
            perror("UDP bind fail");
            this->m_RunStatus = -1;
            return 0;
        }

        this->m_RunStatus = 1;

        return 0;
    }

    //启动
    int UDPClient::OnStart()
    {
		//使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
		//创建接收消息的线程
		r_thread = std::thread(&UDPClient::OnReceive, this);
		r_thread.detach();//主线程与子线程分离，保证主线程结束不影响子线程，确保子线程在后台运行
		printf("UDPClient create Receive thread succeed\n");
		return 0;
    }


    //停止
    int UDPClient::OnStop()
    {
        this->m_RunStatus = -1;

        return 0;
    }



    //设置回调函数
    int UDPClient::SetCallbackFunc(UDP_CALLBACK_FUNC func)
    {
        this->m_Callback_func = func;
        return 0;
    }

    //发送数据
    int UDPClient::OnSend(short laserDist,double outlat,double outlong,int obj_x,int obj_y,int obj_class)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8888);
        addr.sin_addr.s_addr = inet_addr("192.168.1.187");
        char buf[19];
        buf[0] = 0x6a;
        buf[1] = 0xa6;
        //测距： 低位，中位，高位

        buf[2] = static_cast<char>(laserDist & 0xFF);
        buf[3] = static_cast<char>((laserDist >> 8) & 0xFF);
        buf[4] = static_cast<char>((laserDist >> 16) & 0xFF);

        //纬度：4字节
        int outlat_i = outlat*1000000;
        
        buf[5] = static_cast<char>(outlat_i & 0xFF);
        buf[6] = static_cast<char>((outlat_i >> 8) & 0xFF);
        buf[7] = static_cast<char>((outlat_i >> 16) & 0xFF);
        buf[8] = static_cast<char>((outlat_i >> 24) & 0xFF);

        //经度：4字节
        // uint32_value = *reinterpret_cast<uint32_t*>(&outlong);

        int outlong_i = outlong*1000000;
        buf[9] = static_cast<char>(outlong_i & 0xFF);
        buf[10]= static_cast<char>((outlong_i >> 8) & 0xFF);
        buf[11] = static_cast<char>((outlong_i >> 16) & 0xFF);
        buf[12]= static_cast<char>((outlong_i >> 24) & 0xFF);

        //目标像素位置
        buf[13] = static_cast<char>(obj_x & 0xFF);
        buf[14] = static_cast<char>((obj_x >> 8) & 0xFF);
        buf[15] = static_cast<char>(obj_y & 0xFF);
        buf[16] = static_cast<char>((obj_y >> 8) & 0xFF);



        //目标类别
        buf[17] = static_cast<char>(obj_class & 0xFF);



        int ret = sendto(this->m_Socket, buf, sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(addr));

        printf("udp send:%d\n", ret);
        return 0;
    }


    
    //使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
    //从客户端接收数据执行体
    void UDPClient::OnReceive()
    {
        while(this->m_RunStatus > 0)
        {
            uint8_t buf[150]={0};
            printf("UDPClient::OnReceive, %d\n", this->m_Socket);
            int rcvLen = recvfrom(this->m_Socket, buf, sizeof(buf), 0, NULL, NULL);
            printf("rec len:%d\n", rcvLen);
            if(rcvLen > 0)
            {
                for(int i=0;i<rcvLen;i++)
                {
                    printf("%#x, ", buf[i]);
                }
                std::cout<<std::endl;

                if(this->m_Callback_func != nullptr && this->m_Callback_func != NULL)
                {
                    this->m_Callback_func(buf, rcvLen);
                }
                else
                {
                    std::cout<<std::endl<<"UDP callback function is NULL"<<std::endl;
                }
                usleep(300000);

            }

        }
    }


}//end namespace IPSERVER