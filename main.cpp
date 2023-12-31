// /* mbed Microcontroller Library
//  * Copyright (c) 2017 ARM Limited
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include <stdio.h>

// #include "platform/Callback.h"
// #include "events/EventQueue.h"
// #include "platform/NonCopyable.h"

// #include "ble/BLE.h"
// #include "ble/Gap.h"
// #include "ble/GattClient.h"
// #include "ble/GapAdvertisingParams.h"
// #include "ble/GapAdvertisingData.h"
// #include "ble/GattServer.h"
// #include "BLEProcess.h"

// using mbed::callback;

// /**
//  * A Clock service that demonstrate the GattServer features.
//  *
//  * The clock service host three characteristics that model the current hour,
//  * minute and second of the clock. The value of the second characteristic is
//  * incremented automatically by the system.
//  *
//  * A client can subscribe to updates of the clock characteristics and get
//  * notified when one of the value is changed. Clients can also change value of
//  * the second, minute and hour characteristric.
//  */
// class ClockService {
//     typedef ClockService Self;

// public:
//     ClockService() :
//         _hour_char("485f4145-52b9-4644-af1f-7a6b9322490f", 0),
//         _minute_char("0a924ca7-87cd-4699-a3bd-abdcd9cf126a", 0),
//         _second_char("8dd6a1b7-bc75-4741-8a26-264af75807de", 0),
//         _clock_service(
//             /* uuid */ "51311102-030e-485f-b122-f8f381aa84ed",
//             /* characteristics */ _clock_characteristics,
//             /* numCharacteristics */ sizeof(_clock_characteristics) /
//                                      sizeof(_clock_characteristics[0])
//         ),
//         _server(NULL),
//         _event_queue(NULL)
//     {
//         // update internal pointers (value, descriptors and characteristics array)
//         _clock_characteristics[0] = &_hour_char;
//         _clock_characteristics[1] = &_minute_char;
//         _clock_characteristics[2] = &_second_char;

//         // setup authorization handlers
//         _hour_char.setWriteAuthorizationCallback(this, &Self::authorize_client_write);
//         _minute_char.setWriteAuthorizationCallback(this, &Self::authorize_client_write);
//         _second_char.setWriteAuthorizationCallback(this, &Self::authorize_client_write);
//     }



//     void start(BLE &ble_interface, events::EventQueue &event_queue)
//     {
//          if (_event_queue) {
//             return;
//         }

//         _server = &ble_interface.gattServer();
//         _event_queue = &event_queue;

//         // register the service
//         printf("Adding demo service\r\n");
//         ble_error_t err = _server->addService(_clock_service);

//         if (err) {
//             printf("Error %u during demo service registration.\r\n", err);
//             return;
//         }

//         // read write handler
//         _server->onDataSent(as_cb(&Self::when_data_sent));
//         _server->onDataWritten(as_cb(&Self::when_data_written));
//         _server->onDataRead(as_cb(&Self::when_data_read));

//         // updates subscribtion handlers
//         _server->onUpdatesEnabled(as_cb(&Self::when_update_enabled));
//         _server->onUpdatesDisabled(as_cb(&Self::when_update_disabled));
//         _server->onConfirmationReceived(as_cb(&Self::when_confirmation_received));

//         // print the handles
//         printf("clock service registered\r\n");
//         printf("service handle: %u\r\n", _clock_service.getHandle());
//         printf("\thour characteristic value handle %u\r\n", _hour_char.getValueHandle());
//         printf("\tminute characteristic value handle %u\r\n", _minute_char.getValueHandle());
//         printf("\tsecond characteristic value handle %u\r\n", _second_char.getValueHandle());

//         _event_queue->call_every(1000 /* ms */, callback(this, &Self::increment_second));
//     }

// private:

//     /**
//      * Handler called when a notification or an indication has been sent.
//      */
//     void when_data_sent(unsigned count)
//     {
//         printf("sent %u updates\r\n", count);
//     }

//     /**
//      * Handler called after an attribute has been written.
//      */
//     void when_data_written(const GattWriteCallbackParams *e)
//     {
//         printf("data written:\r\n");
//         printf("\tconnection handle: %u\r\n", e->connHandle);
//         printf("\tattribute handle: %u", e->handle);
//         if (e->handle == _hour_char.getValueHandle()) {
//             printf(" (hour characteristic)\r\n");
//         } else if (e->handle == _minute_char.getValueHandle()) {
//             printf(" (minute characteristic)\r\n");
//         } else if (e->handle == _second_char.getValueHandle()) {
//             printf(" (second characteristic)\r\n");
//         } else {
//             printf("\r\n");
//         }
//         printf("\twrite operation: %u\r\n", e->writeOp);
//         printf("\toffset: %u\r\n", e->offset);
//         printf("\tlength: %u\r\n", e->len);
//         printf("\t data: ");

//         for (size_t i = 0; i < e->len; ++i) {
//             printf("%02X", e->data[i]);
//         }

//         printf("\r\n");
//     }

//     /**
//      * Handler called after an attribute has been read.
//      */
//     void when_data_read(const GattReadCallbackParams *e)
//     {
//         printf("data read:\r\n");
//         printf("\tconnection handle: %u\r\n", e->connHandle);
//         printf("\tattribute handle: %u", e->handle);
//         if (e->handle == _hour_char.getValueHandle()) {
//             printf(" (hour characteristic)\r\n");
//         } else if (e->handle == _minute_char.getValueHandle()) {
//             printf(" (minute characteristic)\r\n");
//         } else if (e->handle == _second_char.getValueHandle()) {
//             printf(" (second characteristic)\r\n");
//         } else {
//             printf("\r\n");
//         }
//     }

//     /**
//      * Handler called after a client has subscribed to notification or indication.
//      *
//      * @param handle Handle of the characteristic value affected by the change.
//      */
//     void when_update_enabled(GattAttribute::Handle_t handle)
//     {
//         printf("update enabled on handle %d\r\n", handle);
//     }

//     /**
//      * Handler called after a client has cancelled his subscription from
//      * notification or indication.
//      *
//      * @param handle Handle of the characteristic value affected by the change.
//      */
//     void when_update_disabled(GattAttribute::Handle_t handle)
//     {
//         printf("update disabled on handle %d\r\n", handle);
//     }

//     /**
//      * Handler called when an indication confirmation has been received.
//      *
//      * @param handle Handle of the characteristic value that has emitted the
//      * indication.
//      */
//     void when_confirmation_received(GattAttribute::Handle_t handle)
//     {
//         printf("confirmation received on handle %d\r\n", handle);
//     }

//     /**
//      * Handler called when a write request is received.
//      *
//      * This handler verify that the value submitted by the client is valid before
//      * authorizing the operation.
//      */
//     void authorize_client_write(GattWriteAuthCallbackParams *e)
//     {
//         printf("characteristic %u write authorization\r\n", e->handle);

//         if (e->offset != 0) {
//             printf("Error invalid offset\r\n");
//             e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_OFFSET;
//             return;
//         }

//         if (e->len != 1) {
//             printf("Error invalid len\r\n");
//             e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_ATT_VAL_LENGTH;
//             return;
//         }

//         if ((e->data[0] >= 60) ||
//             ((e->data[0] >= 24) && (e->handle == _hour_char.getValueHandle()))) {
//             printf("Error invalid data\r\n");
//             e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_WRITE_NOT_PERMITTED;
//             return;
//         }

//         e->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
//     }

//     /**
//      * Increment the second counter.
//      */
//     void increment_second(void)
//     {
//         uint8_t second = 0;
//         ble_error_t err = _second_char.get(*_server, second);
//         if (err) {
//             printf("read of the second value returned error %u\r\n", err);
//             return;
//         }

//         second = (second + 1) % 60;

//         err = _second_char.set(*_server, second);
//         if (err) {
//             printf("write of the second value returned error %u\r\n", err);
//             return;
//         }

//         if (second == 0) {
//             increment_minute();
//         }
//     }

//     /**
//      * Increment the minute counter.
//      */
//     void increment_minute(void)
//     {
//         uint8_t minute = 0;
//         ble_error_t err = _minute_char.get(*_server, minute);
//         if (err) {
//             printf("read of the minute value returned error %u\r\n", err);
//             return;
//         }

//         minute = (minute + 1) % 60;

//         err = _minute_char.set(*_server, minute);
//         if (err) {
//             printf("write of the minute value returned error %u\r\n", err);
//             return;
//         }

//         if (minute == 0) {
//             increment_hour();
//         }
//     }

//     /**
//      * Increment the hour counter.
//      */
//     void increment_hour(void)
//     {
//         uint8_t hour = 0;
//         ble_error_t err = _hour_char.get(*_server, hour);
//         if (err) {
//             printf("read of the hour value returned error %u\r\n", err);
//             return;
//         }

//         hour = (hour + 1) % 24;

//         err = _hour_char.set(*_server, hour);
//         if (err) {
//             printf("write of the hour value returned error %u\r\n", err);
//             return;
//         }
//     }

// private:
//     /**
//      * Helper that construct an event handler from a member function of this
//      * instance.
//      */
//     template<typename Arg>
//     FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg))
//     {
//         return makeFunctionPointer(this, member);
//     }

//     /**
//      * Read, Write, Notify, Indicate  Characteristic declaration helper.
//      *
//      * @tparam T type of data held by the characteristic.
//      */
//     template<typename T>
//     class ReadWriteNotifyIndicateCharacteristic : public GattCharacteristic {
//     public:
//         /**
//          * Construct a characteristic that can be read or written and emit
//          * notification or indication.
//          *
//          * @param[in] uuid The UUID of the characteristic.
//          * @param[in] initial_value Initial value contained by the characteristic.
//          */
//         ReadWriteNotifyIndicateCharacteristic(const UUID & uuid, const T& initial_value) :
//             GattCharacteristic(
//                 /* UUID */ uuid,
//                 /* Initial value */ &_value,
//                 /* Value size */ sizeof(_value),
//                 /* Value capacity */ sizeof(_value),
//                 /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
//                                 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
//                                 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
//                                 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
//                 /* Descriptors */ NULL,
//                 /* Num descriptors */ 0,
//                 /* variable len */ false
//             ),
//             _value(initial_value) {
//         }

//         /**
//          * Get the value of this characteristic.
//          *
//          * @param[in] server GattServer instance that contain the characteristic
//          * value.
//          * @param[in] dst Variable that will receive the characteristic value.
//          *
//          * @return BLE_ERROR_NONE in case of success or an appropriate error code.
//          */
//         ble_error_t get(GattServer &server, T& dst) const
//         {
//             uint16_t value_length = sizeof(dst);
//             return server.read(getValueHandle(), &dst, &value_length);
//         }

//         /**
//          * Assign a new value to this characteristic.
//          *
//          * @param[in] server GattServer instance that will receive the new value.
//          * @param[in] value The new value to set.
//          * @param[in] local_only Flag that determine if the change should be kept
//          * locally or forwarded to subscribed clients.
//          */
//         ble_error_t set(
//             GattServer &server, const uint8_t &value, bool local_only = false
//         ) const {
//             return server.write(getValueHandle(), &value, sizeof(value), local_only);
//         }

//     private:
//         uint8_t _value;
//     };

//     ReadWriteNotifyIndicateCharacteristic<uint8_t> _hour_char;
//     ReadWriteNotifyIndicateCharacteristic<uint8_t> _minute_char;
//     ReadWriteNotifyIndicateCharacteristic<uint8_t> _second_char;

//     // list of the characteristics of the clock service
//     GattCharacteristic* _clock_characteristics[3];

//     // demo service
//     GattService _clock_service;

//     GattServer* _server;
//     events::EventQueue *_event_queue;
// };

// int main() {
//     BLE &ble_interface = BLE::Instance();
//     events::EventQueue event_queue;
//     ClockService demo_service;
//     BLEProcess ble_process(event_queue, ble_interface);

//     ble_process.on_init(callback(&demo_service, &ClockService::start));

//     // bind the event queue to the ble interface, initialize the interface
//     // and start advertising
//     ble_process.start();

//     // Process the event queue.
//     event_queue.dispatch_forever();

//     return 0;
// }



/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"
#include "pretty_printer.h"
#include "mbed-trace/mbed_trace.h"

using namespace std::literals::chrono_literals;

const static char DEVICE_NAME[] = "r11921009";

static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);

class HeartrateDemo : ble::Gap::EventHandler {
public:
    HeartrateDemo(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
        _heartrate_uuid(GattService::UUID_HEART_RATE_SERVICE),
        _heartrate_value(10000),
        _heartrate_service(ble, _heartrate_value, HeartRateService::LOCATION_FINGER),
        _adv_data_builder(_adv_buffer)
    {
    }
        void onDataWrittenCallback(const GattWriteCallbackParams *params) {
        printf("data written");
}
    void start()
    {

        _ble.init(this, &HeartrateDemo::on_init_complete);



        printf("Start sensor init\n");

        BSP_TSENSOR_Init();
        BSP_HSENSOR_Init();
        BSP_PSENSOR_Init();

        BSP_MAGNETO_Init();
        BSP_GYRO_Init();
        BSP_ACCELERO_Init();
        _event_queue.dispatch_forever();
        // _ble.gattServer().onDataRead (makeFunctionPointer(this,&HeartrateDemo::onDataWrittenCallback));

    }

// private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params)
    {
        if (params->error != BLE_ERROR_NONE) {
            printf("Ble initialization failed.");
            return;
        }

        print_mac_address();

        /* this allows us to receive events like onConnectionComplete() */
        _ble.gap().setEventHandler(this);

        /* heart rate value updated every second */
        _event_queue.call_every(
            1000ms,
            [this] {
                update_sensor_value();
            }
        
        );
        // _ble.gattServer().onUpdatesEnabled(
        // makeFunctionPointer(this, &BatteryDemo::when_update_enabled));

        start_advertising();
    }

    void start_advertising()
    {
        /* Create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(100))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setAppearance(ble::adv_data_appearance_t::GENERIC_HEART_RATE_SENSOR);
        _adv_data_builder.setLocalServiceList({&_heartrate_uuid, 1});
        _adv_data_builder.setName(DEVICE_NAME);

        /* Setup advertising */

        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );

        if (error) {
            printf("_ble.gap().setAdvertisingParameters() failed\r\n");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            printf("_ble.gap().setAdvertisingPayload() failed\r\n");
            return;
        }

        /* Start advertising */

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            printf("_ble.gap().startAdvertising() failed\r\n");
            return;
        }

        printf("Heart rate sensor advertising, please connect\r\n");
    }

    void update_sensor_value()
    {
        /* you can read in the real value but here we just simulate a value */
        _heartrate_value++;
        float sensor_value = 0;
        int16_t pDataXYZ[3] = {0};
        float pGyroDataXYZ[3] = {0};
        BSP_MAGNETO_GetXYZ(pDataXYZ);

        // /*  60 <= bpm value < 110 */
        // if (_heartrate_value == 110) {
        //     _heartrate_value = 60;
        // }
        _heartrate_value=pDataXYZ[0];
        printf("%d,%d,%d\n",pDataXYZ[0],pDataXYZ[1],pDataXYZ[2]);
        _heartrate_service.updateHeartRate(pDataXYZ);
        // printf("%p",pDataXYZ);
        // _ble.gattServer().onDataWritten()
    }

    /* these implement ble::Gap::EventHandler */
// private:
 
    /* when we connect we stop advertising, restart advertising so others can connect */
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event)
    {
        if (event.getStatus() == ble_error_t::BLE_ERROR_NONE) {
            printf("Client connected, you may now subscribe to updates\r\n");
        }
    }

    /* when we connect we stop advertising, restart advertising so others can connect */
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event)
    {
        printf("Client disconnected, restarting advertising\r\n");

        ble_error_t error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            printf("_ble.gap().startAdvertising() failed\r\n");
            return;
        }
    }

// private:
    BLE &_ble;
    events::EventQueue &_event_queue;

    UUID _heartrate_uuid;
    uint16_t _heartrate_value;
    HeartRateService _heartrate_service;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

/* Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context)
{
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main()
{
    mbed_trace_init();
    printf("66666");

    BLE &ble = BLE::Instance();
    printf("33333");
    ble.onEventsToProcess(schedule_ble_events);
    printf("HeartrateDemo demo(ble, event_queue);");

    HeartrateDemo demo(ble, event_queue);
   
    printf(" demo.start();");
    demo.start();

    return 0;
}
