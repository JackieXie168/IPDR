package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Hashtable;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterException;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.configuration.SessionConfigI;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.DataException;
import org.ipdr.protocol.InvalidSessionStateException;
import org.ipdr.protocol.ProtocolDataBigEndian;
import org.ipdr.protocol.ProtocolFieldTypes;
import org.ipdr.protocol.ProtocolMsgDATA;
import org.ipdr.protocol.ProtocolMsgMODIFY_TEMPLATE_RESPONSE;
import org.ipdr.protocol.ProtocolMsgSESSION_START;
import org.ipdr.protocol.ProtocolMsgTEMPLATE_DATA;
import org.ipdr.protocol.SessionException;
import org.ipdr.utility.IPDRUtilities;

/**
 * Encapsulates session state for a single session
 */
public class IPDRSessionState
{
    public final static int FLOW_STARTED = 0;

    public final static int NEGOTIATE_START = 1; // we have received TPL_DATA
                                                 // negotiable, sent
                                                 // MODIFY_TEMPLATE and expect
                                                 // M_T_RESPONSE

    public final static int NEGOTIATE_FINISH = 2; // wait for SESSION_START

    public final static int DATA = 3; //receiving and processing data

    public final static int RE_NEGOTIATING = 4; // we have asked a
                                                // re-negotiation and are
                                                // waiting for answer

    public final static int ERROR = 5;

    /**
     * Holds session state
     */
    private int state;

    /**
     * Holds number of processes DATA messages since last acknowledge
     */
    private int dataAckCount;

    /**
     * Holds number of DATA messages after which the acknowledge should be sent
     */
    private int ackSequenceInterval;

    /**
     * Holds time interval after which a data acknowledge message should be sent
     */
    private int ackTimeInterval;

    /**
     * Holds exporter's boot time as provided in SESSION_START message
     */
    private int exporterBootTime;

    /**
     * Holds first record sequence number to expect
     */
    private long firstRecordSequenceNumber;

    /**
     * Holds current record sequence number
     */
    private long currentRecordSequenceNumber;

    /**
     * Holds dropped record count, as provided by Exporter
     */
    private long droppedRecordCount;

    /**
     * Indicates that this collector is the primary one
     */
    private boolean isPrimary;

    /**
     * Holds document id as provided by the exporter
     */
    private byte[] documentId;

    /**
     * Holds config id as provided by the exporter
     */
    private short configId;

    /**
     * Holds templates, mapped by template id
     */
    private Hashtable templateMap = new Hashtable();

    /**
     * Holds configuration of the active session
     */
    private SessionConfigI sessionConfiguration;

    /**
     * Holds the parent connection
     */
    private IPDRConnectionState parent;

    /**
     * Initializes the session state holder with given session configuration and
     * <code>IPDRConnectionState</code>
     * 
     * @param sessionConfiguration -
     *            session configuration
     * @param parent -
     *            <code>IPDRConnectionState</code> instance
     */
    public IPDRSessionState(SessionConfigI sessionConfiguration,
            IPDRConnectionState parent)
    {
        this.sessionConfiguration = sessionConfiguration;
        this.parent = parent;
        this.state = FLOW_STARTED;
        this.dataAckCount = 0;
    }

    /**
     * Retrieves session id
     * 
     * @return Session id
     */
    public byte getSessionId()
    {
        return this.sessionConfiguration.getSessionId();
    }

    /**
     * Processes the SESSION_START message
     * 
     * @param message
     */
    public void processSessionStart(ProtocolMsgSESSION_START message)
    {
        this.exporterBootTime = message.getExporterBootTime();
        this.firstRecordSequenceNumber = message.getFirstRecordSequenceNumber();
        this.droppedRecordCount = message.getDroppedRecordCount();
        this.isPrimary = message.isPrimary();
        this.ackTimeInterval = message.getAckTimeInterval();
        this.ackSequenceInterval = message.getAckSequenceInterval();
        this.documentId = message.getDocumentId();
    }

    /**
     * Resets session data acknowledge counter
     */
    public void resetSessionAcknowledge()
    {
        this.dataAckCount = 0;
    }

    /**
     * Registers templates as provided in MODIFY_TEMPLATE_RESPONSE message
     * 
     * @param message
     *            MODIFY_TEMPLATE_RESPONSE message
     * @throws ConfigException
     */
    public void processModifyTemplateResponse(
            ProtocolMsgMODIFY_TEMPLATE_RESPONSE message) throws ConfigException
    {
        this.setTemplates(message.getTemplates());
    }

    /**
     * Registers templates as provided in TEMPLATE_DATA message
     * 
     * @param message
     *            TEMPLATE_DATA message
     * @throws ConfigException
     */
    public void processTemplateData(ProtocolMsgTEMPLATE_DATA message)
            throws ConfigException
    {
        this.setTemplates(message.getTemplates());
    }

    /**
     * Stores the actual session templates
     * 
     * @param templates
     *            <code>ArrayList</code> of templates to store
     * @throws ConfigException
     */
    protected void setTemplates(ArrayList templates) throws ConfigException
    {
        templateMap.clear();

        for (int i = 0; i < templates.size(); i++)
        {
            TemplateConfig template = (TemplateConfig) templates.get(i);
            template.validateSelf();
            templateMap.put(new Short(template.getId()), template);
        }
    }

    /**
     * Increase the DATA acknowledge counter by 1. Returns true if the data
     * message should be acknowledged
     * 
     * @return true if DATA_ACK should be sent, false otherwise
     */
    public boolean acknowledge()
    {
        dataAckCount++;
        if (dataAckCount >= ackSequenceInterval)
            return true;
        return false;
    }

    /**
     * Returns the state.
     * 
     * @return int
     */
    public int getState()
    {
        return state;
    }

    /**
     * Sets the state.
     * 
     * @param state
     *            The state to set
     */
    public void setState(int state)
    {
        this.state = state;
    }

    /**
     * Returns the configId.
     * 
     * @return short
     */
    public short getConfigId()
    {
        return configId;
    }

    /**
     * Sets the configId.
     * 
     * @param configId
     *            The configId to set
     */
    public void setConfigId(short configId)
    {
        this.configId = configId;
    }

    /**
     * Outputs the data message
     * 
     * @param data
     *            protocol DATA message
     * @throws SessionException
     * @throws DataException
     * @throws OutputterException
     */
    public void outputDataMessage(ProtocolMsgDATA data)
            throws SessionException, DataException, OutputterException
    {
        setCurrentRecordSequenceNumber(data.getDataSeqNo());

        TemplateConfig template = (TemplateConfig) this.templateMap
                .get(new Short(data.getTemplateId()));
        if (template == null)
            throw new InvalidSessionStateException(data.getSessionId(),
                    "Template not found");

        Outputter outputter = parent.getOutputter();
        ProtocolDataBigEndian bigE = new ProtocolDataBigEndian();
        ConnectionConfigI connectionConfig = sessionConfiguration
                .getConnectionConfig();

        outputter.startRecord(parent.getRealExporterAddress(),
                parent.getRealExporterPort(), IPDRUtilities
                        .dateFromSeconds(this.exporterBootTime), data
                        .getDataSeqNo(), this.getConfigId(), data.getSessionId(), data
                        .getTemplateId(), getDocumentId(), data.isDuplicateFlag());

        int fieldIdListLength = template.getFieldListLength();
        int offset = data.getRawDataOffset();

        try
        {
            for (int i = 0; i < fieldIdListLength; i++)
            {
                FieldConfigI field = template.getFieldConfigAt(i);
                boolean isEnabledForOutput = field.isEnabledForOutput();
                if (LogManager.debug())
                {
                    LogManager.debug(this, "outputDataRecord", "field #" + i
                            + " offset=" + offset + ": ");
                }
                if (!isEnabledForOutput)
                {
                    LogManager.debug(this, "outputDataRecord", "Skipping the field");
                    continue;
                }

                switch (field.getTypeId())
                {
                case ProtocolFieldTypes.TYPE_BOOLEAN:
                    if (isEnabledForOutput)
                        outputter.outputBoolean(i, field, bigE.getByte(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.BOOLEAN_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_BYTE:
                    if (isEnabledForOutput)
                        outputter.outputByte(i, field, bigE.getByte(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.BYTE_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_DATETIME:
                    if (isEnabledForOutput)
                        outputter.outputDateTimeSec(i, field, bigE.getInt(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.DATETIME_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_DATETIMEMSEC:
                    if (isEnabledForOutput)
                        outputter.outputDateTimeMsec(i, field, bigE.getLong(
                                data.getRawData(), offset));
                    offset += ProtocolFieldTypes.DATETIMEMSEC_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_DATETIMEUSEC:
                    if (isEnabledForOutput)
                        outputter.outputDateTimeUsec(i, field, bigE.getLong(
                                data.getRawData(), offset));
                    offset += ProtocolFieldTypes.DATETIMEUSEC_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_DOUBLE:
                    if (isEnabledForOutput)
                        outputter.outputDouble(i, field, bigE.getDouble(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.DOUBLE_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_FLOAT:
                    if (isEnabledForOutput)
                        outputter.outputFloat(i, field, bigE.getFloat(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.FLOAT_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_INT:
                    if (isEnabledForOutput)
                        outputter.outputInt(i, field, bigE.getInt(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.INT_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_IP4ADDR:
                    if (isEnabledForOutput)
                        outputter.outputIPv4Addr(i, field, bigE.getAddr4(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.IP4ADDR_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_IP6ADDR:
                    if (isEnabledForOutput)
                        outputter.outputIPv6Addr(i, field, bigE.getAddr6(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.IP6ADDR_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_LONG:
                    if (isEnabledForOutput)
                        outputter.outputLong(i, field, bigE.getLong(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.LONG_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_MACADDRESS:
                    if (isEnabledForOutput)
                        outputter.outputMACAddress(i, field, bigE
                                .getLong(data.getRawData(), offset)); // Chnaged from getInt to be getLong (Shaig)
                    offset += ProtocolFieldTypes.MACADDRESS_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_SHORT:
                    if (isEnabledForOutput)
                        outputter.outputShort(i, field, bigE.getShort(data
                                .getRawData(), offset));
                    offset += ProtocolFieldTypes.SHORT_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_HEXBINARY: // Shaig (Was not implemented - behaved lik a string)
                {
                    int stringLen = bigE.getInt(data.getRawData(), offset);
                    offset += ProtocolFieldTypes.INT_LEN;
                    if (isEnabledForOutput)
                        outputter.outputHexBinary(i, field, stringLen, bigE.getFixedByteArray(
                                data.getRawData(), offset, stringLen));
                    offset += stringLen;
                }
                break;
                case ProtocolFieldTypes.TYPE_STRING:
                {
                    int stringLen = bigE.getInt(data.getRawData(), offset);
                    offset += ProtocolFieldTypes.INT_LEN;
                    if (isEnabledForOutput)
                        outputter.outputString(i, field, bigE.getStringWithLen(
                                data.getRawData(), stringLen, offset));
                    offset += stringLen;
                }
                break;
                case ProtocolFieldTypes.TYPE_UBYTE:
                    if (isEnabledForOutput)
                        outputter.outputUnsignedByte(i, field, bigE.getByte(
                                data.getRawData(), offset));
                    offset += ProtocolFieldTypes.UBYTE_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_UINT:
                    if (isEnabledForOutput)
                    {
                        byte [] rawBytes = {0,0,0,0,0,0,0,0};
                        System.arraycopy(data.getRawData(), offset, rawBytes, 4, 4);
                        outputter.outputUnsignedInt(i, field, bigE.getLong(rawBytes, 0));
                    }
                    offset += ProtocolFieldTypes.UINT_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_ULONG:
                    if (isEnabledForOutput)
                        outputter.outputUnsignedLong(i, field, bigE.getLong(
                                data.getRawData(), offset));
                    offset += ProtocolFieldTypes.ULONG_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_USHORT:
                    if (isEnabledForOutput)
                        outputter.outputUnsignedShort(i, field, bigE.getShort(
                                data.getRawData(), offset));
                    offset += ProtocolFieldTypes.USHORT_LEN;
                    break;
                case ProtocolFieldTypes.TYPE_UUID:
                    if (isEnabledForOutput)
                        outputter.outputUUID(i, field, bigE.getFixedByteArray(
                                data.getRawData(), offset,
                                ProtocolFieldTypes.UUID_LEN));
                    offset += ProtocolFieldTypes.UUID_LEN;
                    break;
                default:
                    throw new DataException(this, "outputDataRecord",
                            "Unknown type: " + field.getTypeId());
                }
            }
        } catch (IndexOutOfBoundsException e)
        {
            throw new DataException(this, "outputDataMessage", e.getMessage());
        }
        outputter.endRecord();
    }

    /**
     * Returns the currentRecordSequenceNumber.
     * 
     * @return long
     */
    public long getCurrentRecordSequenceNumber()
    {
        return currentRecordSequenceNumber;
    }

    /**
     * Sets the currentRecordSequenceNumber.
     * 
     * @param currentRecordSequenceNumber
     *            The currentRecordSequenceNumber to set
     */
    public void setCurrentRecordSequenceNumber(long currentRecordSequenceNumber)
    {
        this.currentRecordSequenceNumber = currentRecordSequenceNumber;
    }

    /**
     * Returns the ackTimeInterval.
     * 
     * @return int
     */
    public int getAckTimeInterval()
    {
        return ackTimeInterval;
    }

    /**
     * @return Returns the firstRecordSequenceNumber.
     */
    public long getFirstRecordSequenceNumber()
    {
        return this.firstRecordSequenceNumber;
    }

    /**
     * @return Returns the droppedRecordCount.
     */
    public long getDroppedRecordCount()
    {
        return this.droppedRecordCount;
    }

    /**
     * @return Returns the isPrimary.
     */
    public boolean isPrimary()
    {
        return this.isPrimary;
    }

    /**
     * @return Returns the documentId.
     */
    public byte[] getDocumentId()
    {
        return this.documentId;
    }

    /**
     * @return Returns the dataAckCount.
     */
    public int getDataAckCount()
    {
        return dataAckCount;
    }
}

