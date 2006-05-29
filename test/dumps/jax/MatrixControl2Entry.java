/*
 * This Java file has been generated by smidump 0.4.5. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row matrixControl2Entry defined in RMON2-MIB.

    @version 1
    @author  smidump 0.4.5
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class MatrixControl2Entry extends AgentXEntry
{

    protected long matrixControlDroppedFrames = 0;
    protected long matrixControlCreateTime = 0;

    public MatrixControl2Entry()
    {

    }

    public long get_matrixControlDroppedFrames()
    {
        return matrixControlDroppedFrames;
    }

    public long get_matrixControlCreateTime()
    {
        return matrixControlCreateTime;
    }

}

