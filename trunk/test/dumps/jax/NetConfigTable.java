/*
 * This Java file has been generated by smidump 0.3.0. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table netConfigTable defined in RMON2-MIB.

    @version 1
    @author  smidump 0.3.0
    @see     AgentXTable
 */

import java.util.Vector;

import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXResponsePDU;
import jax.AgentXSetPhase;
import jax.AgentXTable;
import jax.AgentXEntry;

public class NetConfigTable extends AgentXTable
{

    // entry OID
    private final static long[] OID = {1, 3, 6, 1, 2, 1, 16, 19, 11, 1};

    // constructors
    public NetConfigTable()
    {
        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(1));
        columns.addElement(new Long(2));
        columns.addElement(new Long(3));
    }

    public NetConfigTable(boolean shared)
    {
        super(shared);

        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(1));
        columns.addElement(new Long(2));
        columns.addElement(new Long(3));
    }

    public AgentXVarBind getVarBind(AgentXEntry entry, long column)
    {
        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());

        switch ((int)column) {
        case 1: // netConfigIPAddress
        {
            byte[] value = ((NetConfigEntry)entry).get_netConfigIPAddress();
            return new AgentXVarBind(oid, AgentXVarBind.IPADDRESS, value);
        }
        case 2: // netConfigSubnetMask
        {
            byte[] value = ((NetConfigEntry)entry).get_netConfigSubnetMask();
            return new AgentXVarBind(oid, AgentXVarBind.IPADDRESS, value);
        }
        case 3: // netConfigStatus
        {
            int value = ((NetConfigEntry)entry).get_netConfigStatus();
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
        case 1: // netConfigIPAddress
        {
            if (vb.getType() != AgentXVarBind.IPADDRESS)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((NetConfigEntry)entry).set_netConfigIPAddress(phase, vb.bytesValue());
        }
        case 2: // netConfigSubnetMask
        {
            if (vb.getType() != AgentXVarBind.IPADDRESS)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((NetConfigEntry)entry).set_netConfigSubnetMask(phase, vb.bytesValue());
        }
        case 3: // netConfigStatus
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((NetConfigEntry)entry).set_netConfigStatus(phase, vb.intValue());
        }
        }

        return AgentXResponsePDU.NOT_WRITABLE;
    }

}

