
#include "../common/Common-mq.h"
// #include "./SERVER/HttpServer.h"

// #include "./rknn/detector.cpp"





using namespace std;
using namespace MQTTCLIENT;

int b_flag = 1;


static uint32_t requestInt=0;

static uint32_t respondInt=0;



int Is_In_List(MQTTCLIENT::MapMarkerInfo &target)
{
    // if(MQTTCLIENT::Common::m_markers.empty())
    //     return -1;
    // for(int i=0;i<MQTTCLIENT::Common::m_markers.size();i++)
    // {
    //     if((strcmp(MQTTCLIENT::Common::m_markers[i].mdiInfo.camp,target.mdiInfo.camp) == 0)
    //         && (strcmp(MQTTCLIENT::Common::m_markers[i].mdiInfo.targetName, target.mdiInfo.targetName) == 0)
    //         && (fabs(MQTTCLIENT::Common::m_markers[i].mdiInfo.longitude - target.mdiInfo.longitude) < 0.000005)
    //         && (fabs(MQTTCLIENT::Common::m_markers[i].mdiInfo.latitude - target.mdiInfo.latitude) < 0.000005)
    //         && MQTTCLIENT::Common::m_markers[i].mdiInfo.targetCount == target.mdiInfo.targetCount
    //         )
    //         return i;
    // }

    // return -1;

    if(MQTTCLIENT::Common::m_markers.empty())
        return -1;
    for(int i=0;i<MQTTCLIENT::Common::m_markers.size();i++)
    {
        if((strcmp(MQTTCLIENT::Common::m_markers[i].timestampAndUserId,target.timestampAndUserId) == 0))
            return i;
    }

    return -1;
}

void Ts_Fus(MQTTCLIENT::ST_TSSEND_RCV &input, MQTTCLIENT::ST_TSSEND_RECV &output)
{
    // output.vect.clear();
    // for(auto &marker:input.vect)
    // {
    //     int idx = Is_In_List(marker);
    //     if(-1 == idx)
    //         MQTTCLIENT::Common::m_markers.push_back(marker);
    //     else
    //     {
    //         MQTTCLIENT::Common::m_markers[idx] = marker;
    //         MQTTCLIENT::Common::m_markers[idx].mdiInfo.mHitRadius = 10;
    //     }
    // }

    // output.vect = MQTTCLIENT::Common::m_markers;
    MQTTCLIENT::ST_TSSEND_RET sendret;
    for(auto &data:input.vect)
    {
        if(data.type == 0)
        {
            MQTTCLIENT::Common::m_markers.push_back(data.info);
            MQTTCLIENT::ST_TSSEND_OBJ sendobj;
            strncpy(sendobj.timestampAndUserId, data.info.timestampAndUserId, strlen(data.info.timestampAndUserId));
            sendobj.Info = data.info;
            sendobj.Info.mdiInfo.mHitRadius = 10;
            sendobj.type = 0;
            sendobj.res = 0;
            sendret.vect.push_back(sendobj);
        }
        else if(data.type == 1)
        {
            int idx = Is_In_List(data.info);
            MQTTCLIENT::ST_TSSEND_OBJ sendobj;
            strncpy(sendobj.timestampAndUserId, data.info.timestampAndUserId, strlen(data.info.timestampAndUserId));
            sendobj.Info = data.info;
            sendobj.Info.mdiInfo.mHitRadius = 10;
            sendobj.type = 1;
            idx == -1 ? sendobj.res = 1 : sendobj.res = 0;
            sendret.vect.push_back(sendobj);
            if(idx != -1)
            {
                MQTTCLIENT::Common::m_markers.erase(MQTTCLIENT::Common::m_markers.begin() + idx);
            }
        }
        else if(data.type == 2)
        {
            int idx = Is_In_List(data.info);
            MQTTCLIENT::ST_TSSEND_OBJ sendobj;
            strncpy(sendobj.timestampAndUserId, data.info.timestampAndUserId, strlen(data.info.timestampAndUserId));
            sendobj.Info = data.info;
            sendobj.Info.mdiInfo.mHitRadius = 10;
            sendobj.type = 1;
            idx == -1 ? sendobj.res = 1 : sendobj.res = 0;
            sendret.vect.push_back(sendobj);
            if(idx != -1)
            {
                MQTTCLIENT::Common::m_markers[idx] = data.info;
            }
        }
    }
    output.st_type = 0;
    output.st_obj = sendret;


}

void Ts_Anals(MQTTCLIENT::ST_ANALYSISSEND_RECV &output)
{
    MQTTCLIENT::ST_ANALYSISSEND_RET sendret;
    for(auto &data: MQTTCLIENT::Common::m_markers)
    {
        MQTTCLIENT::ST_ANALYSISSEND_OBJ obj;
        obj.longitude = data.longitude;
        obj.latitude = data.latitude;
        obj.angle = 0;
        sendret.vect.push_back(obj);
    }

    output.st_obj = sendret;
}


static void signal_handle(int signum)
{
	b_flag = 0;
}



//发布关于给定主题的消息。
int my_mosquitto_publish(struct mosquitto *mosq, int  payloadlen, const void *payload)
{
    // int mosquitto_publish(struct 	mosquitto 	*mosq,
    // int 			*mid,
    // const 	char 	*topic,
    // int 			payloadlen,
    // const 	void 	*payload,
    // int 			qos,
    // bool 			retain)
    // 作用：发布关于给定主题的消息。该功能适用​​于使用所有 MQTT 协议版本的客户端。如果需要设置 MQTT v5 PUBLISH 属性，请改用mosquitto_publish_v5。

    // 参数：
    // mosq：一个有效的mosquitto实例。
    // mid：指向 int 的指针。如果不为 NULL，该函数会将其设置为此特定消息的消息 ID。然后可以将其与发布回调一起使用，以确定何时发送消息。请注意，尽管 MQTT 协议不对 QoS=0 的消息使用消息 ID，但 libmosquitto 会为它们分配消息 ID，以便可以使用此参数跟踪它们。
    // topic：要发布到的主题的以 null 结尾的字符串。
    // payloadlen：有效载荷的大小（字节）。有效值介于 0 和 268,435,455 之间。
    // payload：有效载荷，指向要发送的数据的指针。如果 payloadlen > 0 这必须是一个有效的内存位置。
    // qos：服务质量，整数值 0、1 或 2，指示要用于消息的服务质量。
    // retain：保持，设置为 true 以使消息保留。

    // 返回值：
    // MOSQ_ERR_SUCCESS：关于成功。
    // MOSQ_ERR_INVAL：如果输入参数无效。
    // MOSQ_ERR_NOMEM：如果发生内存不足的情况。
    // MOSQ_ERR_NO_CONN：如果客户端未连接到代理。
    // MOSQ_ERR_PROTOCOL：如果与代理通信时出现协议错误。
    // MOSQ_ERR_PAYLOAD_SIZE：如果 payloadlen 太大。
    // MOSQ_ERR_MALFORMED_UTF8：如果主题不是有效的 UTF-8
    // MOSQ_ERR_QOS_NOT_SUPPORTED：如果 QoS 大于代理所支持的。
    // MOSQ_ERR_OVERSIZE_PACKET：如果生成的数据包比代理支持的大。

    return mosquitto_publish(mosq,NULL, MQTTCLIENT::Common::top_Rcv, payloadlen, payload, 2, true);
}


//推送威胁等级数据, i_interval数据推送间隔事件ms
void PushData(struct mosquitto *mosq)
{
    //从配置文件获取间隔时间，以秒计算
    char s_intevel[IP_LEN] = {0}; 
    Common::GetValueByKeyFromConfig("THREAT_PUSH_DATA", s_intevel, MQTTCLIENT::IP_LEN);

    uint32_t i_intevel = atoi(s_intevel==NULL?"300":s_intevel);
    

    std::cout<<"start push threat data thread!"<<std::endl;
    while(b_flag && mosq != NULL && mosq != nullptr)
    {
        char data[MAX_MSG_LEN]={0};
    
        ST_THREATLEVEL_RET retObj;

        for(int i=0; i < MQTTCLIENT::Common::m_markers.size();i++)
        {
            ST_THREATLEVEL_OBJ objitem0;
            memset(&objitem0, 0, sizeof(objitem0));
            strncpy(objitem0.id, MQTTCLIENT::Common::m_markers[i].timestampAndUserId, strlen(MQTTCLIENT::Common::m_markers[i].timestampAndUserId));
            strncpy(objitem0.grade, "二级", strlen("二级"));//威胁等级
            strncpy(objitem0.targetName, MQTTCLIENT::Common::m_markers[i].mdiInfo.targetName, strlen(MQTTCLIENT::Common::m_markers[i].mdiInfo.targetName));//目标名称
            retObj.vect.push_back(objitem0);
        }

        ST_THREATLEVEL_RECV obj;
        memset(&obj, 0, sizeof(obj));
        obj.st_type = 3;
        
        memcpy(obj.st_api, "mapPlugin/ai", strlen("mapPlugin/ai")); 

        obj.st_obj = retObj;

        MQTTCLIENT::Common::ThreatST_ConvertTo_JSON(obj, data, MAX_MSG_LEN);
        
        //发布回应消息
        int res = my_mosquitto_publish(mosq, strlen(data), data);
        if(res == MOSQ_ERR_SUCCESS)
        {
            //std::cout<<"send threat msg:\n"<< data <<std::endl;
        }

        //等待
        usleep(i_intevel*1000000);
    }
    std::cout<<"end push threat data thread!"<<std::endl;
}


//接收订阅主题的消息处理   
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    return ;
    if(message->payloadlen)
    {
        printf("my_message_callback %s %s\n", message->topic, message->payload);
        return ;
    }
    
	if(message->payloadlen)
    {
        //态势融合
        if(strncmp(message->topic, "/AIService/TS/Send", strlen(message->topic)) == 0)
        {
            //std::cout<<"TS send rcv msg:"<<std::endl<<(char*)(message->payload)<<std::endl;
            std::cout<<"TS send rcv msg:"<<std::endl;
            requestInt++;

            ST_TSSEND_RCV rcvObj;
            
            //接收到的JSON消息转struct
            if(MQTTCLIENT::Common::JSON_ConvertTo_TSSendST((char*)(message->payload), strlen((char*)(message->payload)),rcvObj) == -1)
            {
                std::cout<<"JSON_ConvertTo_TSSendST failed"<<std::endl;
            }
            else
            {
                std::cout<<"JSON_ConvertTo_TSSendST succeed"<<std::endl;
            }
            

            
            ST_TSSEND_RECV obj;

            obj.st_type = 0;
            memset(obj.st_api, 0, STR_LEN);
            memcpy(obj.st_api, "mapPlugin/ai", strlen("mapPlugin/ai"));

            

            //执行底层t态势融合算法
            Ts_Fus(rcvObj, obj);

            //根据算法返回结果将struct转换成JSON字符串发送出去

            char data[MAX_MSG_LEN]={0};
            
            if(MQTTCLIENT::Common::TSSendST_ConvertTo_JSON(obj, data, MAX_MSG_LEN) == -1)
            {
                std::cout<<"TSSendST_ConvertTo_JSON failed!!!!"<<std::endl;
            }


            //发布回应消息
            int res = my_mosquitto_publish(mosq, strlen(data), data);
            if(res == MOSQ_ERR_SUCCESS)
            {
                //std::cout<<"\n\nTS send respond msg:\n"<<data<<std::endl;
                //respondInt++;
            }


            //std::cout<<"request count:"<<requestInt<<"\trespond count:"<<respondInt<<std::endl;

        }
        //态势分析
        else if(strncmp(message->topic, "/AIService/analysis/Send", strlen(message->topic)) == 0)
        {
            std::cout<<"TS analysis send rcv msg:"<<std::endl<<(char*)(message->payload)<<std::endl;

            ST_ANALYSISSEND_RCV rcvObj;
            if(MQTTCLIENT::Common::JSON_ConvertTo_AnalysisSendST((char*)(message->payload), strlen((char*)(message->payload)),rcvObj) == -1)
            {
                std::cout<<"JSON_ConvertTo_AnalysisSendST failed"<<std::endl;
            }
            else
            {
                std::cout<<"JSON_ConvertTo_AnalysisSendST succeed"<<std::endl;
            }


            char data[MAX_MSG_LEN]={0};
            
            ST_ANALYSISSEND_RECV obj;

            obj.st_type = 1;
            memset(obj.st_api, 0, STR_LEN);
            memcpy(obj.st_api, "mapPlugin/ai", strlen("mapPlugin/ai"));

            //接收到的JSON消息转struct

            //执行底层算法

            Ts_Anals(obj);

            //根据算法返回结果将struct转换成JSON字符串发送出去


            MQTTCLIENT::Common::AnalysisSendST_ConvertTo_JSON(obj, data, MAX_MSG_LEN);

            
            //发布回应消息
            int res = my_mosquitto_publish(mosq, strlen(data), data);
            if(res == MOSQ_ERR_SUCCESS)
            {
                //std::cout<<"TS analysis send msg:\n"<<data<<std::endl;
            }


        }
        //情报融合
        // else if(strncmp(message->topic, "/AIService/analysis/Fusion", strlen(message->topic)) == 0)
        // {
        //     std::cout<<"Analysis fusion rcv msg:"<<std::endl<<(char*)(message->payload)<<std::endl;
        //     ST_ANALYSISFUSION_RCV rcvObj;
        //     if(MQTTCLIENT::Common::JSON_ConvertTo_FusionST((char*)(message->payload), strlen((char*)(message->payload)),rcvObj) == -1)
        //     {
        //         std::cout<<"JSON_ConvertTo_FusionST failed"<<std::endl;
        //     }
        //     else
        //     {
        //         std::cout<<"JSON_ConvertTo_FusionST succeed"<<std::endl;
        //     }



        //     char data[MAX_MSG_LEN]={0};
            
            
        //     ST_ANALYSISFUSION_RECV obj;

        //     ST_ANALYSISFUSION_RET objRet;


        //     obj.st_type = 2;
        //     memset(obj.st_api, 0, STR_LEN);
        //     memcpy(obj.st_api, "mapPlugin/ai", strlen("mapPlugin/ai"));

        //     //接收到的JSON消息转struct

        //     //执行底层算法
        //     for(int i=0; i< MQTTCLIENT::Common::files.size();i++)
        //     {
        //         cv::Mat img = cv::imread(MQTTCLIENT::Common::files[i]);
        //         std::vector<std::string> detret;
        //         DetectorRun(img, detret);
        //         for(auto &ret:detret)
        //         {
        //             MQTTCLIENT::MapMarkerInfo marker;
        //             strncpy(marker.mdiInfo.targetName, ret.c_str(), 10);
        //             MQTTCLIENT::Common::m_markers.push_back(marker);

        //             ST_ANALYSISFUSION_OBJ objitem;
        //             strncpy(objitem.lon_lat, "114,116", 10);
                    
        //             objRet.vect.push_back(objitem);
        //         }

        //         char script[MSG_LEN] = {0};
        //         sprintf(script, "rm -rf %s", MQTTCLIENT::Common::files[i]);
        //         std::cout<<"execute rm <png/jpg> command:"<<MQTTCLIENT::Common::files[i]<<std::endl;
        //         system(script);
        //     }
            
        //     // char *imgpath = "";
        //     // cv::Mat img = cv::imread(imgpath);
        //     // std::vector<std::string> detret;
        //     // DetectorRun(img, detret);
        //     // for(auto &ret:detret)
        //     // {
        //     //     MQTTCLIENT::MapMarkerInfo marker;
        //     //     strncpy(marker.mdiInfo.targetName, ret.c_str(), 10);
        //     //     MQTTCLIENT::Common::m_markers.push_back(marker);

        //     //     ST_ANALYSISFUSION_OBJ objitem;
        //     //     strncpy(objitem.lon_lat, "114,116", 10);
                
        //     //     objRet.vect.push_back(objitem);
        //     // }

        //     obj.st_obj = objRet;

        //     //根据算法返回结果将struct转换成JSON字符串发送出去


        //     MQTTCLIENT::Common::FusionST_ConvertTo_JSON(obj, data, MAX_MSG_LEN);
            
        //     //发布回应消息
        //     int res = my_mosquitto_publish(mosq, strlen(data), data);
        //     if(res == MOSQ_ERR_SUCCESS)
        //     {
        //         std::cout<<"Analysis fusion send msg:\n"<<data<<std::endl;

        //         //清空files
        //         MQTTCLIENT::Common::files.clear();
        //     }


        // }
        else
        {
            std::cout<<"Unknow Topic!!!!:"<<message->topic<<std::endl;
            
		    printf("my_message_callback %s %s\n", message->topic, message->payload);
        }
        
	}
    else
    {
		printf("my_message_callback %s (null)\n", message->topic);
	}
	fflush(stdout);
}
 
void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result)
    {
        std::cout<<"Connect broker succeed\n"<<std::endl;
		/* Subscribe to broker information topics on successful connect. */
        //订阅一个主题
        //mosq：一个有效的mosquitto实例。
        //mid：指向 int 的指针。如果不为 NULL，该函数会将其设置为此特定消息的消息 ID。然后可以将其与 subscribe 回调一起使用，以确定消息何时发送。
        //sub：订阅模式。
        //qos：服务质量，此订阅请求的服务质量。
        //TS融合信息接收主题订阅
		int ret = mosquitto_subscribe(mosq, NULL, "/AIService/TS/Send", 2);
        if(ret == MOSQ_ERR_SUCCESS)
        {
            std::cout<<"subscribe /AIService/TS/Send succeed!"<<std::endl;
        }
        else 
        {
            std::cout<<"subscribe /AIService/TS/Send fail!"<<std::endl;
        }

        //TS分析信息接收主题订阅
		ret = mosquitto_subscribe(mosq, NULL, "/AIService/analysis/Send", 2);
        if(ret == MOSQ_ERR_SUCCESS)
        {
            std::cout<<"subscribe /AIService/analysis/Send succeed!"<<std::endl;
        }
        else 
        {
            std::cout<<"subscribe /AIService/analysis/Send fail!"<<std::endl;
        }
        
        //情报融合信息接收主题订阅
		// ret = mosquitto_subscribe(mosq, NULL, "/AIService/analysis/Fusion", 2);
        // if(ret == MOSQ_ERR_SUCCESS)
        // {
        //     std::cout<<"subscribe /AIService/analysis/Fusion succeed!"<<std::endl;
        // }
        // else 
        // {
        //     std::cout<<"subscribe /AIService/analysis/Fusion fail!"<<std::endl;
        // }

	}
    else
    {
		fprintf(stderr, "Connect broker failed\n");
	}
}
 
//mosq：进行回调的 mosquitto 实例。
//obj：mosquitto_new中提供的用户数据
//mid：订阅消息的消息 ID。
//qos_count：授予订阅的数量（granted_qos 的大小）。
//grant_qos：一个整数数组，指示每个订阅的授予 QoS。
void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;
 
	printf("my_subscribe_callback Subscribed (mid: %d): %d", mid, granted_qos[0]);

	for(i=1; i<qos_count; i++)
    {
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}
 
void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	printf("%s\n", str);
}



void MQTTStart(struct mosquitto *mosq)
{

    printf("MQTTStart\n");

    int i;

    //主机、端口、保活
	char host[MQTTCLIENT::IP_LEN] = "192.168.1.87";
    char s_port[MQTTCLIENT::PORT_LEN] = "1883";
    char s_keepalive[MQTTCLIENT::PORT_LEN] = "60";

    char s_username[MQTTCLIENT::IP_LEN] = "testuser";
    char s_password[MQTTCLIENT::IP_LEN] = "testpw";
    
    //读取配置文件中的服务器IP、端口号、保活值
    // MQTTCLIENT::Common::GetValueByKeyFromConfig("MQTT_SERVER_IP", host, MQTTCLIENT::IP_LEN);
    // MQTTCLIENT::Common::GetValueByKeyFromConfig("MQTT_SERVER_PORT", s_port, MQTTCLIENT::PORT_LEN);
    // MQTTCLIENT::Common::GetValueByKeyFromConfig("MQTT_KEEPALIVE", s_keepalive, MQTTCLIENT::PORT_LEN);
    // MQTTCLIENT::Common::GetValueByKeyFromConfig("MQTT_USERNAME", s_username, MQTTCLIENT::IP_LEN);
    // MQTTCLIENT::Common::GetValueByKeyFromConfig("MQTT_PASSWORD", s_password, MQTTCLIENT::IP_LEN);

    // host = "192.168.1.87";
    // s_port = "1883";
    // s_keepalive = "60";
    // s_username = "testuser"
    // s_password = "testpw";

    printf("rea   d end\n");
    std::cout<<"从配置文件中读取的 MQTT Server IP:"<<host<<std::endl;
    std::cout<<"从配置文件中读取的 MQTT Server PORT:"<<s_port<<std::endl;
    std::cout<<"从配置文件中读取的 MQTT keep alive:"<<s_keepalive<<std::endl;
    std::cout<<"从配置文件中读取的 MQTT Server username:"<<s_username<<std::endl;
    std::cout<<"从配置文件中读取的 MQTT Server password:"<<s_password<<std::endl;


	int port = atoi(s_port);
	int keepalive = atoi(s_keepalive);

	bool clean_session = true;

    printf("55555555\n");
 
    //初始化 mosquitto
    int ret = mosquitto_lib_init();
	if(ret == MOSQ_ERR_SUCCESS)
    {
        std::cout<<"mosquitto_lib_init succeed"<<std::endl;
    }
    else
    {
        std::cout<<"mosquitto_lib_init failed\n"<<std::endl;
    }

    //创建客户端对象实例
	MQTTCLIENT::Common::mosq = mosquitto_new(NULL, clean_session, NULL);
	if(!MQTTCLIENT::Common::mosq)
    {
		fprintf(stderr, "moquitto create object Error: Out of memory.\n");
		return;
	}
    else
    {
        std::cout<<"moquitto create object succeed\n"<<std::endl;
    }


    //日志打印回调
	mosquitto_log_callback_set(MQTTCLIENT::Common::mosq, my_log_callback);

    //broker服务连接回调
    //设置连接回调。这在代理发送 CONNACK 消息以响应连接时调用。
	mosquitto_connect_callback_set(MQTTCLIENT::Common::mosq, my_connect_callback);

    //设置消息回调。当从代理接收到消息时调用它。
	mosquitto_message_callback_set(MQTTCLIENT::Common::mosq, my_message_callback);

    //设置订阅回调。当代理响应订阅请求时调用。
	mosquitto_subscribe_callback_set(MQTTCLIENT::Common::mosq, my_subscribe_callback);
 
    //连接到 MQTT 代理。
    //mosq：一个有效的mosquitto实例。
    //host：要连接的代理的主机名或 IP 地址。
    //port：要连接的网络端口。通常是 1883 。
    //keepalive：如果在这段时间内（秒数）没有交换其他消息，代理应该向客户端发送 PING 消息。

    printf("mosquitto_connect\n");
    ret = mosquitto_connect(MQTTCLIENT::Common::mosq, host, port, keepalive);
	if(ret != MOSQ_ERR_SUCCESS)
    {
		printf("MQTT Unable to connect to ServerIP-%s Port-%d\nret:%d\n", host, port, ret);
        
        //释放 mosquitto 客户端实例关联的内存
        mosquitto_destroy(MQTTCLIENT::Common::mosq);

        MQTTCLIENT::Common::mosq = nullptr;

        //调用与库相关的资源释放。
        mosquitto_lib_cleanup();
		return;
	}
    else
    {
        printf("MQTT succeed to connect to ServerIP-%s Port-%d\n", host, port);
    }
    printf("mosquitto_connect eeeeee\n");
    
    //启动线程,一直发送威胁等级信息
    // std::thread t1(PushData, MQTTCLIENT::Common::mosq);
    // t1.detach();


 
    //此函数在无限阻塞循环中为您调用 loop()。这对于您只想在程序中运行 MQTT 客户端循环的情况很有用。
    //**如果服务器连接丢失，它会处理重新连接。**如果您在回调中调用 mosquitto_disconnect()，它将返回。
    //参数：
    //mosq：一个有效的mosquitto实例。
    //timeout：在超时之前等待 select() 调用中的网络活动的最大毫秒数。设置为 0 以立即返回。设置负数以使用默认值 1000 毫秒。
    //max_packets：此参数当前未使用，应设置为 1 以便将来兼容。
	ret = mosquitto_loop_forever(MQTTCLIENT::Common::mosq, -1, 1);
    if(ret == MOSQ_ERR_SUCCESS)
    {
        std::cout<<"MQTT mosquitto_loop_forever succeed!"<<std::endl;
    }
    else
    {
        std::cout<<"MQTT mosquitto_loop_forever failed!"<<std::endl;
    }



    //释放 mosquitto 客户端实例关联的内存
	mosquitto_destroy(MQTTCLIENT::Common::mosq);

    MQTTCLIENT::Common::mosq = nullptr;

    //调用与库相关的资源释放。
	mosquitto_lib_cleanup();
	return;
}

 
// int main(int argc, char *argv[])
// {
//     //信号处理
// 	struct sigaction sig_action;
// 	sig_action.sa_handler = signal_handle;
// 	sigemptyset(&sig_action.sa_mask);
// 	sig_action.sa_flags = 0;
// 	sigaction(SIGINT, &sig_action, NULL);


//     //Detector初始化
//     // DetectorInit();


//     //////////////////detector test start


 
//     // std::cout<<"DetectorRun img:/home/fex/data/mmexport1686472541157.jpg"<<std::endl;
//     // cv::Mat img = cv::imread("/home/fex/data/mmexport1686472541157.jpg");
//     // std::vector<std::string> detret;
//     // DetectorRun(img, detret);
//     // for(auto &ret:detret)
//     // {
//     //     MQTTCLIENT::MapMarkerInfo marker;
//     //     strncpy(marker.mdiInfo.targetName, ret.c_str(), 10);
//     //     MQTTCLIENT::Common::m_markers.push_back(marker);

//     //     ST_ANALYSISFUSION_OBJ objitem;
//     //     strncpy(objitem.lon_lat, "114,116", 10);
        
//     //     //objRet.vect.push_back(objitem);
//     // }
    

//     //////////////////detector test end
    

  
// /*################################## MQTT Client Start #######################################################*/

//     //启动线程, 开启MQSTT通信
//     std::thread t1(MQTTStart, MQTTCLIENT::Common::mosq);
//     t1.detach();      
// /*################################## MQTT Client end #######################################################*/


    
// /*################################## HTTP SERVER Start #######################################################*/

// 	HttpApplication app;
// 	//拥塞
// 	app.run(argc, argv);
// /*################################## HTTP SERVER End #######################################################*/



//     std::cout<<"exit 0!!!"<<std::endl;
//     return 0;

	
// }