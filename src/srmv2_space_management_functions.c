#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"

/* returns space tokens associated to the space description */
int srmv2_getspacetokens (
		struct srm_context *context,struct srm_getspacetokens_input *input)

//		const char *spacetokendescspacetokendesc, const char *srm_endpoint, int *nbtokens, char ***spacetokens,
	//	char *errbuf, int errbufsz, int timeout)
{
	/*int flags;
	int sav_errno = 0;
	int i, ret;
	struct soap soap;
	struct srm2__srmGetSpaceTokensResponse_ tknrep;
	struct srm2__srmGetSpaceTokensRequest tknreq;
	struct srm2__TReturnStatus *tknrepstatp;
	struct srm2__ArrayOfString *tknrepp;
	const char srmfunc[] = "GetSpaceTokens";

	if (spacetokendesc == NULL || srm_endpoint == NULL || spacetokens == NULL || nbtokens == NULL) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[GFAL][srmv2_getspacetokens][EINVAL] Invalid arguments");
		errno = EINVAL;
		return (-1);
	}
	*nbtokens = 0;
	*spacetokens = NULL;

	srm_soap_init(&soap);

	memset (&tknreq, 0, sizeof(tknreq));

	tknreq.userSpaceTokenDescription = (char *) spacetokendesc;

	if ((ret = soap_call_srm2__srmGetSpaceTokens (&soap, srm_endpoint, srmfunc, &tknreq, &tknrep))) {
		if (soap.fault != NULL && soap.fault->faultstring != NULL)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: %s",
					gfal_remote_type, srmfunc, srm_endpoint, soap.fault->faultstring);
		else if (soap.error == SOAP_EOF)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Connection fails or timeout",
				   	gfal_remote_type, srmfunc, srm_endpoint);
		else
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Unknown SOAP error (%d)",
					gfal_remote_type, srmfunc, srm_endpoint, soap.error);

		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	if (tknrep.srmGetSpaceTokensResponse == NULL ||
			(tknrepstatp = tknrep.srmGetSpaceTokensResponse->returnStatus) == NULL) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: <empty response>",
				gfal_remote_type, srmfunc, srm_endpoint);
		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	if (tknrepstatp->statusCode != SRM_USCORESUCCESS) {
		sav_errno = statuscode2errno (tknrepstatp->statusCode);
		if (tknrepstatp->explanation && tknrepstatp->explanation[0])
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s",
					gfal_remote_type, srmfunc, statuscode2errmsg (tknrepstatp->statusCode),
					srm_endpoint, tknrepstatp->explanation);
		else
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <none>",
					gfal_remote_type, srmfunc, statuscode2errmsg (tknrepstatp->statusCode), srm_endpoint);

		soap_end (&soap);
		soap_done (&soap);
		errno = sav_errno;
		return (-1);
	}

	tknrepp = tknrep.srmGetSpaceTokensResponse->arrayOfSpaceTokens;

	if (!tknrepp) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <empty response>",
				gfal_remote_type, srmfunc, statuscode2errmsg (tknrepstatp->statusCode), srm_endpoint);
		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	*nbtokens = tknrepp->__sizestringArray;
	if (*nbtokens < 1 || !tknrepp->stringArray) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s: No such space token descriptor",
				gfal_remote_type, srmfunc, statuscode2errmsg (tknrepstatp->statusCode),
				srm_endpoint, spacetokendesc);
		soap_end (&soap);
		soap_done (&soap);
		errno = EINVAL;
		return (-1);
	}

	if ((*spacetokens = (char **) calloc (*nbtokens + 1, sizeof (char *))) == NULL) {
		soap_end (&soap);
		soap_done (&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < *nbtokens; ++i)
		(*spacetokens)[i] = strdup(tknrepp->stringArray[i]);

	soap_end (&soap);
	soap_done (&soap);
    errno = 0;*/
	return (0);
}



// Calls first srmv2_getspacetokens
// and srmv2_getspacemd
/*char * srmv2_getbestspacetoken (const char *spacetokendesc,
		const char *srm_endpoint, GFAL_LONG64 neededsize,
		char *errbuf, int errbufsz, int timeout)
{
	int sav_errno = 0;
	int i, ret, nbtokens = -1, numtoken = -1;
	GFAL_LONG64 unusedsize = -1;
	char **spacetokens = NULL;
	gfal_spacemd *spacemd = NULL;
	char *spacetoken = NULL;

	ret = srmv2_getspacetokens (spacetokendesc, srm_endpoint, &nbtokens, &spacetokens, errbuf, errbufsz, timeout);
	if (ret < 0 || spacetokens == NULL || nbtokens < 1) {
		errno = ret == 0 ? ENOMEM : errno;
		return (NULL);
	}

   if (nbtokens == 1) {
        errno = 0;
        spacetoken = spacetokens[0];
        free (spacetokens);
        return (spacetoken);
    }

	ret = srmv2_getspacemd (nbtokens, (const char **) spacetokens, srm_endpoint, &spacemd, errbuf, errbufsz, timeout);
	if (ret < 0 || spacemd == NULL) {
		sav_errno = ret == 0 ? ENOMEM : errno;

		for (i = 0; i < nbtokens; ++i)
			if (spacetokens[i]) free (spacetokens[i]);

		free (spacetokens);
		errno = sav_errno;
		return (NULL);
	}

	// Get the spacetoken with the least free space, but a bit more than needed
	for (i = 0; i < nbtokens; ++i) {
		if (spacetokens[i]) free (spacetokens[i]);
		if (spacemd[i].unusedsize < neededsize + GFAL_SIZE_MARGIN)
			continue;
		if (numtoken < 0 || spacemd[i].unusedsize < unusedsize) {
			numtoken = i;
			unusedsize = spacemd[i].unusedsize;
		}
	}

	if (numtoken < 0) {
		// no suitable space token
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR,
				"[GFAL][srmv2_getbestspacetoken][EINVAL] %s: no associated space token with enough free space", spacetokendesc);
		errno = EINVAL;
		return (NULL);
	}

	spacetoken = spacemd[numtoken].spacetoken;
	spacemd[numtoken].spacetoken = NULL;  // prevent it to be freed
	gfal_spacemd_free (nbtokens, spacemd);
	free (spacetokens);
    errno = 0;
	return (spacetoken);
}*/
