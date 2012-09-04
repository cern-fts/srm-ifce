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
    return res;
}

void srm_context_extension_free(srm_context_extension_t context){
    if(context){
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
    return context;
}


//
void srm_context_free(srm_context_t context){
    if(context){
        if(context->ext){ // ext -> dynamically allocated
            g_free(context->srm_endpoint);
            srm_context_extension_free(context->ext);
        }
    }
}


//
void srm_context_init(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose)
{
    GFAL_SRM_IFCE_ASSERT(context);
    context->ext = NULL;
    context->errbuf = errbuf;
    context->errbufsz = errbufsz;
    context->version = VERSION_2_2;
    context->srm_endpoint = srm_endpoint;
    context->timeout  = 60;
    context->verbose = verbose;
    context->timeout_conn = srm_get_timeout_connect ();
    context->timeout_ops = srm_get_timeout_sendreceive();
}
