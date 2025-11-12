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

int do_INTF_MSG_TEST(uint8_t *buffer, size_t buffer_size, uint8_t int_data) {
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