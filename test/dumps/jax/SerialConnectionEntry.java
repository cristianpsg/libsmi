/*
 * This Java file has been generated by smidump 0.2.7. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row serialConnectionEntry defined in RMON2-MIB.

    @version 1
    @author  smidump 0.2.7
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class SerialConnectionEntry extends AgentXEntry
{

    protected int serialConnectIndex = 0;
    protected byte[] serialConnectDestIpAddress = new byte[4];
    protected byte[] undo_serialConnectDestIpAddress = new byte[4];
    protected int serialConnectType = 0;
    protected int undo_serialConnectType = 0;
    protected byte[] serialConnectDialString = new byte[0];
    protected byte[] undo_serialConnectDialString = new byte[0];
    protected byte[] serialConnectSwitchConnectSeq = new byte[0];
    protected byte[] undo_serialConnectSwitchConnectSeq = new byte[0];
    protected byte[] serialConnectSwitchDisconnectSeq = new byte[0];
    protected byte[] undo_serialConnectSwitchDisconnectSeq = new byte[0];
    protected byte[] serialConnectSwitchResetSeq = new byte[0];
    protected byte[] undo_serialConnectSwitchResetSeq = new byte[0];
    protected byte[] serialConnectOwner = new byte[0];
    protected byte[] undo_serialConnectOwner = new byte[0];
    protected int serialConnectStatus = 0;
    protected int undo_serialConnectStatus = 0;

    public SerialConnectionEntry(int serialConnectIndex)
    {
        this.serialConnectIndex = serialConnectIndex;

        instance.append(serialConnectIndex);
    }

    public int get_serialConnectIndex()
    {
        return serialConnectIndex;
    }

    public byte[] get_serialConnectDestIpAddress()
    {
        return serialConnectDestIpAddress;
    }

    public int set_serialConnectDestIpAddress(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectDestIpAddress = serialConnectDestIpAddress;
            serialConnectDestIpAddress = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectDestIpAddress[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectDestIpAddress = undo_serialConnectDestIpAddress;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectDestIpAddress = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_serialConnectType()
    {
        return serialConnectType;
    }

    public int set_serialConnectType(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectType = serialConnectType;
            serialConnectType = value;
            break;
        case AgentXSetPhase.UNDO:
            serialConnectType = undo_serialConnectType;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_serialConnectDialString()
    {
        return serialConnectDialString;
    }

    public int set_serialConnectDialString(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectDialString = serialConnectDialString;
            serialConnectDialString = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectDialString[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectDialString = undo_serialConnectDialString;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectDialString = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_serialConnectSwitchConnectSeq()
    {
        return serialConnectSwitchConnectSeq;
    }

    public int set_serialConnectSwitchConnectSeq(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectSwitchConnectSeq = serialConnectSwitchConnectSeq;
            serialConnectSwitchConnectSeq = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectSwitchConnectSeq[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectSwitchConnectSeq = undo_serialConnectSwitchConnectSeq;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectSwitchConnectSeq = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_serialConnectSwitchDisconnectSeq()
    {
        return serialConnectSwitchDisconnectSeq;
    }

    public int set_serialConnectSwitchDisconnectSeq(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectSwitchDisconnectSeq = serialConnectSwitchDisconnectSeq;
            serialConnectSwitchDisconnectSeq = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectSwitchDisconnectSeq[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectSwitchDisconnectSeq = undo_serialConnectSwitchDisconnectSeq;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectSwitchDisconnectSeq = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_serialConnectSwitchResetSeq()
    {
        return serialConnectSwitchResetSeq;
    }

    public int set_serialConnectSwitchResetSeq(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectSwitchResetSeq = serialConnectSwitchResetSeq;
            serialConnectSwitchResetSeq = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectSwitchResetSeq[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectSwitchResetSeq = undo_serialConnectSwitchResetSeq;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectSwitchResetSeq = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_serialConnectOwner()
    {
        return serialConnectOwner;
    }

    public int set_serialConnectOwner(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectOwner = serialConnectOwner;
            serialConnectOwner = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                serialConnectOwner[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            serialConnectOwner = undo_serialConnectOwner;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_serialConnectOwner = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_serialConnectStatus()
    {
        return serialConnectStatus;
    }

    public int set_serialConnectStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_serialConnectStatus = serialConnectStatus;
            serialConnectStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            serialConnectStatus = undo_serialConnectStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

