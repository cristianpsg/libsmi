/*
 * This Java file has been generated by smidump 0.2.10. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row ifJackEntry defined in MAU-MIB.

    @version 1
    @author  smidump 0.2.10
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class IfJackEntry extends AgentXEntry
{

    protected int ifJackIndex = 0;
    protected int ifJackType = 0;
    // foreign indices
    protected int ifMauIfIndex;
    protected int ifMauIndex;

    public IfJackEntry(int ifMauIfIndex,
                       int ifMauIndex,
                       int ifJackIndex)
    {
        this.ifMauIfIndex = ifMauIfIndex;
        this.ifMauIndex = ifMauIndex;
        this.ifJackIndex = ifJackIndex;

        instance.append(ifMauIfIndex);
        instance.append(ifMauIndex);
        instance.append(ifJackIndex);
    }

    public int get_ifMauIfIndex()
    {
        return ifMauIfIndex;
    }

    public int get_ifMauIndex()
    {
        return ifMauIndex;
    }

    public int get_ifJackIndex()
    {
        return ifJackIndex;
    }

    public int get_ifJackType()
    {
        return ifJackType;
    }

}

