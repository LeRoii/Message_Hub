/*****************
 * project:IntelligentPerceptionServer
 * source:UDPClient.h
 * author:FEX
 * time:2023-05-23
 * description:
 * UDP通信服务端类定义
 * copyright:
 *	
 *	
 * ***************/

#ifndef  UDPCLIENT_H
#define  UDPCLIENT_H


#include "../common/Common.h"

namespace IPSERVER
{
    class UDPClient
    {
        private:
            
            //服务器IP地址
            char m_ServerIP[IP_LEN];
            //服务器端口号
            char m_ServerPort[PORT_LEN];


            UDP_CALLBACK_FUNC m_Callback_func;

            //当前运行状态
            int m_RunStatus;

            
			std::thread r_thread;


            //使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
			//从客户端接收数据执行体
			void OnReceive();


            int m_Socket;

        public:
            //构造函数
            UDPClient();
            //析构函数
            ~UDPClient();



            //初始化
            int Init();

            //启动
            int OnStart();


            //停止
            int OnStop();



            //设置回调函数
            int SetCallbackFunc(UDP_CALLBACK_FUNC func);

            //发送数据
            int OnSend(short laserDist,double outlat,double outlong,int obj_x,int obj_y,int obj_class);




    };//end class UDPClient


}//end namespace IPSERVER





#endif
 