/*
 * This Java file has been generated by smidump 0.2.14. It
 * is intended to be edited by the application programmer and
 * to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class extends the Java AgentX (JAX) implementation of
    the scalar group probeConfig defined in RMON2-MIB.
 */

import java.util.Vector;
import java.util.Enumeration;
import jax.AgentXOID;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;

public class ProbeConfigImpl extends ProbeConfig
{

    public byte[] get_probeCapabilities()
    {
        return probeCapabilities;
    }

    public byte[] get_probeSoftwareRev()
    {
        return probeSoftwareRev;
    }

    public byte[] get_probeHardwareRev()
    {
        return probeHardwareRev;
    }

    public byte[] get_probeDateTime()
    {
        return probeDateTime;
    }

    public int set_probeDateTime(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_probeDateTime = probeDateTime;
            probeDateTime = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                probeDateTime[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            probeDateTime = undo_probeDateTime;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_probeDateTime = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }

    public int get_probeResetControl()
    {
        return probeResetControl;
    }

    public int set_probeResetControl(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_probeResetControl = probeResetControl;
            probeResetControl = value;
            break;
        case AgentXSetPhase.UNDO:
            probeResetControl = undo_probeResetControl;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_probeResetControl = -1; // TODO: better check!
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }

    public byte[] get_probeDownloadFile()
    {
        return probeDownloadFile;
    }

    public int set_probeDownloadFile(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_probeDownloadFile = probeDownloadFile;
            probeDownloadFile = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                probeDownloadFile[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            probeDownloadFile = undo_probeDownloadFile;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_probeDownloadFile = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }

    public byte[] get_probeDownloadTFTPServer()
    {
        return probeDownloadTFTPServer;
    }

    public int set_probeDownloadTFTPServer(AgentXSetPhase phase, byte[] value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_probeDownloadTFTPServer = probeDownloadTFTPServer;
            probeDownloadTFTPServer = new byte[value.length];
            for(int i = 0; i < value.length; i++)
                probeDownloadTFTPServer[i] = value[i];
            break;
        case AgentXSetPhase.UNDO:
            probeDownloadTFTPServer = undo_probeDownloadTFTPServer;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_probeDownloadTFTPServer = null;
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }

    public int get_probeDownloadAction()
    {
        return probeDownloadAction;
    }

    public int set_probeDownloadAction(AgentXSetPhase phase, int value)
    {
        switch (phase.getPhase()) {
        case AgentXSetPhase.TEST_SET:
            break;
        case AgentXSetPhase.COMMIT:
            undo_probeDownloadAction = probeDownloadAction;
            probeDownloadAction = value;
            break;
        case AgentXSetPhase.UNDO:
            probeDownloadAction = undo_probeDownloadAction;
            break;
        case AgentXSetPhase.CLEANUP:
            undo_probeDownloadAction = -1; // TODO: better check!
            break;
        default:
            return AgentXResponsePDU.PROCESSING_ERROR;
        }
        return AgentXResponsePDU.NO_ERROR;
    }

    public int get_probeDownloadStatus()
    {
        return probeDownloadStatus;
    }

}

