#ifndef TASK_ESPNOW_HPP
#define TASK_ESPNOW_HPP

#include "thread.hpp"
#include "injector/singleton.hpp"
#include "SharedStruct.hpp"
#include "esp_log.h"


#include <string>
#include <cstring>
#include <mutex>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"
#include "esp_wifi.h"


using namespace cpp_freertos;

class TaskEspNow : public Thread, public Singleton<TaskEspNow>
{
    public:
        TaskEspNow(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData);
        SharedStruct getPacket();

        void Run() override;

        /// @brief Envia dados via ESPNOW
        void Send(SharedStruct Packet);
    private:

        bool wifiAlreadyInit = false;
        
        uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        //uint8_t broadcastAddress[6] = {0xe0,0xe2,0xe6,0x0d,0x43,0x0c};
        //uint8_t broadcastAddress[6] = {0x98,0xcd,0xac,0xad,0xef,0xec};

        void ESPNOWInit(uint8_t canal, uint8_t *Mac, bool criptografia); // Inicia o espnow e registra os dados do peer
        
        static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status); // Evento para enviar o dado
        static void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len); // Evento de dado Recebido


        esp_now_peer_info_t peerInfo; // Variável para adicionar o peer
        SemaphoreHandle_t xSemaphorePeerInfo;
        SemaphoreHandle_t xSemaphoreSharedPacket;

        SharedStruct packetReceived;
        static QueueHandle_t queuePacketsReceived;

        SharedStruct *shared = NULL;
};

#endif