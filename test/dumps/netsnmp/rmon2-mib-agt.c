/*
 * This C file has been generated by smidump 0.2.14.
 * It is intended to be used with the NET-SNMP agent library.
 *
 * This C file is derived from the RMON2-MIB module.
 *
 * $Id$
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "rmon2-mib.h"

#include <ucd-snmp/asn1.h>
#include <ucd-snmp/snmp.h>
#include <ucd-snmp/snmp_api.h>
#include <ucd-snmp/snmp_impl.h>
#include <ucd-snmp/snmp_vars.h>

static oid rmon2_mib_caps[] = {0,0};

void init_rmon2_mib(void)
{
}

void deinit_rmon2_mib()
{
    unregister_sysORTable(rmon2_mib_caps, sizeof(rmon2_mib_caps));
}

int term_rmon2_mib()
{
    deinit_rmon2_mib();
    return 0;
}

