/*********************************************************************
 *
 * riak_operation.h: A single Riak Operation
 *
 * Copyright (c) 2007-2013 Basho Technologies, Inc.  All Rights Reserved.
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
#include "riak_operation-internal.h"

riak_error
riak_operation_new(riak_connection        *cxn,
                   riak_operation       **rop_target,
                   riak_response_callback response_cb,
                   riak_response_callback error_cb,
                   void                  *cb_data) {
    riak_config    *cfg = riak_connection_get_config(cxn);
    riak_operation *rop = (riak_operation*)riak_config_clean_allocate(cfg, sizeof(riak_operation));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_operation");
        return ERIAK_OUT_OF_MEMORY;
    }
    *rop_target = rop;

    rop->connection  = cxn;
    rop->response_cb = response_cb;
    rop->error_cb    = error_cb;
    rop->cb_data     = cb_data;

    return ERIAK_OK;
}

void
riak_operation_free(riak_operation **rop_target) {
    riak_operation *rop = *rop_target;
    riak_config *cfg = riak_operation_get_config(rop);
    if (rop->pb_request) {
        riak_pb_message_free(cfg, &(rop->pb_request));
    }
    riak_free(cfg, rop_target);
}

riak_config*
riak_operation_get_config(riak_operation *rop) {
    return riak_connection_get_config(rop->connection);
}

riak_connection*
riak_operation_get_connection(riak_operation *rop) {
    return rop->connection;
}

riak_server_error*
riak_operation_get_server_error(riak_operation *rop) {
    return rop->error;
}

void
riak_operation_set_response_cb(riak_operation          *rop,
                               riak_response_callback  cb) {
    rop->response_cb = cb;
}

void
riak_operation_set_error_cb(riak_operation         *rop,
                            riak_response_callback  cb) {
    rop->error_cb = cb;
}

void
riak_operation_set_cb_data(riak_operation     *rop,
                           void               *cb_data) {
    rop->cb_data = cb_data;
}
void
riak_operation_set_response_decoder(riak_operation       *rop,
                                    riak_response_decoder decoder) {
    rop->decoder = decoder;
}

