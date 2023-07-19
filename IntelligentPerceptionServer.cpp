/*****************
 * project:IntelligentPerceptionServer
 * source:IntelligentPerceptionServer.cpp
 * author:FEX
 * time:2023-03-24
 * description:
 * 程序主函数入口
 * copyright:
 *		附文件《Socket通信协议.docx》《底层通信协议.docx》
 *
 * ***************/

 
#include "./network/TCPServer.h"
#include "./ipc/MessageQueueHandle.h"
//#include "./serial/serial.h"
#include <ctime>

//#include "./viewlink/ViewLinkHandle.h"
#include "./network/UDPClient.h"
#include "MQTTClient.h"
#include <math.h>
#include "common/Common.h"


using namespace std;
using namespace IPSERVER;

IPSERVER::UDPClient* client;

double latitude, longitude;


//回调函数，当服务端socket每次接收到客户端发送的消息时就会调用这个回调函数对客户端消息进行解析、执行、回馈
//参数及返回值类型可查阅Model.h文件
void* Callback_Func(void* parameters);

//回调函数，从串口收到的数据，数据长度
int Serial_Callback(uint8_t*, uint32_t);


//回调函数，从串口收到的数据，数据长度
int UDP_Callback(uint8_t*, uint32_t);

static void signal_handle(int signum)
{
	IPSERVER::Common::SetSigFlag(true);
	// _xdma_reader_ch0.xdma_close();
}

static double distFactor = 0.02f;//dist1/w1
double CalObjDist(int w)
{
    return w*distFactor;
}

//服务器启动必须在一个单独的线程里运行，这样不影响主线程
int main(int argc, char *argv[])
{
    
    //信号处理
	struct sigaction sig_action;
	sig_action.sa_handler = signal_handle;
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = 0;
	sigaction(SIGINT, &sig_action, NULL);

      
/*################################## MQTT Client Start #######################################################*/

    //启动线程, 开启MQSTT通信
    std::thread t1(MQTTStart, MQTTCLIENT::Common::mosq);
    t1.detach();      
/*################################## MQTT Client end #######################################################*/
    // usleep(100000);
    // printf("11111sedn\n");
    // char data[MAX_MSG_LEN]={0};
    // ST_THREATLEVEL_RECV obj;
    // memset(&obj, 0, sizeof(obj));
    // obj.st_type = 3;
    // memcpy(obj.st_api, "mapPlugin/ai", strlen("mapPlugin/ai")); 

    // char cc[100] = "{\"ww\":\"123\"}"; 
    // MQTTCLIENT::Common::ThreatST_ConvertTo_JSON(obj, data, MAX_MSG_LEN);
    // int mret = my_mosquitto_publish(MQTTCLIENT::Common::mosq, strlen(data), data);

        

    // if(mret == MOSQ_ERR_SUCCESS)
    // {
    //     printf("send okkkk\n");
    //     //std::cout<<"\n\nTS send respond msg:\n"<<data<<std::endl;
    //     //respondInt++;
    // }

    // //释放 mosquitto 客户端实例关联的内存
	// mosquitto_destroy(MQTTCLIENT::Common::mosq);

    // MQTTCLIENT::Common::mosq = nullptr;

    // //调用与库相关的资源释放。
	// mosquitto_lib_cleanup();

    // printf("11111sedn\n");

    // return 0;
/*################################## MQTT Client end #######################################################*/



    //创建UDP对象
    // IPSERVER::UDPClient* client = new IPSERVER::UDPClient;
    client = new IPSERVER::UDPClient;
    client->Init();
    client->SetCallbackFunc(UDP_Callback);
    client->OnStart();


    IPSERVER::ViewLinkHandle* VLHandle = IPSERVER::ViewLinkHandle::GetInstance();
    if(VLHandle != NULL)
    {
        //初始化光电球
        VLHandle->Init(client);
        //光电球启动
        VLHandle->Start();
        //光电球连接
        //IPSERVER::ViewLinkHandle::Connect();
        
        IPSERVER::MessageQueueHandle::GetInstance()->SetVLHandle(VLHandle);

    }

    IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(1.0);

    
    
    //启动IPC消息队列，线程detached 
    IPSERVER::MessageQueueHandle::GetInstance()->OnStart();
    
/*################################## serial port #######################################################*/

    // char str[1024] = {0};
    // //定义两个串口对象
    // IPSERVER::Serial serial_servo[2];
    // //读取配置文件中串口1使能标志
    // if(IPSERVER::Common::GetValueByKeyFromConfig("SERIAL_PORT_1_ENABLE", str, 1024) == -1)
    // {
    //     std::cout<<"从配置文件读取串口1使能信息错误"<<std::endl;
    // }
    // else
    // {
    //     std::cout<<"串口1使能值为："<<str<<std::endl;
    //     if(strncmp(str, "1", 1)==0)
    //     {
    //         //设置回调函数
    //         serial_servo[0].set_callback_func(Serial_Callback);
    //         //设置串口
    //         serial_servo[0].set_serial(1);
    //         //启动串口数据接收线程
    //         serial_servo[0].OnStart();

    //         //给IPC消息队列Handle传送串口handle
    //         IPSERVER::MessageQueueHandle::GetInstance()->SetSerialPort(&(serial_servo[0]));


    //     }
    // }
    // //读取配置文件中串口2使能标志
    // if(IPSERVER::Common::GetValueByKeyFromConfig("SERIAL_PORT_2_ENABLE", str, 1024) == -1)
    // {
    //     std::cout<<"从配置文件读取串口2使能信息错误"<<std::endl;
    // }
    // else
    // {
    //     std::cout<<"串口2使能值为："<<str<<std::endl;
    //     if(strncmp(str, "1", 1)==0)
    //     {
    //         //设置回调函数
    //         //serial_servo[1].set_callback_func(Serial_Callback);
    //         //设置串口
    //         serial_servo[1].set_serial(2);
    //         //启动串口数据接收线程
    //         serial_servo[1].OnStart();
    //     }
    // }

/*################################## serial port #######################################################*/




    //获取对象，TCP服务
    // IPSERVER::TCPServer *server = TCPServer::GetInstance(Callback_Func);
    
    // if(NULL != server)
    // {
    //     //启动服务器，阻塞在listen
    //     server->Start();
        
    // }

    

    // //消息队列发送查询状态信息
    // IPSERVER::ST_MQ_COMSEND_MSG msg;
    // msg.st_MQ_Type = sizeof(IPSERVER::ST_MQ_COMSEND_MSG)-sizeof(msg.st_MQ_Type);
    // msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_SearchStatus;
    // msg.st_Tracking_Status = 0;//跟踪状态
    // msg.st_Identify_Status = 0;//识别状态
    // msg.st_Fusion_Status = 0;//融合状态
    // msg.st_Plugflow_Status = 0;//推流状态
    // IPSERVER::MessageQueueHandle::GetInstance()->SendMsg(msg);





    // //如果TCP服务器不开启listen，则在此处主线程一直阻塞，直到收到Crtl+C命令后推出
    while(!IPSERVER::Common::GetSigFlag())
    {
        printf("IN WHILE\n");
        usleep(100000);
        auto boxes = IPSERVER::MessageQueueHandle::GetInstance()->m_boxes;
        double outlat, outlong;
        double x,y,z;
        // convertCoordinate(latitude, longitude, 0, 0, 0,x,y,z,outlat, outlong); 

        cJSON *js = cJSON_CreateObject();
        cJSON *Arr = cJSON_CreateArray();
        for(int i=0;i<boxes.size();i++)
        {
            // print("before cal:w:%d, longitude:%f, latitude:%f\n", boxes[i].w, longitude, latitude);
            // convertCoordinate(latitude, longitude, 0, 0, 0,boxes[i].x,boxes[i].y,CalObjDist(boxes[i].w), outlat, outlong);
            calculateTargetPosition(latitude, longitude, CalAngl(boxes[i].x), CalObjDist(boxes[i].w)/1000, outlat, outlong);
            printf("in while:x:%d, y:%d, z:%f, w:%d, log:%f, lat:%f, outlog:%f, outlat:%f\n",boxes[i].x,boxes[i].y,CalObjDist(boxes[i].w),
            boxes[i].w, latitude, longitude, outlat, outlong);
            // convertCoordinate(latitude, longitude, 0, 0, 0,boxes[i].x_3d,boxes[i].y_3d,boxes[i].z_3d, outlat, outlong);
            //处理markerinfo信息
            cJSON *mi = cJSON_CreateObject();

            cJSON_AddStringToObject(mi, "timestampAndUserId", std::to_string(i).c_str());
            cJSON_AddNumberToObject(mi, "latitude", outlat);
            cJSON_AddNumberToObject(mi, "longitude", outlong);
            cJSON_AddStringToObject(mi, "markerUrl", "");

            //markerdetailinfo信息
            cJSON *mdi = cJSON_CreateObject();
            
            cJSON_AddStringToObject(mdi, "camp", "unknown");
            cJSON_AddStringToObject(mdi, "targetName", boxes[i].obj_id == 0 ? "person" : "vehicle");
            cJSON_AddNumberToObject(mdi, "longitude", outlong);
            cJSON_AddNumberToObject(mdi, "latitude", outlat);
            cJSON_AddNumberToObject(mdi, "targetCount", 1);
            cJSON_AddStringToObject(mdi, "targetDirection", "");
            cJSON_AddNumberToObject(mdi, "targetSpeed", 0);
            cJSON_AddNumberToObject(mdi, "targetState", 0);
            cJSON_AddNumberToObject(mdi, "isWeapon", 0);
            cJSON_AddNumberToObject(mdi, "mHitRadius", 10);
            cJSON_AddStringToObject(mdi, "targetType", boxes[i].obj_id == 0 ? "person" : "vehicle");


            cJSON_AddItemToObject(mi, "markerDetailInfo", mdi);

            cJSON_AddItemToArray(Arr, mi);

        }

        if(boxes.size() == 0)
        {
            printf("boxes size:%d\n", boxes.size());
            continue;
        }

        char *tmp = cJSON_Print(Arr);
        // memcpy(str, tmp, strlen(tmp)<len?strlen(tmp):len);
        std::cout<<"qjSend_JSON:"<<tmp<<std::endl;

        int res = my_mosquitto_publish(MQTTCLIENT::Common::mosq, strlen(tmp), tmp);
        if(res == MOSQ_ERR_SUCCESS)
        {
            //std::cout<<"\n\nTS send respond msg:\n"<<data<<std::endl;
            //respondInt++;
            IPSERVER::MessageQueueHandle::GetInstance()->m_boxes.clear();
        }




        // cJSON_AddItemToObject(js, "data", Arr);


    }

    //光电球调用结束
    VLHandle->Stop();


    //结束TCP服务
    // server->SetStatus(-1);
    
    //结束消息队列线程
    IPSERVER::MessageQueueHandle::GetInstance()->SetStatus(-1);

    //结束串口对象
    // serial_servo[0].SetStatus(-1);
    // serial_servo[1].SetStatus(-1);
       
    return 0;
}


//回调函数，当服务端socket每次接收到客户端发送的消息时就会调用这个回调函数对客户端消息进行解析、执行、回馈
//参数及返回值类型可查阅Model.h文件
void* Callback_Func(void* parameters)
{
	if(NULL == parameters || nullptr == parameters)
	{
		printf("Callback_Func input parameter is NULL\n");
		
		return NULL;
	}
	
	//接收到的消息
	IPSERVER::ST_MSG* s_recv_msg = (IPSERVER::ST_MSG*)parameters;
	
	if(NULL == s_recv_msg || nullptr == s_recv_msg)
	{
		printf("Callback_Func input parameter convert to ST_MSG faild\n");
		return NULL;
	}
	
	//定义respond消息
	IPSERVER::ST_MSG* s_send_msg = new IPSERVER::ST_MSG;
	IPSERVER::ST_RESPONSE* s_respond = new IPSERVER::ST_RESPONSE;
	
	//命令执行结果
	s_respond->st_result = 0;
	//时间戳YYYY-MM-DD hh:mm:ss
	memset(s_respond->st_date, 0, sizeof(s_respond->st_date));
	time_t nowTime;
	time(&nowTime);
	struct tm *p;
	p = localtime(&nowTime);
	sprintf(s_respond->st_date,"%04d-%02d-%02d %02d:%02d:%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	std::cout<<s_respond->st_date<<std::endl;
	//附加信息
	s_respond->st_msg = NULL;
	
	
	//消息头
	s_send_msg->st_head.st_len = sizeof(IPSERVER::ST_RESPONSE);
	s_send_msg->st_head.st_command = s_recv_msg->st_head.st_command;
	
	//消息体
	s_send_msg->st_msg = new uint8_t[s_send_msg->st_head.st_len];
	//返回结果
	memcpy(s_send_msg->st_msg, &s_respond->st_result, sizeof(s_respond->st_result));
	//时间戳
	memcpy(s_send_msg->st_msg+sizeof(s_respond->st_result), s_respond->st_date, sizeof(s_respond->st_date));
	//附加信息
	//memcpy(s_send_msg->st_msg+sizeof(s_respond->st_result)+sizeof(s_respond->st_date),s_respond->st_msg,sizeof(s_respond->st_msg));
	
	
	//消息尾
	s_send_msg->st_tail.st_crc16 = 0;
	
	
	return s_send_msg;
}



//回调函数
int Serial_Callback(uint8_t* buffRcvData_servo, uint32_t retLen)
{
    if(NULL == buffRcvData_servo || nullptr == buffRcvData_servo || retLen <= 0)
	{
		printf("serial Callback_Func input parameter is NULL\n");
		
		return -1;
	}


    IPSERVER::ST_MQ_COMSEND_MSG send_msg;
    memset(&send_msg, 0, sizeof(IPSERVER::ST_MQ_COMSEND_MSG));

    send_msg.st_MQ_Type = sizeof(IPSERVER::ST_MQ_COMSEND_MSG)-sizeof(send_msg.st_MQ_Type);
    send_msg.st_Resp_Key = 8898;
    send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_SearchStatus;
    send_msg.st_Tracking_Status = 0;//跟踪状态
    send_msg.st_Identify_Status = 0;//识别状态
    send_msg.st_Fusion_Status = 0;//融合状态
    send_msg.st_Plugflow_Status = 0;//推流状态
    send_msg.st_Tracking_Target_ID = 0;//标号跟踪id
    send_msg.st_Tracking_Gate_Size = 0;//0-small,1-mid,2-large
    send_msg.st_Draw_Cross = 0;//十字分划叠加使能，0-否，1-是
    send_msg.st_Draw_Cross_Point[0] = 0;//十字划分叠加-点位坐标，[0]-X,[1]-Y
    send_msg.st_Draw_Cross_Point[1] = 0;//十字划分叠加-点位坐标，[0]-X,[1]-Y


    //根据协议文档，解析串口接收的数据，组装消息队列对象，发送给消息队列底层
    //头尾符合主控->光电通信协议
    if(retLen >= 12 && buffRcvData_servo[0] == 0x50 && buffRcvData_servo[1] == 0x60 &&
    buffRcvData_servo[3] == 0x08 && buffRcvData_servo[11] == 0xAF)
    {
    //伺服
    if(0xA1 == buffRcvData_servo[2])
    {
        std::cout<<"伺服-";
        //光电归零
        if(0xB1 == buffRcvData_servo[4])
        {
            std::cout<<"光电归零"<<endl;

            // if(IPSERVER::ViewLinkHandle::GetInstance()->GetStatus() >= 1)
            // {
            //     IPSERVER::ViewLinkHandle::GetInstance()->Home();
            // }
            // else
            // {
            //     std::cout<<"光电球状态不正常"<<std::endl<<"ViewLink Status:"<<IPSERVER::ViewLinkHandle::GetInstance()->GetStatus()<<std::endl;
            // }

        }

        //零位设定
        if(0xB3 == buffRcvData_servo[4])
        {
            std::cout<<"零位设定"<<endl;
            
        }

        //位置锁定当前
        if(0xB4 == buffRcvData_servo[4])
        {
            std::cout<<"位置锁定当前"<<endl;
            
        }

        //手动（遥感）
        if(0xB2 == buffRcvData_servo[4])
        {
            std::cout<<"手动（遥感）"<<endl;

            // if(IPSERVER::ViewLinkHandle::GetInstance()->GetStatus() >= 1)
            // {
            //     uint16_t X = buffRcvData_servo[5]*256+buffRcvData_servo[6];
            //     uint16_t Y = buffRcvData_servo[7]*256+buffRcvData_servo[8];

            //     std::cout<<"X:"<<X<<std::endl<<"Y:"<<Y<<std::endl;

            //     IPSERVER::ViewLinkHandle::GetInstance()->Move(X, Y);
            // }
            // else
            // {
            //     std::cout<<"光电球状态不正常"<<std::endl<<"ViewLink Status:"<<IPSERVER::ViewLinkHandle::GetInstance()->GetStatus()<<std::endl;
            // }
            
        }
        
        //周视（低速）
        if(0xB5 == buffRcvData_servo[4])
        {
            std::cout<<"周视（低速）"<<endl;
            
        }

        //方位校漂-
        if(0xB6 == buffRcvData_servo[4])
        {
            std::cout<<"方位校漂-"<<endl;
            
        }

        //方位校漂+
        if(0xB7 == buffRcvData_servo[4])
        {
            std::cout<<"方位校漂+"<<endl;
            
        }

        //俯仰校漂-
        if(0xB8 == buffRcvData_servo[4])
        {
            std::cout<<"俯仰校漂-"<<endl;
            
        }

        //俯仰校漂+
        if(0xB9 == buffRcvData_servo[4])
        {
            std::cout<<"俯仰校漂+"<<endl;
            
        }

        //光电随动武器模式
        if(0xBA == buffRcvData_servo[4])
        {
            std::cout<<"光电随动武器模式"<<endl;
            
        }

        //光电独立模式
        if(0xBB == buffRcvData_servo[4])
        {
            std::cout<<"光电独立模式"<<endl;
            
        }
    }

    //白光电视
    if(0xB1 == buffRcvData_servo[2])
    {
        std::cout<<"白光电视-";
        //调焦+
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"调焦+"<<endl;
            
        }
        //调焦-
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"调焦-"<<endl;
            
        }
        //调焦停
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x03 == buffRcvData_servo[6])
        {
            std::cout<<"调焦停"<<endl;
            
        }
        //视场+
        if(0xA1 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"视场+"<<endl;
            
        }
        //视场-
        if(0xA1 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"视场-"<<endl;
            
        }
        //视场停
        if(0xA1 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x03 == buffRcvData_servo[6])
        {
            std::cout<<"视场停"<<endl;
            
        }
        //亮度+
        if(0xA2 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"亮度+"<<endl;
            
        }
        //亮度-
        if(0xA2 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"亮度-"<<endl;
            
        }
        //对比度+
        if(0xA2 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"对比度+"<<endl;
            
        }
        //对比度-
        if(0xA2 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"对比度-"<<endl; 
            
        }
        //电子变倍1X
        if(0xA2 == buffRcvData_servo[4] &&
            0x04 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"电子变倍1X"<<endl; 
            
        }
        //电子变倍2X
        if(0xA2 == buffRcvData_servo[4] &&
            0x04 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"电子变倍2X"<<endl; 
            
        }
    }

    //红外
    if(0xC1 == buffRcvData_servo[2])
    {
        std::cout<<"红外-";
        //调焦+
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"调焦+"<<endl; 
            
        }
        //调焦-
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"调焦-"<<endl; 
            
        }
        //调焦停
        if(0xA1 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x03 == buffRcvData_servo[6])
        {
            std::cout<<"调焦停"<<endl; 
            
        }
        //非均匀性矫正
        if(0xA2 == buffRcvData_servo[4] &&
            0x0A == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"非均匀性矫正"<<endl; 
            
        }
        //去噪+
        if(0xA2 == buffRcvData_servo[4] &&
            0x27 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"去噪+"<<endl; 
            
        }
        //去噪-
        if(0xA2 == buffRcvData_servo[4] &&
            0x27 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"去噪-"<<endl; 
            
        }
        //亮度+
        if(0xA2 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"亮度+"<<endl;
            
        }
        //亮度-
        if(0xA2 == buffRcvData_servo[4] &&
            0x01 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"亮度-"<<endl;
            
        }
        //对比度+
        if(0xA2 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"对比度+"<<endl;
            
        }
        //对比度-
        if(0xA2 == buffRcvData_servo[4] &&
            0x02 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"对比度-"<<endl;
            
        }
        //电子变倍1X
        if(0xA2 == buffRcvData_servo[4] &&
            0x04 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"电子变倍1X"<<endl;
            
        }
        //电子变倍2X
        if(0xA2 == buffRcvData_servo[4] &&
            0x04 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"电子变倍2X"<<endl;
            
        }
        //细节增强+
        if(0xA2 == buffRcvData_servo[4] &&
            0x28 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"细节增强+"<<endl;
            
        }
        //细节增强-
        if(0xA2 == buffRcvData_servo[4] &&
            0x28 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"细节增强-"<<endl;
            
        }
    }

    //激光测距/照明
    if(0xD1 == buffRcvData_servo[2])
    {
        std::cout<<"激光测距/照明-";
        //单次测距
        if(0xA3 == buffRcvData_servo[4] &&
            0x03 == buffRcvData_servo[5])
        {
            std::cout<<"单次测距"<<endl;
            
        }
        //连续测距
        if(0xA3 == buffRcvData_servo[4] &&
            0x05 == buffRcvData_servo[5])
        {
            std::cout<<"连续测距"<<endl;
            
        }
        //测距停止
        if(0xA3 == buffRcvData_servo[4] &&
            0x07 == buffRcvData_servo[5])
        {
            std::cout<<"测距停止"<<endl;
            
        }
        //补光灯开
        if(0xA3 == buffRcvData_servo[4] &&
            0x10 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"补光灯开"<<endl;
            
        }
        //补光灯关
        if(0xA3 == buffRcvData_servo[4] &&
            0x10 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"补光灯关"<<endl;
            
        }
        //光斑+
        if(0xA3 == buffRcvData_servo[4] &&
            0x11 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"光斑+"<<endl;
            
        }
        //光斑-
        if(0xA3 == buffRcvData_servo[4] &&
            0x11 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"光斑-"<<endl;
            
        }
        //光斑停
        if(0xA3 == buffRcvData_servo[4] &&
            0x11 == buffRcvData_servo[5] &&
            0x03 == buffRcvData_servo[6])
        {
            std::cout<<"光斑停"<<endl;
            
        }
        //亮度+
        if(0xA3 == buffRcvData_servo[4] &&
            0x12 == buffRcvData_servo[5] &&
            0x01 == buffRcvData_servo[6])
        {
            std::cout<<"亮度+"<<endl;
            
        }
        //亮度-
        if(0xA3 == buffRcvData_servo[4] &&
            0x12 == buffRcvData_servo[5] &&
            0x02 == buffRcvData_servo[6])
        {
            std::cout<<"亮度-"<<endl;
            
        }
    }

    //跟踪器
    if(0xE1 == buffRcvData_servo[2] && 0xA4 == buffRcvData_servo[4])
    {
        std::cout<<"跟踪器-";
        //电视图像-可见光
        if(0x01 == buffRcvData_servo[5] && 0x02 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Fusion_Mode;
            std::cout<<"电视图像"<<endl;
            send_msg.st_Fusion_Status = 0;
        }
        //红外图像
        if(0x01 == buffRcvData_servo[5] && 0x03 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Fusion_Mode;
            std::cout<<"红外图像"<<endl;
            send_msg.st_Fusion_Status = 1;

        }

        //识别开
        if(0x08 == buffRcvData_servo[5] && 0x01 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Identify_Mode;
            std::cout<<"识别开"<<endl;
            send_msg.st_Identify_Status = 1;
        }

        //识别关
        if(0x08 == buffRcvData_servo[5] && 0x02 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Identify_Mode;
            std::cout<<"识别关"<<endl;
            send_msg.st_Identify_Status = 0;
        }

        //中心点跟踪
        if(0x05 == buffRcvData_servo[5] && 0x01 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Tracking_Mode;
            std::cout<<"中心点跟踪"<<endl;
            send_msg.st_Tracking_Status = 1;
        }

        //取消跟踪
        if(0x05 == buffRcvData_servo[5] && 0x02 == buffRcvData_servo[6])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Tracking_Mode;
            std::cout<<"取消跟踪"<<endl;
            send_msg.st_Tracking_Status = 0;
        }

        //标号跟踪
        if(0x06 == buffRcvData_servo[5])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Tracking_Mode;
            std::cout<<"标号跟踪"<<endl;
            send_msg.st_Tracking_Status = 2;
            send_msg.st_Tracking_Target_ID = buffRcvData_servo[7]*256+buffRcvData_servo[8];
            std::cout<<"标号值："<<send_msg.st_Tracking_Target_ID<<std::endl;
        }
        //触屏引导
        if(0x05 == buffRcvData_servo[5] && 0x03 == buffRcvData_servo[6])
        {
            std::cout<<"触屏引导"<<endl;

        }
        //十字分划叠加
        if(0x0A == buffRcvData_servo[5])
        {
            send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_Set_Draw_Cros_Mode;
            std::cout<<"十字分划叠加"<<endl;

            send_msg.st_Draw_Cross = 1;
            send_msg.st_Draw_Cross_Point[0] = buffRcvData_servo[6]*256+buffRcvData_servo[7];
            send_msg.st_Draw_Cross_Point[1] = buffRcvData_servo[8]*256+buffRcvData_servo[9];
            
            std::cout<<"X："<<send_msg.st_Draw_Cross_Point[0]<<std::endl;
            std::cout<<"Y："<<send_msg.st_Draw_Cross_Point[1]<<std::endl;
        }

    }
    }


    //
    printf("串口下发给光电的数据\n命令号：%d\n跟踪状态:%d\n",
            send_msg.st_Command,send_msg.st_Tracking_Status);//跟踪状态
    printf("识别状态:%d\n",send_msg.st_Identify_Status);//识别状态
    printf("融合状态:%d\n",send_msg.st_Fusion_Status);//融合状态
    printf("推流状态:%d\n",send_msg.st_Plugflow_Status);//推流状态
    printf("标号跟踪ID:%d\n",send_msg.st_Tracking_Target_ID);//标号跟踪ID
    printf("十字分划叠加使能:%d\n",send_msg.st_Draw_Cross);//十字分划叠加使能
    printf("十字分划叠加位置:(%d,%d)\n\n\n",send_msg.st_Draw_Cross_Point[0],send_msg.st_Draw_Cross_Point[1]);//十字分划叠加位置


	//将消息发送给底层IPC消息队列
    int ret = IPSERVER::MessageQueueHandle::GetInstance()->SendMsg(send_msg);


    return ret;
}



//回调函数，从串口收到的数据，数据长度
int UDP_Callback(uint8_t* buff, uint32_t len)
{
    printf("UDP_Callback\n");
    double outlat=0.0;
    double outlong=0.0;
    int obj_x = 9999;
    int obj_y = 9999;
    int obj_class = 255;
    if(len >= 115)
    {
        //****************************** IPC start *********************************************//
        IPSERVER::ST_MQ_COMSEND_MSG send_msg;
        memset(&send_msg, 0, sizeof(IPSERVER::ST_MQ_COMSEND_MSG));

        send_msg.st_MQ_Type = sizeof(IPSERVER::ST_MQ_COMSEND_MSG)-sizeof(send_msg.st_MQ_Type);
        send_msg.st_Resp_Key = 8898;
        send_msg.st_Command = IPSERVER::enum_MQ_Command::e_MQ_SearchStatus;
        send_msg.st_Tracking_Status = 0;//跟踪状态
        send_msg.st_Identify_Status = 0;//识别状态
        send_msg.st_Fusion_Status = 0;//融合状态
        send_msg.st_Plugflow_Status = 0;//推流状态
        send_msg.st_Tracking_Target_ID = 0;//标号跟踪id
        send_msg.st_Tracking_Gate_Size = 0;//0-small,1-mid,2-large
        send_msg.st_Draw_Cross = 0;//十字分划叠加使能，0-否，1-是
        send_msg.st_Draw_Cross_Point[0] = 0;//十字划分叠加-点位坐标，[0]-X,[1]-Y
        send_msg.st_Draw_Cross_Point[1] = 0;//十字划分叠加-点位坐标，[0]-X,[1]-Y

        send_msg.st_ZoomScale = IPSERVER::ViewLinkHandle::GetInstance()->ZoomScale();

        if(buff[0] != 0x5a || buff[1] != 0xa5)
        {
            printf("unvalid buff, return\n");
            return 0;
        }

        //红外开关
        uint8_t s_flag = (buff[66] >> 3) & 0x01;
        //白光开关
        uint8_t v_flag = (buff[66] >> 2) & 0x01;
        //图像类型，0-可见光，1-红外，2-融合
        // if(v_flag == 0x01 && s_flag == 0x01)//融合
        // if(v_flag == 0x01 && s_flag == 0x01)//融合
        // {
        //     send_msg.st_Fusion_Status = 2;
        //     IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(send_msg.st_ZoomScale);
            
        // }
        // else if(s_flag == 0x01)//红外
        // {
        //     send_msg.st_Fusion_Status = 1;
        //     //IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(1.0);
        // }
        // else//其他都是 可见光类型
        // {
        //     send_msg.st_Fusion_Status = 0;
        //     //IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(1.0);
        // }

        
        if(s_flag == 0x01)//红外
        {
            send_msg.st_Fusion_Status = 2;
            // IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(send_msg.st_ZoomScale);
            if(send_msg.st_ZoomScale != 3.0)
                IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(3.0);
        }
        else//vision
        {
            send_msg.st_Fusion_Status = 0;
            if(send_msg.st_ZoomScale != 1.0)
                IPSERVER::ViewLinkHandle::GetInstance()->ZoomTo(1.0);
        }

        //目标自动识别
        uint8_t i_flag = (buff[76] >> 7) & 0x01;
        if(i_flag == 0x01)//目标识别开
        {
            send_msg.st_Identify_Status = 1;
        }
        else//目标识别关
        {
            send_msg.st_Identify_Status = 0;
        }

        //跟踪
        //像素点跟踪
        uint8_t p_flag = (buff[76] >> 4) & 0x01;
        //标号跟踪
        uint8_t ttID_flag = (buff[76] >> 3) & 0x01;
        static bool trackingon = false;
        if(p_flag == 0x01 && ttID_flag == 0x01)
        {
            send_msg.st_Tracking_Status = 1;//像素跟踪
            send_msg.st_Tracking_Point[0] = buff[78]*256 + buff[77];//X坐标
            send_msg.st_Tracking_Point[1] = buff[80]*256 + buff[79];//Y坐标
            trackingon = true;
        }
        else if(ttID_flag == 0x01)
        {
            send_msg.st_Tracking_Status = 2;//标号跟踪
            send_msg.st_Tracking_Target_ID = buff[81];//标号ID
            trackingon = true;
        }
        else if(p_flag == 0x01)
        {
            send_msg.st_Tracking_Status = 1;//像素跟踪
            send_msg.st_Tracking_Point[0] = buff[78]*256 + buff[77];//X坐标
            send_msg.st_Tracking_Point[1] = buff[80]*256 + buff[79];//Y坐标
            trackingon = true;

            // VLK_StartTrack();
        }
        else
        {
            send_msg.st_Tracking_Status = 0;//跟踪关
            if(trackingon == true)
            {
                IPSERVER::ViewLinkHandle::GetInstance()->Move(0,0);
				IPSERVER::ViewLinkHandle::GetInstance()->Stopp();
				IPSERVER::ViewLinkHandle::GetInstance()->Home();
                printf("stop tracking\n\n");
                trackingon = false;
            }

        }
        

        // send_msg.st_Tracking_Status = 0;


        //十字分划，上-FF,下-EE,左FF，右EE
        uint8_t d_flag = (buff[72] >> 6) & 0x01;
        if(d_flag == 0x01)//开
        {
            send_msg.st_Draw_Cross = 1;
            //十字上下
            uint8_t dc_P0 = buff[73] >> 6;
            if(dc_P0 == 0x01)
            {
                send_msg.st_Draw_Cross_Point[0] = 0xFF;
            }
            else if(dc_P0 == 0x10)
            {
                send_msg.st_Draw_Cross_Point[0] = 0xEE;
            }
            else
            {
                send_msg.st_Draw_Cross_Point[0] = 0x00;
            }
            //十字左右
            uint8_t dc_P1 = (buff[73] & 0x3F) >> 4;
            if(dc_P1 == 0x01)
            {
                send_msg.st_Draw_Cross_Point[1] = 0xFF;
            }
            else if(dc_P1 == 0x10)
            {
                send_msg.st_Draw_Cross_Point[1] = 0xEE;
            }
            else
            {
                send_msg.st_Draw_Cross_Point[1] = 0x00;
            }
        }
        else//关
        {
            send_msg.st_Draw_Cross = 0;
        }

        //get loglat
        uint64_t latdata = 0;
        latdata = buff[119];
        latdata = (latdata << 8) + buff[118];
        latdata = (latdata << 8) + buff[117];
        latdata = (latdata << 8) + buff[116];
        latdata = (latdata << 8) + buff[115];

        latitude = (double)latdata / 1000000;

        uint64_t logdata = 0;
        logdata = buff[124];
        logdata = (logdata << 8) + buff[123];
        logdata = (logdata << 8) + buff[122];
        logdata = (logdata << 8) + buff[121];
        logdata = (logdata << 8) + buff[120];

        longitude = (double)logdata / 1000000;

        printf("recv udp longitude:%f, latitude:%f\n", longitude, latitude);

        if(longitude > 0.0f && latitude > 0.0f)
        {
            latitude = CorrectLatitude(latitude);
            longitude = CorrectLongitude(longitude);
        }

        



        //
        printf("msg send to algo\n命令号：%d\n跟踪状态:%d\n",
                send_msg.st_Command,send_msg.st_Tracking_Status);//跟踪状态
        printf("识别状态:%d\n",send_msg.st_Identify_Status);//识别状态
        printf("融合状态:%d\n",send_msg.st_Fusion_Status);//融合状态
        // printf("推流状态:%d\n",send_msg.st_Plugflow_Status);//推流状态
        // printf("标号跟踪ID:%d\n",send_msg.st_Tracking_Target_ID);//标号跟踪ID
        // printf("十字分划叠加使能:%d\n",send_msg.st_Draw_Cross);//十字分划叠加使能
        // printf("十字分划叠加位置:(%d,%d)\n",send_msg.st_Draw_Cross_Point[0],send_msg.st_Draw_Cross_Point[1]);//十字分划叠加位置
        // printf("像素跟踪位置：(%d,%d)\n",send_msg.st_Tracking_Point[0], send_msg.st_Tracking_Point[1]);
        printf("zoom scale:%d\n",send_msg.st_ZoomScale);//zoom scale

        //将消息发送给底层IPC消息队列
        int ret = IPSERVER::MessageQueueHandle::GetInstance()->SendMsg(send_msg);

        //****************************** IPC end *********************************************//



        //****************************** 光电 start *********************************************//
        //取云台俯仰和方向，确定光电球方向速度最大为5000、俯仰最大速度为2000
        //工控机下发的俯仰和方向速度范围为0~10000，需按等比例进行缩放
        int16_t X=0, Y=0;
        char temp_camer_direct = (buff[56] >> 6);
        int16_t camer_direct = ((buff[56] << 8) | buff[55]) & 0x3fff;
        
        if(temp_camer_direct == 0x00)
        {
            X = static_cast<int>(camer_direct) / 2;
            X = X > 1200 ? 3000 : 0;
            std::cout<<std::endl<<"云台顺方向X："<<X<<std::endl;
        }
        else if(temp_camer_direct == 0x01)
        {
            X = -(static_cast<int>(camer_direct) / 2);
            X = X < -1200 ? -3000 : 0;
            std::cout<<"云台逆方向X："<<X<<std::endl;
        }
        else
        {
            printf("云台方向未知！\n");
        }
        
        char temp_camer_pitch = (buff[54] >> 6);
        uint16_t camer_pitch = ((buff[54] << 8) | buff[53]) & 0x3fff;
        if(temp_camer_pitch == 0x00)
        {
            Y = static_cast<int>(camer_pitch) / 5;
            Y = Y > 1200 ? 3000 : 0;
            std::cout<<"云台顺俯仰Y："<<Y<<std::endl;
        }
        else if(temp_camer_pitch == 0x01)
        {
            Y = -(static_cast<int>(camer_pitch) / 5);
            Y = Y < -1200 ? -3000 : 0;
            std::cout<<"云台逆俯仰Y："<<Y<<std::endl;
        }
        else
        {
            printf("云台俯仰未知！\n");
        }

        static int lastX = 0;
        static int lastY = 0;

        printf("IPSERVER::ViewLinkHandle::GetInstance()->GetStatus():%d\n",IPSERVER::ViewLinkHandle::GetInstance()->GetStatus());
        // if(IPSERVER::ViewLinkHandle::GetInstance()->GetStatus() != -1 && (X != 0 || Y!=0))
        // {
            
        //     printf("send Move to pod, X;%d, Y:%d\n", X, Y);
        //     IPSERVER::ViewLinkHandle::GetInstance()->Move(X, Y);
        // }
        // else if(IPSERVER::ViewLinkHandle::GetInstance()->GetStatus() != -1 && (X == 0 && Y==0))
        // {
        //     printf("send Stopp\n");
        //     IPSERVER::ViewLinkHandle::GetInstance()->Stopp();
        // }

        if(lastX != X || lastY != Y)
        {
            printf("send Move to pod, X;%d, Y:%d\n", X, Y);
            IPSERVER::ViewLinkHandle::GetInstance()->Move(X, Y);
            lastX = X;
            lastY = Y;
        }

        else if(IPSERVER::ViewLinkHandle::GetInstance()->GetStatus() == -1)
        {
            
            printf("光电球未准备就绪\n状态为：%d\n", IPSERVER::ViewLinkHandle::GetInstance()->GetStatus());
        }

        

        //光电归零 
        uint8_t zero_flag = (buff[74] >> 6) & 0x01;
        static int zero_flagcnt = 0;
        // printf("zero_flagzero_flagzero_flagzero_flagzero_flagzero_flagzero_flagzero_flag:%d\n", zero_flag);
        if(zero_flag == 0x01)
        //归零
        {
            if(zero_flagcnt == 0)
            {
                printf("****************************************************光电归零\n");
                IPSERVER::ViewLinkHandle::GetInstance()->Home();
                zero_flagcnt = 2;
            }
            zero_flagcnt--;
        }
        else
        //关闭
        {

        }


        uint8_t zoom_flag = buff[69] & 0x0F;
        //调焦+
        if(zoom_flag == 0x01)
        {
            std::cout<<"调焦+"<<std::endl;
            IPSERVER::ViewLinkHandle::GetInstance()->ZoomIn();
        }
        else if(zoom_flag == 0x02)//调焦-
        {
            std::cout<<"调焦-"<<std::endl;
            IPSERVER::ViewLinkHandle::GetInstance()->ZoomOut();
        }
        else
        {
            //printf("调焦：[%02X]\n", zoom_flag);
        }


        //****************************** 光电 end *********************************************//
        


    }
    else
    {
        printf("接收到的字符长度：%d\n", len);
    }

    printf("laser dist:%d\n",IPSERVER::ViewLinkHandle::GetInstance()->m_laserDist);
    auto boxes = IPSERVER::MessageQueueHandle::GetInstance()->m_boxes;
    int random_number = std::rand();
    short random_value = random_number%121+80;
    

    obj_x = 0;
    obj_y = 0;
    if(boxes.size()){
        // convertCoordinate(latitude, longitude, 0, 0, 0,boxes[0].x_3d,boxes[0].y_3d,boxes[0].z_3d, outlat, outlong);
        // convertCoordinate(latitude, longitude, 0, 0, 0,boxes[0].x,boxes[0].y,CalObjDist(boxes[0].w), outlat, outlong);
        calculateTargetPosition(latitude, longitude, CalAngl(boxes[0].x), CalObjDist(boxes[0].w)/1000, outlat, outlong);

        obj_x = boxes[0].x;
        obj_y = boxes[0].y;
        obj_class = boxes[0].obj_id;
        printf("send obj info:x:%d,y:%d,lat:%f, log:%f\n", obj_x, obj_y, outlat, outlong);

    }
    

        // client->OnSend(IPSERVER::ViewLinkHandle::GetInstance()->m_laserDist*100,outlat,outlong,obj_x,obj_y,obj_class);
        client->OnSend(IPSERVER::ViewLinkHandle::GetInstance()->m_laserDist*100,outlat,outlong,obj_x,obj_y,obj_class);

    return 0;
}