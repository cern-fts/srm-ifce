/*
 * Copyright (c) Members of the EGEE Collaboration. 2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Devresse Adrien
 */

#include "srm_ifce_internal.h"

srm_context_extension_t srm_context_extension_new(){
    struct srm_context_extension* res = g_new0(struct srm_context_extension,1);
    res->polling_logic = SRM_POLLING_LOGIC_MIN_MAX_EXP;
    res->min_waittime.tv_nsec = 200000000;
    res->max_waittime.tv_sec = 5;
    res->turl_resolution_timeout = 300;
    res->keep_alive = 0;
    g_datalist_init(&res->additional_headers);

    return res;
}

void srm_context_extension_free(srm_context_extension_t context){
    if(context){
        g_free(context->ucert);
        g_free(context->ukey);
        g_datalist_clear(&context->additional_headers);
        g_free(context);
    }
}

//
srm_context_t srm_context_new(const char * srm_endpoint, char *errbuf,int errbufsz,int verbose){
    struct srm_context* context = g_new0(struct srm_context, 1);
    context->errbuf = errbuf;
    context->errbufsz = errbufsz;
    context->version = VERSION_2_2;
    context->srm_endpoint = g_strdup(srm_endpoint);
    context->timeout  = 60;
    context->verbose = verbose;
    context->timeout_conn = srm_get_timeout_connect ();
    context->timeout_ops = srm_get_timeout_sendreceive();
    context->ext = srm_context_extension_new();
    context->ext->keep_alive = TRUE;
    return context;
}

srm_context_t srm_context_new2(const char * srm_endpoint, char *errbuf,int errbufsz,int verbose, int keep_alive){
    srm_context_t c = srm_context_new(srm_endpoint, errbuf, errbufsz, verbose);
    c->ext->keep_alive = keep_alive;
    return c;
}


//
void srm_context_free(srm_context_t context){
    if(context){
        if(context->ext) {
            srm_context_extension_free(context->ext);
        }
        if (context->soap) {
            srm_soap_free(context->soap);
        }
        g_free(context->srm_endpoint);
        g_free(context);
    }
}


//
void srm_context_init(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose)
{
    GFAL_SRM_IFCE_ASSERT(context);
    memset(context, 0, sizeof(*context));

    context->ext = srm_context_extension_new();
    context->errbuf = errbuf;
    context->errbufsz = errbufsz;
    context->version = VERSION_2_2;
    context->srm_endpoint = srm_endpoint;
    context->timeout  = 60;
    context->verbose = verbose;
    context->timeout_conn = srm_get_timeout_connect ();
    context->timeout_ops = srm_get_timeout_sendreceive();
    context->soap = NULL;
}


void srm_context_init2(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose,
                       int keep_alive){
    srm_context_init(context, srm_endpoint, errbuf, errbufsz, verbose);
    context->ext = srm_context_extension_new();
    context->ext->keep_alive = keep_alive;
}


void srm_set_credentials(struct srm_context *context, const char *ucert, const char *ukey)
{
    if (context->ext == NULL)
        context->ext = srm_context_extension_new();
    context->ext->ucert = g_strdup(ucert);
    context->ext->ukey = g_strdup(ukey);

    // Drop soap context, so it will recreated
    if (context->soap) {
        srm_soap_free(context->soap);
        context->soap = NULL;
    }
}

void srm_set_desired_request_time(struct srm_context *context, int timeout)
{
    if (context->ext == NULL)
        context->ext = srm_context_extension_new();
    context->ext->turl_resolution_timeout = timeout;
}


void srm_set_http_header(struct srm_context *context, const char *key, const char *value)
{
    if (context->ext == NULL)
        context->ext = srm_context_extension_new();

    if (value) {
        g_datalist_set_data_full(&context->ext->additional_headers, key, g_strdup(value), g_free);
    }
    else {
        g_datalist_remove_data(&context->ext->additional_headers, key);
    }
}


void srm_set_user_agent(struct srm_context *context, const char *user_agent, ...)
{
    if (context->ext == NULL)
        context->ext = srm_context_extension_new();

    if (user_agent) {
        va_list args;
        va_start(args, user_agent);
        g_vsnprintf(context->ext->user_agent, sizeof(context->ext->user_agent), user_agent, args);
        va_end(args);
    }
    else {
        context->ext->user_agent[0] = '\0';
    }
}
