/*********************************************************************
 *
 * riak_messages.h: Riak C Client List Buckets Message
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

#ifndef _RIAK_LISTBUCKETS_MESSAGE_H
#define _RIAK_LISTBUCKETS_MESSAGE_H

typedef struct _riak_listbuckets_response riak_listbuckets_response;
typedef void (*riak_listbuckets_response_callback)(riak_listbuckets_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_listbuckets_response`
 * @param response Result from a List buckets request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_listbuckets_response(riak_listbuckets_response *response,
                                char                      *target,
                                riak_size_t                len);

/**
 * @brief Free listbuckets response
 * @param cfg Riak Configuration
 * @param resp List buckets message to be cleaned up
 */
void
riak_free_listbuckets_response(riak_config                *cfg,
                               riak_listbuckets_response **resp);

#endif
