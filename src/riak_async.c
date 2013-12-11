/*********************************************************************
 *
 * riak_async.c: Riak Asynchronous Operations
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
#include "riak_connection.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_operation-internal.h"

riak_error
riak_async_register_ping(riak_operation        *rop,
                         riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_ping_request(rop, &(rop->pb_request));
}

riak_error
riak_async_register_serverinfo(riak_operation        *rop,
                               riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_serverinfo_request_encode(rop, &(rop->pb_request));
}

riak_error
riak_async_register_get(riak_operation        *rop,
                        riak_binary           *bucket,
                        riak_binary           *key,
                        riak_get_options      *get_options,
                        riak_response_callback cb) {

    riak_operation_set_response_cb(rop, cb);
    return riak_get_request_encode(rop, bucket, key, get_options, &(rop->pb_request));
}

riak_error
riak_async_register_put(riak_operation        *rop,
                        riak_object           *riak_obj,
                        riak_put_options      *options,
                        riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_put_request(rop, riak_obj, options, &(rop->pb_request));
}

riak_error
riak_async_register_delete(riak_operation        *rop,
                           riak_binary           *bucket,
                           riak_binary           *key,
                           riak_delete_options   *options,
                           riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_delete_request_encode(rop, bucket, key, NULL, &(rop->pb_request));
}

riak_error
riak_async_register_listbuckets(riak_operation        *rop,
                                riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_listbuckets_request(rop, &(rop->pb_request));
}

riak_error
riak_async_register_listkeys(riak_operation        *rop,
                             riak_binary           *bucket,
                             riak_uint32_t          timeout,
                             riak_response_callback cb ) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_listkeys_request(rop, bucket, timeout, &(rop->pb_request));
}

riak_error
riak_async_register_get_clientid(riak_operation       *rop,
                                 riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_get_clientid_request_encode(rop, &(rop->pb_request));
}

riak_error
riak_async_register_set_clientid(riak_operation  *rop,
                                 riak_binary     *clientid,
                                 riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_set_clientid_request_encode(rop, clientid, &(rop->pb_request));
}

riak_error
riak_async_register_get_bucketprops(riak_operation  *rop,
                                    riak_binary     *bucket,
                                    riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_get_bucketprops_request(rop, bucket, &(rop->pb_request));
}

riak_error
riak_async_register_reset_bucketprops(riak_operation  *rop,
                                      riak_binary     *bucket,
                                      riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_reset_bucketprops_request(rop, bucket, &(rop->pb_request));
}

riak_error
riak_async_register_set_bucketprops(riak_operation        *rop,
                                    riak_binary           *bucket,
                                    riak_bucket_props     *props,
                                    riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_encode_set_bucketprops_request(rop, bucket, props, &(rop->pb_request));
}


