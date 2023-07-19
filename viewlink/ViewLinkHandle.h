/*****************
 * project:IntelligentPerceptionServer
 * source:ViewLinkHandle.h
 * author:FEX
 * time:2023-05-23
 * description:
 * 光电球通信类定义
 *	
 * ***************/


#ifndef VIEWLINKHANDLE_H
#define VIEWLINKHANDLE_H

#include <thread>
#include <iostream>
#include <dirent.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
  
#include <sys/select.h>  
#include <sys/time.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <sys/stat.h> 

//#include "../common/Common.h"
//#include "../serial/serial.h"

#include "../network/UDPClient.h"

#include "./ViewLink.h"


using namespace std;


namespace IPSERVER
{
    class ViewLinkHandle
    {
        private:
            
            //光电球服务器IP地址
            char m_ServerIP[IP_LEN];
            //光电球服务器端口号
            char m_ServerPort[PORT_LEN];


            //回调函数
            //VLK_DevStatus_CB m_CallBack;

            //光电球连接参数
            //VLK_CONN_PARAM m_Param;


            //串口操作句柄
            //IPSERVER::Serial *m_SerialHandle;

            //UDPClient操作句柄
            IPSERVER::UDPClient *m_UDPClient;

            //静态对象
            static ViewLinkHandle* m_Object;

            //运行状态 -1-无效，0-初始化成功，1-连接成功，
            static int m_RunStatus;



            //构造函数
            ViewLinkHandle();

            
            //
            int UnInit();
            
            //注册回调函数
            int RegisterCallback(VLK_DevStatus_CB pDevStatusCB, void* pUserParam);

            

            //connect callback，连接状态处理
            //static int VLK_ConnStatusCallback(int iConnStatus, const char* szMessage, int iMsgLen, void* pUserParam);

            //Register Callback，光电球状态数据回馈处理
            //static int VLK_DevStatusCallback(int iType, const char *szBuffer, int iBufLen, void *pUserParam);

            
            //连接
            static int Connect();

            

        public:
            //析构函数
            //~ViewLinkHandle();
            //禁用拷贝构造
            ViewLinkHandle(const ViewLinkHandle& object) = delete;
            ViewLinkHandle(const ViewLinkHandle&& object) = delete;
            //禁用运算发重载
            ViewLinkHandle& operator=(ViewLinkHandle& object) = delete;
            ViewLinkHandle& operator=(ViewLinkHandle&& object) = delete;


            //获取单例对象
            static ViewLinkHandle* GetInstance();

            //初始化
            int Init(IPSERVER::UDPClient *client);

            
            //光电归零
            static int Home();

            //移动
            int Move(short sHorizontalSpeed, short sVeritcalSpeed);
            //
            int Stop();

            //启动
            static int Start();


            //运行运行状态 -1-无效，0-初始化成功，1-连接成功，
            const int GetStatus() const;

            
            //运行运行状态 -1-无效，0-初始化成功，1-连接成功，
            const int SetStatus(int status);

            //调焦
            int ZoomTo(float ft);

            //调焦+
            int ZoomIn();
            //调焦-
            int ZoomOut();

            //mv to private
            uint32_t m_ZoomScale;

            uint32_t ZoomScale();
            short m_laserDist;

            int Stopp();


    };//end class ViewLinkHandle


}//end namespace IPSERVER






#endif