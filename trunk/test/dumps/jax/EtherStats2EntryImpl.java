/*
 * This Java file has been generated by smidump 0.4.5. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row etherStats2Entry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class EtherStats2EntryImpl extends EtherStats2Entry
{

    // constructor
    public EtherStats2EntryImpl()
    {
        super();
    }

    public long get_etherStatsDroppedFrames()
    {
        return etherStatsDroppedFrames;
    }

    public long get_etherStatsCreateTime()
    {
        return etherStatsCreateTime;
    }

}

