/*****************
 * project:IntelligentPerceptionServer
 * source:ViewLinkHandle.h
 * author:FEX
 * time:2023-05-23
 * description:
 * 光电球通信类实�?
 *	
 * ***************/

#include "./ViewLinkHandle.h"


namespace IPSERVER
{

    int ViewLinkHandle::m_RunStatus = -1;
    
    //connect callback，连接状态处�?
    int VLK_ConnStatusCallback(int iConnStatus, const char *szMessage, int iMsgLen, void *pUserParam)
    {
        if (VLK_CONN_STATUS_TCP_CONNECTED == iConnStatus)
        {
            cout << "TCP Gimbal connected !!!" << endl;
            //this->m_RunStatus = 1;
            //ViewLinkHandle::m_RunStatus = 1;
            IPSERVER::ViewLinkHandle::GetInstance()->SetStatus(1);

            //归零
            //IPSERVER::ViewLinkHandle::GetInstance()->Home();
        }
        else if (VLK_CONN_STATUS_TCP_DISCONNECTED == iConnStatus)
        {
            cout << "TCP Gimbal disconnected !!!" << endl;
            //g_bConnected = false;
        }
        else if (VLK_CONN_STATUS_SERIAL_PORT_CONNECTED == iConnStatus)
        {
            cout << "serial port connected !!!" << endl;
            //g_bConnected = true;
        }
        else if (VLK_CONN_STATUS_SERIAL_PORT_DISCONNECTED == iConnStatus)
        {
            cout << "serial port disconnected !!!" << endl;
            //g_bConnected = false;
        }
        else
        {
            cout << "unknown connection stauts: " << iConnStatus << endl;
            //g_bConnected = false;
        }

        return 0;
    }


    
    //Register Callback，光电球状态数据回馈处�?
    int VLK_DevStatusCallback(int iType, const char *szBuffer, int iBufLen, void *pUserParam)
    {
        if (VLK_DEV_STATUS_TYPE_MODEL == iType)
        {
            VLK_DEV_MODEL *pModel = (VLK_DEV_MODEL *)szBuffer;
            cout << "model code: " << pModel->cModelCode << ", model name: " << pModel->szModelName << endl;
        }
        else if (VLK_DEV_STATUS_TYPE_CONFIG == iType)
        {
            VLK_DEV_CONFIG *pDevConfig = (VLK_DEV_CONFIG *)szBuffer;
            cout << "VersionNO: " << pDevConfig->cVersionNO << ", DeviceID: " << pDevConfig->cDeviceID << ", SerialNO: " << pDevConfig->cSerialNO << endl;
        }
        else if (VLK_DEV_STATUS_TYPE_TELEMETRY == iType)
        {
            /*
            * once device is connected, telemetry information will keep updating,
            * in order to avoid disturbing user input, comment out printing telemetry information
            */
            VLK_DEV_TELEMETRY *pTelemetry = (VLK_DEV_TELEMETRY *)szBuffer;
             cout << "Yaw: " << pTelemetry->dYaw << ", Pitch: " << pTelemetry->dPitch << ", sensor type: " 
             << pTelemetry->emSensorType << ", Zoom mag times: " << pTelemetry->dZoomMagTimes << endl;
            // cout << "Yaw: " << pTelemetry->dYaw << ", Pitch: " << pTelemetry->dPitch << ", sensor type: " 
            // << pTelemetry->emSensorType << endl;
            //gettm(getTimeStamp());
            //printf("Yaw:%lf, Pitch:%lf\n", pTelemetry->dYaw, pTelemetry->dPitch);

            IPSERVER::ViewLinkHandle::GetInstance()->m_ZoomScale = uint32_t(pTelemetry->dZoomMagTimes);
            IPSERVER::ViewLinkHandle::GetInstance()->m_laserDist = pTelemetry->sLaserDistance;

            cout<<"pTelemetry->sLaserDistance"<<pTelemetry->sLaserDistance<<endl;

            //处理光电球返回的数据


        }
        else
        {
            cout << "error: unknown status type: " << iType << endl;
        }

        return 0;
    }

    //静态对象初始化
    ViewLinkHandle* ViewLinkHandle::m_Object = new ViewLinkHandle();

    
    //构造函�?
    ViewLinkHandle::ViewLinkHandle()
    {
        ViewLinkHandle::m_RunStatus = -1;

        //获取配置文件中的IP配置
        Common::GetValueByKeyFromConfig("VIEWLINK_IP", m_ServerIP, IP_LEN);

        //获取配置文件中的端口配置
		Common::GetValueByKeyFromConfig("VIEWLINK_PORT", m_ServerPort, PORT_LEN);

        std::cout<<"从配置文件中读取�?光电球光电球 Server IP:"<<m_ServerIP<<std::endl;
        std::cout<<"从配置文件中读取�?光电球光电球 Server PORT:"<<m_ServerPort<<std::endl;

        //m_SerialHandle = nullptr;
        this->m_UDPClient = nullptr;
        
    }

    //获取单例对象
    ViewLinkHandle* ViewLinkHandle::GetInstance()
    {
        if(ViewLinkHandle::m_Object == nullptr || ViewLinkHandle::m_Object == NULL)
        {
            ViewLinkHandle::m_Object = new ViewLinkHandle();
        }

        return ViewLinkHandle::m_Object;
    }

    //初始�?
    int ViewLinkHandle::Init(IPSERVER::UDPClient *client)
    {
        //this->m_SerialHandle = sPort;
        this->m_UDPClient = client;
        // print sdk version
        cout << "ViewLink SDK version: " << GetSDKVersion() << endl;

        // initialize sdk
        int iRet = VLK_Init();
        if (VLK_ERROR_NO_ERROR != iRet)
        {
            cout << "VLK_Init failed, error: " << iRet << endl;

            ViewLinkHandle::m_RunStatus = -1;
            return -1;
        }

        printf("viewlink init success\n");

        VLK_SwitchLaser(1);
        ViewLinkHandle::m_RunStatus = 0;
        return 0;
    }


    //注册回调函数
    int ViewLinkHandle::RegisterCallback(VLK_DevStatus_CB pDevStatusCB, void* pUserParam)
    {
        VLK_RegisterDevStatusCB(pDevStatusCB, pUserParam);

        return 0;
    }

    //连接   
    int ViewLinkHandle::Connect()
    {
        VLK_CONN_PARAM param;
        memset(&param, 0, sizeof(param));
        param.emType = VLK_CONN_TYPE_TCP;
        strncpy(param.ConnParam.IPAddr.szIPV4, "192.168.2.119", sizeof("192.168.2.119"));
        param.ConnParam.IPAddr.iPort = 2000;

        std::this_thread::sleep_for(std::chrono::seconds(10));
        int iRet = VLK_Connect(&param, VLK_ConnStatusCallback, NULL);
        if (VLK_ERROR_NO_ERROR != iRet)
        {
            cout << "VLK_Connect failed, error: " << iRet << endl;

            ViewLinkHandle::m_RunStatus = -1;

            //断开
            VLK_UnInit();

            return -1;
        }

        printf("[ViewLinkHandle] connect end\n");

        return 0;
    }


    //移动
    int ViewLinkHandle::Move(short sHorizontalSpeed, short sVeritcalSpeed)
    {
        VLK_Move(sHorizontalSpeed, sVeritcalSpeed);

        return 0;
    }

    //
    int ViewLinkHandle::Home()
    {
        VLK_Home();
        return 0;
    }

    //
    int ViewLinkHandle::UnInit()
    {
        //断开
        VLK_UnInit();
        return 0;
    }

    //
    int ViewLinkHandle::Stop()
    {
        //归零
        VLK_Home();
        //断开
        VLK_UnInit();

        return 0;
    }

    int ViewLinkHandle::Stopp()
    {
        VLK_Stop();
        return 0;
    }


    //
    int ViewLinkHandle::Start()
    {
        if(ViewLinkHandle::m_RunStatus >= 0)
        {
            //注册回调函数
            VLK_RegisterDevStatusCB(VLK_DevStatusCallback, NULL);
            //连接服务
            ViewLinkHandle::Connect();

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));

            printf("[ViewLinkHandle] start end\n");

            //归零
            VLK_Home();
        }

        return 0;
    }


    //获取状�?
    const int ViewLinkHandle::GetStatus() const
    {
        return ViewLinkHandle::m_RunStatus;
    }


    //运行运行状�?-1-无效�?-初始化成功，1-连接成功�?
    const int ViewLinkHandle::SetStatus(int status)
    {
        ViewLinkHandle::m_RunStatus = status;
        return 0;
    }

    
    //
    int ViewLinkHandle::ZoomTo(float ft)
    {
        std::cout<<"调焦到："<<ft<<std::endl;
        VLK_ZoomTo(ft);

        return 0;
    }


    //调焦+
    int ViewLinkHandle::ZoomIn()
    {
        VLK_ZoomIn(4);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        VLK_StopZoom();

        // VLK_ZoomTo(4.0);

        

        return 0;
    }

    //调焦-
    int ViewLinkHandle::ZoomOut()
    {
        VLK_ZoomOut(4);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        VLK_StopZoom();

        return 0;
    }

    uint32_t ViewLinkHandle::ZoomScale()
    {
        return m_ZoomScale;
    }
    

}//end namespace IPSERVER