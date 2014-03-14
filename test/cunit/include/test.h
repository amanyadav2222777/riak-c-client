/*********************************************************************
 *
 * test.h: Riak C Common Testing Utilities
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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "riak.h"
#include "riak_async.h"
#include "test_bucket_key_value.h"

#ifndef _RIAK_C_TEST_H
#define _RIAK_C_TEST_H

#define RIAK_TEST_NUM_THREADS   10
#define RIAK_TEST_BUCKET_PREFIX "riak_c_test_"

#define RIAK_TEST_MAX_BUCKETS       50
#define RIAK_TEST_MAX_KEYS          50
#define RIAK_TEST_BUCKET_KEY_LEN    20
#define RIAK_TEST_VALUE_LEN         200

#define RIAK_TEST_HOST          "RIAK_TEST_HOST"
#define RIAK_TEST_PB_PORT       "RIAK_TEST_PB_PORT"

/**
 * @brief Fake a connected socket
 * @param domain for now usually PF_INET
 * @param type SOCK_STREAM
 * @param protocol IPPROTO_IPV4
 * @returns 0
 */
riak_socket_t
test_dummy_socket(int domain,
                  int type,
                  int protocol);
/**
 * @brief Fake a successful connection to a fake socket
 * @param socket Ignored
 * @param address Ignored
 * @param address_len Ignored
 * @returns 0
 */
int
test_dummy_connect(riak_socket_t socket,
                   const riak_sockaddr *address,
                   riak_uint32_t address_len);

/**
 * @brief Fake socket closer
 * @param socket Ignored
 * @returns 0
 */
int
test_dummy_close(riak_socket_t socket);

/**
 * Global testing connection options
 */
extern riak_connection_options test_connection_dummy_options;

/**
 * @brief Set up testing environment
 * @param cfg Returned Riak Configuration
 * @returns Error Code
 */
riak_error
test_setup(riak_config **cfg);

/**
 * @brief Connect to a live Riak cluster
 * @param cfg Riak Configuration
 * @param cxn Returned Riak Connection
 * @returns Error Code
 */
riak_error
test_connect(riak_config      *cfg,
             riak_connection **cxn);

/**
 * @brief Disconnect from a Riak cluster
 * @param cfg Riak Configuration
 * @param cxn Riak Connection
*/
void
test_disconnect(riak_config      *cfg,
                riak_connection **cxn);

/**
 * @brief Clean up Testing environment
 * @param cfg Riak Configuration
 * @param cxn Riak Connection
*/
void
test_cleanup(riak_config **cfg);

//
// A S Y N C H R O N O U S
//

typedef struct _test_async_connection {
    riak_config       *cfg;
    riak_connection   *cxn;
    riak_operation    *rop;
    riak_libevent     *rev;
    struct event_base *base;
    riak_error         err;     // Returned from callback
    char               err_msg[1024]; // Optionally returned by callback
} test_async_connection;

typedef struct _test_async_pthread {
    test_async_connection *conn;
    pthread_t              tid;
    void                  *result;
    void                  *args;
} test_async_pthread;

typedef void*(*test_async_pthread_fun)(void*);

/**
 * @brief Set up asynchronous testing environment
 * @param cfg Riak Configuration
 * @param conn Returned Async Test Connection
 * @returns Error Code
 */
riak_error
test_async_connect(riak_config            *cfg,
                   test_async_connection **conn);

/**
 * @brief Clean up asynchronous testing environment
 * @param conn Async Test Connection
 */
void
test_async_disconnect(test_async_connection **conn);

/**
 * @brief Build a random printable ASCII string
 * @param cfg Riak Configuration
 * @param len Length of generated string
 * @returns Random ASCII string
 */
char*
test_random_string(riak_config  *cfg,
                   riak_uint32_t len);

/**
 * @brief Fire off the asynchronous thread
 * @param cfg Riak Configuration
 * @param f Thread function to run
 * @param args Arguments passed to the thread
 * @returns Error Code
 */
riak_error
test_async_thread_runner(riak_config             *cfg,
                         test_async_pthread_fun   f,
                         void                    *args);

/**
 * @brief Queue up asynchronous message and fire event loop
 * @oaram conn Testing Connection
 * @returns Error Code
 */
riak_error
test_async_send_message(test_async_connection *conn);

/**
 * @brief Throw lots of crap into Riak for testing
 * @param cfg Riak Configuration
 * @param cxn Riak Connection
 * @param root Returned linked-list of Bucket/Key/Values
 */
riak_error
test_load_db(riak_config            *cfg,
             riak_connection        *cxn,
             test_bucket_key_value **root);

/**
 * @brief Destroy all buckets starting with RIAK_TEST_BUCKET_PREFIX
 * @param conn Test connection to the DB
 */
riak_error
test_cleanup_db(riak_connection* cxn);

#endif // _RIAK_C_TEST_H
