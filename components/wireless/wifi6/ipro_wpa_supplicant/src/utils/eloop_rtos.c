/*
 * Event loop based on rtos_queue
 * Copyright (c) 2002-2009
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"
#ifndef CONFIG_RWNX
#include <assert.h>
#else
#define assert ASSERT_ERR
#endif

#include "common.h"
#include "trace.h"
#include "list.h"
#include "os.h"
#include "eloop.h"
#include "eloop_rtos.h"
#include "rtos_def.h"

#define ELOOP_MSGQ_SIZE_MAX (16)
#define printf(...)

struct eloop_timeout {
    struct dl_list list;
    struct os_reltime time;
    void *eloop_data;
    void *user_data;
    eloop_timeout_handler handler;
    WPA_TRACE_REF(eloop);
    WPA_TRACE_REF(user);
    WPA_TRACE_INFO
};

struct eloop_signal {
    int sig;
    void *user_data;
    eloop_signal_handler handler;
    int signaled;
};

struct eloop_event_observer {
    int flags;
#define ELOOP_OBSERVER_F_ALLOCATED    0x1
    eloop_sock_handler handler;
    void *eloop_data;
    void *userdata;
};

#define MAX_ELOOP_EVENT_OBSERVER_NUMBER 4
#define MAX_ELOOP_EVENT_NUMBER          5

struct eloop_event {
    /* user-defined event ID */
    int type;
#define ELOOP_EVT_F_ALLOCATED    0x1
    int flags;
    size_t observer_num;
    struct eloop_event_observer observers[MAX_ELOOP_EVENT_OBSERVER_NUMBER];
    /* the current processing msg */
    struct eloop_msg *msg;
};

struct eloop_data {
    rtos_mutex mtx;
    /* the queue for all kinds of events */
    rtos_queue msgq;
    size_t event_cnt;
    struct eloop_event events[MAX_ELOOP_EVENT_NUMBER];

    /* timer events */
    struct dl_list timeout;

    size_t signal_count;
    struct eloop_signal *signals;
    int signaled;
    int pending_terminate;

    int terminate;
    /* statistics */
    unsigned int commit_discard_cnt;
};

static struct eloop_data eloop;

struct eloop_msg {
    /* actor dispatches msg with event type */
    int type;
    /* supplied by requestor, read-only for actor */
    char inbuf[ELOOP_MSG_INBUF_LEN];
    int inlen;
    /* sync is true if completion notification is needed */
    int sync;
    rtos_semaphore done;

    /* writable for actor */
    char *outbuf;
    int *outlen;
};


int eloop_msg_init(struct eloop_msg *msg, int type, int sync,
        const char *inbuf, int len)
{
    if (len > sizeof(msg->inbuf)) {
        wpa_printf(MSG_ERROR,
                   "msg input buffer is too short, input len %d\r\n", len);
        return -1;
    }
    os_memcpy(msg->inbuf, inbuf, len);
    msg->sync = sync;
    msg->inlen = len;
    msg->type = type;
    msg->outlen = NULL;
    msg->outbuf = NULL;
    if (!msg->sync)
        return 0;

    msg->outlen = os_malloc(sizeof(int));
    if (!msg->outlen) {
        wpa_printf(MSG_ERROR, "no mem for msg->outlen\r\n");
        return -1;
    }
    *msg->outlen = ELOOP_MSG_OUTBUF_LEN;
    msg->outbuf = os_calloc(1, *msg->outlen);
    if (!msg->outbuf) {
        wpa_printf(MSG_ERROR, "no mem for msg->outbuf\r\n");
        goto err_resp_buf;
    }
    if (rtos_semaphore_create(&msg->done, 1, 0) < 0) {
        wpa_printf(MSG_ERROR, "failed to create eloop_msg sema\r\n");
        goto err_sem;
    }
    return 0;

err_sem:
    os_free(msg->outbuf);
    msg->outbuf = NULL;
err_resp_buf:
    os_free(msg->outlen);
    msg->outlen = NULL;
    return -1;
}


static int eloop_msg_deinit(struct eloop_msg *msg)
{
    if (!msg->sync)
        return 0;

    if (msg->outlen) {
        os_free(msg->outlen);
        msg->outlen = NULL;
    }
    if (msg->outbuf) {
        os_free(msg->outbuf);
        msg->outbuf = NULL;
    }
    if (msg->done)
        rtos_queue_delete(msg->done);
    return 0;
}


int eloop_event_commit_sync(int type, const char *inbuf, int inlen,
                            char *outbuf, int *resp_len, int timeout_ms)
{
    int len;
    int err = -1;
    struct eloop_msg msg;

    /* XXX wait for response forever */
    err = eloop_msg_init(&msg, type, 1, inbuf, inlen);
    if (err < 0) {
        wpa_printf(MSG_ERROR, "failed to init eloop msg\r\n");
        return -1;
    }
    if (rtos_queue_write(eloop.msgq, (void *)&msg, -1, false) == 1) {
        wpa_printf(MSG_ERROR, "eloop failed to write msgq\r\n");
        goto err_out;
    }

    if (rtos_semaphore_wait(msg.done, -1) == 1) {
        wpa_printf(MSG_ERROR, "eloop failed to wait semaphore\r\n");
        goto err_out;
    }
    /* copy result to response */
    len = *msg.outlen;
    if (*resp_len < len) {
        len = *resp_len;
        wpa_printf(MSG_WARNING, "output buffer is truncated\r\n");
    }

    *resp_len = len;
    os_memcpy(outbuf, msg.outbuf, len);
    err = 0;

err_out:
    eloop_msg_deinit(&msg);
    return err;
}


int eloop_event_commit(int type, const char *inbuf, int inlen, int discardable)
{
    int err = -1;
    struct eloop_msg msg;

    err = eloop_msg_init(&msg, type, 0, inbuf, inlen);
    if (err < 0) {
        wpa_printf(MSG_ERROR, "failed to init eloop msg\r\n");
        return -1;
    }

    if (discardable && (rtos_queue_cnt(eloop.msgq) > (ELOOP_MSGQ_SIZE_MAX/2))) {
        err = -1;
        eloop.commit_discard_cnt++;
        wpa_printf(MSG_ERROR, "failed to send disacrdable msg to msgq\r\n");
        goto err_out;
    }

    /* XXX wait for sending forever? */
    if (rtos_queue_write(eloop.msgq, (void *)&msg, -1, false) == 1) {
        wpa_printf(MSG_ERROR, "eloop failed to write msgq\r\n");
        goto err_out;
    }
    err = 0;

err_out:
    eloop_msg_deinit(&msg);
    return err;
}


static struct eloop_event *eloop_event_lookup(int type)
{
    int i;
    struct eloop_event *event;

    for (i = 0; i < eloop.event_cnt; i++) {
        event = &eloop.events[i];
        if (event->type == type)
            return event;
    }
    return NULL;
}


static int eloop_event_init(struct eloop_event *event, int type, int flags)
{
    os_memset(event->observers, 0, sizeof(event->observers));
    event->observer_num = 0;
    event->flags = flags;
    event->type = type;
    event->msg = NULL;
    return 0;
}


static int eloop_event_deinit(struct eloop_event *event)
{
    os_memset(event->observers, 0, sizeof(event->observers));
    event->observer_num = 0;
    event->flags = 0;
    event->type = -1;
    event->msg = NULL;
    return 0;
}


int eloop_init(void)
{
    int i;
    struct eloop_event *events;

    os_memset(&eloop, 0, sizeof(eloop));
    dl_list_init(&eloop.timeout);

    if (rtos_mutex_create(&eloop.mtx)) {
        wpa_printf(MSG_ERROR, "failed to create mutex\r\n");
        return -1;
    }

    if (rtos_queue_create(sizeof(struct eloop_msg),
                          ELOOP_MSGQ_SIZE_MAX, &eloop.msgq)) {
        wpa_printf(MSG_ERROR, "failed to create msgq\r\n");
        goto err_msgq;
    }

    for (i = 0; i < MAX_ELOOP_EVENT_NUMBER; i++) {
        if (eloop_event_init(&eloop.events[i], -1, 0)) {
            wpa_printf(MSG_ERROR, "%s failed to init event\r\n", __func__);
            break;
        }
    }

    if (i < MAX_ELOOP_EVENT_NUMBER)
        goto err_init_events;
    return 0;

err_init_events:
    i--;
    for (; i >= 0; i--)
        eloop_event_deinit(&events[i]);
    rtos_queue_delete(eloop.msgq);
err_msgq:
    rtos_mutex_delete(eloop.mtx);
    return -1;
}


static void eloop_observer_init(struct eloop_event_observer *observer,
        eloop_sock_handler handler, void *eloop_data, void *userdata)
{
    observer->handler = handler;
    observer->eloop_data = eloop_data;
    observer->userdata = userdata;
    observer->flags |= ELOOP_OBSERVER_F_ALLOCATED;
}


static void eloop_observer_deinit(struct eloop_event_observer *observer)
{
    observer->handler = NULL;
    observer->eloop_data = NULL;
    observer->userdata = NULL;
    observer->flags = 0;
}


static inline int eloop_observer_is_valid(struct eloop_event_observer *observer)
{
    return !!(observer->flags & ELOOP_OBSERVER_F_ALLOCATED);
}


/* TODO remove me */
static void eloop_events_dump(const char *tag)
{
    int i, j;

    printf("%s event total count %d\r\n", tag, eloop.event_cnt);
    for (i = 0; i < eloop.event_cnt; i++) {
        struct eloop_event *event = &eloop.events[i];

        printf("event type %d, observer number %d\r\n",
               event->type, event->observer_num);
        for (j = 0; j < event->observer_num; j++) {
            struct eloop_event_observer *observer = &event->observers[j];
            printf("event type %d, observer %d, handler %p, "
                   "eloop data %p, userdata %p\r\n",
                   event->type, j, observer->handler,
                   observer->eloop_data, observer->userdata);
        }
    }
}


static int eloop_event_add_observer(struct eloop_event *event,
        eloop_sock_handler handler, void *eloop_data, void *userdata)
{
    int i;
    struct eloop_event_observer *observer = NULL;

    if (event->observer_num >= MAX_ELOOP_EVENT_OBSERVER_NUMBER) {
        wpa_printf(MSG_ERROR, "event[type %d] observer number reached "
                   "its limit, can't register anymore\r\n",
                   event->type, event->observer_num);
        return -1;
    }

    for (i = 0; i < event->observer_num; i++) {
        struct eloop_event_observer *tmp = &event->observers[i];
        /* duplicate observer */
        if (!eloop_observer_is_valid(tmp)) {
            wpa_printf(MSG_ERROR, "event[type %d] observer %d flags %x, "
                                  "it's' not allocated?!\r\n",
                                  event->type, i, tmp->flags);
            continue;
        }
        if (tmp->handler == handler && tmp->userdata == userdata &&
            tmp->eloop_data == eloop_data)
            return 0;
    }

    /* add a new observer */
    if (!observer) {
        observer = &event->observers[event->observer_num];
        eloop_observer_init(observer, handler, eloop_data, userdata);
        event->observer_num++;
    }

    eloop_events_dump("register");
    wpa_printf(MSG_INFO, "event[type %d] added a new observer %p, "
               "total count %d\r\n",
               event->type, handler, event->observer_num);
    return 0;
}


static inline int eloop_event_is_valid(struct eloop_event *event)
{
    return !!(event->flags & ELOOP_EVT_F_ALLOCATED);
}


static int __eloop_register_event_observer(int ev_type,
        eloop_sock_handler handler, void *eloop_data, void *userdata)
{
    int i;
    struct eloop_event *event = NULL;

    if (eloop.event_cnt >= MAX_ELOOP_EVENT_NUMBER) {
        wpa_printf(MSG_ERROR,"eloop event reached its limit %d, "
                "can't register anymore'\r\n", eloop.event_cnt);
        return -1;
    }
    for (i = 0; i < eloop.event_cnt; i++) {
        struct eloop_event *tmp = &eloop.events[i];
        if (eloop_event_is_valid(tmp) && tmp->type == ev_type) {
            event = tmp;
            break;
        }
    }

    /* add a new event */
    if (!event) {
        event = &eloop.events[eloop.event_cnt];
        eloop_event_init(event, ev_type, ELOOP_EVT_F_ALLOCATED);
        eloop.event_cnt++;
        wpa_printf(MSG_INFO, "eloop_msgq added a new event 0x%x\r\n", ev_type);
    }

    return eloop_event_add_observer(event, handler, eloop_data, userdata);
}


int eloop_register_event_observer(int ev_type, eloop_sock_handler handler,
                                  void *eloop_data, void *userdata)
{
    int err;

    /* sanity check */
    if (!handler)
        return -1;

    rtos_mutex_lock(eloop.mtx);
    err = __eloop_register_event_observer(ev_type, handler, eloop_data, userdata);
    rtos_mutex_unlock(eloop.mtx);
    return err;
}


static int eloop_event_del_observer(struct eloop_event *event,
        eloop_sock_handler handler, void *eloop_data, void *userdata)
{
    int i;
    struct eloop_event_observer *observer = NULL;

    for (i = 0; i < event->observer_num; i++) {
        struct eloop_event_observer *tmp = &event->observers[i];
        if (eloop_observer_is_valid(tmp) && tmp->handler == handler &&
            tmp->eloop_data == eloop_data && tmp->userdata == userdata) {
            observer = tmp;
            break;
        }
    }

    if (!observer)
        return 0;

    eloop_observer_deinit(observer);
    if (i < event->observer_num - 1) {
        os_memmove(&event->observers[i], &event->observers[i + 1],
                (event->observer_num - i - 1) *
                sizeof(struct eloop_event_observer));
    }
    event->observer_num--;
    wpa_printf(MSG_INFO, "event[type 0x%x] deleted observer %p, "
               "total observer count %d\r\n",
               event->type, handler, event->observer_num);
    return 0;
}


static int __eloop_unregister_event_observer(int ev_type,
        eloop_sock_handler handler, void *eloop_data, void *userdata)
{
    int i, err;
    struct eloop_event *event = NULL;

    for (i = 0; i < eloop.event_cnt; i++) {
        struct eloop_event *tmp = &eloop.events[i];

        if (eloop_event_is_valid(tmp) && tmp->type == ev_type) {
            event = tmp;
            break;
        }
    }

    if (!event)
        return 0;

    if (handler) {
        err = eloop_event_del_observer(event, handler, eloop_data, userdata);
        if (err < 0) {
            wpa_printf(MSG_ERROR, "event[type %d] failed to delete observer\r\n",
                       event->type);
            return -1;
        }
    } else {
        /* remove all observers */
        event->observer_num = 0;
        os_memset(event->observers, 0, sizeof(event->observers));
    }

    if (!event->observer_num) {
        eloop_event_deinit(event);
        if (i < eloop.event_cnt - 1) {
            os_memmove(&eloop.events[i], &eloop.events[i + 1],
                    sizeof(struct eloop_event) * (eloop.event_cnt - i - 1));
        }
        eloop.event_cnt--;
        wpa_printf(MSG_INFO, "eloop_msgq deleted event 0x%x\r\n", ev_type);
    }
    eloop_events_dump("unregister");
    return 0;
}


/*
 * @brief: unregister event with ev_type.
 * @param: remove all observers if handler is NULL or only remove
 *         the matched one.
 * @return: 0 on success, negative number otherwise.
 */
int eloop_unregister_event_observer(int ev_type, eloop_sock_handler handler,
                                    void *eloop_data, void *userdata)
{
    int err;

    rtos_mutex_lock(eloop.mtx);
    err = __eloop_unregister_event_observer(ev_type, handler, eloop_data,
                                            userdata);
    rtos_mutex_unlock(eloop.mtx);
    return err;
}


int eloop_sock_requeue(void)
{
    wpa_printf(MSG_ERROR, "eloop_msgq %s is NOT supported yet!\r\n", __func__);
    return 0;
}


int eloop_register_read_sock(int sock, eloop_sock_handler handler,
                 void *eloop_data, void *user_data)
{
    return eloop_register_sock(sock, EVENT_TYPE_READ, handler,
                   eloop_data, user_data);
}


void eloop_unregister_read_sock(int sock)
{
    eloop_unregister_sock(sock, EVENT_TYPE_READ);
}


int eloop_register_sock(int event, eloop_event_type type,
            eloop_sock_handler handler,
            void *eloop_data, void *user_data)
{
    if (!handler)
        return -1;

    assert(type == EVENT_TYPE_READ);
    return eloop_register_event_observer(event, handler, eloop_data, user_data);
}


void eloop_unregister_sock(int sock, eloop_event_type type)
{
    assert(type == EVENT_TYPE_READ);
    eloop_unregister_event_observer(sock, NULL, NULL, NULL);
}


int eloop_register_timeout(unsigned int secs, unsigned int usecs,
               eloop_timeout_handler handler,
               void *eloop_data, void *user_data)
{
    struct eloop_timeout *timeout, *tmp;
    os_time_t now_sec;

    timeout = os_zalloc(sizeof(*timeout));
    if (timeout == NULL)
        return -1;
    if (os_get_reltime(&timeout->time) < 0) {
        os_free(timeout);
        return -1;
    }
    now_sec = timeout->time.sec;
    timeout->time.sec += secs;
    if (timeout->time.sec < now_sec) {
        /*
         * Integer overflow - assume long enough timeout to be assumed
         * to be infinite, i.e., the timeout would never happen.
         */
        wpa_printf(MSG_DEBUG, "ELOOP: Too long timeout (secs=%u) to "
                  "ever happen - ignore it", secs);
        os_free(timeout);
        return 0;
    }
    timeout->time.usec += usecs;
    while (timeout->time.usec >= 1000000) {
        timeout->time.sec++;
        timeout->time.usec -= 1000000;
    }
    timeout->eloop_data = eloop_data;
    timeout->user_data = user_data;
    timeout->handler = handler;
    wpa_trace_add_ref(timeout, eloop, eloop_data);
    wpa_trace_add_ref(timeout, user, user_data);
    wpa_trace_record(timeout);

    /* Maintain timeouts in order of increasing time */
    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (os_reltime_before(&timeout->time, &tmp->time)) {
            dl_list_add(tmp->list.prev, &timeout->list);
            return 0;
        }
    }
    dl_list_add_tail(&eloop.timeout, &timeout->list);

    return 0;
}


static void eloop_remove_timeout(struct eloop_timeout *timeout)
{
    dl_list_del(&timeout->list);
    wpa_trace_remove_ref(timeout, eloop, timeout->eloop_data);
    wpa_trace_remove_ref(timeout, user, timeout->user_data);
    os_free(timeout);
}


int eloop_cancel_timeout(eloop_timeout_handler handler,
             void *eloop_data, void *user_data)
{
    struct eloop_timeout *timeout, *prev;
    int removed = 0;

    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                  struct eloop_timeout, list) {
        if (timeout->handler == handler &&
            (timeout->eloop_data == eloop_data ||
             eloop_data == ELOOP_ALL_CTX) &&
            (timeout->user_data == user_data ||
             user_data == ELOOP_ALL_CTX)) {
            eloop_remove_timeout(timeout);
            removed++;
        }
    }

    return removed;
}


int eloop_cancel_timeout_one(eloop_timeout_handler handler,
                 void *eloop_data, void *user_data,
                 struct os_reltime *remaining)
{
    struct eloop_timeout *timeout, *prev;
    int removed = 0;
    struct os_reltime now;

    os_get_reltime(&now);
    remaining->sec = remaining->usec = 0;

    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                  struct eloop_timeout, list) {
        if (timeout->handler == handler &&
            (timeout->eloop_data == eloop_data) &&
            (timeout->user_data == user_data)) {
            removed = 1;
            if (os_reltime_before(&now, &timeout->time))
                os_reltime_sub(&timeout->time, &now, remaining);
            eloop_remove_timeout(timeout);
            break;
        }
    }
    return removed;
}


int eloop_is_timeout_registered(eloop_timeout_handler handler,
                void *eloop_data, void *user_data)
{
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
            tmp->eloop_data == eloop_data &&
            tmp->user_data == user_data)
            return 1;
    }

    return 0;
}


int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
              eloop_timeout_handler handler, void *eloop_data,
              void *user_data)
{
    struct os_reltime now, requested, remaining;
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
            tmp->eloop_data == eloop_data &&
            tmp->user_data == user_data) {
            requested.sec = req_secs;
            requested.usec = req_usecs;
            os_get_reltime(&now);
            os_reltime_sub(&tmp->time, &now, &remaining);
            if (os_reltime_before(&requested, &remaining)) {
                eloop_cancel_timeout(handler, eloop_data,
                             user_data);
                eloop_register_timeout(requested.sec,
                               requested.usec,
                               handler, eloop_data,
                               user_data);
                return 1;
            }
            return 0;
        }
    }

    return -1;
}


int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
                eloop_timeout_handler handler, void *eloop_data,
                void *user_data)
{
    struct os_reltime now, requested, remaining;
    struct eloop_timeout *tmp;

    dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
        if (tmp->handler == handler &&
            tmp->eloop_data == eloop_data &&
            tmp->user_data == user_data) {
            requested.sec = req_secs;
            requested.usec = req_usecs;
            os_get_reltime(&now);
            os_reltime_sub(&tmp->time, &now, &remaining);
            if (os_reltime_before(&remaining, &requested)) {
                eloop_cancel_timeout(handler, eloop_data,
                             user_data);
                eloop_register_timeout(requested.sec,
                               requested.usec,
                               handler, eloop_data,
                               user_data);
                return 1;
            }
            return 0;
        }
    }

    return -1;
}


#if !defined CONFIG_RWNX
static void eloop_handle_signal(int sig)
{
    size_t i;

    eloop.signaled++;
    for (i = 0; i < eloop.signal_count; i++) {
        if (eloop.signals[i].sig == sig) {
            eloop.signals[i].signaled++;
            break;
        }
    }
}
#endif


static void eloop_process_pending_signals(void)
{
    size_t i;

    if (eloop.signaled == 0)
        return;
    eloop.signaled = 0;

    if (eloop.pending_terminate) {
#if !defined CONFIG_RWNX
        alarm(0);
#endif /* CONFIG_NATIVE_WINDOWS */
        eloop.pending_terminate = 0;
    }

    for (i = 0; i < eloop.signal_count; i++) {
        if (eloop.signals[i].signaled) {
            int sig = eloop.signals[i].sig;
            void *user_data = eloop.signals[i].user_data;
			eloop_signal_handler handler = eloop.signals[i].handler;

            eloop.signals[i].signaled = 0;
            rtos_mutex_unlock(eloop.mtx);
			handler(sig, user_data);
            rtos_mutex_lock(eloop.mtx);
        }
    }
}


int eloop_register_signal(int sig, eloop_signal_handler handler,
              void *user_data)
{
    struct eloop_signal *tmp;

    tmp = os_realloc_array(eloop.signals, eloop.signal_count + 1,
                   sizeof(struct eloop_signal));
    if (tmp == NULL)
        return -1;

    tmp[eloop.signal_count].sig = sig;
    tmp[eloop.signal_count].user_data = user_data;
    tmp[eloop.signal_count].handler = handler;
    tmp[eloop.signal_count].signaled = 0;
    eloop.signal_count++;
    eloop.signals = tmp;
#if !defined CONFIG_RWNX
    signal(sig, eloop_handle_signal);
#endif

    return 0;
}


int eloop_register_signal_terminate(eloop_signal_handler handler,
                    void *user_data)
{
    wpa_printf(MSG_WARNING, "SIGTERM is NOT supported yet!\r\n");
    return 0;
}


int eloop_register_signal_reconfig(eloop_signal_handler handler,
                   void *user_data)
{
    wpa_printf(MSG_WARNING, "SIGHUP is NOT supported yet!\r\n");
    return 0;
}


static void eloop_event_dispatch(struct eloop_msg *msg)
{
    int i;
    struct eloop_event *event;
    struct eloop_event_observer *observer;

    wpa_printf(MSG_DEBUG, "eloop dispatching event %d\r\n", msg->type);
    event = eloop_event_lookup(msg->type);
    if (!event) {
        wpa_printf(MSG_WARNING, "recved unknown event 0x%x\r\n", msg->type);
        return;
    }

    /*
     * XXX chill, there is only one observer for each event now, this will
     * be buggy if multiple observers write to the same eloop_msg, because
     * msg will be overwritten.
     */
    event->msg = msg;
    for (i = 0; i < event->observer_num; i++) {
        observer = &event->observers[i];
        if (!eloop_observer_is_valid(observer)) {
            wpa_printf(MSG_ERROR, "observer %d not valid\r\n", i);
            continue;
        }
        wpa_printf(MSG_DEBUG, "eloop dispatching event %d, observer %p\r\n",
                   msg->type, observer->handler);

        eloop_sock_handler handler = observer->handler;
        void *userdata = observer->userdata;
        void *eloop_data = observer->eloop_data;

        if (handler) {
            rtos_mutex_unlock(eloop.mtx);
            handler(msg->type, eloop_data, userdata);
            rtos_mutex_lock(eloop.mtx);
        }
    }
    event->msg = NULL;

    if (msg->sync) {
        if (rtos_semaphore_signal(msg->done, false) == 1)
            wpa_printf(MSG_ERROR, "eloop failed to signal completion\r\n");
    }
}


int eloop_rtos_sock_do_recv(int ev_type, void *buf, int len, int flags)
{
    struct eloop_event *event;

    event = eloop_event_lookup(ev_type);
    if (!event) {
        wpa_printf(MSG_WARNING, "recved unknown event 0x%x\r\n", ev_type);
        return -1;
    }

    struct eloop_msg *msg = event->msg;
    if (!msg) {
        wpa_printf(MSG_ERROR, "event %d has no eloop_msg attached\r\n",
                   ev_type);
        return -1;
    }
    int copylen = (msg->inlen > len ? len : msg->inlen);
    os_memcpy(buf, msg->inbuf, copylen);
    return copylen;
}


int eloop_rtos_sock_recv(int ev_type, void *buf, int len, int flags)
{
    int err;

    rtos_mutex_lock(eloop.mtx);
    err = eloop_rtos_sock_do_recv(ev_type, buf, len, flags);
    rtos_mutex_unlock(eloop.mtx);
    return err;
}


static int eloop_rtos_sock_do_send(int ev_type, const void *buf, int len)
{
    struct eloop_event *event;

    event = eloop_event_lookup(ev_type);
    if (!event) {
        wpa_printf(MSG_ERROR, "unknown event 0x%x\r\n", ev_type);
        return -1;
    }

    struct eloop_msg *msg = event->msg;
    if (!msg) {
        wpa_printf(MSG_ERROR, "event %d has no msg attached\r\n", ev_type);
        return -1;
    }

    assert(len <= *msg->outlen);
    os_memcpy(msg->outbuf, buf, len);
    *msg->outlen = len;
    return len;
}


int eloop_rtos_sock_send(int ev_type, const void *buf, int len)
{
    int err;

    rtos_mutex_lock(eloop.mtx);
    err = eloop_rtos_sock_do_send(ev_type, buf, len);
    rtos_mutex_unlock(eloop.mtx);
    return err;
}


void eloop_run(void)
{
    int res, timeout_ms;
    struct eloop_msg msg;
    struct os_reltime tv, now;

    rtos_mutex_lock(eloop.mtx);
    while (!eloop.terminate &&
           (!dl_list_empty(&eloop.timeout) || eloop.event_cnt > 0)) {
        struct eloop_timeout *timeout;

        if (eloop.pending_terminate) {
            /*
             * This may happen in some corner cases where a signal
             * is received during a blocking operation. We need to
             * process the pending signals and exit if requested to
             * avoid hitting the SIGALRM limit if the blocking
             * operation took more than two seconds.
             */
            eloop_process_pending_signals();
            if (eloop.terminate)
                break;
        }

        timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
        if (timeout) {
            os_get_reltime(&now);
            if (os_reltime_before(&now, &timeout->time))
                os_reltime_sub(&timeout->time, &now, &tv);
            else
                tv.sec = tv.usec = 0;
        }

        timeout_ms = tv.sec * 1000 + tv.usec / 1000;
        rtos_mutex_unlock(eloop.mtx);
        res = rtos_queue_read(eloop.msgq, (void *)&msg, timeout_ms, 0);
        rtos_mutex_lock(eloop.mtx);

        eloop_process_pending_signals();

        /* check if some registered timeouts have occurred */
        timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
        if (timeout) {
            os_get_reltime(&now);
            if (!os_reltime_before(&now, &timeout->time)) {
                void *eloop_data = timeout->eloop_data;
                void *user_data = timeout->user_data;

                eloop_timeout_handler handler =
                    timeout->handler;
                eloop_remove_timeout(timeout);
                rtos_mutex_unlock(eloop.mtx);
                handler(eloop_data, user_data);
                rtos_mutex_lock(eloop.mtx);
            }
        }

        /* timeouted, got nothing from the queue */
        if (res == 1)
            continue;

        /* we got something, try to demux the msg */
        eloop_event_dispatch(&msg);
    }
    eloop.terminate = 0;
    rtos_mutex_unlock(eloop.mtx);
    wpa_printf(MSG_INFO, "eloop exits\r\n");
}


void eloop_terminate(void)
{
    eloop.terminate = 1;
}


void eloop_destroy(void)
{
    struct eloop_timeout *timeout, *prev;
    struct os_reltime now;

    os_get_reltime(&now);
    dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                  struct eloop_timeout, list) {
        int sec, usec;
        sec = timeout->time.sec - now.sec;
        usec = timeout->time.usec - now.usec;
        if (timeout->time.usec < now.usec) {
            sec--;
            usec += 1000000;
        }
        wpa_printf(MSG_INFO, "ELOOP: remaining timeout: %d.%06d "
               "eloop_data=%p user_data=%p handler=%p",
               sec, usec, timeout->eloop_data, timeout->user_data,
               timeout->handler);
        wpa_trace_dump_funcname("eloop unregistered timeout handler",
                    timeout->handler);
        wpa_trace_dump("eloop timeout", timeout);
        eloop_remove_timeout(timeout);
    }
    os_free(eloop.signals);
    for (int i = 0; i < MAX_ELOOP_EVENT_NUMBER; i++)
        eloop_event_deinit(&eloop.events[i]);
    rtos_queue_delete(eloop.msgq);
    rtos_mutex_delete(eloop.mtx);
}


int eloop_terminated(void)
{
    return eloop.terminate || eloop.pending_terminate;
}


void eloop_wait_for_read_sock(int sock)
{
    wpa_printf(MSG_WARNING, "%s is NOT supported yet\r\n", __func__);
}
