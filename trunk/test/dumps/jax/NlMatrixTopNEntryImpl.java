/*
 * This Java file has been generated by smidump 0.2.10. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the table row nlMatrixTopNEntry defined in RMON2-MIB.
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class NlMatrixTopNEntryImpl extends NlMatrixTopNEntry
{

    // constructor
    public NlMatrixTopNEntryImpl(int nlMatrixTopNControlIndex,
                             int nlMatrixTopNIndex)
    {
        super(nlMatrixTopNControlIndex,
                   nlMatrixTopNIndex);
    }

    public int get_nlMatrixTopNProtocolDirLocalIndex()
    {
        return nlMatrixTopNProtocolDirLocalIndex;
    }

    public byte[] get_nlMatrixTopNSourceAddress()
    {
        return nlMatrixTopNSourceAddress;
    }

    public byte[] get_nlMatrixTopNDestAddress()
    {
        return nlMatrixTopNDestAddress;
    }

    public long get_nlMatrixTopNPktRate()
    {
        return nlMatrixTopNPktRate;
    }

    public long get_nlMatrixTopNReversePktRate()
    {
        return nlMatrixTopNReversePktRate;
    }

    public long get_nlMatrixTopNOctetRate()
    {
        return nlMatrixTopNOctetRate;
    }

    public long get_nlMatrixTopNReverseOctetRate()
    {
        return nlMatrixTopNReverseOctetRate;
    }

}

