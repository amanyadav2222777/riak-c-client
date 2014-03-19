/*********************************************************************
 *
 * riak_connection.c: Management of the Riak Connection
 *
 * Copyright (c) 2007-2014 Basho Technologies, Inc.  All Rights Reserved.
 *
 * This file is provided to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *********************************************************************/

#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_connection.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"
#include "riak_network.h"
#include <limits.h>

riak_error
riak_connection_new(riak_config             *cfg,
                    riak_connection        **cxn_target,
                    const char              *hostname,
                    const char              *portnum,
                    riak_connection_options *opt_in) {
    riak_connection *cxn = riak_config_clean_allocate(cfg, sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
    *cxn_target = cxn;
    cxn->config = cfg;
    // Setup default options
    cxn->opt.resolver        = getaddrinfo;
    cxn->opt.socket_fn       = socket;
    cxn->opt.connect_fn      = connect;
    cxn->opt.close_fn        = close;
    cxn->opt.timeout.tv_sec  = 0;
    cxn->opt.timeout.tv_usec = 0;

    // Apply any user-defined overrides
    if (opt_in && opt_in->resolver) {
        cxn->opt.resolver = opt_in->resolver;
    }
    if (opt_in && opt_in->socket_fn) {
        cxn->opt.socket_fn = opt_in->socket_fn;
    }
    if (opt_in && opt_in->connect_fn) {
        cxn->opt.connect_fn = opt_in->connect_fn;
    }
    if (opt_in && opt_in->close_fn) {
        cxn->opt.close_fn = opt_in->close_fn;
    }
    if (opt_in && opt_in->timeout.tv_sec) {
        cxn->opt.timeout.tv_sec = opt_in->timeout.tv_sec;
    }
    if (opt_in && opt_in->timeout.tv_usec) {
        cxn->opt.timeout.tv_usec = opt_in->timeout.tv_usec;
    }

    riak_strlcpy(cxn->hostname, hostname, sizeof(cxn->hostname));
    riak_strlcpy(cxn->portnum, portnum, sizeof(cxn->portnum));

    riak_error err = riak_resolve_address(cfg, cxn->opt.resolver, hostname, portnum, &(cxn->addrinfo));
    if (err) {
        riak_log_critical_config(cfg, "Could not resolve hostname '%s'", hostname);
        return ERIAK_DNS_RESOLUTION;
    }

    // TODO: Implement retry logic
    cxn->fd = riak_just_open_a_socket(cfg, cxn->addrinfo, cxn->opt.socket_fn, cxn->opt.connect_fn);
    if (cxn->fd < 0) {
        riak_log_critical_config(cfg, "%s", "Could not just open a socket");
        return ERIAK_CONNECT;
    }

    int result;
    // Optionally set the socket timeout
    if (cxn->opt.timeout.tv_sec > 0 || cxn->opt.timeout.tv_sec) {
        result = setsockopt(cxn->fd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&cxn->opt.timeout, sizeof(struct timeval));
        if (result < 0) {
            riak_log_critical_config(cfg, "%s", "Could not set the socket send timeout");
            return ERIAK_CONNECT;
        }
        setsockopt(cxn->fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&cxn->opt.timeout, sizeof(struct timeval));
        if (result < 0) {
            riak_log_critical_config(cfg, "%s", "Could not set the socket receive timeout");
            return ERIAK_CONNECT;
        }
    }

    // Collect initial stats
    result = gettimeofday(&cxn->open_time, &cxn->open_tz);
    if (result < 0) {
        memset(&cxn->open_time, '\0', sizeof(struct timeval));
        memset(&cxn->open_tz, '\0', sizeof(struct timezone));
    }
    return ERIAK_OK;
}

riak_socket_t
riak_connection_get_fd(riak_connection *cxn) {
    return cxn->fd;
}
riak_config*
riak_connection_get_config(riak_connection *cxn) {
    return cxn->config;
}

void riak_connection_free(riak_connection** cxn_target) {
    if (cxn_target == NULL || *cxn_target == NULL) return;
    riak_connection *cxn = *cxn_target;
    riak_config *cfg = riak_connection_get_config(cxn);

    if (cxn->fd) {
        cxn->opt.close_fn(cxn->fd);
    }
    if (cxn->addrinfo != NULL) freeaddrinfo(cxn->addrinfo);
    riak_free(cfg, cxn_target);
}

riak_error
riak_connection_pool_new(riak_config             *cfg,
                         riak_connection_pool   **pool_out,
                         riak_connection_options *opt,
                         riak_uint32_t            total_cxns,
                         riak_boolean_t           lazy_connect) {
    riak_connection_pool *pool = riak_config_clean_allocate(cfg, sizeof(riak_connection_pool));
    if (pool == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *pool_out            = NULL;
    pool->cfg            = cfg;
    pool->idx            = 0;
    pool->max_total      = total_cxns;
    pool->lazy_connect   = lazy_connect;
    if (opt) {
        pool->opt = riak_config_clean_allocate(cfg, sizeof(riak_connection_options));
        if (pool->opt == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        memcpy(pool->opt, opt, sizeof(riak_connection_options));
    }
    pool->host_configs = riak_array_new(cfg, RIAK_CONN_HOST_INIT_SIZE);
    if (pool->host_configs == NULL) {
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    pool->available_cxns = riak_array_new(cfg, RIAK_CONN_POOL_INIT_SIZE);
    if (pool->available_cxns == NULL) {
        riak_array_free(&pool->host_configs);
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    pool->in_use_cxns = riak_array_new(cfg, RIAK_CONN_POOL_INIT_SIZE);
    if (pool->in_use_cxns == NULL) {
        riak_array_free(&pool->available_cxns);
        riak_array_free(&pool->host_configs);
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    *pool_out = pool;
    return ERIAK_OK;
}

void
riak_connection_pool_free(riak_connection_pool **pool_in) {
    riak_connection_pool *pool = *pool_in;

    // Clean up any host configurations
    if (pool->host_configs) {
        riak_uint32_t num_hosts = riak_array_length(pool->host_configs);
        for(int i = 0; i < num_hosts; i++) {
            riak_connection_host *config = (riak_connection_host *)riak_array_pop(pool->host_configs);
            riak_free(pool->cfg, &config);
        }
    }

    // Close all unused connections
    if (pool->available_cxns) {
        riak_uint32_t num_avail = riak_array_length(pool->available_cxns);
        for(int i = 0; i < num_avail; i++) {
            riak_connection *cxn = (riak_connection *)riak_array_pop(pool->available_cxns);
            riak_connection_free(&cxn);
        }
    }

    // Close all in-use connections
    if (pool->in_use_cxns) {
        riak_uint32_t num_avail = riak_array_length(pool->in_use_cxns);
        for(int i = 0; i < num_avail; i++) {
            riak_connection *cxn = (riak_connection *)riak_array_pop(pool->in_use_cxns);
            riak_connection_free(&cxn);
        }
    }

    if (pool->opt) {
        riak_free(pool->cfg, &(pool->opt));
    }
    riak_free(pool->cfg, pool_in);
}

riak_error
riak_connection_pool_add_host(riak_connection_pool *pool,
                              const char           *hostname,
                              const char           *portnum,
                              riak_uint32_t         max_cxns) {
    riak_connection_host* host = riak_config_clean_allocate(pool->cfg, sizeof(riak_connection_host));
    if (host == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_strlcpy(host->hostname, hostname, sizeof(host->hostname));
    riak_strlcpy(host->portnum, portnum, sizeof(host->portnum));
    host->max_cxns = (max_cxns) ? max_cxns : LONG_MAX;
    host->cxns = riak_array_new(pool->cfg, 8);
    if (host->cxns == NULL) {
        riak_free(pool->cfg, &host);
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_int64_t n_config = riak_array_length(pool->host_configs);
    for(int i = 0; i < n_config; i++) {
        riak_connection_host* existing = riak_array_get(pool->host_configs, i);
        if (strncmp(existing->hostname, hostname, sizeof(existing->hostname)) == 0 &&
            strncmp(existing->portnum, portnum, sizeof(existing->portnum)) == 0) {
            riak_free(pool->cfg, &host);
            return ERIAK_DUPLICATE;  // Host is already in the list
        }
    }
    return riak_array_push(pool->host_configs, host);
}

riak_error
riak_connection_pool_remove_host(riak_connection_pool *pool,
                                 const char           *hostname,
                                 const char           *portnum) {
    riak_int64_t n_config = riak_array_length(pool->host_configs);
    for(int i = 0; i < n_config; i++) {
        riak_connection_host* host = riak_array_get(pool->host_configs, i);
        if (strncmp(host->hostname, hostname, sizeof(host->hostname)) == 0 &&
            strncmp(host->portnum, portnum, sizeof(host->portnum)) == 0) {
            // Houston, we have a match
            riak_free(pool->cfg, &host);
            void *result = riak_array_remove(pool->host_configs, i);
            if (result == NULL) {
                return ERIAK_OUT_OF_RANGE;
            }
            return ERIAK_OK;
        }
    }
    return ERIAK_NOT_FOUND;
}

riak_error
riak_connection_pool_add_connection(riak_connection_pool *pool) {
    riak_config  *cfg = pool->cfg;
    riak_uint32_t hosts_len = riak_array_length(pool->host_configs);
    if (hosts_len == 0) {
        return ERIAK_UNINITIALIZED;
    }
    riak_uint32_t avail_len  = riak_array_length(pool->available_cxns);
    riak_uint32_t in_use_len = riak_array_length(pool->in_use_cxns);

    if (avail_len + in_use_len >= pool->max_total) {
        return ERIAK_MAX_CONNECTIONS;
    }
    riak_error err = ERIAK_OK;

    // Go through the entire lists of hosts if we have problems connecting
    for(riak_uint32_t attempted_hosts = 0; attempted_hosts < hosts_len; attempted_hosts++) {
        // If for some reason the next round robin host is out of range, loop back to zero
        if (pool->idx >= hosts_len) {
            pool->idx = 0;
        }
        riak_connection_host *host = (riak_connection_host *)riak_array_get(pool->host_configs, pool->idx);
        if (host == NULL) {
            return ERIAK_OUT_OF_RANGE;
        }
        // Round robin to next host in the queue
        pool->idx++;
        riak_connection *cxn;
        for(int tries = 0; tries < RIAK_CONNECTION_RETRIES; tries++) {
            err = riak_connection_new(cfg, &cxn, host->hostname, host->portnum, pool->opt);
            if (err == ERIAK_OUT_OF_MEMORY) {
                return err;
            } else if (err == ERIAK_OK) {
                // We have a valid connection so add it to the ready queue
                cxn->host = host;
                err = riak_array_push(pool->available_cxns, (void*)cxn);
                if (err == ERIAK_OK) {
                    // And add it to the host's connection queue
                    printf("trying to add %x\n", cxn);
                    err = riak_array_push(host->cxns, (void*)cxn);
printf("done\n");
                }
                return err;
            }
            riak_log_error_config(cfg, "Could not connect to %s:%s. Attempt %d", host->hostname, host->portnum, tries);
            // Something is wrong here; wait and try again
            sleep(1);
        }
    }

    return err;
}

riak_error
riak_connection_pool_remove_connection(riak_connection_pool *pool,
                                       riak_connection      *cxn) {
    riak_uint32_t hosts_len = riak_array_length(pool->host_configs);
    if (hosts_len == 0) {
        return ERIAK_UNINITIALIZED;
    }

    // First look in the available connections
    riak_uint32_t avail_len  = riak_array_length(pool->available_cxns);
    riak_boolean_t found     = RIAK_FALSE;
    for(int i = 0; i < avail_len; i++) {
        riak_connection *test_cxn = (riak_connection *)riak_array_get(pool->available_cxns, i);
        if (cxn == test_cxn) {
            void *out = riak_array_remove(pool->available_cxns, i);
            if (out == NULL) {
                return ERIAK_OUT_OF_RANGE;
            }
            found = RIAK_TRUE;
        }
    }

    // Then look in the active connections
    if (!found) {
        riak_uint32_t in_use_len = riak_array_length(pool->in_use_cxns);
        for(int i = 0; i < in_use_len; i++) {
            riak_connection *test_cxn = (riak_connection *)riak_array_get(pool->in_use_cxns, i);
            if (cxn == test_cxn) {
                void *out = riak_array_remove(pool->in_use_cxns, i);
                if (out == NULL) {
                    return ERIAK_OUT_OF_RANGE;
                }
                found = RIAK_TRUE;
            }
        }
    }

    // Now remove it from the host's list of connections
    if (found) {
        riak_connection_host *host = cxn->host;
        riak_uint32_t in_use_len = riak_array_length(host->cxns);
        for(int i = 0; i < in_use_len; i++) {
            riak_connection *test_cxn = (riak_connection *)riak_array_get(host->cxns, i);
            if (cxn == test_cxn) {
                void *out = riak_array_remove(host->cxns, i);
                if (out == NULL) {
                    return ERIAK_OUT_OF_RANGE;
                }
                // Finally close up shop
                riak_connection_free(&cxn);
                return ERIAK_OK;
            }
        }
    }

    return ERIAK_NOT_FOUND;
}

riak_error
riak_connection_pool_get(riak_connection_pool *pool,
                         riak_connection     **cxn) {
    // Simplest case in which a connection is queued up...
    if (riak_array_length(pool->available_cxns) > 0) {
        *cxn = riak_array_remove(pool->available_cxns, 0);
        if (*cxn == NULL) {
            return ERIAK_OUT_OF_RANGE;
        }
        // ...And move it to the ready queue
        return riak_array_push(pool->in_use_cxns, cxn);
    } else {
        if (riak_array_length(pool->in_use_cxns) >= pool->max_total) {
            return ERIAK_MAX_CONNECTIONS;
        } else {
            // Since we need a connection and none are available, add one
            riak_error err = riak_connection_pool_add_connection(pool);

            // It's fine if the pool is already topped up
            if (err != ERIAK_OK && err != ERIAK_MAX_CONNECTIONS) {
                return err;
            }
        }
    }
    return ERIAK_UNINITIALIZED;
}

riak_error
riak_connection_pool_return(riak_connection_pool *pool,
                            riak_connection      *cxn) {
    return ERIAK_OK;
}
