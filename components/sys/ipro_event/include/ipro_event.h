/*
 * SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
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
 *
 * IPRO Event Loop Library
 *
 * A lightweight, FreeRTOS-native event loop framework for embedded systems.
 * Design inspired by ESP-IDF esp_event, reorganized for IPRO SDK.
 *
 * Features:
 *   - Event base + event ID hierarchical event system
 *   - Default loop for simple use cases
 *   - Custom loops for advanced scenarios
 *   - Handler instance tracking for flexible unregistration
 *   - ISR-safe event posting
 *   - Timer/delayed action support
 *   - Thread-safe operation
 */

#ifndef IPRO_EVENT_H
#define IPRO_EVENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ipro_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error codes
 */
#define IPRO_OK                     0
#define IPRO_ERR_NO_MEM            -1
#define IPRO_ERR_INVALID_ARG       -2
#define IPRO_ERR_INVALID_STATE     -3
#define IPRO_ERR_NOT_FOUND         -4
#define IPRO_ERR_TIMEOUT           -5

/**
 * @brief Event loop handle
 */
typedef struct ipro_event_loop *ipro_event_loop_handle_t;

/**
 * @brief Event handler instance handle
 *
 * Used to track handler registrations for selective unregistration.
 */
typedef struct ipro_event_handler_instance *ipro_event_handler_instance_t;

/**
 * @brief Event data passed to handlers
 */
typedef struct {
    ipro_event_base_t event_base;   /**< Event base identifier */
    int32_t event_id;               /**< Event ID within the base */
    void *event_data;               /**< Event-specific data (may be NULL) */
    size_t event_data_size;         /**< Size of event data */
} ipro_event_t;

/**
 * @brief Event handler callback function
 *
 * @param event     Event data structure
 * @param user_data User data passed during registration
 */
typedef void (*ipro_event_handler_t)(const ipro_event_t *event, void *user_data);

/**
 * @brief Delayed action callback function
 *
 * @param arg User argument passed during scheduling
 */
typedef void (*ipro_action_t)(void *arg);

/**
 * @brief Event loop configuration
 */
typedef struct {
    int32_t queue_size;             /**< Event queue size (default: 32) */
    const char *task_name;          /**< Task name (default: "ipro_event") */
    uint32_t task_priority;         /**< Task priority (default: 5) */
    uint32_t task_stack_size;       /**< Task stack size in bytes (default: 2048) */
} ipro_event_loop_config_t;

/**
 * @brief Default event loop configuration initializer
 */
#define IPRO_EVENT_LOOP_CONFIG_DEFAULT() { \
    .queue_size = 32,                      \
    .task_name = "ipro_event",             \
    .task_priority = 5,                    \
    .task_stack_size = 2048,               \
}

/*
 * ============================================================================
 * Default Event Loop API
 * ============================================================================
 * Use these functions for simple applications that only need one event loop.
 */

/**
 * @brief Create the default event loop
 *
 * Creates a system-wide default event loop. Only one default loop can exist.
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_loop_create_default(void);

/**
 * @brief Delete the default event loop
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_loop_delete_default(void);

/**
 * @brief Register an event handler to the default loop
 *
 * @param event_base    Event base to listen for (or IPRO_EVENT_ANY_BASE)
 * @param event_id      Event ID to listen for (or IPRO_EVENT_ANY_ID)
 * @param handler       Handler callback function
 * @param user_data     User data passed to handler
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_handler_register(ipro_event_base_t event_base,
                                int32_t event_id,
                                ipro_event_handler_t handler,
                                void *user_data);

/**
 * @brief Register an event handler with instance tracking
 *
 * @param event_base    Event base to listen for (or IPRO_EVENT_ANY_BASE)
 * @param event_id      Event ID to listen for (or IPRO_EVENT_ANY_ID)
 * @param handler       Handler callback function
 * @param user_data     User data passed to handler
 * @param instance      [out] Handler instance for later unregistration
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_handler_instance_register(ipro_event_base_t event_base,
                                         int32_t event_id,
                                         ipro_event_handler_t handler,
                                         void *user_data,
                                         ipro_event_handler_instance_t *instance);

/**
 * @brief Unregister an event handler from the default loop
 *
 * @param event_base    Event base the handler was registered for
 * @param event_id      Event ID the handler was registered for
 * @param handler       Handler callback function to unregister
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_handler_unregister(ipro_event_base_t event_base,
                                  int32_t event_id,
                                  ipro_event_handler_t handler);

/**
 * @brief Unregister a handler instance from the default loop
 *
 * @param event_base    Event base the handler was registered for
 * @param event_id      Event ID the handler was registered for
 * @param instance      Handler instance to unregister
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_handler_instance_unregister(ipro_event_base_t event_base,
                                           int32_t event_id,
                                           ipro_event_handler_instance_t instance);

/**
 * @brief Post an event to the default loop
 *
 * @param event_base        Event base identifier
 * @param event_id          Event ID
 * @param event_data        Event data (copied internally, can be NULL)
 * @param event_data_size   Size of event data
 * @param timeout_ms        Timeout in milliseconds (0 = no wait, -1 = forever)
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_post(ipro_event_base_t event_base,
                    int32_t event_id,
                    const void *event_data,
                    size_t event_data_size,
                    int32_t timeout_ms);

/**
 * @brief Post an event from ISR context
 *
 * @param event_base        Event base identifier
 * @param event_id          Event ID
 * @param event_data        Event data (max 4 bytes, copied by value)
 * @param event_data_size   Size of event data (max 4)
 * @param task_woken        [out] Set to true if a higher priority task was woken
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_isr_post(ipro_event_base_t event_base,
                        int32_t event_id,
                        const void *event_data,
                        size_t event_data_size,
                        bool *task_woken);

/*
 * ============================================================================
 * Custom Event Loop API
 * ============================================================================
 * Use these functions when you need multiple event loops or custom configuration.
 */

/**
 * @brief Create a custom event loop
 *
 * @param config    Event loop configuration
 * @param loop      [out] Handle to the created event loop
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_loop_create(const ipro_event_loop_config_t *config,
                           ipro_event_loop_handle_t *loop);

/**
 * @brief Delete a custom event loop
 *
 * @param loop      Event loop handle
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_loop_delete(ipro_event_loop_handle_t loop);

/**
 * @brief Run event loop (for loops without dedicated task)
 *
 * Process pending events. Returns when queue is empty or timeout expires.
 *
 * @param loop          Event loop handle
 * @param timeout_ms    Maximum time to run (0 = process once, -1 = block)
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_loop_run(ipro_event_loop_handle_t loop, int32_t timeout_ms);

/**
 * @brief Register handler to a specific event loop
 */
int ipro_event_handler_register_with(ipro_event_loop_handle_t loop,
                                     ipro_event_base_t event_base,
                                     int32_t event_id,
                                     ipro_event_handler_t handler,
                                     void *user_data);

/**
 * @brief Unregister handler from a specific event loop
 */
int ipro_event_handler_unregister_with(ipro_event_loop_handle_t loop,
                                       ipro_event_base_t event_base,
                                       int32_t event_id,
                                       ipro_event_handler_t handler);

/**
 * @brief Post event to a specific event loop
 */
int ipro_event_post_to(ipro_event_loop_handle_t loop,
                       ipro_event_base_t event_base,
                       int32_t event_id,
                       const void *event_data,
                       size_t event_data_size,
                       int32_t timeout_ms);

/*
 * ============================================================================
 * Timer/Delayed Action API
 * ============================================================================
 */

/**
 * @brief Timer handle
 */
typedef struct ipro_event_timer *ipro_event_timer_t;

/**
 * @brief Schedule a delayed action on the default loop
 *
 * @param delay_ms      Delay in milliseconds
 * @param action        Callback function
 * @param arg           Argument passed to callback
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_post_delayed(uint32_t delay_ms, ipro_action_t action, void *arg);

/**
 * @brief Cancel a scheduled delayed action
 *
 * @param action        Callback function to cancel
 * @param arg           Argument that was passed (used for matching)
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_cancel_delayed(ipro_action_t action, void *arg);

/**
 * @brief Create a periodic timer
 *
 * @param period_ms     Timer period in milliseconds
 * @param action        Callback function
 * @param arg           Argument passed to callback
 * @param timer         [out] Timer handle
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_timer_create(uint32_t period_ms,
                            ipro_action_t action,
                            void *arg,
                            ipro_event_timer_t *timer);

/**
 * @brief Start a timer
 *
 * @param timer         Timer handle
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_timer_start(ipro_event_timer_t timer);

/**
 * @brief Stop a timer
 *
 * @param timer         Timer handle
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_timer_stop(ipro_event_timer_t timer);

/**
 * @brief Delete a timer
 *
 * @param timer         Timer handle
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_timer_delete(ipro_event_timer_t timer);

/*
 * ============================================================================
 * Utility API
 * ============================================================================
 */

/**
 * @brief Schedule a function call on the event loop thread
 *
 * Useful for calling functions that must run in the event loop context.
 *
 * @param action        Function to call
 * @param arg           Argument passed to function
 *
 * @return IPRO_OK on success, error code otherwise
 */
int ipro_event_schedule_call(ipro_action_t action, void *arg);

/**
 * @brief Get the default event loop handle
 *
 * @return Default loop handle, or NULL if not created
 */
ipro_event_loop_handle_t ipro_event_loop_get_default(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_EVENT_H */
