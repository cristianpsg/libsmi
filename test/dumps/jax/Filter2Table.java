/*
 * This Java file has been generated by smidump 0.2.8. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table filter2Table defined in RMON2-MIB.

    @version 1
    @author  smidump 0.2.8
    @see     AgentXTable
 */

import java.util.Vector;

import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXResponsePDU;
import jax.AgentXSetPhase;
import jax.AgentXTable;
import jax.AgentXEntry;

public class Filter2Table extends AgentXTable
{

    // entry OID
    private final static long[] OID = {1, 3, 6, 1, 2, 1, 16, 7, 4, 1};

    // constructor
    public Filter2Table()
    {
        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(1));
        columns.addElement(new Long(2));
    }

    public AgentXVarBind getVarBind(AgentXEntry entry, long column)
    {
        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());

        switch ((int)column) {
        case 1: // filterProtocolDirDataLocalIndex
        {
            int value = ((Filter2Entry)entry).get_filterProtocolDirDataLocalIndex();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 2: // filterProtocolDirLocalIndex
        {
            int value = ((Filter2Entry)entry).get_filterProtocolDirLocalIndex();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        }

        return null;
    }

    public int setEntry(AgentXSetPhase phase,
                        AgentXEntry entry,
                        long column,
                        AgentXVarBind vb)
    {

        switch ((int)column) {
        case 1: // filterProtocolDirDataLocalIndex
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((Filter2Entry)entry).set_filterProtocolDirDataLocalIndex(phase, vb.intValue());
        }
        case 2: // filterProtocolDirLocalIndex
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((Filter2Entry)entry).set_filterProtocolDirLocalIndex(phase, vb.intValue());
        }
        }

        return AgentXResponsePDU.NOT_WRITABLE;
    }

}

