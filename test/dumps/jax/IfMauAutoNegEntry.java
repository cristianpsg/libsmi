/*
 * This Java file has been generated by smidump 0.2.11. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row ifMauAutoNegEntry defined in MAU-MIB.

    @version 1
    @author  smidump 0.2.11
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class IfMauAutoNegEntry extends AgentXEntry
{

    protected int ifMauAutoNegAdminStatus = 0;
    protected int undo_ifMauAutoNegAdminStatus = 0;
    protected int ifMauAutoNegRemoteSignaling = 0;
    protected int ifMauAutoNegConfig = 0;
    protected int ifMauAutoNegCapability = 0;
    protected int ifMauAutoNegCapAdvertised = 0;
    protected int undo_ifMauAutoNegCapAdvertised = 0;
    protected int ifMauAutoNegCapReceived = 0;
    protected int ifMauAutoNegRestart = 0;
    protected int undo_ifMauAutoNegRestart = 0;
    protected byte[] ifMauAutoNegCapabilityBits = new byte[0];
    protected byte[] ifMauAutoNegCapAdvertisedBits = new byte[0];
    protected byte[] undo_ifMauAutoNegCapAdvertisedBits = new byte[0];
    protected byte[] ifMauAutoNegCapReceivedBits = new byte[0];
    protected int ifMauAutoNegRemoteFaultAdvertised = 0;
    protected int undo_ifMauAutoNegRemoteFaultAdvertised = 0;
    protected int ifMauAutoNegRemoteFaultReceived = 0;
    // foreign indices
    protected int ifMauIfIndex;
    protected int ifMauIndex;

    public IfMauAutoNegEntry(int ifMauIfIndex,
                             int ifMauIndex)
    {
        this.ifMauIfIndex = ifMauIfIndex;
        this.ifMauIndex = ifMauIndex;

        instance.append(ifMauIfIndex);
        instance.append(ifMauIndex);
    }

    public int get_ifMauIfIndex()
    {
        return ifMauIfIndex;
    }

    public int get_ifMauIndex()
    {
        return ifMauIndex;
    }

    public int get_ifMauAutoNegAdminStatus()
    {
        return ifMauAutoNegAdminStatus;
    }

    public int set_ifMauAutoNegAdminStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauAutoNegAdminStatus = ifMauAutoNegAdminStatus;
            ifMauAutoNegAdminStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauAutoNegAdminStatus = undo_ifMauAutoNegAdminStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifMauAutoNegRemoteSignaling()
    {
        return ifMauAutoNegRemoteSignaling;
    }

    public int get_ifMauAutoNegConfig()
    {
        return ifMauAutoNegConfig;
    }

    public int get_ifMauAutoNegCapability()
    {
        return ifMauAutoNegCapability;
    }

    public int get_ifMauAutoNegCapAdvertised()
    {
        return ifMauAutoNegCapAdvertised;
    }

    public int set_ifMauAutoNegCapAdvertised(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauAutoNegCapAdvertised = ifMauAutoNegCapAdvertised;
            ifMauAutoNegCapAdvertised = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauAutoNegCapAdvertised = undo_ifMauAutoNegCapAdvertised;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifMauAutoNegCapReceived()
    {
        return ifMauAutoNegCapReceived;
    }

    public int get_ifMauAutoNegRestart()
    {
        return ifMauAutoNegRestart;
    }

    public int set_ifMauAutoNegRestart(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauAutoNegRestart = ifMauAutoNegRestart;
            ifMauAutoNegRestart = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauAutoNegRestart = undo_ifMauAutoNegRestart;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_ifMauAutoNegCapabilityBits()
    {
        return ifMauAutoNegCapabilityBits;
    }

    public byte[] get_ifMauAutoNegCapAdvertisedBits()
    {
        return ifMauAutoNegCapAdvertisedBits;
    }

    public int set_ifMauAutoNegCapAdvertisedBits(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauAutoNegCapAdvertisedBits = ifMauAutoNegCapAdvertisedBits;
            ifMauAutoNegCapAdvertisedBits = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                ifMauAutoNegCapAdvertisedBits[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            ifMauAutoNegCapAdvertisedBits = undo_ifMauAutoNegCapAdvertisedBits;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_ifMauAutoNegCapAdvertisedBits = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_ifMauAutoNegCapReceivedBits()
    {
        return ifMauAutoNegCapReceivedBits;
    }

    public int get_ifMauAutoNegRemoteFaultAdvertised()
    {
        return ifMauAutoNegRemoteFaultAdvertised;
    }

    public int set_ifMauAutoNegRemoteFaultAdvertised(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauAutoNegRemoteFaultAdvertised = ifMauAutoNegRemoteFaultAdvertised;
            ifMauAutoNegRemoteFaultAdvertised = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauAutoNegRemoteFaultAdvertised = undo_ifMauAutoNegRemoteFaultAdvertised;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifMauAutoNegRemoteFaultReceived()
    {
        return ifMauAutoNegRemoteFaultReceived;
    }

}

