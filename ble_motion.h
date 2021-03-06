#ifndef BLE_MOTION_H__
#define BLE_MOTION_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "drv_lis2dh.h"


#define MOTION_SERVICE_UUID_BASE         {0x04, 0x00, 0x13, 0xAC, 0x42, 0x02, 0xDE, 0xB3, \
                                          0xEA, 0x11, 0xB2, 0xAF, 0x26, 0x31, 0xAC, 0x2B}

#define MOTION_SERVICE_UUID               0x1400
#define ACCELERATION_VALUE_CHAR_UUID      0x1401
#define CONFIGURATION_CHAR_UUID           0x1402

#define NRF_BLE_MAX_MTU_SIZE 247

#ifdef __GNUC__
    #ifdef PACKED
        #undef PACKED
    #endif

    #define PACKED(TYPE) TYPE __attribute__ ((packed))
#endif

typedef PACKED( struct
{
    uint8_t                  resolution;
    uint8_t                  frequency;
    uint8_t                  scale;
   
}) ble_motion_config_t;


/**@brief   Macro for defining a ble_cus instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_MOTION_DEF(_name)                                                              \
static ble_motion_t _name;                                                                 \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                        \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                            \
                     ble_motion_on_ble_evt, &_name)                                        

typedef enum
{
    BLE_MOTION_EVT_NOTIFICATION_ENABLED,                             /**< Custom value notification enabled event. */
    BLE_MOTION_EVT_NOTIFICATION_DISABLED,                            /**< Custom value notification disabled event. */
    BLE_MOTION_EVT_CONFIG_RECEIVED,
    BLE_MOTION_EVT_DISCONNECTED,
    BLE_MOTION_EVT_CONNECTED
} ble_motion_evt_type_t;

/**@brief Custom Service event. */
typedef struct
{
    ble_motion_evt_type_t evt_type;
    uint8_t          * p_data;
    uint16_t           length;
} ble_motion_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_motion_s ble_motion_t;

/**@brief Custom Service event handler type. */
typedef void (*ble_motion_evt_handler_t) (ble_motion_t * p_motion, ble_motion_evt_t * p_evt);

/**@brief Custom Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_motion_evt_handler_t      evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    ble_motion_config_t           initial_motion_value;           /**< Initial custom value */
    ble_srv_cccd_security_mode_t  motion_value_char_attr_md;     /**< Initial security level for Custom characteristics attribute */
} ble_motion_init_t;

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_motion_s
{
    ble_motion_evt_handler_t      evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      configuration_handles;          /**< Handles related to the Custom Value characteristic. */
    ble_gatts_char_handles_t      acceleration_value_handles;     /**< Handles related to the Custom Value characteristic. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type;
    bool                          is_acceleration_notif_enabled; 
};

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_motion       Custom Service structure. This structure will have to be supplied by
 *                             the application. It will be initialized by this function, and will later
 *                             be used to identify this particular service instance.
 * @param[in]   p_motion_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_motion_init(ble_motion_t * p_motion, const ble_motion_init_t * p_motion_init);

static uint32_t configuration_char_add(ble_motion_t * p_motion, const ble_motion_init_t * p_motion_init);
static uint32_t acceleration_value_char_add(ble_motion_t * p_motion, const ble_motion_init_t * p_motion_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 * @param[in]   p_context  Custom Service structure.
 */
void ble_motion_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

static void on_connect(ble_motion_t * p_motion, ble_evt_t const * p_ble_evt);
static void on_disconnect(ble_motion_t * p_motion, ble_evt_t const * p_ble_evt);
static void on_write(ble_motion_t * p_motion, ble_evt_t const * p_ble_evt);

uint32_t m_motion_configuration_apply(ble_motion_config_t * p_config);

uint32_t m_motion_configuration_default_init();

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note 
 *       
 * @param[in]   p_cus          Custom Service structure.
 * @param[in]   Custom value 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_motion_acceleration_value_update(ble_motion_t * p_motion, int16_t * acceleration_value);

#endif // BLE_MOTION_H__