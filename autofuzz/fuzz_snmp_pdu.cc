#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" int LLVMFuzzerTestOneInput(
    const uint8_t *data, size_t size) {

  if (size == 0) {
    return 0;
  }
  netsnmp_pdu *pdu;
  pdu = snmp_pdu_create(SNMP_MSG_TRAP);
  snmp_pdu_parse(pdu, (u_char*)data, &size);
  snmp_free_pdu(pdu);
  return 0;
}

