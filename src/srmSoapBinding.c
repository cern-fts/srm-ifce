
#include "srmv2H.h"
SOAP_NMAC struct Namespace namespaces_srmv2[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"srm2", "http://srm.lbl.gov/StorageResourceManager", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

struct Namespace namespaces[] ={{NULL, NULL}};
