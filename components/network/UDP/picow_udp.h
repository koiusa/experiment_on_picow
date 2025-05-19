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
            try_wifi_connect_count(0),
            try_wifi_connect_max(TRY_WIFI_CONNECT_MAX),
            wifi_connect_timeout_ms(WIFI_CONNECT_TIMEOUT_MS) {
                memset(&addr, 0, sizeof(addr));
                memset(&pcb, 0, sizeof(pcb));
        }
        ~PicowUDP(){
            if (pcb) {
                udp_remove(pcb);
            }
            if (spcb) {
                udp_remove(spcb);
            }
        }
        void prototype(); 
        int init_arch();
        void init_udp();
        void flush_udp();
        void try_wifi_connect();
        void reset_wifi_connect_count() { try_wifi_connect_count = 0; }
        void set_wifi_connect_max(const int count) { try_wifi_connect_max = count; }
        void set_wifi_connect_timeout(const int timeout) { wifi_connect_timeout_ms = timeout; }
        void set_wifi_config(const wifi_config& config) {
            wifi_config_ = config;
        }
        void set_udp_config(const udp_config& config) {
            udp_config_ = config;
        }
        void apply_msg(osc::bundle& msg) {
            msg << (osc::message{ "/host" } << int32_t(&pcb->local_ip));
            msg << (osc::message{ "/port" } << int32_t(pcb->local_port));
            msg_queue.push(msg.to_packet());
            send_udp();
        }
        void bind_receive_udp() {
            	spcb = udp_new();
	            err_t err = udp_bind(spcb, IP_ADDR_ANY, OWN_PORT);
	            udp_recv (spcb, RcvFromUDP, NULL);
        }
    private:
        static void RcvFromUDP (void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
        std::queue<osc::packet> msg_queue;
        wifi_config wifi_config_;
        udp_config udp_config_;
        int wifi_connect_timeout_ms;
        int try_wifi_connect_max;
        int try_wifi_connect_count;
        bool is_connected;
        int counter;
        struct udp_pcb* pcb;
        struct udp_pcb* spcb;
        ip_addr_t addr;
        void wifi_connect();
        void send_udp();
        
};

#endif // PICOW_UDP_H
