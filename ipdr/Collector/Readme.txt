This is README file for IPDR 3.5 XML Outputter application 
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.


1. Description
   IPDR 3.5 XML Outputter produces files in XML format of IPDR.
   Each time Collector get "Session Start" protocol message, the outputter creates an empty file.
   The name of each file consists of parametric prefix, Exporter ip address, Exporter port,
   Session ID, Document ID and index. The outputter enlarges the index if the file system
   contains a file with the same name.
   Important ! Previously created XML files are not deleted automatically. 		 	
2. Configuration.

	In order to output collected records by IPDR 3.5 XML Outputter configure an XMl file
	with following elements:
	IPDR35XMLOutputConfig- root Element
	IPDR35XMLOutputConfig' childs Elements:
		1. Text Element 'SchemaLocation' - mandatory, contains XML Schema location 
		2. Text Element - 'IPDRRecorderInfo' - mandatory, contains information
 								   about Collector
		3. Text Element 'version' - mandatory, contains IPDR version number
		4. Text Element 'dateFormat' - not mandatory, contains date format for all
							 time XML Elements of IPDR 3.5 XML Outputter
				                   (see java.text.SimpleDateFormat.java)
						       the default format is 'yyyy-dd-MM'TO'hh:mm:ssz'
		5. Complex Element 'Templates' - contains Elements 'Template'
		6. Element 'Template'  - contains attribute 'templateID', String template id
					     - contains attribute 'typeName', String, to assign 
						 template type to record	 		  	


3. How to run IPDR Collector with IPDR 3.5 XML Outputter
	In order to run Collector with IPDR 3.5 XML Outputter:
	1. Put all configuration files (Collector config xml, Templates config xml, 
	   CRANE config xml, IPDR 3.5 XML Outputter config xml)in CLASSPATH.
	2. Run org.ipdr.collector.Test with following parameters:
		1. Collector config xml
		2. Templates config xml
		3. CRANE config xml
		4. Outputter type - 'xml'
		5. Outputter prefix
		6. IPDR 3.5 XML Outputter config xml

