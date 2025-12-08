#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h> /* for atoi(3) */
#include <unistd.h> /* for getopt(3) */
#include <string.h> /* for strerror(3) */
#include <sysexits.h> /* for EX_* exit codes */
#include <errno.h>  /* for errno */

#include "oai_asn1.h"
#include "common/utils/LOG/log.h"

#include "NR_INTF-TEST-Message.h"

int do_INTF_MSG_TEST(uint8_t *buffer, size_t buffer_size, long int_data) {
    NR_INTF_TEST_Message_t intf_test_msg;
    memset(&intf_test_msg, 0, sizeof(NR_INTF_TEST_Message_t));
    
    intf_test_msg.int_data = int_data;

    asn_enc_rval_t enc_rval = uper_encode_to_buffer(&asn_DEF_NR_INTF_TEST_Message,
                                                  NULL,
                                                  (void *)&intf_test_msg,
                                                  buffer,
                                                  buffer_size);
    AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %lu)!\n", enc_rval.failed_type->name, enc_rval.encoded);
    LOG_I(INTF, "INTF_TEST_Message Encoded %zd bits (%zd bytes)\n", enc_rval.encoded, (enc_rval.encoded+7)/8);

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NR_INTF_TEST_Message, &intf_test_msg);

    return ((enc_rval.encoded+7)/8);
}

void do_INTF_MSG_DECODE_TEST(uint8_t *buf, uint32_t len){
    // 1. 声明目标结构体指针 和 解码返回值
    NR_INTF_TEST_Message_t *intf_test_msg = NULL;
    asn_dec_rval_t dec_rval;

    // 2. 调用解码函数
    dec_rval = uper_decode_complete(NULL,
                                   &asn_DEF_NR_INTF_TEST_Message,
                                   (void **)&intf_test_msg,
                                   buf,
                                   len);
    
    // 3. 检查解码结果
    if (dec_rval.code != RC_OK) {
        LOG_E(INTF, "ASN1 message decoding failed (code=%d)!\n", dec_rval.code);
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NR_INTF_TEST_Message, intf_test_msg);
        return;
    }

    // 4. 访问数据
    LOG_I(INTF, "INTF_TEST_Message decoded: int_data=%ld\n", intf_test_msg->int_data);

    // 5. 释放内存
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NR_INTF_TEST_Message, intf_test_msg);
    
}