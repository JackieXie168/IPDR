package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.net.UnknownHostException;

import org.ipdr.api.CollectorManager;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.log.LogManager;
import org.ipdr.collector.configuration.CollectorConfigXml;
import org.ipdr.collector.log.CollectorLog;
import org.jdom.DataConversionException;
import org.jdom.JDOMException;

import com.xacct.CRANE.collector.CRANELegacyThread;

/**
 * The class provides test implementation of a basic collector
 */
public class Test {
  /**
   * This is the main entry point of a simple collector implementation.
   *
   * The syntax of collector command line is as follows:
   * <xmp>Test <debug level ([D]ebug | [I]nfo | [W]arn | [E]rror | [F]atal)> <collector config file> <CRANE legacy config file> <output type: xml | csv | null> <output file (CSV) or output prefix(IPDRXML)><IPD RXML config file (IPDR XML only)></xmp>
   * The parameters may only be omitted from the end of the command line, in which case the following default
   * values will be used:
   * <TABLE>
   * <TR><TH>Parameter</TH><TH>Default value</TH></TR>
   * <TR><TD></TD>debug level<TD>[D]ebug</TD></TR>
   * <TR><TD>collector config file</TD><TD>config.xml</TD></TR>
   * <TR><TD>CRANE legacy config file</TD><TD>CRANEConfig.xml</TD></TR>
   * <TR><TD>output type</TD><TD>xml</TD></TR>
   * <TR><TD>output file or output prefix</TD><TD>IPDRXMLOutput</TD></TR>
   * <TR><TD>IPDR XML config file</TD><TD>IPDR35XMLOutputConfig.xml</TD></TR>
   * </TABLE>
   *
   */
  public static void main(String[] args) {
    CollectorLog handler;
    CollectorManager cm     = null;
    CollectorConfigI config = null;
    String fileName         = "config.xml";
    String craneFileName    = "CRANEConfig.xml";
    String outputType       = "xml";
    String outputFileName   = "output.txt";
    String outputPrefix     = "IPDRXMLOutput";
    String outputConfigXML  = "IPDR35XMLOutputConfig.xml";
    char debugLevel         = 'D';

    if (args.length > 0) {   // Get debug level (or help request)
      String tmp = args[0].trim();
      if (tmp.equals("help") || tmp.equals("-help")) {
        help();
        System.exit(0);
      }
      debugLevel = tmp.charAt(0);
    }
    if (args.length > 1)    // Get the configuration file name
      fileName = args[1];
    if (args.length > 2)    // Get the CRANE configuration file name
      craneFileName = args[2];
    if (args.length > 3) {  // Get the outputter type
      outputType = args[3];
      if (!outputType.equals("csv") && !outputType.equals("xml") && !outputType.equals("xml") && !outputType.equals("null")) {
        System.err.print(
            "The fourth parameter (output type) has to be csv or xml or null");
        help();
        System.exit(0);
      }
    }
    if (args.length > 4) {  // According to the outputter's type interpret the next parameter
      if (outputType.equals("csv")) {
        outputFileName = args[4];
      }
      else if (outputType.equals("xml")) {
        outputPrefix = args[4];
      }
    }
    if (args.length > 5) {  // Get the IPDR XML configuration file name
      outputConfigXML = args[5];
    }
//
// Instantiate the Collector Configuration from XML file
//
    try {
      config = new CollectorConfigXml(fileName);
    }
    catch (UnknownHostException e3) {
      e3.printStackTrace();
      return;
    }
    catch (DataConversionException e3) {
      e3.printStackTrace();
      return;
    }
    catch (ConfigException e3) {
      e3.printStackTrace();
      return;
    }
    catch (JDOMException e3) {
      e3.printStackTrace();
      return;
    }
//
// Instantiate the proper outputter
//
    try {
      if (outputType.equals("csv")) {

        PrintStream ps = new PrintStream(new FileOutputStream(outputFileName, false));
        cm = new CollectorManager(config, new CsvOutputterFactory(ps, ","),
                                  new SimpleTemplateNegotiatorFactory(), new SimpleStateMachineListener(), null);
      }
      else if(outputType.equals("xml")) {
        StateMachineListenerManager stateMachineListenersManager = new StateMachineListenerManager();
        cm = new CollectorManager( config, new IPDR35XMLOutputterFactory(outputConfigXML, outputPrefix,stateMachineListenersManager) ,
                                   new  SimpleTemplateNegotiatorFactory(), stateMachineListenersManager, null);


      }
      else if(outputType.equals("null"))
      {
        cm = new CollectorManager( config, new NullOutputterFactory() ,
                                   new  SimpleTemplateNegotiatorFactory(/*templatesFileName*/), new SimpleStateMachineListener(), null);

      }
    }
    catch (FileNotFoundException e1) {
      e1.printStackTrace();
      return;
    }
//
// Instantiate the Collector
//
    try {
      handler = new CollectorLog("IPDR");
      //
      // Set debug level
      //
      if (args.length > 0) {  // Get the CRANE configuration file name
        if ( (debugLevel == 'D') || (debugLevel == 'd')) {
          handler.setDebug();
        }
        else if ( (debugLevel == 'I') || (debugLevel == 'i')) {
          handler.setInfo();
        }
        else if ( (debugLevel == 'W') || (debugLevel == 'w')) {
          handler.setWarn();
        }
        else if ( (debugLevel == 'E') || (debugLevel == 'e')) {
          handler.setError();
        }
        else if ( (debugLevel == 'F') || (debugLevel == 'f')) {
          handler.setFatal();
        }
        else {
          System.err.println("incorrect debug level");
          help();
          System.exit(0);
        }
      }
      else {
        handler.setDebug();
      }
      //
      // Run Collector
      //
      LogManager.registerLogHandler(handler);
      cm.run();
      CRANELegacyThread legacyThread = null;
      if (config.getSupportCRANE()) { // If support CRANE (IPDR/SP 1.0)
        String[] fakeArguments = {
            craneFileName, "100000", "csv"};
        legacyThread = new CRANELegacyThread(fakeArguments);
        legacyThread.start();
      }

      System.in.read();
      if (legacyThread != null)
        legacyThread.interrupt();
    }
    catch (Exception ex) {
      LogManager.stackTrace(ex);
    }
    finally {
      try {
        cm.disconnectAll();
        cm.joinAll();
      }
      catch (InterruptedException e) {}
    }
  }
  private static void help () {
    System.out.println("The syntax of collector command line is as follows:");
    System.out.println("Test [[[[[[<debug level ([D]ebug | [I]nfo | [W]arn | [E]rror | [F]atal)>] <collector config file>] <CRANE legacy config file>] <output type: xml | csv | null>] <output file (CSV) or output prefix(IPDRXML)>] <IPD RXML config file (IPDR XML only)>]");
    System.out.println("");
    System.out.println("The parameters may only be omitted from the end of the command line, in which case the following default values will be used:");
    System.out.println("\t debug level: Debug");
    System.out.println("\t collector config file:  config.xml");
    System.out.println("\t CRANE legacy config file: CRANEConfig.xml");
    System.out.println("\t output type:  xml");
    System.out.println("\t output file or output prefix: IPDRXMLOutput");
    System.out.println("\t IPDR XML config file: IPDR35XMLOutputConfig.xml");
    System.out.println("");
    System.out.println("Example:");
    System.out.println("/opt/j2sdk1.4.2_07/bin/java -classpath jdom.jar:ipdrapi.jar:collector.jar:/opt/j2sdk1.4.2_07/jre/lib/rt.jar org.ipdr.collector.Test D config.xml  CRANEConfig.xml xml IPDRXMLOutput IPDR35XMLOutputConfig.xml");
  }
}
