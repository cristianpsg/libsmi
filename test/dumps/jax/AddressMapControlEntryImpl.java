/*
 * This Java file has been generated by smidump 0.2.13. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row addressMapControlEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class AddressMapControlEntryImpl extends AddressMapControlEntry
{

    // constructor
    public AddressMapControlEntryImpl(int addressMapControlIndex)
    {
        super(addressMapControlIndex);
    }

    public AgentXOID get_addressMapControlDataSource()
    {
        return addressMapControlDataSource;
    }

    public int set_addressMapControlDataSource(AgentXSetPhase phase, AgentXOID value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_addressMapControlDataSource = addressMapControlDataSource;
            addressMapControlDataSource = value;
            break;
        case AgentXSetPhase.UNDO:
            addressMapControlDataSource = undo_addressMapControlDataSource;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public long get_addressMapControlDroppedFrames()
    {
        return addressMapControlDroppedFrames;
    }

    public byte[] get_addressMapControlOwner()
    {
        return addressMapControlOwner;
    }

    public int set_addressMapControlOwner(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_addressMapControlOwner = addressMapControlOwner;
            addressMapControlOwner = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                addressMapControlOwner[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            addressMapControlOwner = undo_addressMapControlOwner;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_addressMapControlOwner = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_addressMapControlStatus()
    {
        return addressMapControlStatus;
    }

    public int set_addressMapControlStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_addressMapControlStatus = addressMapControlStatus;
            addressMapControlStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            addressMapControlStatus = undo_addressMapControlStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

