/*
 * This Java file has been generated by smidump 0.3.0. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the table row alMatrixTopNControlEntry defined in RMON2-MIB.

    @version 1
    @author  smidump 0.3.0
    @see     AgentXTable, AgentXEntry
 */

import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXEntry;

public class AlMatrixTopNControlEntry extends AgentXEntry
{

    protected int alMatrixTopNControlIndex = 0;
    protected int alMatrixTopNControlMatrixIndex = 0;
    protected int undo_alMatrixTopNControlMatrixIndex = 0;
    protected int alMatrixTopNControlRateBase = 0;
    protected int undo_alMatrixTopNControlRateBase = 0;
    protected int alMatrixTopNControlTimeRemaining = 0;
    protected int undo_alMatrixTopNControlTimeRemaining = 0;
    protected long alMatrixTopNControlGeneratedReports = 0;
    protected int alMatrixTopNControlDuration = 0;
    protected int alMatrixTopNControlRequestedSize = 0;
    protected int undo_alMatrixTopNControlRequestedSize = 0;
    protected int alMatrixTopNControlGrantedSize = 0;
    protected long alMatrixTopNControlStartTime = 0;
    protected byte[] alMatrixTopNControlOwner = new byte[0];
    protected byte[] undo_alMatrixTopNControlOwner = new byte[0];
    protected int alMatrixTopNControlStatus = 0;
    protected int undo_alMatrixTopNControlStatus = 0;

    public AlMatrixTopNControlEntry(int alMatrixTopNControlIndex)
    {
        this.alMatrixTopNControlIndex = alMatrixTopNControlIndex;

        instance.append(alMatrixTopNControlIndex);
    }

    public int get_alMatrixTopNControlIndex()
    {
        return alMatrixTopNControlIndex;
    }

    public int get_alMatrixTopNControlMatrixIndex()
    {
        return alMatrixTopNControlMatrixIndex;
    }

    public int set_alMatrixTopNControlMatrixIndex(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlMatrixIndex = alMatrixTopNControlMatrixIndex;
            alMatrixTopNControlMatrixIndex = value;
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlMatrixIndex = undo_alMatrixTopNControlMatrixIndex;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_alMatrixTopNControlRateBase()
    {
        return alMatrixTopNControlRateBase;
    }

    public int set_alMatrixTopNControlRateBase(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlRateBase = alMatrixTopNControlRateBase;
            alMatrixTopNControlRateBase = value;
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlRateBase = undo_alMatrixTopNControlRateBase;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_alMatrixTopNControlTimeRemaining()
    {
        return alMatrixTopNControlTimeRemaining;
    }

    public int set_alMatrixTopNControlTimeRemaining(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlTimeRemaining = alMatrixTopNControlTimeRemaining;
            alMatrixTopNControlTimeRemaining = value;
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlTimeRemaining = undo_alMatrixTopNControlTimeRemaining;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public long get_alMatrixTopNControlGeneratedReports()
    {
        return alMatrixTopNControlGeneratedReports;
    }

    public int get_alMatrixTopNControlDuration()
    {
        return alMatrixTopNControlDuration;
    }

    public int get_alMatrixTopNControlRequestedSize()
    {
        return alMatrixTopNControlRequestedSize;
    }

    public int set_alMatrixTopNControlRequestedSize(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlRequestedSize = alMatrixTopNControlRequestedSize;
            alMatrixTopNControlRequestedSize = value;
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlRequestedSize = undo_alMatrixTopNControlRequestedSize;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_alMatrixTopNControlGrantedSize()
    {
        return alMatrixTopNControlGrantedSize;
    }

    public long get_alMatrixTopNControlStartTime()
    {
        return alMatrixTopNControlStartTime;
    }

    public byte[] get_alMatrixTopNControlOwner()
    {
        return alMatrixTopNControlOwner;
    }

    public int set_alMatrixTopNControlOwner(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlOwner = alMatrixTopNControlOwner;
            alMatrixTopNControlOwner = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                alMatrixTopNControlOwner[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlOwner = undo_alMatrixTopNControlOwner;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_alMatrixTopNControlOwner = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
    public int get_alMatrixTopNControlStatus()
    {
        return alMatrixTopNControlStatus;
    }

    public int set_alMatrixTopNControlStatus(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_alMatrixTopNControlStatus = alMatrixTopNControlStatus;
            alMatrixTopNControlStatus = value;
            break;
        case AgentXSetPhase.UNDO:
            alMatrixTopNControlStatus = undo_alMatrixTopNControlStatus;
            break;
        case AgentXSetPhase.CLEANUP:
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }
}

