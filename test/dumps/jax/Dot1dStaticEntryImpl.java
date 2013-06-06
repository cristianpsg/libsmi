/*
 * This Java file has been generated by smidump 0.5.0. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row dot1dStaticEntry defined in BRIDGE-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class Dot1dStaticEntryImpl extends Dot1dStaticEntry
{

    // constructor
    public Dot1dStaticEntryImpl(byte[] dot1dStaticAddress,
                            int dot1dStaticReceivePort)
    {
        super(dot1dStaticAddress,
                  dot1dStaticReceivePort);
    }

    public byte[] get_dot1dStaticAddress()
    {
        return dot1dStaticAddress;
    }

    public int set_dot1dStaticAddress(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_dot1dStaticAddress = dot1dStaticAddress;
            dot1dStaticAddress = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                dot1dStaticAddress[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            dot1dStaticAddress = undo_dot1dStaticAddress;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_dot1dStaticAddress = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_dot1dStaticReceivePort()
    {
        return dot1dStaticReceivePort;
    }

    public int set_dot1dStaticReceivePort(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_dot1dStaticReceivePort = dot1dStaticReceivePort;
            dot1dStaticReceivePort = value;
            break;
        case AgentXSetPhase.UNDO:
            dot1dStaticReceivePort = undo_dot1dStaticReceivePort;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public byte[] get_dot1dStaticAllowedToGoTo()
    {
        return dot1dStaticAllowedToGoTo;
    }

    public int set_dot1dStaticAllowedToGoTo(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_dot1dStaticAllowedToGoTo = dot1dStaticAllowedToGoTo;
            dot1dStaticAllowedToGoTo = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                dot1dStaticAllowedToGoTo[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            dot1dStaticAllowedToGoTo = undo_dot1dStaticAllowedToGoTo;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_dot1dStaticAllowedToGoTo = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_dot1dStaticStatus()
    {
        return dot1dStaticStatus;
    }

    public int set_dot1dStaticStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_dot1dStaticStatus = dot1dStaticStatus;
            dot1dStaticStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            dot1dStaticStatus = undo_dot1dStaticStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

