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
 * Authors: Todor Manev  IT-GT CERN
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "srm_soap.h"
#include "gfal_srm_ifce_types.h"
#include "srmv2H.h"
#include "srm_dependencies.h"

/**
 * @file file for the memory management help functions
 * @version 0.0.1
 * @date 09/06/2011
 */


/**
 *  @brief delete properly and free memory for a number n of of struct srmv2_pinstatuses
 * If called on a NULL value, simply return
 **/ 
void srm_srmv2_pinfilestatus_delete(struct srmv2_pinfilestatus*  srmv2_pinstatuses, int n){
	if(srmv2_pinstatuses){
		int i ;
		for(i=0; i < n; ++i){
			free(srmv2_pinstatuses[i].surl);
			free(srmv2_pinstatuses[i].turl);
			free(srmv2_pinstatuses[i].explanation);
		}
		free(srmv2_pinstatuses);	
	}
}

/**
 *  @brief delete properly and free memory for a number n of struct srmv2_mdfilestatus 
 *  If called on a NULL value, simply return
 **/ 
void srm_srmv2_mdfilestatus_delete(struct srmv2_mdfilestatus* mdfilestatus, int n){
	if(mdfilestatus){
		int i,j ;
		for(i=0; i < n; ++i){
			free(mdfilestatus[i].surl);
			free(mdfilestatus[i].explanation);		
			free(mdfilestatus[i].checksum);
			free(mdfilestatus[i].checksumtype);	
			for(j=0; j < mdfilestatus[i].nbspacetokens;++j){
				free(mdfilestatus[i].spacetokens[i]);
			}	
		}
		free(mdfilestatus);
	}
}

/**
 *  @brief delete properly and free memory for a number n of struct srmv2_filestatuses
 *  If called on a NULL value, simply return
 **/ 
void srm_srmv2_filestatus_delete(struct srmv2_filestatus*  srmv2_statuses, int n){
	if(srmv2_statuses){
		int i;
		for(i=0; i < n; ++i){
			free(srmv2_statuses[i].surl);
			free(srmv2_statuses[i].turl);
			free(srmv2_statuses[i].explanation);
		}
		free(srmv2_statuses);	
	}
}

/**
 * 
 * @brief delete properly and free memory for a struct srm2__TReturnStatus
 *  If called on a NULL value, simply return
 */
void srm_srm2__TReturnStatus_delete(struct srm2__TReturnStatus* status){
	if(status){
		free(status->explanation);
		free(status);
	}
}
