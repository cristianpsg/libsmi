/*
 * This Java file has been generated by smidump 0.2.10. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row protocolDistStatsEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class ProtocolDistStatsEntryImpl extends ProtocolDistStatsEntry
{

    // constructor
    public ProtocolDistStatsEntryImpl(int protocolDistControlIndex,
                                  int protocolDirLocalIndex)
    {
        super(protocolDistControlIndex,
                        protocolDirLocalIndex);
    }

    public long get_protocolDistStatsPkts()
    {
        return protocolDistStatsPkts;
    }

    public long get_protocolDistStatsOctets()
    {
        return protocolDistStatsOctets;
    }

}

