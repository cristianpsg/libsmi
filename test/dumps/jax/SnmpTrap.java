/*
 * This Java file has been generated by smidump 0.3.0. Do not edit!
 * It is intended to be used within a Java AgentX sub-agent environment.
 *
 * $Id$
 */

/**
    This class represents a Java AgentX (JAX) implementation of
    the scalar group snmpTrap defined in SNMPv2-MIB.

    @version 1
    @author  smidump 0.3.0
    @see     AgentXGroup, AgentXScalars
 */

import java.util.Vector;
import java.util.Enumeration;
import jax.AgentXOID;
import jax.AgentXVarBind;
import jax.AgentXSetPhase;
import jax.AgentXResponsePDU;
import jax.AgentXScalars;

public class SnmpTrap extends AgentXScalars
{

    private final static long[] SnmpTrapOID = {1, 3, 6, 1, 6, 3, 1, 1, 4};

    protected AgentXOID SnmpTrapOIDOID;
    protected final static long[] SnmpTrapOIDName = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};
    protected AgentXOID snmpTrapOID = new AgentXOID();
    protected AgentXOID SnmpTrapEnterpriseOID;
    protected final static long[] SnmpTrapEnterpriseName = {1, 3, 6, 1, 6, 3, 1, 1, 4, 3, 0};
    protected AgentXOID snmpTrapEnterprise = new AgentXOID();
    public SnmpTrap()
    {
        oid = new AgentXOID(SnmpTrapOID);
        data = new Vector();
        SnmpTrapOIDOID = new AgentXOID(SnmpTrapOIDName);
        data.addElement(SnmpTrapOIDOID);
        SnmpTrapEnterpriseOID = new AgentXOID(SnmpTrapEnterpriseName);
        data.addElement(SnmpTrapEnterpriseOID);
    }

    public AgentXOID get_snmpTrapOID()
    {
        return snmpTrapOID;
    }

    public AgentXOID get_snmpTrapEnterprise()
    {
        return snmpTrapEnterprise;
    }

    public AgentXVarBind getScalar(AgentXOID pos, AgentXOID oid)
    {
        if ((pos == null) || (pos.compareTo(oid) != 0))
            return new AgentXVarBind(oid, AgentXVarBind.NOSUCHOBJECT);
        else {
            if (pos == SnmpTrapOIDOID)
                return new AgentXVarBind(oid, AgentXVarBind.OBJECTIDENTIFIER, 
                                         get_snmpTrapOID());
            if (pos == SnmpTrapEnterpriseOID)
                return new AgentXVarBind(oid, AgentXVarBind.OBJECTIDENTIFIER, 
                                         get_snmpTrapEnterprise());
        }
        return new AgentXVarBind(oid, AgentXVarBind.NOSUCHOBJECT);
    }

    public int setScalar(AgentXSetPhase phase, AgentXOID pos,
                         AgentXVarBind inVb)
    {
        if ((pos == null) || (pos.compareTo(inVb.getOID()) != 0))
            return AgentXResponsePDU.INCONSISTENT_NAME;
        else {
        }
        return AgentXResponsePDU.NOT_WRITABLE;
    }

    public AgentXVarBind getNextScalar(AgentXOID pos, AgentXOID oid)
    {
        if ((pos == null) || (pos.compareTo(oid) <= 0))
            return new AgentXVarBind(oid, AgentXVarBind.ENDOFMIBVIEW);
        else {
            if (pos == SnmpTrapOIDOID)
                return new AgentXVarBind(pos, AgentXVarBind.OBJECTIDENTIFIER, 
                                         get_snmpTrapOID());
            if (pos == SnmpTrapEnterpriseOID)
                return new AgentXVarBind(pos, AgentXVarBind.OBJECTIDENTIFIER, 
                                         get_snmpTrapEnterprise());
        }
        return new AgentXVarBind(pos, AgentXVarBind.ENDOFMIBVIEW);
    }

}

