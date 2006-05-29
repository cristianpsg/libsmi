/*
 * This Java file has been generated by smidump 0.4.5. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row alHostEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class AlHostEntryImpl extends AlHostEntry
{

    // constructor
    public AlHostEntryImpl(int hlHostControlIndex,
                       long alHostTimeMark,
                       int protocolDirLocalIndex,
                       byte[] nlHostAddress,
                       int protocolDirLocalIndex)
    {
        super(hlHostControlIndex,
             alHostTimeMark,
             protocolDirLocalIndex,
             nlHostAddress,
             protocolDirLocalIndex);
    }

    public long get_alHostInPkts()
    {
        return alHostInPkts;
    }

    public long get_alHostOutPkts()
    {
        return alHostOutPkts;
    }

    public long get_alHostInOctets()
    {
        return alHostInOctets;
    }

    public long get_alHostOutOctets()
    {
        return alHostOutOctets;
    }

    public long get_alHostCreateTime()
    {
        return alHostCreateTime;
    }

}

