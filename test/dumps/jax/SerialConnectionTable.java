/*
 * This Java file has been generated by smidump 0.2.14. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table serialConnectionTable defined in RMON2-MIB.

    @version 1
    @author  smidump 0.2.14
    @see     AgentXTable
 */

import java.util.Vector;

import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXResponsePDU;
import jax.AgentXSetPhase;
import jax.AgentXTable;
import jax.AgentXEntry;

public class SerialConnectionTable extends AgentXTable
{

    // entry OID
    private final static long[] OID = {1, 3, 6, 1, 2, 1, 16, 19, 14, 1};

    // constructors
    public SerialConnectionTable()
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
    }

    public SerialConnectionTable(boolean shared)
    {
        super(shared);

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
    }

    public AgentXVarBind getVarBind(AgentXEntry entry, long column)
    {
        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());

        switch ((int)column) {
        case 2: // serialConnectDestIpAddress
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectDestIpAddress();
            return new AgentXVarBind(oid, AgentXVarBind.IPADDRESS, value);
        }
        case 3: // serialConnectType
        {
            int value = ((SerialConnectionEntry)entry).get_serialConnectType();
            return new AgentXVarBind(oid, AgentXVarBind.INTEGER, value);
        }
        case 4: // serialConnectDialString
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectDialString();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 5: // serialConnectSwitchConnectSeq
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectSwitchConnectSeq();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 6: // serialConnectSwitchDisconnectSeq
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectSwitchDisconnectSeq();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 7: // serialConnectSwitchResetSeq
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectSwitchResetSeq();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 8: // serialConnectOwner
        {
            byte[] value = ((SerialConnectionEntry)entry).get_serialConnectOwner();
            return new AgentXVarBind(oid, AgentXVarBind.OCTETSTRING, value);
        }
        case 9: // serialConnectStatus
        {
            int value = ((SerialConnectionEntry)entry).get_serialConnectStatus();
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
        case 2: // serialConnectDestIpAddress
        {
            if (vb.getType() != AgentXVarBind.IPADDRESS)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectDestIpAddress(phase, vb.bytesValue());
        }
        case 3: // serialConnectType
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectType(phase, vb.intValue());
        }
        case 4: // serialConnectDialString
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectDialString(phase, vb.bytesValue());
        }
        case 5: // serialConnectSwitchConnectSeq
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectSwitchConnectSeq(phase, vb.bytesValue());
        }
        case 6: // serialConnectSwitchDisconnectSeq
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectSwitchDisconnectSeq(phase, vb.bytesValue());
        }
        case 7: // serialConnectSwitchResetSeq
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectSwitchResetSeq(phase, vb.bytesValue());
        }
        case 8: // serialConnectOwner
        {
            if (vb.getType() != AgentXVarBind.OCTETSTRING)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectOwner(phase, vb.bytesValue());
        }
        case 9: // serialConnectStatus
        {
            if (vb.getType() != AgentXVarBind.INTEGER)
                return AgentXResponsePDU.WRONG_TYPE;
            else
                return ((SerialConnectionEntry)entry).set_serialConnectStatus(phase, vb.intValue());
        }
        }

        return AgentXResponsePDU.NOT_WRITABLE;
    }

}

