/*
 * This Java file has been generated by smidump 0.2.8. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row alMatrixSDEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class AlMatrixSDEntryImpl extends AlMatrixSDEntry
{

    // constructor
    public AlMatrixSDEntryImpl(int hlMatrixControlIndex,
                           long alMatrixSDTimeMark,
                           int protocolDirLocalIndex,
                           byte[] nlMatrixSDSourceAddress,
                           byte[] nlMatrixSDDestAddress,
                           int protocolDirLocalIndex)
    {
        super(hlMatrixControlIndex,
                 alMatrixSDTimeMark,
                 protocolDirLocalIndex,
                 nlMatrixSDSourceAddress,
                 nlMatrixSDDestAddress,
                 protocolDirLocalIndex);
    }

    public long get_alMatrixSDPkts()
    {
        return alMatrixSDPkts;
    }

    public long get_alMatrixSDOctets()
    {
        return alMatrixSDOctets;
    }

    public long get_alMatrixSDCreateTime()
    {
        return alMatrixSDCreateTime;
    }

}

