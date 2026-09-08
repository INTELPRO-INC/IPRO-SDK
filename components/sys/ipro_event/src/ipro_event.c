/*
 * SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO Event Loop Library - Core Implementation
 */

#include <string.h>
#include <stdlib.h>
#include "ipro_event.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*
 * Define event bases
 */
IPRO_EVENT_DEFINE_BASE(IPRO_SYSTEM_EVENT);
IPRO_EVENT_DEFINE_BASE(IPRO_WIFI_EVENT);
IPRO_EVENT_DEFINE_BASE(IPRO_BLE_EVENT);
IPRO_EVENT_DEFINE_BASE(IPRO_USER_EVENT);

/*
 * Internal structures
 */

/* Event handler node */
typedef struct ipro_event_handler_node {
    struct ipro_event_handler_node *next;
    ipro_event_base_t event_base;
    int32_t event_id;
    ipro_event_handler_t handler;
    void *user_data;
} ipro_event_handler_node_t;

/* Handler instance structure */
struct ipro_event_handler_instance {
    ipro_event_handler_node_t *node;
};

/* Internal event message */
typedef struct {
    ipro_event_base_t event_base;
    int32_t event_id;
    size_t data_size;
    uint8_t data[];  /* Flexible array member */
} ipro_event_msg_t;

/* Delayed action node */
typedef struct ipro_delayed_action {
    struct ipro_delayed_action *next;
    ipro_action_t action;
    void *arg;
    TimerHandle_t timer;
} ipro_delayed_action_t;

/* Timer structure */
struct ipro_event_timer {
    TimerHandle_t timer;
    ipro_action_t action;
    void *arg;
};

/* Event loop structure */
struct ipro_event_loop {
    QueueHandle_t queue;
    TaskHandle_t task;
    SemaphoreHandle_t mutex;
    ipro_event_handler_node_t *handlers;
    ipro_delayed_action_t *delayed_actions;
    bool running;
    bool has_task;
    ipro_event_loop_config_t config;
};

/* Default event loop */
static ipro_event_loop_handle_t s_default_loop = NULL;

/*
 * Internal functions
 */

static void event_loop_task(void *arg)
{
    ipro_event_loop_handle_t loop = (ipro_event_loop_handle_t)arg;
    ipro_event_msg_t *msg;

    loop->running = true;

    while (loop->running) {
        if (xQueueReceive(loop->queue, &msg, portMAX_DELAY) == pdTRUE) {
            if (msg == NULL) {
                /* Shutdown signal */
                break;
            }

            /* Prepare event data */
            ipro_event_t event = {
                .event_base = msg->event_base,
                .event_id = msg->event_id,
                .event_data = msg->data_size > 0 ? msg->data : NULL,
                .event_data_size = msg->data_size,
            };

            /* Dispatch to handlers */
            xSemaphoreTake(loop->mutex, portMAX_DELAY);
            ipro_event_handler_node_t *node = loop->handlers;
            while (node != NULL) {
                bool base_match = (node->event_base == IPRO_EVENT_ANY_BASE) ||
                                  (node->event_base == msg->event_base);
                bool id_match = (node->event_id == IPRO_EVENT_ANY_ID) ||
                                (node->event_id == msg->event_id);

                if (base_match && id_match) {
                    xSemaphoreGive(loop->mutex);
                    node->handler(&event, node->user_data);
                    xSemaphoreTake(loop->mutex, portMAX_DELAY);
                }
                node = node->next;
            }
            xSemaphoreGive(loop->mutex);

            /* Free message */
            vPortFree(msg);
        }
    }

    vTaskDelete(NULL);
}

static int post_event_internal(ipro_event_loop_handle_t loop,
                               ipro_event_base_t event_base,
                               int32_t event_id,
                               const void *event_data,
                               size_t event_data_size,
                               TickType_t timeout)
{
    if (loop == NULL || loop->queue == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    /* Allocate message with data */
    size_t msg_size = sizeof(ipro_event_msg_t) + event_data_size;
    ipro_event_msg_t *msg = pvPortMalloc(msg_size);
    if (msg == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    msg->event_base = event_base;
    msg->event_id = event_id;
    msg->data_size = event_data_size;
    if (event_data_size > 0 && event_data != NULL) {
        memcpy(msg->data, event_data, event_data_size);
    }

    /* Send to queue */
    if (xQueueSend(loop->queue, &msg, timeout) != pdTRUE) {
        vPortFree(msg);
        return IPRO_ERR_TIMEOUT;
    }

    return IPRO_OK;
}

static int register_handler_internal(ipro_event_loop_handle_t loop,
                                     ipro_event_base_t event_base,
                                     int32_t event_id,
                                     ipro_event_handler_t handler,
                                     void *user_data,
                                     ipro_event_handler_instance_t *instance)
{
    if (loop == NULL || handler == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    /* Allocate handler node */
    ipro_event_handler_node_t *node = pvPortMalloc(sizeof(ipro_event_handler_node_t));
    if (node == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    node->event_base = event_base;
    node->event_id = event_id;
    node->handler = handler;
    node->user_data = user_data;
    node->next = NULL;

    /* Add to list */
    xSemaphoreTake(loop->mutex, portMAX_DELAY);
    node->next = loop->handlers;
    loop->handlers = node;
    xSemaphoreGive(loop->mutex);

    /* Return instance if requested */
    if (instance != NULL) {
        ipro_event_handler_instance_t inst = pvPortMalloc(sizeof(struct ipro_event_handler_instance));
        if (inst == NULL) {
            return IPRO_ERR_NO_MEM;
        }
        inst->node = node;
        *instance = inst;
    }

    return IPRO_OK;
}

static int unregister_handler_internal(ipro_event_loop_handle_t loop,
                                       ipro_event_base_t event_base,
                                       int32_t event_id,
                                       ipro_event_handler_t handler,
                                       ipro_event_handler_node_t *target_node)
{
    if (loop == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    xSemaphoreTake(loop->mutex, portMAX_DELAY);

    ipro_event_handler_node_t *prev = NULL;
    ipro_event_handler_node_t *node = loop->handlers;

    while (node != NULL) {
        bool match = false;

        if (target_node != NULL) {
            match = (node == target_node);
        } else {
            match = (node->event_base == event_base) &&
                    (node->event_id == event_id) &&
                    (node->handler == handler);
        }

        if (match) {
            if (prev == NULL) {
                loop->handlers = node->next;
            } else {
                prev->next = node->next;
            }
            xSemaphoreGive(loop->mutex);
            vPortFree(node);
            return IPRO_OK;
        }

        prev = node;
        node = node->next;
    }

    xSemaphoreGive(loop->mutex);
    return IPRO_ERR_NOT_FOUND;
}

/*
 * Public API - Event Loop Lifecycle
 */

int ipro_event_loop_create(const ipro_event_loop_config_t *config,
                           ipro_event_loop_handle_t *loop)
{
    if (config == NULL || loop == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    /* Allocate loop structure */
    ipro_event_loop_handle_t new_loop = pvPortMalloc(sizeof(struct ipro_event_loop));
    if (new_loop == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    memset(new_loop, 0, sizeof(struct ipro_event_loop));
    memcpy(&new_loop->config, config, sizeof(ipro_event_loop_config_t));

    /* Create queue */
    new_loop->queue = xQueueCreate(config->queue_size, sizeof(ipro_event_msg_t *));
    if (new_loop->queue == NULL) {
        vPortFree(new_loop);
        return IPRO_ERR_NO_MEM;
    }

    /* Create mutex */
    new_loop->mutex = xSemaphoreCreateMutex();
    if (new_loop->mutex == NULL) {
        vQueueDelete(new_loop->queue);
        vPortFree(new_loop);
        return IPRO_ERR_NO_MEM;
    }

    /* Create task if name is specified */
    if (config->task_name != NULL && config->task_stack_size > 0) {
        BaseType_t ret = xTaskCreate(event_loop_task,
                                     config->task_name,
                                     config->task_stack_size / sizeof(StackType_t),
                                     new_loop,
                                     config->task_priority,
                                     &new_loop->task);
        if (ret != pdPASS) {
            vSemaphoreDelete(new_loop->mutex);
            vQueueDelete(new_loop->queue);
            vPortFree(new_loop);
            return IPRO_ERR_NO_MEM;
        }
        new_loop->has_task = true;
    }

    *loop = new_loop;
    return IPRO_OK;
}

int ipro_event_loop_delete(ipro_event_loop_handle_t loop)
{
    if (loop == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    /* Signal shutdown */
    loop->running = false;
    if (loop->has_task) {
        ipro_event_msg_t *null_msg = NULL;
        xQueueSend(loop->queue, &null_msg, portMAX_DELAY);
        /* Wait for task to finish */
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /* Free handlers */
    xSemaphoreTake(loop->mutex, portMAX_DELAY);
    ipro_event_handler_node_t *node = loop->handlers;
    while (node != NULL) {
        ipro_event_handler_node_t *next = node->next;
        vPortFree(node);
        node = next;
    }
    xSemaphoreGive(loop->mutex);

    /* Free delayed actions */
    ipro_delayed_action_t *action = loop->delayed_actions;
    while (action != NULL) {
        ipro_delayed_action_t *next = action->next;
        if (action->timer != NULL) {
            xTimerDelete(action->timer, portMAX_DELAY);
        }
        vPortFree(action);
        action = next;
    }

    /* Drain queue */
    ipro_event_msg_t *msg;
    while (xQueueReceive(loop->queue, &msg, 0) == pdTRUE) {
        if (msg != NULL) {
            vPortFree(msg);
        }
    }

    /* Delete resources */
    vSemaphoreDelete(loop->mutex);
    vQueueDelete(loop->queue);
    vPortFree(loop);

    return IPRO_OK;
}

int ipro_event_loop_run(ipro_event_loop_handle_t loop, int32_t timeout_ms)
{
    if (loop == NULL || loop->has_task) {
        return IPRO_ERR_INVALID_ARG;
    }

    TickType_t timeout = (timeout_ms < 0) ? portMAX_DELAY :
                         pdMS_TO_TICKS(timeout_ms);

    ipro_event_msg_t *msg;
    if (xQueueReceive(loop->queue, &msg, timeout) == pdTRUE) {
        if (msg != NULL) {
            ipro_event_t event = {
                .event_base = msg->event_base,
                .event_id = msg->event_id,
                .event_data = msg->data_size > 0 ? msg->data : NULL,
                .event_data_size = msg->data_size,
            };

            xSemaphoreTake(loop->mutex, portMAX_DELAY);
            ipro_event_handler_node_t *node = loop->handlers;
            while (node != NULL) {
                bool base_match = (node->event_base == IPRO_EVENT_ANY_BASE) ||
                                  (node->event_base == msg->event_base);
                bool id_match = (node->event_id == IPRO_EVENT_ANY_ID) ||
                                (node->event_id == msg->event_id);

                if (base_match && id_match) {
                    xSemaphoreGive(loop->mutex);
                    node->handler(&event, node->user_data);
                    xSemaphoreTake(loop->mutex, portMAX_DELAY);
                }
                node = node->next;
            }
            xSemaphoreGive(loop->mutex);

            vPortFree(msg);
        }
    }

    return IPRO_OK;
}

/*
 * Default Loop API
 */

int ipro_event_loop_create_default(void)
{
    if (s_default_loop != NULL) {
        return IPRO_ERR_INVALID_STATE;
    }

    ipro_event_loop_config_t config = IPRO_EVENT_LOOP_CONFIG_DEFAULT();
    return ipro_event_loop_create(&config, &s_default_loop);
}

int ipro_event_loop_delete_default(void)
{
    if (s_default_loop == NULL) {
        return IPRO_ERR_INVALID_STATE;
    }

    int ret = ipro_event_loop_delete(s_default_loop);
    if (ret == IPRO_OK) {
        s_default_loop = NULL;
    }
    return ret;
}

ipro_event_loop_handle_t ipro_event_loop_get_default(void)
{
    return s_default_loop;
}

/*
 * Handler Registration API
 */

int ipro_event_handler_register(ipro_event_base_t event_base,
                                int32_t event_id,
                                ipro_event_handler_t handler,
                                void *user_data)
{
    return register_handler_internal(s_default_loop, event_base, event_id,
                                     handler, user_data, NULL);
}

int ipro_event_handler_instance_register(ipro_event_base_t event_base,
                                         int32_t event_id,
                                         ipro_event_handler_t handler,
                                         void *user_data,
                                         ipro_event_handler_instance_t *instance)
{
    return register_handler_internal(s_default_loop, event_base, event_id,
                                     handler, user_data, instance);
}

int ipro_event_handler_unregister(ipro_event_base_t event_base,
                                  int32_t event_id,
                                  ipro_event_handler_t handler)
{
    return unregister_handler_internal(s_default_loop, event_base, event_id,
                                       handler, NULL);
}

int ipro_event_handler_instance_unregister(ipro_event_base_t event_base,
                                           int32_t event_id,
                                           ipro_event_handler_instance_t instance)
{
    if (instance == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }
    int ret = unregister_handler_internal(s_default_loop, event_base, event_id,
                                          NULL, instance->node);
    if (ret == IPRO_OK) {
        vPortFree(instance);
    }
    return ret;
}

int ipro_event_handler_register_with(ipro_event_loop_handle_t loop,
                                     ipro_event_base_t event_base,
                                     int32_t event_id,
                                     ipro_event_handler_t handler,
                                     void *user_data)
{
    return register_handler_internal(loop, event_base, event_id,
                                     handler, user_data, NULL);
}

int ipro_event_handler_unregister_with(ipro_event_loop_handle_t loop,
                                       ipro_event_base_t event_base,
                                       int32_t event_id,
                                       ipro_event_handler_t handler)
{
    return unregister_handler_internal(loop, event_base, event_id,
                                       handler, NULL);
}

/*
 * Event Posting API
 */

int ipro_event_post(ipro_event_base_t event_base,
                    int32_t event_id,
                    const void *event_data,
                    size_t event_data_size,
                    int32_t timeout_ms)
{
    TickType_t timeout = (timeout_ms < 0) ? portMAX_DELAY :
                         pdMS_TO_TICKS(timeout_ms);
    return post_event_internal(s_default_loop, event_base, event_id,
                               event_data, event_data_size, timeout);
}

int ipro_event_post_to(ipro_event_loop_handle_t loop,
                       ipro_event_base_t event_base,
                       int32_t event_id,
                       const void *event_data,
                       size_t event_data_size,
                       int32_t timeout_ms)
{
    TickType_t timeout = (timeout_ms < 0) ? portMAX_DELAY :
                         pdMS_TO_TICKS(timeout_ms);
    return post_event_internal(loop, event_base, event_id,
                               event_data, event_data_size, timeout);
}

int ipro_event_isr_post(ipro_event_base_t event_base,
                        int32_t event_id,
                        const void *event_data,
                        size_t event_data_size,
                        bool *task_woken)
{
    if (s_default_loop == NULL || s_default_loop->queue == NULL) {
        return IPRO_ERR_INVALID_STATE;
    }

    if (event_data_size > sizeof(uint32_t)) {
        return IPRO_ERR_INVALID_ARG;
    }

    /* For ISR, we use a small fixed-size message */
    size_t msg_size = sizeof(ipro_event_msg_t) + sizeof(uint32_t);
    ipro_event_msg_t *msg = pvPortMalloc(msg_size);
    if (msg == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    msg->event_base = event_base;
    msg->event_id = event_id;
    msg->data_size = event_data_size;
    if (event_data_size > 0 && event_data != NULL) {
        memcpy(msg->data, event_data, event_data_size);
    }

    BaseType_t higher_prio_woken = pdFALSE;
    if (xQueueSendFromISR(s_default_loop->queue, &msg, &higher_prio_woken) != pdTRUE) {
        vPortFree(msg);
        return IPRO_ERR_TIMEOUT;
    }

    if (task_woken != NULL) {
        *task_woken = (higher_prio_woken == pdTRUE);
    }

    return IPRO_OK;
}

/*
 * Delayed Action API
 */

static void delayed_action_callback(TimerHandle_t timer)
{
    ipro_delayed_action_t *action = (ipro_delayed_action_t *)pvTimerGetTimerID(timer);
    if (action != NULL && action->action != NULL) {
        action->action(action->arg);
    }

    /* Remove from list and cleanup */
    if (s_default_loop != NULL) {
        xSemaphoreTake(s_default_loop->mutex, portMAX_DELAY);
        ipro_delayed_action_t *prev = NULL;
        ipro_delayed_action_t *node = s_default_loop->delayed_actions;
        while (node != NULL) {
            if (node == action) {
                if (prev == NULL) {
                    s_default_loop->delayed_actions = node->next;
                } else {
                    prev->next = node->next;
                }
                break;
            }
            prev = node;
            node = node->next;
        }
        xSemaphoreGive(s_default_loop->mutex);
    }

    xTimerDelete(timer, 0);
    vPortFree(action);
}

int ipro_event_post_delayed(uint32_t delay_ms, ipro_action_t action, void *arg)
{
    if (s_default_loop == NULL || action == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    ipro_delayed_action_t *delayed = pvPortMalloc(sizeof(ipro_delayed_action_t));
    if (delayed == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    delayed->action = action;
    delayed->arg = arg;
    delayed->next = NULL;

    delayed->timer = xTimerCreate("delayed",
                                  pdMS_TO_TICKS(delay_ms),
                                  pdFALSE,  /* One-shot */
                                  delayed,
                                  delayed_action_callback);
    if (delayed->timer == NULL) {
        vPortFree(delayed);
        return IPRO_ERR_NO_MEM;
    }

    /* Add to list */
    xSemaphoreTake(s_default_loop->mutex, portMAX_DELAY);
    delayed->next = s_default_loop->delayed_actions;
    s_default_loop->delayed_actions = delayed;
    xSemaphoreGive(s_default_loop->mutex);

    if (xTimerStart(delayed->timer, portMAX_DELAY) != pdPASS) {
        xSemaphoreTake(s_default_loop->mutex, portMAX_DELAY);
        s_default_loop->delayed_actions = delayed->next;
        xSemaphoreGive(s_default_loop->mutex);
        xTimerDelete(delayed->timer, 0);
        vPortFree(delayed);
        return IPRO_ERR_INVALID_STATE;
    }

    return IPRO_OK;
}

int ipro_event_cancel_delayed(ipro_action_t action, void *arg)
{
    if (s_default_loop == NULL) {
        return IPRO_ERR_INVALID_STATE;
    }

    xSemaphoreTake(s_default_loop->mutex, portMAX_DELAY);
    ipro_delayed_action_t *prev = NULL;
    ipro_delayed_action_t *node = s_default_loop->delayed_actions;
    while (node != NULL) {
        if (node->action == action && node->arg == arg) {
            if (prev == NULL) {
                s_default_loop->delayed_actions = node->next;
            } else {
                prev->next = node->next;
            }
            xSemaphoreGive(s_default_loop->mutex);

            xTimerStop(node->timer, portMAX_DELAY);
            xTimerDelete(node->timer, portMAX_DELAY);
            vPortFree(node);
            return IPRO_OK;
        }
        prev = node;
        node = node->next;
    }
    xSemaphoreGive(s_default_loop->mutex);

    return IPRO_ERR_NOT_FOUND;
}

/*
 * Timer API
 */

static void timer_callback(TimerHandle_t timer)
{
    ipro_event_timer_t evt_timer = (ipro_event_timer_t)pvTimerGetTimerID(timer);
    if (evt_timer != NULL && evt_timer->action != NULL) {
        evt_timer->action(evt_timer->arg);
    }
}

int ipro_event_timer_create(uint32_t period_ms,
                            ipro_action_t action,
                            void *arg,
                            ipro_event_timer_t *timer)
{
    if (action == NULL || timer == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    ipro_event_timer_t evt_timer = pvPortMalloc(sizeof(struct ipro_event_timer));
    if (evt_timer == NULL) {
        return IPRO_ERR_NO_MEM;
    }

    evt_timer->action = action;
    evt_timer->arg = arg;

    evt_timer->timer = xTimerCreate("evt_timer",
                                    pdMS_TO_TICKS(period_ms),
                                    pdTRUE,  /* Auto-reload */
                                    evt_timer,
                                    timer_callback);
    if (evt_timer->timer == NULL) {
        vPortFree(evt_timer);
        return IPRO_ERR_NO_MEM;
    }

    *timer = evt_timer;
    return IPRO_OK;
}

int ipro_event_timer_start(ipro_event_timer_t timer)
{
    if (timer == NULL || timer->timer == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    if (xTimerStart(timer->timer, portMAX_DELAY) != pdPASS) {
        return IPRO_ERR_INVALID_STATE;
    }

    return IPRO_OK;
}

int ipro_event_timer_stop(ipro_event_timer_t timer)
{
    if (timer == NULL || timer->timer == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    if (xTimerStop(timer->timer, portMAX_DELAY) != pdPASS) {
        return IPRO_ERR_INVALID_STATE;
    }

    return IPRO_OK;
}

int ipro_event_timer_delete(ipro_event_timer_t timer)
{
    if (timer == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    if (timer->timer != NULL) {
        xTimerDelete(timer->timer, portMAX_DELAY);
    }

    vPortFree(timer);
    return IPRO_OK;
}

/*
 * Utility API
 */

int ipro_event_schedule_call(ipro_action_t action, void *arg)
{
    return ipro_event_post_delayed(0, action, arg);
}
