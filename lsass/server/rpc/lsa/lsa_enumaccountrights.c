/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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
 *        lsa_enumaccountrights.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        LsaEnumAccountRights function
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


NTSTATUS
LsaSrvEnumAccountRights(
    /* [in] */ handle_t IDL_handle,
    /* [in] */ POLICY_HANDLE hPolicy,
    /* [in] */ PSID pAccountSid,
    /* [out] */ PLSA_ACCOUNT_RIGHTS pAccountRights
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD err = ERROR_SUCCESS;
    PPOLICY_CONTEXT pPolicyContext = (PPOLICY_CONTEXT)hPolicy;
    PWSTR *ppwszAccountRights = NULL;
    DWORD numAccountRights = 0;
    DWORD i = 0;

    if (pPolicyContext == NULL || pPolicyContext->Type != LsaContextPolicy)
    {
        ntStatus = STATUS_INVALID_HANDLE;
        BAIL_ON_NTSTATUS_ERROR(ntStatus);
    }

    err = LsaSrvPrivsEnumAccountRights(
            NULL,
            pPolicyContext->pUserToken,
            pAccountSid,
            &ppwszAccountRights,
            &numAccountRights);
    BAIL_ON_LSA_ERROR(err);

    err = LsaSrvAllocateMemory(
                    OUT_PPVOID(&pAccountRights->pAccountRight),
                    sizeof(pAccountRights->pAccountRight[0]) * numAccountRights);
    BAIL_ON_LSA_ERROR(err);

    for (i = 0; i < numAccountRights; i++)
    {
        err = LsaSrvInitUnicodeString(
                            &pAccountRights->pAccountRight[i],
                            ppwszAccountRights[i]);
        BAIL_ON_LSA_ERROR(err);
    }

    pAccountRights->NumAccountRights = numAccountRights;

error:
    if (err || ntStatus)
    {
        for (i = 0; i < numAccountRights; i++)
        {
            LsaSrvFreeUnicodeString(&pAccountRights->pAccountRight[i]);
        }

        pAccountRights->pAccountRight    = NULL;
        pAccountRights->NumAccountRights = 0;
    }

    for (i = 0; i < numAccountRights; i++)
    {
        LW_SAFE_FREE_MEMORY(ppwszAccountRights[i]);
    }
    LW_SAFE_FREE_MEMORY(ppwszAccountRights);

    if (ntStatus == STATUS_SUCCESS &&
        err != ERROR_SUCCESS)
    {
        ntStatus = LwWin32ErrorToNtStatus(err);
    }

    return ntStatus;
}





/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
