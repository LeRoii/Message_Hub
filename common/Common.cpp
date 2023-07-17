/*****************
 * project:IntelligentPerceptionServer
 * source:common.cpp
 * author:FEX
 * time:2021-09-15
 * description:
 * 公共处理类实现 
 * copyright:
 *
 *
 * ***************/

#include "./Common.h"

#include <iostream>
#include <cmath>

// 地球半径
static const double EARTH_RADIUS = 6371000.0;

// 将角度转换为弧度
static inline double toRadians(double angle) {
    return angle * M_PI / 180.0;
}

// 将弧度转换为角度
static inline double toDegrees(double angle) {
    return angle * 180.0 / M_PI;
}

// 计算两点之间的距离（单位：米）
static double distance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(toRadians(lat1)) * cos(toRadians(lat2)) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

// 将相机坐标系下的坐标转换为地球坐标系下的经纬度坐标
	void convertCoordinate(double lat, double lon, double alt,
						double bearing, double pitch,
						double x, double y, double z,
						double& outLat, double& outLon) {
		// 计算相机坐标系下的方向向量
		double cameraX = cos(toRadians(bearing)) * cos(toRadians(pitch));
		double cameraY = sin(toRadians(pitch));
		double cameraZ = sin(toRadians(bearing)) * cos(toRadians(pitch));

		// 计算相机坐标系下的目标坐标
		double targetX = x - cameraX * alt;
		double targetY = y - cameraY * alt;
		double targetZ = z - cameraZ * alt;

		// 计算地球坐标系下的目标坐标
		double d = sqrt(targetX * targetX + targetY * targetY);
		double lat0 = atan2(targetZ, d);
		double lon0 = atan2(targetY, targetX);

		// 计算目标的经纬度坐标
		outLat = toDegrees(lat0);
		outLon = toDegrees(lon0);
	}

// void convertCoordinate(double lat, double lon, double alt,
// 						double bearing, double pitch,
// 						double x, double y, double z,
// 						double& outLat, double& outLon)
// {
// 	double delta_lat = x / (111111 * cos(lat));
// 	double delta_lon = y / 111111;
// 	double R = 6371000;  // radius of Earth in meters
// 	double new_lat = lat + (delta_lat / R) * (180 / M_PI);
// 	double new_lon = lon + (delta_lon / R) * (180 / M_PI) / cos(lat);
// 	outLat = new_lat;
// 	outLon = new_lon;
// }



// int main() {
//     // 相机的经纬度和海拔高度
//     double lat = 30.0;
//     double lon = 120.0;
//     double alt = 100.0;

//     // 相机方向的方向角和俯仰角
//     double bearing = 0.0;
//     double pitch = 0.0;

//     // 目标在相机坐标系下的坐标
//     double x = 10.0;
//     double y = 0.0;
//     double z = 5.0;

//     // 将相机坐标系下的坐标转换为经纬度坐标
//     double targetLat = 0.0;
//     double targetLon = 0.0;
//     convertCoordinate(lat, lon, alt, bearing, pitch, x, y, z, targetLat, targetLon);

//     // 输出目标的经纬度坐标
//     std::cout << "目标的经纬度坐标为：" << targetLat << ", " << targetLon << std::endl;

//     return 0;
// }



namespace IPSERVER
{
	//静态成员初始化
	IPSERVER::enum_Status Common::m_RunStatus = IPSERVER::e_None;

    //退出信号
    bool Common::SIGQUITFLAG = false;
	
	Common::Common()
	{

	}

	//读取配置文件中的配置信息，
	//key-要查询的关键字，value-查询到的值，length-保存值的字符串缓冲区最大长度
	//配置文件中每行的信息格式为：key=value
	const int Common::GetValueByKeyFromConfig(const char* key, char* value, const unsigned int length)
	{
		if(NULL == key || NULL == value || length <= 0)
		{
			return -1;
		}
		
		int ret=0;
		int FD = 0;
		char* ConfigContent = new char[2048];
		char *findStr = NULL;
		

		//打开配置文件，读写模式
		FD  = open("./Config.ini",O_RDWR|O_CREAT,0644);

        if(FD < 0)
        {
            std::cout<<"./Config.ini配置文件打开错误！"<<std::endl;
            return -1;
        }

		//读取文件中的内容到ConfigContent缓冲区，缓冲区最多容纳2048字节
		ret = read(FD,ConfigContent,2048);
		//关闭打开的文件
		close(FD);
		//从文件中读取到的字符数大于等于1时，进行处理
		if(1 <= ret)
		{	
			//字符串查找函数，在ConfigContent中查找key字符串，并返回找到的位置，如果没找到，则返回NULL
			findStr = strstr(ConfigContent,key);
			//如果没找到子字符串
			if(findStr == NULL)
			{
				return -1;
			}
			
			const int keyLen = strlen(key);
			int i=0 ,j=0;
			for( ; i< strlen(findStr)-1-keyLen && j<length-1; i++)
			{
				if(findStr[keyLen+1+i] == ' ' || findStr[keyLen+1+i] == '\n' || findStr[keyLen+1+i] == '\r'  
					|| findStr[keyLen+1+i] == '\t'  || findStr[keyLen+1+i] == '#'   || findStr[keyLen+1+i] == EOF)
				{
					break;
				}
				else
				{
					value[j++] = findStr[keyLen+1+i];
				}
			}

			printf("key-%s\r\nvalue-%s\r\n", key, value);
			//返回找到的字符串value的实际长度
			return j;
		}
		else//从配置文件中读取到的字符数小于1，则直接返回
		{
            std::cout<<"配置文件读取错误！"<<std::endl;
			return -1;
		}
	}

	//read from  fd
	//输入参数：fd-文件描述符，buffer-存放数据的缓冲区，maxSize-最大读取数据量，timeout-超时时长（ms）
	//输出参数：size-本次读取数据量
	const int64_t Common::Read(const int fd, uint8_t* buffer, const uint64_t maxSize, uint64_t* size, const uint32_t timeout)
	{
		if(buffer == NULL || size == NULL || fd < 3)
		{
			return -1;
		}

		fd_set rd;
		struct timeval tv;
		
		int64_t ret = -1;
		
		//读取的数据总量
		uint64_t pos = 0;
		
		while(pos < maxSize)
		{
		
			FD_ZERO(&rd);
			FD_SET(fd, &rd);

			tv.tv_sec = timeout/1000;
			tv.tv_usec = (timeout%1000) * 1000;

			ret = select(fd+1,&rd,NULL,NULL,&tv);
			
			if(0 > ret)
			{
				printf("select failed where reading!\n");
				break;
			}
			else if(0 == ret)
			{
				printf("select timeout where reading!\n");
				break;
			}
			else
			{
				//如果当前监控描述符有变化，则进行处理
				if(FD_ISSET(fd,&rd))
				{
					ret = read(fd, buffer, maxSize);
					if(0 > ret)
					{
						printf("read data failed\n");
						break;
					}
					else if(0 == ret)
					{
						printf("port disconnect or endfile where reading!\n");
						break;
					}
					else
					{
						pos += ret;
						
						//*size = ret;
					}
				}
			}
		}
		
		//输出参数：size值设定
		memcpy(size, &pos, sizeof(pos));
		
		if(pos != maxSize)
		{
			printf("read max-size %ld bytes，but real read size %ld bytes\n", maxSize, size);
		}

		return pos;
	}

	//write to fd
	//输入参数：fd-文件描述符，buffer-存放数据的缓冲区，maxSize-最大发送数据量，timeout-超时时长（ms）
	//输出参数：size-本次写入的数据量
	const int64_t Common::Write(const int fd, const uint8_t* buffer, const uint64_t maxSize, uint64_t* size, const uint32_t timeout)
	{
		if(buffer == NULL || maxSize < 0 || size == NULL || fd < 3)
		{
			return -1;
		}

		fd_set wd;
		struct timeval tv;
		
		int64_t ret = -1;
		
		//写入的数据总量
		uint64_t pos = 0;
		
		//
		while(pos < maxSize)
		{
		
			FD_ZERO(&wd);
			FD_SET(fd, &wd);

			tv.tv_sec = timeout/1000;
			tv.tv_usec = (timeout%1000) * 1000;

			ret = select(fd+1,NULL,&wd,NULL,&tv);
			
			if(0 > ret)
			{
				printf("select failed where writing!\n");
				break;
			}
			else if(0 == ret)
			{
				printf("select timeout where writing!\n");
				break;
			}
			else
			{
				//如果当前监控描述符有变化，则进行处理
				if(FD_ISSET(fd,&wd))
				{
					ret = write(fd, buffer, maxSize);
					if(0 > ret)
					{
						printf("read data failed\n");
						break;
					}
					else if(0 == ret)
					{
						printf("port disconnect or endfile!\n");
						break;
					}
					else
					{
						pos += ret;
						
						//*size = ret;
					}
				}
			}
		}
		
		//输出参数：size值设定
		memcpy(size, &pos, sizeof(pos));
		
		if(pos != maxSize)
		{
			printf("write max-size %ld bytes，but real write size %ld bytes\n", maxSize, size);
		}

		return pos;
	}
	
	
	//判断system指令是否执行成功
	//输入参数：result调用system函数返回的值
	//返回值：0~成功，-1~失败
	const int Common::SystemCheck(int result)
	{
		if((-1 != result) && (WIFEXITED(result)) && (!(WEXITSTATUS(result))))
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	
	
	//异或校验，末尾四字节为校验值
	//输入参数：buffer-校验数据（含末尾四字节校验值位置），len-需要校验的数据长度（不包括末尾字节校验值）
	//输出参数：校验值
	//返回值：异或校验结果
	const uint8_t Common::XOR(uint8_t* buffer, const uint32_t len)
	{
		//要校验的数据为空，或者要校验的数据长度小于等于0，直接返回
		if(NULL == buffer || 0 >= len)
		{
			return 0;
		}
		uint8_t ret = buffer[0];
		
		//异或计算
		for(int i=0; i<len ;i++)
		{
			ret ^= buffer[i];
		}
		
		buffer[len] = ret;
		
		return ret;
	}
	
	
	//异或校验检查
	//输入参数：buffer-校验检查数据（包含最后一个校验字节在内），len-数据长度（包含最后一个校验字节在内）
	//输出参数:无
	//返回值：0~成功，-1~失败
	const int Common::ChechXOR(const uint8_t* buffer, const uint32_t len)
	{
		int ret=0;
		
		//要校验检查的数据为空，或者要校验的数据长度小于等于0，直接返回
		if(NULL == buffer || 0 >= len)
		{
			return 0;
		}
		
		uint8_t checkRet = buffer[0];
		
		//异或计算
		for(int i=0; i<len-1 ;i++)
		{
			checkRet ^= buffer[i];
		}
		
		//判断计算出的校验值是否和buffer最后一个字节值相等
		if(checkRet == buffer[len-1])
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		
		return ret;
	}
	
	
	//校验计算
	//输入参数：buffer-校验数据（含末尾校验值位置），len-需要校验的数据长度（不包括末尾校验值）
	//输出参数：无
	//返回值：校验值
	const int Common::GetCheckSB(uint8_t* buffer, const int32_t len)
	{
		//要校验的数据为空，或者要校验的数据长度小于等于0，直接返回
		if(NULL == buffer || 0 >= len)
		{
			return 0;
		}
		
		int ret = buffer[0];
		
		int iData = 0;
		//循环计算校验结果
		for(int i=1; i < len - 1 ;i++)
		{
			if(buffer[i] < 0)
			{
				iData = buffer[i] & 0xFF;
			}
			else
			{
				iData = buffer[i];
			}
			if(ret < 0)
			{
				ret = ret & 0xFF;
			}
			
			ret ^= iData;
		}
		
		return ret;
	}
	
	//校验检查
	//输入参数：buffer-校验检查数据（包含最后四个校验字节在内），len-数据长度（包含最后四个校验字节在内）
	//输出参数：无
	//返回值：0~成功，-1~失败
	const int Common::CheckSB(const uint8_t* buffer, const int32_t len)
	{
		//要校验的数据为空，或者要校验的数据长度小于等于0，直接返回
		if(NULL == buffer || 3 >= len)
		{
			return 0;
		}
		
		int ret = buffer[0];
		
		int iData = 0;
		
		for(int i=1; i < len - 4 ;i++)
		{
			if(buffer[i] < 0)
			{
				iData = buffer[i] & 0xFF;
			}
			else
			{
				iData = buffer[i];
			}
			if(ret < 0)
			{
				ret = ret & 0xFF;
			}
			
			ret ^= iData;
		}

        int recValue = (buffer[len-4] + buffer[len-3]*256 + buffer[len-2]*256*256 + buffer[len-1]*256*256*256);

        printf("cal value: %d\nrecv value %d\n", ret, recValue);
		
		if(ret == recValue)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		
		return ret;
	}
	
	
	//设置状态信息
	//输入参数：status状态值
	//输出参数：无
	//返回值：0~成功，-1~失败
	const int Common::SetStatus(const IPSERVER::enum_Status status)
	{
		Common::m_RunStatus = status;
		return 0;
	}
	

	//获取状态信息
	//输入参数：无
	//输出参数：无
	//返回值：状态标志值
	const IPSERVER::enum_Status Common::GetStatus()
	{
		return Common::m_RunStatus;
	}

    

    //将消息转换成字符串
    //msg-消息结构体，str-转换后的字符串保存位置，maxLen-str缓存的最大容量
    //返回str发送内容的总长度
    int Common::ST_ConvertTo_Char(const ST_MSG &msg, char* str, const uint32_t maxLen)
    {
        if(NULL == str || nullptr == str || maxLen == 0)
        {
            return -1;
        }

        //先清空缓存
        memset(str, 0 ,maxLen);

        //拷贝消息头
        memcpy(str, (void*)&msg.st_head, sizeof(ST_HEADER));

        //再拷贝消息体，消息体数据不为空时才可以拷贝
        if(NULL != msg.st_msg || nullptr != msg.st_msg)
        {
            switch(msg.st_head.st_command)
            {
            //移动、缩放图像动作
            case IPSERVER::enum_Command::e_Move:
            {
                IPSERVER::ST_MOVE *s_Move = (IPSERVER::ST_MOVE*)(msg.st_msg);
                std::cout<<"st_MoveType:"<<s_Move->st_MoveType<<endl
                        <<"st_MouseBtnType:"<<s_Move->st_MouseBtnType<<endl
                        <<"st_X:"<<s_Move->st_X<<endl
                        <<"st_Y:"<<s_Move->st_Y<<endl
                        <<"st_moveX:"<<s_Move->st_moveX<<endl
                        <<"st_moveY:"<<s_Move->st_moveY<<endl
                        <<"st_ScaleFactor:"<<s_Move->st_ScaleFactor<<endl;
                memcpy(str+sizeof(ST_HEADER),
                       &s_Move->st_MoveType, sizeof(s_Move->st_MoveType));
                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType),
                       &s_Move->st_MouseBtnType, sizeof(s_Move->st_MouseBtnType));

                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType)+sizeof(s_Move->st_MouseBtnType),
                       &s_Move->st_X, sizeof(s_Move->st_X));
                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType)+sizeof(s_Move->st_MouseBtnType)+sizeof(s_Move->st_X),
                       &s_Move->st_Y, sizeof(s_Move->st_Y));

                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType)+sizeof(s_Move->st_MouseBtnType)+sizeof(s_Move->st_X)+sizeof(s_Move->st_Y),
                       &s_Move->st_moveX, sizeof(s_Move->st_moveX));
                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType)+sizeof(s_Move->st_MouseBtnType)+sizeof(s_Move->st_X)+sizeof(s_Move->st_Y)+sizeof(s_Move->st_moveX),
                       &s_Move->st_moveY, sizeof(s_Move->st_moveY));
                memcpy(str+sizeof(ST_HEADER)+sizeof(s_Move->st_MoveType)+sizeof(s_Move->st_MouseBtnType)+sizeof(s_Move->st_X)+sizeof(s_Move->st_Y)+sizeof(s_Move->st_moveX)+sizeof(s_Move->st_moveY),
                       &s_Move->st_ScaleFactor, sizeof(s_Move->st_ScaleFactor));

                break;
            }
            //开始建图动作
            case IPSERVER::enum_Command::e_StartRTSP:
            {
                IPSERVER::ST_START_RTSP *s_start = (IPSERVER::ST_START_RTSP*)(msg.st_msg);
                std::cout<<"st_ViewpointType:"<<s_start->st_ViewpointType<<endl
                        <<"st_CoodinateSystemType:"<<s_start->st_CoodinateSystemType<<endl;
                memcpy(str+sizeof(ST_HEADER),
                       &s_start->st_ViewpointType, sizeof(s_start->st_ViewpointType));
                memcpy(str+sizeof(ST_HEADER)+sizeof(s_start->st_ViewpointType),
                       &s_start->st_CoodinateSystemType, sizeof(s_start->st_CoodinateSystemType));

                break;
            }
            //视角转换
            case IPSERVER::enum_Command::e_ViewpointSwitch:
            {
                IPSERVER::ST_VIEWPOINT_SWITCH *s_VP = (IPSERVER::ST_VIEWPOINT_SWITCH*)(msg.st_msg);
                std::cout<<"st_ViewpointType:"<<s_VP->st_ViewpointType<<endl;
                memcpy(str+sizeof(ST_HEADER),
                       &s_VP->st_ViewpointType, sizeof(s_VP->st_ViewpointType));

                break;
            }
            //坐标系转换
            case IPSERVER::enum_Command::e_CoordinateSystemSwitch:
            {
                IPSERVER::ST_COORDINATESYSTEM_SWITCH *s_CS = (IPSERVER::ST_COORDINATESYSTEM_SWITCH*)(msg.st_msg);
                std::cout<<"st_CoodinateSystemType:"<<s_CS->st_CoodinateSystemType<<endl;
                memcpy(str+sizeof(ST_HEADER),
                       &s_CS->st_CoodinateSystemType, sizeof(s_CS->st_CoodinateSystemType));

                break;
            }
            default:
                break;
            }
        }

        //最后拷贝消息尾
        memcpy(str+sizeof(ST_HEADER)+msg.st_head.st_len, (void*)&msg.st_tail, sizeof(ST_TAIL));

        return sizeof(msg.st_head)+sizeof(msg.st_tail)+msg.st_head.st_len;
    }

    
    //设置退出信号
    const int Common::SetSigFlag(const bool flag)
    {
        IPSERVER::Common::SIGQUITFLAG = flag;
        return 0;
    }
    //获取退出信号
    const bool Common::GetSigFlag()
    {
        return IPSERVER::Common::SIGQUITFLAG;
    }

    
    //加校验计算，将buffer的数据累加起来计算数值，最后对结果取反
    const uint8_t Common::CheckSum(const uint8_t *buffer, const uint32_t num)
    {
        uint8_t ret = 0;
        //计算累加和
        for(uint32_t i =0; i < num; i++)
        {
            ret += buffer[i];
        }
        //取反
        ret = ~ret;

        return ret;
    }

	
	

}//end namespace IPSERVER
