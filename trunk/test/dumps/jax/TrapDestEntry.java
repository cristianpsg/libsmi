/*
 * This Java file has been generated by smidump 0.2.14. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row trapDestEntry defined in RMON2-MIB.

    @version 1
    @author  smidump 0.2.14
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class TrapDestEntry extends AgentXEntry
{

    protected int trapDestIndex = 0;
    protected byte[] trapDestCommunity = new byte[0];
    protected byte[] undo_trapDestCommunity = new byte[0];
    protected int trapDestProtocol = 0;
    protected int undo_trapDestProtocol = 0;
    protected byte[] trapDestAddress = new byte[0];
    protected byte[] undo_trapDestAddress = new byte[0];
    protected byte[] trapDestOwner = new byte[0];
    protected byte[] undo_trapDestOwner = new byte[0];
    protected int trapDestStatus = 0;
    protected int undo_trapDestStatus = 0;

    public TrapDestEntry(int trapDestIndex)
    {
        this.trapDestIndex = trapDestIndex;

        instance.append(trapDestIndex);
    }

    public int get_trapDestIndex()
    {
        return trapDestIndex;
    }

    public byte[] get_trapDestCommunity()
    {
        return trapDestCommunity;
    }

    public int set_trapDestCommunity(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_trapDestCommunity = trapDestCommunity;
            trapDestCommunity = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                trapDestCommunity[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            trapDestCommunity = undo_trapDestCommunity;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_trapDestCommunity = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_trapDestProtocol()
    {
        return trapDestProtocol;
    }

    public int set_trapDestProtocol(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_trapDestProtocol = trapDestProtocol;
            trapDestProtocol = value;
            break;
        case AgentXSetPhase.UNDO:
            trapDestProtocol = undo_trapDestProtocol;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_trapDestAddress()
    {
        return trapDestAddress;
    }

    public int set_trapDestAddress(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_trapDestAddress = trapDestAddress;
            trapDestAddress = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                trapDestAddress[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            trapDestAddress = undo_trapDestAddress;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_trapDestAddress = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_trapDestOwner()
    {
        return trapDestOwner;
    }

    public int set_trapDestOwner(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_trapDestOwner = trapDestOwner;
            trapDestOwner = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                trapDestOwner[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            trapDestOwner = undo_trapDestOwner;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_trapDestOwner = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_trapDestStatus()
    {
        return trapDestStatus;
    }

    public int set_trapDestStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_trapDestStatus = trapDestStatus;
            trapDestStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            trapDestStatus = undo_trapDestStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

