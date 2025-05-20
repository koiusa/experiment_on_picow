#ifndef PICOW_UDP_H
#define PICOW_UDP_H

#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <queue>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include <osc++.hpp>

#define BEACON_MSG_LEN_MAX 127
#define BEACON_INTERVAL_MS 0

#define TRY_WIFI_CONNECT_MAX 2
#define WIFI_CONNECT_TIMEOUT_MS 30000

#define OWN_PORT 49153
class PicowUDP {
    // Initialization of counter moved to the constructor
    public:
        class IUdpListener
        {   
            public:
                void Bind(PicowUDP* udp) { this->udp = udp; };
                void send_bundle(osc::bundle& msg) {
                    if (udp) {
                        udp->send_bundle(msg);
                    } else {
                        printf("Not bind udp.\n");
                    }
                }
            private:
                PicowUDP* udp = 0;
        };
        struct wifi_config {
            const char* ssid;
            const char* password;
        };
        struct udp_config {
            const char* target_host;
            int target_port;
        };
        PicowUDP() :
            wifi_config_({ .ssid = NULL, .password = NULL }),
            udp_config_({ .target_host = NULL, .target_port = 0 }),
            counter(0),
            is_connected(false),
            wifi_connect_try_count(0),
            wifi_connect_try_max(TRY_WIFI_CONNECT_MAX),
            wifi_connect_timeout_ms(WIFI_CONNECT_TIMEOUT_MS) {
                memset(&addr, 0, sizeof(addr));
                memset(&sender_pcb, 0, sizeof(sender_pcb));
        }
        ~PicowUDP(){
            if (sender_pcb) {
                udp_remove(sender_pcb);
            }
            if (receiver_pcb) {
                udp_remove(receiver_pcb);
            }
        }
        void prototype(); 
        int init_arch();
        void flush_udp();
        void try_wifi_connect();
        void reset_wifi_connect_try_count() { wifi_connect_try_count = 0; }
        void set_wifi_connect_try_max(const int count) { wifi_connect_try_max = count; }
        void set_wifi_connect_timeout(const int timeout) { wifi_connect_timeout_ms = timeout; }
        void set_wifi_config(const wifi_config& config) { wifi_config_ = config; }
        void set_udp_config(const udp_config& config) { udp_config_ = config; }
        void send_bundle(osc::bundle& msg);
        void bind_listener(IUdpListener* listener) { listener->Bind(this); }
    private:
        static void receive_msg_fn (void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
        void send_msg_fn();
        std::queue<osc::packet> msg_queue;
        wifi_config wifi_config_;
        udp_config udp_config_;
        int wifi_connect_timeout_ms;
        int wifi_connect_try_max;
        int wifi_connect_try_count;
        bool is_connected;
        int counter;
        struct udp_pcb* sender_pcb;
        struct udp_pcb* receiver_pcb;
        ip_addr_t addr;
        bool wifi_connect();
        void prepare_udp_sender();
        void prepare_udp_receiver();
        
};

#endif // PICOW_UDP_H
