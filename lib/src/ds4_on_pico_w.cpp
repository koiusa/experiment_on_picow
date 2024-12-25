
#include "ds4_on_pico_w.hpp"

#include "btstack.h"
#include "btstack_config.h"
#include "btstack_run_loop.h"
#include "classic/sdp_server.h"
#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////
#ifndef ERROR_DS4_REPORTS
#define ERROR_DS4_REPORTS 1
#endif
#ifndef INFO_DS4_REPORTS
#define INFO_DS4_REPORTS 1
#endif
#ifndef DEBUG_DS4_REPORTS
#define DEBUG_DS4_REPORTS 1
#endif
#ifndef VERBOSE_DS4_REPORTS
#define VERBOSE_DS4_REPORTS 0
#endif

#define MAX_ATTRIBUTE_VALUE_SIZE 512
#define MAX_DEVICES              20
#define INQUIRY_INTERVAL         1

///////////////////////////////////////////////////////
// Structures
///////////////////////////////////////////////////////
#pragma region Ds4forPicoW_Structures
enum DEVICE_STATE
{
    REMOTE_NAME_REQUEST,
    REMOTE_NAME_INQUIRED,
    REMOTE_NAME_FETCHED
};
struct device {
    bd_addr_t address;
    uint8_t pageScanRepetitionMode;
    uint16_t clockOffset;
    enum DEVICE_STATE state;
};
enum STATE
{
    INIT,
    ACTIVE
};

const struct DualShock4_state default_state = {
    .hat         = 0x8,
    .share       = false,
    .options     = false,
    .ps          = false,
    .triangle    = false,
    .square      = false,
    .circle      = false,
    .cross       = false,
    .touch       = false,
    .touch_x     = 0x00,
    .touch_y     = 0x00,
    .l1          = false,
    .l2          = false,
    .l2_value    = 0x00,
    .r1          = false,
    .r2          = false,
    .r2_value    = 0x00,
    .l3          = false,
    .l3_x        = 0x00,
    .l3_y        = 0x00,
    .r3          = false,
    .r3_x        = 0x00,
    .r3_y        = 0x00,
    .gyro_x      = 0x00,
    .gyro_y      = 0x00,
    .gyro_z      = 0x00,
    .accel_x     = 0x00,
    .accel_y     = 0x00,
    .accel_z     = 0x00,
    .battery     = 0x00,
    .temperature = 0x00,
    .timestamp   = 0x00,
    .connected   = false,
};

struct __attribute__((packed)) input_report_17 {
    uint8_t report_id;
    uint8_t pad[2];

    uint8_t lx, ly;
    uint8_t rx, ry;
    uint8_t buttons[3];
    uint8_t l2, r2;

    uint16_t timestamp;
    uint16_t temperature;
    uint16_t gyro[3];
    uint16_t accel[3];
    uint8_t pad2[5];
    uint8_t status[2];
    uint8_t pad3;
};
#pragma endregion

///////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////
#pragma region Ds4forPicoW_Variables
struct DualShock4_state latest;
static hid_protocol_mode_t hid_host_report_mode = HID_PROTOCOL_MODE_REPORT;
static bool hid_host_descriptor_available       = false;
static uint16_t hid_host_cid                    = 0;
static uint8_t hid_descriptor_storage[MAX_ATTRIBUTE_VALUE_SIZE];

static bd_addr_t remote_addr;
static bd_addr_t connected_addr;
static btstack_packet_callback_registration_t hci_event_callback_registration;

static char *remote_addr_string;

/////////////////
struct DualShock4_state hid_state;
static bool hid_can_use = false;

int deviceCount = 0;
struct device devices[MAX_DEVICES];
enum STATE state = INIT;
#pragma endregion

///////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Blink Timer
///////////////////////////////////////////////////////
#pragma region Ds4forPicoW_BlinkTimer

#define DS4_FOR_PICO_W_BLINK_MS 250
volatile bool g_flag_blink_led = false;
static btstack_timer_source_t blink_timer;
static void func_blink_handler(btstack_timer_source_t *ts)
{
    static bool on = 0;

    if (hid_host_cid != 0) {
        on = true;
    } else {
        on = !on;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !!on);

    btstack_run_loop_set_timer(&blink_timer, DS4_FOR_PICO_W_BLINK_MS);
    btstack_run_loop_add_timer(&blink_timer);
}
#pragma endregion

///////////////////////////////////////////////////////
// Scan Functions
///////////////////////////////////////////////////////
#pragma region Ds4forPicoW_ScanFunctions
static int func_get_device_index_for_address(bd_addr_t addr)
{
    int j;
    for (j = 0; j < deviceCount; j++) {
        if (bd_addr_cmp(addr, devices[j].address) == 0) {
            return j;
        }
    }
    return -1;
}

static int func_has_more_remote_name_requests(void)
{
    int i;
    for (i = 0; i < deviceCount; i++) {
        if (devices[i].state == REMOTE_NAME_REQUEST)
            return 1;
    }
    return 0;
}

static void func_do_next_remote_name_request(void)
{
    int i;
    for (i = 0; i < deviceCount; i++) {
        // remote name request
        if (devices[i].state == REMOTE_NAME_REQUEST) {
            devices[i].state = REMOTE_NAME_INQUIRED;
#if VERBOSE_DS4_REPORTS
            printf("[--] Get remote name of [%s]\n", bd_addr_to_str(devices[i].address));
#endif
            gap_remote_name_request(devices[i].address, devices[i].pageScanRepetitionMode, devices[i].clockOffset | 0x8000);
            return;
        }
    }
}

static void func_continue_remote_names(void)
{
    if (func_has_more_remote_name_requests()) {
        func_do_next_remote_name_request();
        return;
    }
    gap_inquiry_start(INQUIRY_INTERVAL);
}

char *func_get_mac(uint8_t packet_type, uint8_t *packet, uint8_t event)
{
    bd_addr_t addr;
    int i;
    int index;

    char *mac = (char *)"";

    switch (state) {
        case INIT: {
            switch (event) {
                case BTSTACK_EVENT_STATE:
                    if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                        gap_inquiry_start(INQUIRY_INTERVAL);
                        state = ACTIVE;
                    }
                    break;
                default:
                    break;
            }
        } break;

        case ACTIVE:
            switch (event) {
                case GAP_EVENT_INQUIRY_RESULT: {
                    if (deviceCount >= MAX_DEVICES)
                        break; // already full
                    gap_event_inquiry_result_get_bd_addr(packet, addr);
                    index = func_get_device_index_for_address(addr);
                    if (index >= 0) {
                        break; // already in our list
                    }
                    memcpy(devices[deviceCount].address, addr, 6);
                    devices[deviceCount].pageScanRepetitionMode = gap_event_inquiry_result_get_page_scan_repetition_mode(packet);
                    devices[deviceCount].clockOffset            = gap_event_inquiry_result_get_clock_offset(packet);
                    // print info
                    char *mac_addr = bd_addr_to_str(addr);
#if DEBUG_DS4_REPORTS
                    printf("[%02X] Device found: %s ", GAP_EVENT_INQUIRY_RESULT, mac_addr);
                    printf("with COD[0x%06X], ", (unsigned int)gap_event_inquiry_result_get_class_of_device(packet));
                    printf("pageScan[%d], ", devices[deviceCount].pageScanRepetitionMode);
                    printf("clock offset[0x%04X]", devices[deviceCount].clockOffset);
#endif
                    if (gap_event_inquiry_result_get_rssi_available(packet)) {
#if DEBUG_DS4_REPORTS
                        printf(", rssi %d dBm", (int8_t)gap_event_inquiry_result_get_rssi(packet));
#endif
                    }
                    if (gap_event_inquiry_result_get_name_available(packet)) {
                        char name_buffer[240];
                        int name_len = gap_event_inquiry_result_get_name_len(packet);
                        memcpy(name_buffer, gap_event_inquiry_result_get_name(packet), name_len);
                        name_buffer[name_len] = 0;
#if DEBUG_DS4_REPORTS
                        printf(", name '%s'", name_buffer);
#endif
                        devices[deviceCount].state = REMOTE_NAME_FETCHED;

                        if (strcmp(name_buffer, "Wireless Controller") == 0) {
                            mac = mac_addr;
                        }
                    } else {
                        devices[deviceCount].state = REMOTE_NAME_REQUEST;
                    }
#if DEBUG_DS4_REPORTS
                    printf("\n");
#endif
                    deviceCount++;
                } break;

                case GAP_EVENT_INQUIRY_COMPLETE: {
                    for (i = 0; i < deviceCount; i++) {
                        // retry remote name request
                        if (devices[i].state == REMOTE_NAME_INQUIRED)
                            devices[i].state = REMOTE_NAME_REQUEST;
                    }
                    func_continue_remote_names();
                } break;

                case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE: {
                    reverse_bd_addr(&packet[3], addr);
                    index = func_get_device_index_for_address(addr);
                    if (index >= 0) {
                        if (packet[2] == 0) {
#if INFO_DS4_REPORTS
                            printf("[%02X] Device found: Name: '%s'\n", HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE, &packet[9]);
#endif
                            devices[index].state = REMOTE_NAME_FETCHED;

                            if (strcmp((char const *)&packet[9], "Wireless Controller") == 0) {
                                mac = bd_addr_to_str(addr);
                            }
                        } else {
#if ERROR_DS4_REPORTS
                            printf("[%02X] Failed to get name: page timeout\n", HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE);
#endif
                        }
                    }
                    func_continue_remote_names();
                } break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return mac;
}
#pragma endregion

///////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////

static void func_hid_host_handle_interrupt_report(const uint8_t *packet, uint16_t packet_len)
{
    // Only interested in report_id 0x11
    if (packet_len < sizeof(struct input_report_17) + 1) {
        return;
    }

    if ((packet[0] != 0xa1) || (packet[1] != 0x11)) {
        return;
    }

#if VERBOSE_DS4_REPORTS
    printf_hexdump(packet, packet_len);
#endif

    struct input_report_17 *report = (struct input_report_17 *)&packet[1];

    // Note: This assumes that we're protected by async_context's single-threaded-ness
    // TODO: Parse out battery, touchpad, six axis, timestamp, temperature Sensors will also need calibration
    latest = (struct DualShock4_state){
        .hat         = (uint8_t)(report->buttons[0] & 0xFu),
        .share       = (bool)(report->buttons[1] & 0x10u),
        .options     = (bool)(report->buttons[1] & 0x20u),
        .ps          = (bool)(report->buttons[2] & 0x01u),
        .triangle    = (bool)(report->buttons[0] & 0x80u),
        .square      = (bool)(report->buttons[0] & 0x10u),
        .circle      = (bool)(report->buttons[0] & 0x40u),
        .cross       = (bool)(report->buttons[0] & 0x20u),
        .touch       = (bool)(report->buttons[2] & 0x02u),
        .touch_x     = report->pad2[0], // TODO:
        .touch_y     = report->pad2[1], // TODO:
        .l1          = (bool)(report->buttons[1] & 0x01u),
        .l2          = (bool)(report->buttons[1] & 0x04u),
        .l2_value    = report->l2,
        .r1          = (bool)(report->buttons[1] & 0x02u),
        .r2          = (bool)(report->buttons[1] & 0x08u),
        .r2_value    = report->r2,
        .l3          = (bool)(report->buttons[1] & 0x40u),
        .l3_x        = report->lx,
        .l3_y        = report->ly,
        .r3          = (bool)(report->buttons[1] & 0x80u),
        .r3_x        = report->rx,
        .r3_y        = report->ry,
        .gyro_x      = report->gyro[0],
        .gyro_y      = report->gyro[1],
        .gyro_z      = report->gyro[2],
        .accel_x     = report->accel[0],
        .accel_y     = report->accel[1],
        .accel_z     = report->accel[2],
        .battery     = report->status[2],
        .temperature = report->temperature,
        .timestamp   = report->timestamp,
        .connected   = hid_can_use,
    };
}

bool func_bt_hid_get_latest(struct DualShock4_state *dst)
{
    if (true == hid_can_use) {
        async_context_t *context = cyw43_arch_async_context();
        async_context_acquire_lock_blocking(context);
        memcpy(dst, &latest, sizeof(*dst));
        async_context_release_lock(context);
        return true;
    } else {
        return false;
    }
}

static void func_bt_hid_disconnected(bd_addr_t addr)
{
    hid_host_cid                  = 0;
    hid_host_descriptor_available = false;

    memcpy(&latest, &default_state, sizeof(latest));
}

static void func_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);

    uint8_t event;
    uint8_t hid_event;
    bd_addr_t event_addr;
    uint8_t status;
    uint8_t reason;

    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }

    event = hci_event_packet_get_type(packet);

    if (remote_addr_string[0] == '\0') {
        remote_addr_string = func_get_mac(packet_type, packet, event);
        if (remote_addr_string[0] != '\0') {
            sscanf_bd_addr(remote_addr_string, remote_addr);
            func_bt_hid_disconnected(remote_addr);
            status = hid_host_connect(remote_addr, hid_host_report_mode, &hid_host_cid);
#if ERROR_DS4_REPORTS
            if (status != ERROR_CODE_SUCCESS) {
                printf("[--] hid_host_connect command failed: 0x%02x\n", status);
            }
#endif

            return;
        } else {
            return;
        }
    }

    switch (event) {
        case BTSTACK_EVENT_STATE:
            // On boot, we try a manual connection
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
#if DEBUG_DS4_REPORTS
                printf("[%02X] Starting hid_host_connect (%s)\n", BTSTACK_EVENT_STATE, bd_addr_to_str(remote_addr));
#endif
                status = hid_host_connect(remote_addr, hid_host_report_mode, &hid_host_cid);
#if ERROR_DS4_REPORTS
                if (status != ERROR_CODE_SUCCESS) {
                    printf("[%02X] hid_host_connect command failed: 0x%02x\n", BTSTACK_EVENT_STATE, status);
                }
#endif
            }
            break;
        case HCI_EVENT_CONNECTION_COMPLETE:
            status = hci_event_connection_complete_get_status(packet);
#if DEBUG_DS4_REPORTS
            printf("[%02X] Connection complete: %X\n", HCI_EVENT_CONNECTION_COMPLETE, status);
#endif
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            status = hci_event_disconnection_complete_get_status(packet);
            reason = hci_event_disconnection_complete_get_reason(packet);
#if DEBUG_DS4_REPORTS
            printf("[%02X] Disconnection complete: status: %X, reason: %X\n", HCI_EVENT_DISCONNECTION_COMPLETE, status, reason);
#endif
            hid_can_use = false;
            btstack_run_loop_trigger_exit();
            break;
        case HCI_EVENT_MAX_SLOTS_CHANGED:
            status = hci_event_max_slots_changed_get_lmp_max_slots(packet);
#if DEBUG_DS4_REPORTS
            printf("[%02X] Max slots changed: %X\n", HCI_EVENT_MAX_SLOTS_CHANGED, status);
#endif
            break;
        case HCI_EVENT_PIN_CODE_REQUEST:
#if DEBUG_DS4_REPORTS
            printf("[%02X] Pin code request. Responding '0000'\n", HCI_EVENT_PIN_CODE_REQUEST);
#endif
            hci_event_pin_code_request_get_bd_addr(packet, event_addr);
            gap_pin_code_response(event_addr, "0000");
            break;
        case HCI_EVENT_USER_CONFIRMATION_REQUEST:
#if DEBUG_DS4_REPORTS
            printf("[%02X] SSP User Confirmation Request: %d\n", HCI_EVENT_USER_CONFIRMATION_REQUEST, little_endian_read_32(packet, 8));
#endif
            break;
        case HCI_EVENT_HID_META:
            hid_event = hci_event_hid_meta_get_subevent_code(packet);
            switch (hid_event) {
                case HID_SUBEVENT_INCOMING_CONNECTION:
                    hid_subevent_incoming_connection_get_address(packet, event_addr);
#if DEBUG_DS4_REPORTS
                    printf("[%02X,%02X] Accepting connection from %s\n", HCI_EVENT_HID_META, HID_SUBEVENT_INCOMING_CONNECTION, bd_addr_to_str(event_addr));
#endif
                    hid_host_accept_connection(hid_subevent_incoming_connection_get_hid_cid(packet), hid_host_report_mode);
                    break;
                case HID_SUBEVENT_CONNECTION_OPENED:
                    status = hid_subevent_connection_opened_get_status(packet);
                    hid_subevent_connection_opened_get_bd_addr(packet, event_addr);
                    if (status != ERROR_CODE_SUCCESS) {
#if DEBUG_DS4_REPORTS
                        printf("[%02X,%02X] Connection to %s failed: 0x%02x\n", HCI_EVENT_HID_META, HID_SUBEVENT_CONNECTION_OPENED, bd_addr_to_str(event_addr), status);
#endif
                        func_bt_hid_disconnected(event_addr);
                        return;
                    }
                    hid_host_descriptor_available = false;
                    hid_host_cid                  = hid_subevent_connection_opened_get_hid_cid(packet);
#if INFO_DS4_REPORTS
                    printf("[%02X,%02X] Connected to (%s)\n", HCI_EVENT_HID_META, HID_SUBEVENT_CONNECTION_OPENED, bd_addr_to_str(event_addr));
#endif
                    bd_addr_copy(connected_addr, event_addr);
                    hid_can_use = true;
                    break;
                case HID_SUBEVENT_DESCRIPTOR_AVAILABLE:
                    status = hid_subevent_descriptor_available_get_status(packet);
                    if (status == ERROR_CODE_SUCCESS) {
                        hid_host_descriptor_available = true;

                        uint16_t dlen = hid_descriptor_storage_get_descriptor_len(hid_host_cid);
#if DEBUG_DS4_REPORTS
                        printf("[%02X,%02X] HID descriptor available. Len: %d\n", HCI_EVENT_HID_META, HID_SUBEVENT_DESCRIPTOR_AVAILABLE, dlen);
#endif

                        // Send FEATURE 0x05, to switch the controller to "full" report mode
                        hid_host_send_get_report(hid_host_cid, HID_REPORT_TYPE_FEATURE, 0x05);
                    } else {
#if ERROR_DS4_REPORTS
                        printf("[%02X,%02X] Couldn't process HID Descriptor, status: %d\n", HCI_EVENT_HID_META, HID_SUBEVENT_DESCRIPTOR_AVAILABLE, status);
#endif
                    }
                    break;
                case HID_SUBEVENT_REPORT:
                    if (hid_host_descriptor_available) {
                        func_hid_host_handle_interrupt_report(hid_subevent_report_get_report(packet), hid_subevent_report_get_report_len(packet));
                    } else {
#if ERROR_DS4_REPORTS
                        printf("[%02X,%02X] No hid host descriptor available\n", HCI_EVENT_HID_META, HID_SUBEVENT_REPORT);
                        printf_hexdump(hid_subevent_report_get_report(packet), hid_subevent_report_get_report_len(packet));
#endif
                    }
                    break;
                case HID_SUBEVENT_SET_PROTOCOL_RESPONSE: {
                    status = hid_subevent_set_protocol_response_get_handshake_status(packet);
                    if (status != HID_HANDSHAKE_PARAM_TYPE_SUCCESSFUL) {
#if ERROR_DS4_REPORTS
                        printf("[%02X,%02X] Protocol handshake error: 0x%02x\n", HCI_EVENT_HID_META, HID_SUBEVENT_SET_PROTOCOL_RESPONSE, status);
#endif
                        break;
                    }
                    hid_protocol_mode_t proto = static_cast<hid_protocol_mode_t>(hid_subevent_set_protocol_response_get_protocol_mode(packet));
#if DEBUG_DS4_REPORTS
                    switch (proto) {
                        case HID_PROTOCOL_MODE_BOOT:
                            printf("[%02X,%02X,%02X] Negotiated protocol: BOOT\n", HCI_EVENT_HID_META, HID_SUBEVENT_SET_PROTOCOL_RESPONSE, HID_PROTOCOL_MODE_BOOT);
                            break;
                        case HID_PROTOCOL_MODE_REPORT:
                            printf("[%02X,%02X,%02X] Negotiated protocol: REPORT\n", HCI_EVENT_HID_META, HID_SUBEVENT_SET_PROTOCOL_RESPONSE, HID_PROTOCOL_MODE_REPORT);
                            break;
                        default:
#if ERROR_DS4_REPORTS
                            printf("[%02X,%02X,--] Negotiated unknown protocol: 0x%X\n", HCI_EVENT_HID_META, HID_SUBEVENT_SET_PROTOCOL_RESPONSE, proto);
#endif
                            break;
                    }
#endif
                } break;
                case HID_SUBEVENT_CONNECTION_CLOSED:
#if DEBUG_DS4_REPORTS
                    printf("[%02X,%02X] HID connection closed: %s\n", HCI_EVENT_HID_META, HID_SUBEVENT_CONNECTION_CLOSED, bd_addr_to_str(connected_addr));
#endif
                    func_bt_hid_disconnected(connected_addr);
                    break;
                case HID_SUBEVENT_GET_REPORT_RESPONSE: {
                    status        = hid_subevent_get_report_response_get_handshake_status(packet);
                    uint16_t dlen = hid_subevent_get_report_response_get_report_len(packet);
#if DEBUG_DS4_REPORTS
                    printf("[%02X,%02X] GET_REPORT response. status: %d, len: %d\n", HCI_EVENT_HID_META, HID_SUBEVENT_GET_REPORT_RESPONSE, status, dlen);
#endif
                } break;
                default:
#if ERROR_DS4_REPORTS
                    printf("[%02X,--] Unknown HID subevent: 0x%X\n", HCI_EVENT_HID_META, hid_event);
#endif
                    break;
            }
            break;
        default:
            // printf("Unknown HCI event: 0x%X\n", event);
            break;
    }
}

///////////////////////////////////////////////////////
// Main functions
///////////////////////////////////////////////////////
#pragma region MainFunctions

static void func_hid_host_setup(void)
{
    // Initialize L2CAP
    l2cap_init();

    sdp_init();

    // Initialize HID Host
    hid_host_init(hid_descriptor_storage, sizeof(hid_descriptor_storage));
    hid_host_register_packet_handler(func_packet_handler);

    // Allow sniff mode requests by HID device and support role switch
    gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_SNIFF_MODE | LM_LINK_POLICY_ENABLE_ROLE_SWITCH);

    // try to become master on incoming connections
    hci_set_master_slave_policy(HCI_ROLE_MASTER);

    // register for HCI events
    hci_event_callback_registration.callback = &func_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
}

void func_bt_hid_main()
{
    if (cyw43_arch_init()) {
        return;
    }

    gap_set_security_level(LEVEL_2);

    if (true == g_flag_blink_led) {
        blink_timer.process = &func_blink_handler;
        btstack_run_loop_set_timer(&blink_timer, DS4_FOR_PICO_W_BLINK_MS);
        btstack_run_loop_add_timer(&blink_timer);
    }

    func_hid_host_setup();
    if (remote_addr_string[0] != '\0') {
        sscanf_bd_addr(remote_addr_string, remote_addr);
        func_bt_hid_disconnected(remote_addr);
    }

    hci_power_control(HCI_POWER_ON);

    btstack_run_loop_execute();

    if (true == g_flag_blink_led) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        btstack_run_loop_remove_timer(&blink_timer);
    }
}
#pragma endregion

///////////////////////////////////////////////////////
// Class DS4forPicoW
///////////////////////////////////////////////////////
#pragma region Class_DS4forPicoW

DS4forPicoW::DS4forPicoW()
{
}
DS4forPicoW::~DS4forPicoW()
{
    if (true == this->_flag_setup) {
        multicore_reset_core1();
    }
    this->_flag_setup = false;
}
void DS4forPicoW::setup(bool blink_led)
{
    if (false == this->_flag_setup) {
        g_flag_blink_led = blink_led;
        multicore_launch_core1(func_bt_hid_main);
        this->_flag_setup = true;
    }
}
bool DS4forPicoW::scan(int timeout_ms)
{
    bool can_use = false;
    int times    = timeout_ms / TIMEOUT_SPAN_MS;

    for (int i = 0; i < times; i++) {
        if (true == is_use()) {
            can_use = true;
            break;
        }
        sleep_ms(TIMEOUT_SPAN_MS);
    }

    return can_use;
}

bool DS4forPicoW::is_use()
{
    return func_bt_hid_get_latest(&hid_state);
}

DualShock4_state DS4forPicoW::get_state()
{
    return hid_state;
}

#pragma endregion
