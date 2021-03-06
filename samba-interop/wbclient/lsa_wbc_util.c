/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

/*
 * Copyright © BeyondTrust Software 2004 - 2019
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * BEYONDTRUST MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING TERMS AS
 * WELL. IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT WITH
 * BEYONDTRUST, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE TERMS OF THAT
 * SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE APACHE LICENSE,
 * NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU HAVE QUESTIONS, OR WISH TO REQUEST
 * A COPY OF THE ALTERNATE LICENSING TERMS OFFERED BY BEYONDTRUST, PLEASE CONTACT
 * BEYONDTRUST AT beyondtrust.com/contact
 */

/*
 * Copyright (C) BeyondTrust Software. All rights reserved.
 *
 * Module Name:
 *
 *        lsa_wbc.c
 *
 * Abstract:
 *
 *        BeyondTrust Security and Authentication Subsystem (LSASS)
 *
 * Authors: Gerald Carter <gcarter@likewisesoftware.com>
 *
 */

#include "wbclient.h"
#include "lsawbclient_p.h"
#include "lwnet.h"
#include <lsa/ad.h>
#include <string.h>

struct _wbc_err_string {
    wbcErr wbc_err;
    const char *errstr;
};

static struct _wbc_err_string wbcErrorMap[] = {
    { WBC_ERR_SUCCESS, "No error" },
    { WBC_ERR_NOT_IMPLEMENTED, "Function not implemented" },
    { WBC_ERR_UNKNOWN_FAILURE, "Unknown Failure" },
    { WBC_ERR_NO_MEMORY, "Out of memory" },
    { WBC_ERR_INVALID_SID, "Invalid Security Identifier" },
    { WBC_ERR_INVALID_PARAM, "Invalid Parameter" },
    { WBC_ERR_WINBIND_NOT_AVAILABLE, "Security service not available" },
    { WBC_ERR_DOMAIN_NOT_FOUND, "Domain not found" },
    { WBC_ERR_INVALID_RESPONSE, "Invalid response received from security authority" },
    { WBC_ERR_NSS_ERROR, "Name server switch error" },
    { WBC_ERR_AUTH_ERROR, "Authentication error" }
};

/* @brief Convert a wbcErr to a human readable string
 *
 * @param error      Error code to translate
 *
 * @return char*
**/

const char *wbcErrorString(wbcErr error)
{
    int i = 0;
    size_t table_size = sizeof(wbcErrorMap) / sizeof(struct _wbc_err_string);

    for (i=0; i<table_size; i++) {
        if (error == wbcErrorMap[i].wbc_err) {
            return wbcErrorMap[i].errstr;
        }
    }

    return "Unmapped error";
}

void wbcFreeMemory(void* p)
{
    if (p)
        _WBC_FREE(p);

    return;
}

wbcErr wbcPing(void)
{
    HANDLE hLsa = (HANDLE)NULL;
    DWORD dwErr = LW_ERROR_INTERNAL;
    wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;

    /* Just open and close an LsaServerHandle */

    dwErr = LsaOpenServer(&hLsa);
    BAIL_ON_LSA_ERR(dwErr);

    dwErr = LsaCloseServer(hLsa);
    hLsa = (HANDLE)NULL;
    BAIL_ON_LSA_ERR(dwErr);

cleanup:
    wbc_status = map_error_to_wbc_status(dwErr);

    return wbc_status;
}

static int FreeInterfaceDetails(void *p)
{
    struct wbcInterfaceDetails *iface;

    if (!p)
        return 0;

    iface = (struct wbcInterfaceDetails*)p;

    _WBC_FREE_CONST_DISCARD(iface->netbios_domain);
    _WBC_FREE_CONST_DISCARD(iface->dns_domain);

    return 0;
}


wbcErr wbcInterfaceDetails(struct wbcInterfaceDetails **details)
{
    DWORD dwErr = LW_ERROR_INTERNAL;
    wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
    HANDLE hLsa = NULL;
    PLSA_MACHINE_ACCOUNT_INFO_A pAccountInfo = NULL;
    size_t hostnameLen = 0;

    BAIL_ON_NULL_PTR_PARAM(details, dwErr);

    /* Find our domain */

    dwErr = LsaOpenServer(&hLsa);
    BAIL_ON_LSA_ERR(dwErr);

    dwErr = LsaAdGetMachineAccountInfo(hLsa, NULL, &pAccountInfo);
    BAIL_ON_LSA_ERR(dwErr);

    *details = _wbc_malloc(sizeof(struct wbcInterfaceDetails),
                   FreeInterfaceDetails);
    BAIL_ON_NULL_PTR(*details, dwErr);

    (*details)->interface_version = LSA_WBC_INTERFACE_VERSION;
    (*details)->winbind_version   = LSA_WBC_WINBIND_VERSION;
    (*details)->winbind_separator = '\\';

    (*details)->netbios_name = _wbc_strdup(pAccountInfo->SamAccountName);
    BAIL_ON_NULL_PTR((*details)->netbios_name, dwErr);

    // Strip off the trailing dollar sign
    hostnameLen = strlen((*details)->netbios_name);
    if (hostnameLen > 0 && (*details)->netbios_name[hostnameLen - 1] == '$')
    {
        ((char *)(*details)->netbios_name)[hostnameLen - 1] = 0;
    }

    (*details)->netbios_domain = _wbc_strdup(pAccountInfo->NetbiosDomainName);
    BAIL_ON_NULL_PTR((*details)->netbios_domain, dwErr);

    (*details)->dns_domain = _wbc_strdup(pAccountInfo->DnsDomainName);
    BAIL_ON_NULL_PTR((*details)->dns_domain, dwErr);

cleanup:
    if (pAccountInfo)
    {
        LsaAdFreeMachineAccountInfo(pAccountInfo);
    }

    if (hLsa)
    {
        LsaCloseServer(hLsa);
    }

    wbc_status = map_error_to_wbc_status(dwErr);

    return wbc_status;
}

wbcErr wbcLibraryDetails(struct wbcLibraryDetails **details)
{
    DWORD dwErr = LW_ERROR_INTERNAL;
    wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;

    BAIL_ON_NULL_PTR_PARAM(details, dwErr);

    *details = _wbc_malloc(sizeof(struct wbcLibraryDetails), NULL);
    BAIL_ON_NULL_PTR(*details, dwErr);

    (*details)->major_version   = LSA_WBC_LIBRARY_MAJOR_VERSION;
    (*details)->minor_version   = LSA_WBC_LIBRARY_MINOR_VERSION;
    (*details)->vendor_version  = LSA_WBC_LIBRARY_VENDOR_STRING;

cleanup:
    wbc_status = map_error_to_wbc_status(dwErr);

    return wbc_status;
}

