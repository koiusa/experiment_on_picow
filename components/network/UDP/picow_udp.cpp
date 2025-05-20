#include "picow_udp.h"

void PicowUDP::send_msg_fn(const osc::packet msg) {
    if (!is_connected) {
        printf("Not connected to Wi-Fi.\n");
        return;
    }

    // Pop the message from the queue
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, msg.size(), PBUF_RAM);
    std::memcpy(p->payload, msg.data(), msg.size());

    // Send a UDP packet
    err_t er = udp_sendto(sender_pcb, p, &addr, udp_config_.target_port);
    pbuf_free(p);
    if (er != ERR_OK) {
        printf("Failed to send UDP packet! error=%d", er);
    } else {
        printf("Sent packet %d\n", counter);
        printf("udp local_ip %d\n", &sender_pcb->local_ip);
        printf("udp local_port %d\n", sender_pcb->local_port);
        printf("udp remote_ip %d\n", &sender_pcb->remote_ip);
        printf("udp remote_port %d\n", sender_pcb->remote_port);
        printf("udp addr %d\n", addr.addr);
        printf("udp len %d\n", p->len);
        printf("udp payload %d\n", p->payload);
        printf("udp data %d\n", msg.data());
        printf("udp size %d\n", msg.size());
        counter++;
    }

    // Note in practice for this simple UDP transmitter,
    // the end result for both background and poll is the same

#if PICO_CYW43_ARCH_POLL
    // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
    // main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
    cyw43_arch_poll();
    sleep_ms(BEACON_INTERVAL_MS);
#else
    // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
    // is done via interrupt in the background. This sleep is just an example of some (blocking)
    // work you might be doing.
    sleep_ms(BEACON_INTERVAL_MS);
#endif
};

void PicowUDP::flush_udp() {
    if (msg_queue.empty()) {
        printf("No Message Queue.\n");
        return;
    }

    counter = 0;
    while (!msg_queue.empty()) {
        osc::packet msg = msg_queue.front();
        msg_queue.pop();
        send_msg_fn(msg);
    }
};

void PicowUDP::try_wifi_connect() {
    if (is_connected) {
        printf("Already connected.\n");
        return;
    }
    if (wifi_connect_try_count > wifi_connect_try_max) {
        printf("Failed to connect to Wi-Fi.\n");
        return;
    }
    while (!is_connected ) {
        wifi_connect_try_count++;
        printf("Attempting to connect... %d\n", wifi_connect_try_count);
        if (wifi_connect()){
            prepare_udp_sender();
            prepare_udp_receiver();
        }
    }
};

void PicowUDP::prepare_udp_sender() {  
    sender_pcb = udp_new();
    ipaddr_aton(udp_config_.target_host, &addr);
};

void PicowUDP::prepare_udp_receiver() {
    receiver_pcb = udp_new();
    err_t err = udp_bind(receiver_pcb, IP_ADDR_ANY, OWN_PORT);
    udp_recv (receiver_pcb, receive_msg_fn, NULL);
}

int PicowUDP::init_arch() {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    return 0;
};

bool PicowUDP::wifi_connect() {
    // bletuthの処理でcyw43_arch_init()が呼ばれているので、ここでは呼ばない;

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(wifi_config_.ssid, wifi_config_.password, CYW43_AUTH_WPA2_AES_PSK, wifi_connect_timeout_ms)) {
        is_connected = false;
        printf("failed to connect.\n");
    } else {
        is_connected = true;
        wifi_connect_try_count = 0;
        printf("Connected.\n");
    }
    return is_connected;
};

void PicowUDP::send_bundle(osc::bundle& msg) {
    if (!is_connected) {
        printf("Not connected to Wi-Fi.\n");
        return;
    }
    msg << (osc::message{ "/host" } << int32_t(&sender_pcb->local_ip));
    msg << (osc::message{ "/port" } << int32_t(sender_pcb->local_port));
    send_msg_fn(msg.to_packet());
}

void PicowUDP::receive_msg_fn (void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    char ip[32];
	ip4addr_ntoa_r (addr, ip, sizeof(ip));
	if (p->tot_len == p->len) {
		// single fragment
		printf ("Rcv from %s:%d, len %d\n", ip, port, p->tot_len);
	} else {
		printf ("Rcv from %s:%d, total %d [", ip, port, p->tot_len);
		for (struct pbuf *q = p; q != NULL; q = q->next) {	// we need to go over all fragments (it'll be more than one if size > MTU)
			printf("%d", q->len);
			//printf (" (%s)", (char*) p->payload);
			if (q->next) printf(" ");
		}
		printf("]\n");
	}
	pbuf_free(p);	// this is needed or we'll stop receiving packets after a while!
}

void PicowUDP::prototype() {
    stdio_init_all();
    
    init_arch();
    if (wifi_connect()){
        prepare_udp_sender();
        
        osc::bundle bundle{osc::time()};
        bundle << (osc::message{ "/prototype" } << "test");
        msg_queue.push(bundle.to_packet());
        flush_udp();
    }
    cyw43_arch_deinit();

    stdio_deinit_all();
};
