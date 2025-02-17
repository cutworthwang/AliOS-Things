/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "aes.h"

static const TEEC_UUID tee_aes_uuid = AES_SRV_UUID;

int tee_aes_test()
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &tee_aes_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }


    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE, TEEC_NONE);
    ret = TEEC_InvokeCommand(&ss, TEE_AES_TEST, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_AES_TEST);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee aes test success\n");
    }

cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}
