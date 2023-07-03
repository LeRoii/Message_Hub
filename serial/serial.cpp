#include <iostream>
#include "serial.h"

namespace IPSERVER
{

    Serial::Serial()
    {
        this->iSetOpt = 0;
        this->fdSerial = 0;

        this->m_RunStatus = -1;//串口通道未准备好
    }

    Serial::~Serial()
    {
    }

    int Serial::openPort(int fd, int comport)
    {
        /*if (comport == 1)  
        {  
            fd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);  
            if (-1 == fd)  
            {  
                perror("Can't Open Serial Port /dev/ttyTHS1");  
                return(-1);  
            }  
            else  
            {  
                printf("open /dev/ttyTHS1 succeed .....\n");  
            }  
        }  
        else if (comport == 0)  
        {  
            fd = open("/dev/ttyTHS0", O_RDWR | O_NOCTTY | O_NDELAY);  
            if (-1 == fd)  
            {  
                perror("Can't Open Serial Port /dev/ttyTHS0");  
                return(-1);  
            }  
            else  
            {  
                printf("open /dev/ttyTHS0 succeed .....\n");  
            }  
        } 
        else if (comport == 2)  
        {  
            fd = open("/dev/ttyTCU0", O_RDWR | O_NOCTTY | O_NDELAY);  
            if (-1 == fd)  
            {  
                perror("Can't Open Serial Port /dev/ttyTCU0");  
                return(-1);  
            }  
            else  
            {  
                printf("open /dev/ttyTCU0 succeed .....\n");  
            }  
        }*/ 

        //根据串口参数读取设备文件
        char devFile[1024]={0};
        //串口1，读取串口设备文件
        if(comport == 1)
        {
            if(Common::GetValueByKeyFromConfig("SERIAL_PORT_1_DEVICEFILE", devFile, 1024) == -1)
            {
                std::cout<<"从配置文件读取串口1设备信息错误"<<std::endl;
            }
            else
            {
                std::cout<<"串口1设备信息为："<<devFile<<std::endl;
            }
        }
        //串口2，读取串口设备文件
        else if(comport == 2)
        {
            if(Common::GetValueByKeyFromConfig("SERIAL_PORT_2_DEVICEFILE", devFile, 1024) == -1)
            {
                std::cout<<"从配置文件读取串口2设备信息错误"<<std::endl;
            }
            else
            {
                std::cout<<"串口2设备信息为："<<devFile<<std::endl;
            }
        }

        fd = open(devFile, O_RDWR | O_NOCTTY | O_NDELAY);  
        if (-1 == fd)  
        {  
            perror("Can't Open Serial Port");  
            return(-1);  
        }  
        else  
        {  
            printf("open %s succeed .....\n", devFile);  
        }  
    
        if (fcntl(fd, F_SETFL, 0)<0)  
        {  
            printf("fcntl failed!\n");  
        }  
        else  
        {  
            printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));  
        }  
        if (isatty(STDIN_FILENO) == 0)  
        {  
            printf("standard input is not a terminal device\n");  
        }  
        else  
        {  
            printf("is a tty success!\n");  
        }  
        printf("fd-open=%d\n", fd);  
        return fd;
    }

    int Serial::setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop)  
    {  
        struct termios newtio, oldtio;  
        if (tcgetattr(fd, &oldtio) != 0)  
        {  
            perror("SetupSerial 1");  
            return -1;  
        }  
        bzero(&newtio, sizeof(newtio));  
        newtio.c_cflag |= CLOCAL | CREAD;  
        newtio.c_cflag &= ~CSIZE;  
    
        switch (nBits)  
        {  
        case 7:  
            newtio.c_cflag |= CS7;  
            break;  
        case 8:  
            newtio.c_cflag |= CS8;  
            break;  
        }  
    
        switch (nEvent)  
        {  
        case 'O':                     //奇校验  
            newtio.c_cflag |= PARENB;  
            newtio.c_cflag |= PARODD;  
            newtio.c_iflag |= (INPCK | ISTRIP);  
            break;  
        case 'E':                     //偶校验  
            newtio.c_iflag |= (INPCK | ISTRIP);  
            newtio.c_cflag |= PARENB;  
            newtio.c_cflag &= ~PARODD;  
            break;  
        case 'N':                    //无校验  
            newtio.c_cflag &= ~PARENB;  
            break;  
        }  
    
        switch (nSpeed)  
        {  
        case 2400:  
            cfsetispeed(&newtio, B2400);  
            cfsetospeed(&newtio, B2400);  
            break;  
        case 4800:  
            cfsetispeed(&newtio, B4800);  
            cfsetospeed(&newtio, B4800);  
            break;  
        case 9600:  
            cfsetispeed(&newtio, B9600);  
            cfsetospeed(&newtio, B9600);  
            break;  
        case 115200:  
            cfsetispeed(&newtio, B115200);  
            cfsetospeed(&newtio, B115200);  
            break;  
        case 230400:  
            cfsetispeed(&newtio, B230400);  
            cfsetospeed(&newtio, B230400);  
            break; 
        default:  
            cfsetispeed(&newtio, B9600);  
            cfsetospeed(&newtio, B9600);  
            break;  
        }  
        if (nStop == 1)  
        {  
            newtio.c_cflag &= ~CSTOPB;  
        }  
        else if (nStop == 2)  
        {  
            newtio.c_cflag |= CSTOPB;  
        }  
        newtio.c_cc[VTIME] = 0;  
        newtio.c_cc[VMIN] = 0;  
        tcflush(fd, TCIFLUSH);  
        if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)  
        {  
            perror("com set error");  
            return -1;  
        }  
        printf("set done!\n");  
        return 0;  
    }

    int Serial::readDataTty(int fd, uint8_t *rcv_buf, int TimeOut, int Len)  
    {  
        int retval;  
        fd_set rfds;  
        struct timeval tv;  
        int ret, pos;  
        tv.tv_sec = TimeOut / 1000;  //set the rcv wait time    
        tv.tv_usec = TimeOut % 1000 * 1000;  //100000us = 0.1s    
    
        pos = 0;  
        while (1)  
        {  
            FD_ZERO(&rfds);  
            FD_SET(fd, &rfds);  
            retval = select(fd + 1, &rfds, NULL, NULL, &tv);  
            if (retval == -1)  
            {  
                perror("select()");  
                break;  
            }  
            else if (retval)  
            {  
                ret = read(fd, rcv_buf + pos, 1);  
                if (-1 == ret)  
                {  
                    break;  
                }  
    
                pos++;  
                if (Len <= pos)  
                {  
                    break;  
                }  
            }  
            else  
            {  
                break;  
            }  
        }  
    
        return pos;  
    }  
    
    int Serial::sendDataTty(int fd, uint8_t *send_buf, int Len)  
    {  
        ssize_t ret;  
    
        ret = write(fd, send_buf, Len);  
        if (ret == -1)  
        {  
            printf("serial write device error\n");  
        }

        return ret;  
    }

    int Serial::set_serial(int port)
    {
        //openPort  
        if ((fdSerial = openPort(fdSerial, port))<0)//1--"/dev/ttyS0",2--"/dev/ttyS1",3--"/dev/ttyS2",4--"/dev/ttyUSB0" 小电脑上是2--"/dev/ttyS1"  
        {  
            perror("open_port error");  
            return -1;  
        }

        if ((iSetOpt = setOpt(fdSerial, 230400, 8, 'N', 1))<0)  
        {  
            perror("set_opt error");  
            return -1;  
        }  

        /*if (port == 1 || port == 2)
        {
            if ((iSetOpt = setOpt(fdSerial, 230400, 8, 'N', 1))<0)  
            {  
                perror("set_opt error");  
                return -1;  
            }  
        }
        else if (port == 0)  
        {
                if ((iSetOpt = setOpt(fdSerial, 230400, 8, 'N', 1))<0)  
            {  
                perror("set_opt error");  
                return -1;  
            }  
        }*/
    
        printf("Serial fdSerial=%d\n", fdSerial);  
    
        tcflush(fdSerial, TCIOFLUSH);//清掉串口缓存  
        fcntl(fdSerial, F_SETFL, 0);

        this->m_RunStatus = 0;
    }

    int Serial::serial_send(uint8_t* buffSenData, unsigned int sendDataNum)
    {
        return sendDataTty(fdSerial, buffSenData, sendDataNum); 
    }

    int Serial::serial_recieve(uint8_t* buffRcvData)
    { 
        //读取1024字节数据到bufferRcvData，超时时间设置为2ms
        return readDataTty(fdSerial, buffRcvData, 2, 1024);
        // std::cout <<  int(buffRcvData[0]) << " " <<  int(buffRcvData[1]) << " "  <<  int(buffRcvData[2]) << " " <<  int(buffRcvData[3]) << " " 
        //         <<  int(buffRcvData[4]) << " " <<  int(buffRcvData[5]) << " " <<  int(buffRcvData[6]) << std::endl; 
    }

    
    //使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
    //从客户端接收数据执行体
    void Serial::OnReceive()
    {
        uint8_t buffRcvData_servo[1024] = {0};
        while(this->m_RunStatus == 1)
        {
            //从伺服接收数据
            int retLen = this->serial_recieve(buffRcvData_servo);
            if(retLen > 0)
            {
                std::cout<<"从串口接收到"<<std::dec<<retLen<<"字节数据"<<std::endl;
                for(int i=0; i< retLen ;i++)
                {
                    printf("[%02X]", buffRcvData_servo[i]);
                }
                std::cout<<std::endl<<std::endl;

                //数据回调
                if(m_Send_Data_Func != NULL && m_Send_Data_Func != nullptr)
                {
                    int ret = m_Send_Data_Func(buffRcvData_servo, retLen);
                }
                else
                {
                    std::cout<<"串口未设置回调对象"<<std::endl;
                }

                
                memset(buffRcvData_servo,0,1024);
            }            
        }

        std::cout<<"serial OnReceive thread end!"<<std::endl;

        //关闭串口
        this->closePort(this->fdSerial);
    }
    


    //启动线程,
    //返回值1-正在通信, 0-通道正常，准备开启接收消息的线程，-1-断开通信，通信通道不正常，需要重新设置通信MSG_Key值
	int Serial::OnStart()
	{
		if(this->m_RunStatus == 0)
		{
            
            this->SetStatus(1);
            
			// //创建接收消息的线程
			// ret = pthread_create(&this->p_Recv, NULL, OnMQReceive, this);
			// if(ret != 0)
			// {
			// 	perror("create OnMQReceive thread fail\n");
			// 	return -1;
			// }
			// else
			// {
			// 	printf("create OnMQReceive thread succeed\n");
			// }

            //使用C++11标准类thread创建线程，注意需要gcc版本支持c++11
            //创建接收消息的线程
            this->r_thread = std::thread(&Serial::OnReceive, this);
            r_thread.detach();//主线程与子线程分离，保证主线程结束不影响子线程，确保子线程在后台运行
            printf("Serial 创建接收数据的线程成功\n");
            
		}
        else if(this->m_RunStatus == 1)
        {
            printf("Serial 接收数据的线程已经在运行\n");
        }
        else
        {
            printf("Serial 创建接收数据的线程失败，通道未初始化\n");
        }
		return this->m_RunStatus;
	}

    
	//设置状态机信息
	const int Serial::SetStatus(const int status)
	{
		this->m_RunStatus = status;
		return 0;
	}
	
	//获取状态机信息
	const int Serial::GetStatus() const
	{
		return this->m_RunStatus;
	}
	

    //关闭串口
    int Serial::closePort(int fd)
    {
        close(fd);

        return 0;
    }

    
    //设置回调函数
    int Serial::set_callback_func(SERIAL_CALLBACK_FUNC func)
    {
        this->m_Send_Data_Func = func;

        return 0;
    }


}//end namespace IPSERVER
