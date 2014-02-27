/*********************************************************************
 *
 * RiakCTestConnection.cpp: Riak Testing Connection
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

#include "RiakCTestConnection.h"
#include "riak.h"

void
riak_test_error_cb(void *resp,
                   void *ptr) {
    riak_operation *rop = (riak_operation*)ptr;
    char message[1024];
    riak_server_error *error = riak_operation_get_server_error(rop);
    if (error) {
        riak_binary_print(riak_server_error_get_errmsg(error), message, sizeof(message));
        fprintf(stderr, "ERROR: %s\n", message);
    }
    throw new RiakCTestError(message);
}

RiakCTestConnection::RiakCTestConnection(RiakCTest& state) : _state(state),
                                                             _rop(NULL),
                                                             _rev(NULL) {
    char *hostname = (char*)"localhost";
    char *pb_port  = (char*)"8087";

    char *env_host = getenv(RIAK_TEST_HOST);
    char *env_port = getenv(RIAK_TEST_PB_PORT);

    if (env_host) {
        hostname = env_host;
    }
    if (env_port) {
        pb_port = env_port;
    }

    riak_error err = riak_connection_new(_state.getConfig(), &_cxn, hostname, pb_port, NULL);
    if (err) {
        throw new RiakCTestError("Could not set up connection");
    }
}

RiakCTestConnection::RiakCTestConnection(RiakCTest& state,
                                       const char*    hostname,
                                       const char*    portnum) : _state(state), _rop(NULL), _rev(NULL) {

    riak_error err = riak_connection_new(_state.getConfig(), &_cxn, hostname, portnum, NULL);
    if (err) {
        throw new RiakCTestError("Could not set up connection");
    }
}

RiakCTestConnection::~RiakCTestConnection() {
    riak_connection_free(&_cxn);
}

riak_error
RiakCTestConnection::asyncCreateEvent() {
    riak_error err = riak_operation_new(_cxn, &_rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_libevent_new(&_rev, _rop, _state.getLibeventBase());
    if (err) {
        return err;
    }
    riak_operation_set_error_cb(_rop, riak_test_error_cb);
    riak_operation_set_cb_data(_rop, _rop);

    return ERIAK_OK;
}


riak_error
RiakCTestConnection::asyncSendMessage() {
    return riak_libevent_send(_rop, _rev);
}


