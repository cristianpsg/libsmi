/*
 * This Java file has been generated by smidump 0.4.2-pre1. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table nlMatrixDSTable defined in RMON2-MIB.

    @version 1
    @author  smidump 0.4.2-pre1
    @see     AgentXTable
 */

import java.util.Vector;

import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXResponsePDU;
import jax.AgentXSetPhase;
import jax.AgentXTable;
import jax.AgentXEntry;

public class NlMatrixDSTable extends AgentXTable
{

    // entry OID
    private final static long[] OID = {1, 3, 6, 1, 2, 1, 16, 15, 3, 1};

    // constructors
    public NlMatrixDSTable()
    {
        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(4));
        columns.addElement(new Long(5));
        columns.addElement(new Long(6));
    }

    public NlMatrixDSTable(boolean shared)
    {
        super(shared);

        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(4));
        columns.addElement(new Long(5));
        columns.addElement(new Long(6));
    }

    public AgentXVarBind getVarBind(AgentXEntry entry, long column)
    {
        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());

        switch ((int)column) {
        case 4: // nlMatrixDSPkts
        {
            long value = ((NlMatrixDSEntry)entry).get_nlMatrixDSPkts();
            return new AgentXVarBind(oid, AgentXVarBind.GAUGE32, value);
        }
        case 5: // nlMatrixDSOctets
        {
            long value = ((NlMatrixDSEntry)entry).get_nlMatrixDSOctets();
            return new AgentXVarBind(oid, AgentXVarBind.GAUGE32, value);
        }
        case 6: // nlMatrixDSCreateTime
        {
            long value = ((NlMatrixDSEntry)entry).get_nlMatrixDSCreateTime();
            return new AgentXVarBind(oid, AgentXVarBind.GAUGE32, value);
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
        }

        return AgentXResponsePDU.NOT_WRITABLE;
    }

}

