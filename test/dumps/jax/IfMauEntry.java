/*
 * This Java file has been generated by smidump 0.2.14. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row ifMauEntry defined in MAU-MIB.

    @version 1
    @author  smidump 0.2.14
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class IfMauEntry extends AgentXEntry
{

    protected int ifMauIfIndex = 0;
    protected int ifMauIndex = 0;
    protected AgentXOID ifMauType = new AgentXOID();
    protected int ifMauStatus = 0;
    protected int undo_ifMauStatus = 0;
    protected int ifMauMediaAvailable = 0;
    protected long ifMauMediaAvailableStateExits = 0;
    protected int ifMauJabberState = 0;
    protected long ifMauJabberingStateEnters = 0;
    protected long ifMauFalseCarriers = 0;
    protected int ifMauTypeList = 0;
    protected AgentXOID ifMauDefaultType = new AgentXOID();
    protected AgentXOID undo_ifMauDefaultType = new AgentXOID();
    protected int ifMauAutoNegSupported = 0;
    protected byte[] ifMauTypeListBits = new byte[0];

    public IfMauEntry(int ifMauIfIndex,
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

    public int get_ifMauIfIndex()
    {
        return ifMauIfIndex;
    }

    public int get_ifMauIndex()
    {
        return ifMauIndex;
    }

    public AgentXOID get_ifMauType()
    {
        return ifMauType;
    }

    public int get_ifMauStatus()
    {
        return ifMauStatus;
    }

    public int set_ifMauStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauStatus = ifMauStatus;
            ifMauStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauStatus = undo_ifMauStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifMauMediaAvailable()
    {
        return ifMauMediaAvailable;
    }

    public long get_ifMauMediaAvailableStateExits()
    {
        return ifMauMediaAvailableStateExits;
    }

    public int get_ifMauJabberState()
    {
        return ifMauJabberState;
    }

    public long get_ifMauJabberingStateEnters()
    {
        return ifMauJabberingStateEnters;
    }

    public long get_ifMauFalseCarriers()
    {
        return ifMauFalseCarriers;
    }

    public int get_ifMauTypeList()
    {
        return ifMauTypeList;
    }

    public AgentXOID get_ifMauDefaultType()
    {
        return ifMauDefaultType;
    }

    public int set_ifMauDefaultType(AgentXSetPhase phase, AgentXOID value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifMauDefaultType = ifMauDefaultType;
            ifMauDefaultType = value;
            break;
        case AgentXSetPhase.UNDO:
            ifMauDefaultType = undo_ifMauDefaultType;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifMauAutoNegSupported()
    {
        return ifMauAutoNegSupported;
    }

    public byte[] get_ifMauTypeListBits()
    {
        return ifMauTypeListBits;
    }

}

