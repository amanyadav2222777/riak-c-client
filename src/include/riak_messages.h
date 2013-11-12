/*********************************************************************
 *
 * riak_messages.h: Riak C Client External Messages
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

#ifndef RIAK_MESSAGES_H
#define RIAK_MESSAGES_H

typedef struct _riak_serverinfo_response riak_serverinfo_response;
typedef struct _riak_get_response riak_get_response;
typedef struct _riak_get_options riak_get_options;
typedef struct _riak_put_response riak_put_response;
typedef struct _riak_put_options riak_put_options;
typedef struct _riak_listbuckets_response riak_listbuckets_response;
typedef struct _riak_listkeys_response riak_listkeys_response;
typedef struct _riak_error_response riak_error_response;
typedef struct _riak_ping_response riak_ping_response;
typedef struct _riak_delete_options riak_delete_options;
typedef struct _riak_delete_response riak_delete_response;
typedef struct _riak_get_clientid_response riak_get_clientid_response;
typedef struct _riak_get_clientid_request riak_get_clientid_request;
typedef struct _riak_get_bucketprops_request riak_get_bucketprops_request;
typedef struct _riak_get_bucketprops_response riak_get_bucketprops_response;
typedef struct _riak_set_clientid_response riak_set_clientid_response;
typedef struct _riak_set_bucketprops_request riak_set_bucketprops_request;
typedef struct _riak_set_bucketprops_response riak_set_bucketprops_response;
typedef struct _riak_reset_bucketprops_request riak_reset_bucketprops_request;
typedef struct _riak_reset_bucketprops_response riak_reset_bucketprops_response;
typedef void (*riak_ping_response_callback)(riak_ping_response *response, void *ptr);
typedef void (*riak_serverinfo_response_callback)(riak_serverinfo_response *response, void *ptr);
typedef void (*riak_get_response_callback)(riak_get_response *response, void *ptr);
typedef void (*riak_put_response_callback)(riak_put_response *response, void *ptr);
typedef void (*riak_delete_response_callback)(riak_delete_response *response, void *ptr);
typedef void (*riak_listbuckets_response_callback)(riak_listbuckets_response *response, void *ptr);
typedef void (*riak_listkeys_response_callback)(riak_listkeys_response *response, void *ptr);
typedef void (*riak_get_clientid_response_callback)(riak_get_clientid_response *response, void *ptr);
typedef void (*riak_set_clientid_response_callback)(riak_set_clientid_response *response, void *ptr);
typedef void (*riak_get_bucketprops_response_callback)(riak_get_bucketprops_response *response, void *ptr);
typedef void (*riak_set_bucketprops_response_callback)(riak_set_bucketprops_response *response, void *ptr);
typedef void (*riak_reset_bucketprops_response_callback)(riak_reset_bucketprops_response *response, void *ptr);


/**
 * @brief Free memory used by an error response
 * @param cfg Riak Configuration
 * @param resp Error structure to be freed
 */
void
riak_free_error_response(riak_config         *cfg,
                         riak_error_response **resp);

/**
 * @brief Print a summary of a `riak_serverinfo_response`
 * @param response Result from a Server Info request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_serverinfo_response(riak_serverinfo_response *response,
                               char                     *target,
                               riak_size_t               len);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Server Info PBC Response
 */
void
riak_free_serverinfo_response(riak_config              *cfg,
                              riak_serverinfo_response **resp);


/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Ping PBC Response
 */
void
riak_free_ping_response(riak_config        *cfg,
                        riak_ping_response **resp);

/**
 * @brief Print a summary of a `riak_get_response`
 * @param response Result from a Get request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_get_response(riak_get_response *response,
                        char              *target,
                        riak_size_t        len);

/**
 * @brief Free get response
 * @param cfg Riak Configuration
 * @param resp Get response
 */
void
riak_free_get_response(riak_config       *cfg,
                       riak_get_response **resp);

/**
 * @brief Print a summary of a `riak_put_response`
 * @param response Result from a Put request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_put_response(riak_put_response *response,
                        char              *target,
                        riak_size_t        len);

/**
 * @brief Free put response
 * @param cfg Riak Configuration
 * @param resp Put message to be cleaned up
 */
void
riak_free_put_response(riak_config       *cfg,
                       riak_put_response **resp);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Delete PBC Response
 */
void
riak_free_delete_response(riak_config          *cfg,
                          riak_delete_response **resp);

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
riak_free_listbuckets_response(riak_config               *cfg,
                               riak_listbuckets_response **resp);

/**
 * @brief Print a summary of a `riak_listkeys_response`
 * @param response Result from a Listkeys request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_listkeys_response(riak_listkeys_response *response,
                             char                   *target,
                             riak_size_t             len);

/**
 * @brief Free listkeys response
 * @param cfg Riak Configuration
 * @param resp List keys message to be cleaned up
 */
void
riak_free_listkeys_response(riak_config            *cfg,
                            riak_listkeys_response **resp);
/**
 * @brief Print a summary of a `riak_get_clientid_response`
 * @param response Result from a client id request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_get_clientid_response(riak_get_clientid_response *response,
                                 char                       *target,
                                 riak_size_t                 len);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp client id PBC Response
 */
void
riak_free_get_clientid_response(riak_config              *cfg,
                              riak_get_clientid_response **resp);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp client id PBC Response
 */
void
riak_free_set_clientid_response(riak_config              *cfg,
                              riak_set_clientid_response **resp);

/**
 * @brief Print a summary of a `riak_get_bucketprops_response`
 * @param response Result from a get_bucketprops request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_print_get_bucketprops_response(riak_get_bucketprops_response *response,
                                    char                          *target,
                                    riak_size_t                    len);


/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Bucket Properties PBC Response
 */
void
riak_free_get_bucketprops_response(riak_config                   *cfg,
                                   riak_get_bucketprops_response **resp);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Bucket Properties PBC Response
 */
void
riak_free_reset_bucketprops_response(riak_config                     *cfg,
                                     riak_reset_bucketprops_response **resp);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Bucket Properties PBC Response
 */
void
riak_free_set_bucketprops_response(riak_config                   *cfg,
                                   riak_set_bucketprops_response **resp);

#endif // RIAK_MESSAGES_H
