/*
 * This Java file has been generated by smidump 0.2.8. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row ifXEntry defined in IF-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class IfXEntryImpl extends IfXEntry
{

    // constructor
    public IfXEntryImpl()
    {
        super();
    }

    public byte[] get_ifName()
    {
        return ifName;
    }

    public long get_ifInMulticastPkts()
    {
        return ifInMulticastPkts;
    }

    public long get_ifInBroadcastPkts()
    {
        return ifInBroadcastPkts;
    }

    public long get_ifOutMulticastPkts()
    {
        return ifOutMulticastPkts;
    }

    public long get_ifOutBroadcastPkts()
    {
        return ifOutBroadcastPkts;
    }

    public long get_ifHCInOctets()
    {
        return ifHCInOctets;
    }

    public long get_ifHCInUcastPkts()
    {
        return ifHCInUcastPkts;
    }

    public long get_ifHCInMulticastPkts()
    {
        return ifHCInMulticastPkts;
    }

    public long get_ifHCInBroadcastPkts()
    {
        return ifHCInBroadcastPkts;
    }

    public long get_ifHCOutOctets()
    {
        return ifHCOutOctets;
    }

    public long get_ifHCOutUcastPkts()
    {
        return ifHCOutUcastPkts;
    }

    public long get_ifHCOutMulticastPkts()
    {
        return ifHCOutMulticastPkts;
    }

    public long get_ifHCOutBroadcastPkts()
    {
        return ifHCOutBroadcastPkts;
    }

    public int get_ifLinkUpDownTrapEnable()
    {
        return ifLinkUpDownTrapEnable;
    }

    public int set_ifLinkUpDownTrapEnable(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifLinkUpDownTrapEnable = ifLinkUpDownTrapEnable;
            ifLinkUpDownTrapEnable = value;
            break;
        case AgentXSetPhase.UNDO:
            ifLinkUpDownTrapEnable = undo_ifLinkUpDownTrapEnable;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public long get_ifHighSpeed()
    {
        return ifHighSpeed;
    }

    public int get_ifPromiscuousMode()
    {
        return ifPromiscuousMode;
    }

    public int set_ifPromiscuousMode(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifPromiscuousMode = ifPromiscuousMode;
            ifPromiscuousMode = value;
            break;
        case AgentXSetPhase.UNDO:
            ifPromiscuousMode = undo_ifPromiscuousMode;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_ifConnectorPresent()
    {
        return ifConnectorPresent;
    }

    public byte[] get_ifAlias()
    {
        return ifAlias;
    }

    public int set_ifAlias(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_ifAlias = ifAlias;
            ifAlias = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                ifAlias[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            ifAlias = undo_ifAlias;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_ifAlias = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public long get_ifCounterDiscontinuityTime()
    {
        return ifCounterDiscontinuityTime;
    }

}

