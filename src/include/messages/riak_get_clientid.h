/*********************************************************************
 *
 * riak_get_clientid.h: Riak C Client Get Client ID Message
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

#ifndef _RIAK_GET_CLIENTID_MESSAGE_H
#define _RIAK_GET_CLIENTID_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_get_clientid_response riak_get_clientid_response;
typedef void (*riak_get_clientid_response_callback)(riak_get_clientid_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_get_clientid_response`
 * @param state Riak Print State
 * @param response Result from a client id request
 * @returns Number of bytes written
 */
riak_int32_t
riak_get_clientid_response_print(riak_print_state           *state,
                                 riak_get_clientid_response *response);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp client id PBC Response
 */
void
riak_get_clientid_response_free(riak_config                 *cfg,
                                riak_get_clientid_response **resp);

/**
 * @brief Access the Client ID
 * @param resp Response from server
 * @returns Client Identifier
 */
riak_binary*
riak_get_clientid_get_clientid(riak_get_clientid_response *resp);

#ifdef __cplusplus
}
#endif

#endif
