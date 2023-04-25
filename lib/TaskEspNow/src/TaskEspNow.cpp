#include "TaskEspNow.hpp"

QueueHandle_t TaskEspNow::queuePacketsReceived;

TaskEspNow::TaskEspNow(std::string name, uint32_t stackDepth, UBaseType_t priority, SharedStruct *sharedData):Thread(name, stackDepth, priority)
{
    esp_log_level_set(name.c_str(), ESP_LOG_ERROR);
    ESP_LOGI(GetName().c_str(), "Inicializando EspNow");

    (xSemaphorePeerInfo) = xSemaphoreCreateMutex();
    (xSemaphoreSharedPacket) = xSemaphoreCreateMutex();

    queuePacketsReceived = xQueueCreate(10, sizeof(SharedStruct));
    shared = sharedData;

    this->ESPNOWInit(11, broadcastAddress, false);
    this->Start();

}

void TaskEspNow::Run()
{
    for(;;)
    {
        vTaskDelay(0);
        xQueueReceive(queuePacketsReceived, &packetReceived, portMAX_DELAY);
        if(shared->xSemaphore != NULL )
        {
            if( xSemaphoreTake( shared->xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                SemaphoreHandle_t smptmp = shared->xSemaphore;
                (*shared) = packetReceived;
                shared->xSemaphore = smptmp;
                xSemaphoreGive( shared->xSemaphore );
            }
        }
    }   
}

/// @brief Inicia o ESPNOW e registra os dados do peer
/// @param canal Canal do ESPNOW
/// @param Mac Endereço MAC do peer
/// @param criptografia Ativa ou desativa a criptografia
void TaskEspNow::ESPNOWInit(uint8_t canal, uint8_t *Mac, bool criptografia)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    if (!wifiAlreadyInit)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        esp_event_loop_create_default();
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
        ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
        ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
        ESP_ERROR_CHECK( esp_wifi_start());
        ESP_ERROR_CHECK( esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE));

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK( esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
#endif
        wifiAlreadyInit = true;
    }
    if (xSemaphoreTake(xSemaphorePeerInfo, (TickType_t)10) == pdTRUE)
    {
        memcpy(this->peerInfo.peer_addr, Mac, canal);
        this->peerInfo.channel = canal;
        this->peerInfo.encrypt = criptografia;
        this->peerInfo.ifidx = WIFI_IF_STA;
        xSemaphoreGive(xSemaphorePeerInfo);
    }
    else
    {
        ESP_LOGE(GetName().c_str(), "Variável PeerInfo ocupada, não foi possível definir valor.");
    }

    if (esp_now_init() != 0)
        ESP_LOGI(GetName().c_str(), "Falha ao iniciar");

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(TaskEspNow::OnDataSent);
    
    // Adiciona peer
    if (xSemaphoreTake(xSemaphorePeerInfo, (TickType_t)10) == pdTRUE)
    {
        ESP_LOGD(GetName().c_str(), "PeerMac : %x|%x|%x|%x|%x|%x ", this->peerInfo.peer_addr[0], this->peerInfo.peer_addr[1], this->peerInfo.peer_addr[2], this->peerInfo.peer_addr[3], this->peerInfo.peer_addr[4], this->peerInfo.peer_addr[5]);
        if (esp_now_add_peer(&(this->peerInfo)) != ESP_OK)
        {
            ESP_LOGD(GetName().c_str(), "Failed to add peer");
            xSemaphoreGive(xSemaphorePeerInfo);
            return;
        }
        else
            xSemaphoreGive(xSemaphorePeerInfo);
    }

    esp_now_register_recv_cb(TaskEspNow::OnDataRecv);

}

void TaskEspNow::Send(SharedStruct Packet)
{

    ESP_LOGI(GetName().c_str(), "Mac Destino : %x|%x|%x|%x|%x|%x ", this->peerInfo.peer_addr[0], this->peerInfo.peer_addr[1], this->peerInfo.peer_addr[2], this->peerInfo.peer_addr[3], this->peerInfo.peer_addr[4], this->peerInfo.peer_addr[5]);
    esp_now_send(this->peerInfo.peer_addr, (uint8_t *)&Packet, sizeof(SharedStruct));
    vTaskDelay(0);

}

void TaskEspNow::OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len)
{
    uint8_t * mac_addr = recv_info->src_addr;
    ESP_LOGI("TaskEspNow", "Mac Destino : %x|%x|%x|%x|%x|%x ", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    ESP_LOGI("TaskEspNow", "Mensagem recebida, bytes: %d", len);

    SharedStruct *Packet = (SharedStruct *)incomingData;

    xQueueSend(queuePacketsReceived, Packet, 0);
}

void TaskEspNow::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    ESP_LOGI("TaskEspNow", "Mac Destino : %x|%x|%x|%x|%x|%x ", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    ESP_LOGI("TaskEspNow", "Status do envio: %s", status == ESP_NOW_SEND_SUCCESS ? "SUCESSO" : "FALHA");
}

SharedStruct TaskEspNow::getPacket()
{
    SharedStruct tempPacket = packetReceived;
    if (xSemaphoreTake(xSemaphoreSharedPacket, (TickType_t)10) == pdTRUE)
    {
        xSemaphoreGive(xSemaphoreSharedPacket);
        return tempPacket;
    }
    else
    {
        ESP_LOGE(GetName().c_str(), "Semáforo ocupado");
        return tempPacket;
    }
}
