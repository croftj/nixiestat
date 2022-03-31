#ifndef MQTT_H
#define MQTT_H

#include <deque>
#include <list>
#include <string>

#include "mqtt_client.h"
#include "configuration.h"
#include "wifi.h"

#include "freertos/semphr.h"

static void *connectionExec(void *obj);
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

class MQTT
{
public:
   enum
   {
      TOPIC_LEN = 64,
      MESG_LEN  = 880
   };

   typedef std::list<std::string> topic_list_t;

   typedef class MessageEntry
   {
   public:
      MessageEntry()
      {
         m_topic = new char[TOPIC_LEN + 1];
         memset(m_topic, '\0', TOPIC_LEN + 1);
         m_mesg = new char[MESG_LEN + 1];
         memset(m_mesg, '\0', MESG_LEN + 1);
         m_valid = false;
      }

      ~MessageEntry()
      {
         delete m_topic;
         delete m_mesg;
      }

      char *topic()
      {
         return(m_topic);
      }

      char *message()
      {
         return(m_mesg);
      }

      void set(const char *topic, const char *mesg)
      {
         m_valid = true;
         strncpy(m_mesg, mesg, MESG_LEN);
         strncpy(m_topic, topic, TOPIC_LEN);
      }

      bool isValid()
      {
         bool rv = m_valid;
         return(rv);
      }

      void invalidate()
      {
         m_valid = false;
      }

   private:
      char  *m_topic;
      char  *m_mesg;
      bool  m_valid;
   } message_t;

//   typedef std::pair<std::string, std::string> message_t;
   typedef std::deque<message_t> message_queue_t;

   typedef enum 
   {
      DISCONNECTED = 0,
      CONNECTED
   } status_t;

   typedef enum
   {
      NOP               = 0,
      SUBSCRIBE_TOPICS  = 1
   } actions_t;

   MQTT()
   {
      m_msgMutex = xSemaphoreCreateMutexStatic(&m_msgMutexBuf);
      xSemaphoreGive(m_msgMutex);
   }

   void addSubscription(std::string &topic)
   {
      m_subTopics.push_back(topic);
   }

   bool connected()
   {
      return(m_client != NULL);
   }

   void setWifi(Wifi *wifi)
   {
      m_wifi = wifi;
   }

   static bool clientReady();
   void setBroker(std::string &broker);
   void sendData(const char *topic, const char *json);
   void sendData(std::string &topic, std::string &json);
   void subscribe(std::string &topic);
   message_t *getNext(MessageEntry** queue);
   void releaseMessage(message_t *msg);
   static void* procMessages(void *);

protected:
   bool start(void);

private:
   friend void *connectionExec(void*);
   friend esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

   std::string                      m_broker;
   static Wifi*                     m_wifi;
   static actions_t                 m_action;
   static esp_mqtt_client_handle_t  m_client;
   static bool                      m_busDisconnect;
   static bool                      m_notReady;
   static StaticSemaphore_t         m_msgMutexBuf;
   static SemaphoreHandle_t         m_msgMutex;
   static topic_list_t              m_subTopics;
};

extern MQTT* mqtt_bus;

#endif
