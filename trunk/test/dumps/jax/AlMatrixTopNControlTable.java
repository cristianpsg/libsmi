/*
 * This Java file has been generated by smidump 0.2.10. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table alMatrixTopNControlTable defined in RMON2-MIB.

    @version 1
    @author  smidump 0.2.10
    @see     AgentXTable
 */

import java.util.Vector;

import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXResponsePDU;
import jax.AgentXSetPhase;
import jax.AgentXTable;
import jax.AgentXEntry;

public class AlMatrixTopNControlTable extends AgentXTable
{

    // entry OID
    private final static long[] OID = {1, 3, 6, 1, 2, 1, 16, 17, 3, 1};

    // constructor
    public AlMatrixTopNControlTable()
    {
        oid = new AgentXOID(OID);

        // register implemented columns
        columns.addElement(new Long(2));
        columns.addElement(new Long(3));
        columns.addElement(new Long(4));
        columns.addElement(new Long(5));
        columns.addElement(new Long(6));
        columns.addElement(new Long(7));
        columns.addElement(new Long(8));
        columns.addElement(new Long(9));
        columns.addElement(new Long(10));
        columns.addElement(new Long(11));
    }

    public AgentXVarBind getVarBind(AgentXEntry entry, long column)
    {
        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());

        switch ((int)column) {
        case 2: // alMatrixTopNControlMatrixIndex
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlMatrixIndex();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 3: // alMatrixTopNControlRateBase
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlRateBase();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 4: // alMatrixTopNControlTimeRemaining
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlTimeRemaining();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 5: // alMatrixTopNControlGeneratedReports
        {
            long value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlGeneratedReports();
            return new AgentXVarBind(oid, AgentXVarBind.COUNTER32, value);
        }
        case 6: // alMatrixTopNControlDuration
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlDuration();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 7: // alMatrixTopNControlRequestedSize
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlRequestedSize();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 8: // alMatrixTopNControlGrantedSize
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlGrantedSize();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 9: // alMatrixTopNControlStartTime
        {
            long value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlStartTime();
            return new AgentXVarBind(oid, AgentXVarBind.TIMETICKS, value);
        }
        case 10: // alMatrixTopNControlOwner
        {
            byte[] value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlOwner();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 11: // alMatrixTopNControlStatus
        {
            int value = ((AlMatrixTopNControlEntry)entry).get_alMatrixTopNControlStatus();
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
        case 2: // alMatrixTopNControlMatrixIndex
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlMatrixIndex(phase, vb.intValue());
        }
        case 3: // alMatrixTopNControlRateBase
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlRateBase(phase, vb.intValue());
        }
        case 4: // alMatrixTopNControlTimeRemaining
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlTimeRemaining(phase, vb.intValue());
        }
        case 7: // alMatrixTopNControlRequestedSize
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlRequestedSize(phase, vb.intValue());
        }
        case 10: // alMatrixTopNControlOwner
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlOwner(phase, vb.bytesValue());
        }
        case 11: // alMatrixTopNControlStatus
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((AlMatrixTopNControlEntry)entry).set_alMatrixTopNControlStatus(phase, vb.intValue());
        }
        }

        return AgentXResponsePDU.NOT_WRITABLE;
    }

}

