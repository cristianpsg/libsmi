/*
 * This Java file has been generated by smidump 0.3.0. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row protocolDistControlEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class ProtocolDistControlEntryImpl extends ProtocolDistControlEntry
{

    // constructor
    public ProtocolDistControlEntryImpl(int protocolDistControlIndex)
    {
        super(protocolDistControlIndex);
    }

    public AgentXOID get_protocolDistControlDataSource()
    {
        return protocolDistControlDataSource;
    }

    public int set_protocolDistControlDataSource(AgentXSetPhase phase, AgentXOID value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_protocolDistControlDataSource = protocolDistControlDataSource;
            protocolDistControlDataSource = value;
            break;
        case AgentXSetPhase.UNDO:
            protocolDistControlDataSource = undo_protocolDistControlDataSource;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public long get_protocolDistControlDroppedFrames()
    {
        return protocolDistControlDroppedFrames;
    }

    public long get_protocolDistControlCreateTime()
    {
        return protocolDistControlCreateTime;
    }

    public byte[] get_protocolDistControlOwner()
    {
        return protocolDistControlOwner;
    }

    public int set_protocolDistControlOwner(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_protocolDistControlOwner = protocolDistControlOwner;
            protocolDistControlOwner = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                protocolDistControlOwner[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            protocolDistControlOwner = undo_protocolDistControlOwner;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_protocolDistControlOwner = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_protocolDistControlStatus()
    {
        return protocolDistControlStatus;
    }

    public int set_protocolDistControlStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_protocolDistControlStatus = protocolDistControlStatus;
            protocolDistControlStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            protocolDistControlStatus = undo_protocolDistControlStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

