#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <mosquitto.h>
#include "../common/Common-mq.h"

void MQTTStart(struct mosquitto *mosq);
int my_mosquitto_publish(struct mosquitto *mosq, int  payloadlen, const void *payload);



#endif