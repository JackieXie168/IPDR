/** @file IPDR.h
 *
 *  Public header file for the IPDR exporter.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 *  $Id: IPDR.h,v 1.65 2005/05/11 12:38:34 dmitryk Exp $
 *
 *   This file contains the functions for communicating with
 *   the IPDR Exporter.
 *   <ul>
 *   <li>@ref IPDR_applicable_documents
 *   <li>@ref IPDR_terminology
 *   <li>@ref IPDR_system_overview
 *   <ul>
 *     <li>@ref IPDR_special_considerations
 *     <ul>
 *       <li>@ref IPDR_special_considerations_multi_threading
 *       <li>@ref IPDR_special_considerations_strings
 *       <li>@ref IPDR_special_considerations_future_compatitibility
 *     </ul>
 *     <li>@ref IPDR_usage
 *     <ul>
 *       <li>@ref IPDR_cookbook
 *       <li>@ref IPDR_usage_steps
 *       <li>@ref IPDR_usage_template
 *       <li>@ref IPDR_used_functions
 *       <li>@ref IPDR_MIB
 *     </ul>
 *   </ul>
 *   <li>@ref IPDR_reliability
 *   <ul>
 *     <li>@ref IPDR_reliability_base_transport
 *     <li>@ref IPDR_reliability_record_level
 *     <li>@ref IPDR_reliability_protocol
 *     <li>@ref IPDR_reliability_failover
 *     <li>@ref IPDR_reliability_failback
 *     <li>@ref IPDR_reliability_transmitter_failure
 *     <li>@ref IPDR_reliability_other
 *     <li>@ref IPDR_reliability_internal_queue
 *     <li>@ref IPDR_reliability_deduplication
 *   </ul>
 *   <li>@ref IPDR_initialization_starting_deletion
 *   <ul>
 *     <li>@ref IPDR_configuration_values
 *   </ul>
 *   <li>@ref IPDR_templates
 *   <ul>
 *     <li>@ref IPDR_key_type_attributes
 *   </ul>
 *   <li>@ref IPDR_data_delivery
 *   <li>@ref IPDR_MIB_related
 *   <ul>
 *     <li>@ref IPDR_transmitter_MIB
 *     <li>@ref IPDR_MIB
 *   </ul>
 *   <li>@ref IPDR_sessions
 *   <li>@ref IPDR_alternate_collectors
 *   <li>@ref IPDR_general_typedefs
 *   <ul>
 *     <li>@ref IPDR_transitional_typedefs
 *   </ul>
 *   <li>@ref IPDR_key_type_attributes
 *   <li>@ref IPDR_event_notification
 *   <li>@ref IPDR_logging
 *   <li>@ref IPDR_platform
 *   <ul>
 *     <li>@ref IPDR_start_stop_services
 *     <li>@ref IPDR_msg_services
 *     <li>@ref IPDR_event_services
 *     <li>@ref IPDR_memory_services
 *     <li>@ref IPDR_lock_services
 *     <li>@ref IPDR_non_volatile_memory
 *     <li>@ref IPDR_network_services
 *     <ul>
 *       <li>@ref IPDR_TCP_services
 *       <li>@ref IPDR_UDP_services
 *     </ul>
 *     <li>@ref IPDR_time_services
 *     <li>@ref IPDR_platform_calls
 *   </ul>
 *   <li>@ref IPDR_Config
 *   <li>@ref IPDR_utilities
 *   </ul>
 */
#ifndef DOXYGEN_PROCESSING

/* This file is not highly readable because it has had comments added
 * in a form suitable for processing by Doxygen
 * (http://www.doxygen.org).  The output of the Doxygen processing is
 * distributed with the sources; you are encouraged to not read this
 * file but instead use those files plus the other documentation
 * distributed with the API for understanding the API.
 */
#endif


/** @mainpage IPDR Exporter API
 *
 *  This document describes the IPDR Exporter Application
 *  Programming Interface (API), a set of ANSI C declarations and
 *  prototypes enabling a network element's host application and local
 *  operating system properly to configure and control IPDR's
 *  embedded IPDR exporter. The IPDR exporter is
 *  implemented as a library of ANSI C functions and structures.
 *
 *  The "Exporter" is also sometimes known as the "data transmitter".
 *  It is the code which is used by a network element to send data
 *  to a collector, which receives the data and processes it further.
 *
 *  <ul>
 *  <li>@ref IPDR_cookbook
 *  <li>IPDR.h (the API header file and general documentation)
 *  <li>Config.h (control of some features in the API)
 *  <li>Defs.h (ancillary definitions file
 *  <li>@ref IPDR_MIB
 *  <li>IPDRsimple.c - a simple IPDR exporter simulator
 *  <li>IPDRsim.cpp - a more complete IPDR exporter simulator
 *  </ul>
 */

/** @defgroup IPDR_RFC IPDR/SP 2.1 Specifications Document
 *
 *  The new IPDR/SP 2.1 version is available to the public now at: www.ipdr.org/public/DocumentMap/SP2.1.pdf
 */


/** @defgroup IPDR_cookbook Cookbook: How to build a IPDR Exporter
 *
 *  <p><p>
 *  Like most software, there is no one "best way" to build a IPDR Exporter.
 *  But the following steps give a method that should work well in most cases.
 *
 *  An extremely simple Exporter is in given in the
 *  <code>"IPDRsimple.c"</code> file, and a more complex example is
 *  in <code>"IPDRsim.cpp"</code>.
 *
 *  <ol>
 *  <li>@ref IPDR_cookbook_overview
 *  <li>@ref IPDR_cookbook_terminology
 *  <li>@ref IPDR_cookbook_determine_needs
 *  <li>@ref IPDR_cookbookcollectors
 *  <li>@ref IPDR_cookbook_templates
 *  <li>@ref IPDR_cookbook_send
 *  <li>@ref IPDR_cookbook_events
 *  <li>@ref IPDR_cookbook_integration
 *    <ul>
 *    <li>@ref IPDR_cookbook_integration_configuration
 *    <li>@ref IPDR_cookbook_integration_start_stop
 *    <li>@ref IPDR_cookbook_integration_MIB
 *    <li>@ref IPDR_cookbook_integration_monitoring
 *  </ul>
 *  <li>@ref IPDR_cookbook_sizing
 *  <li>@ref IPDR_cookbook_platform
 *  <li>@ref IPDR_cookbook_testing
 *  <li>Samples
 *    <ul>
 *        <li>IPDRsimple.c - a very simple IPDR exporter simulator
 *        <li>IPDRsim.cpp - a more complete IPDR exporter simulator
 *    </ul>
 *  </ol>
 *
 *  @{
 */


/** @defgroup IPDR_cookbook_overview Overview
 *
 *  <p><p>
 *
 *  Here is a list of the steps needed to integrate the IPDR exporter
 *  into a network element, for exporting data. This assumes a
 *  straightforward implementation on a Unix-like operating system
 *  (e.g., Linux, VxWorks, QNX, etc.), done by a single experienced
 *  engineer.
 *
 *  The largest amount of time is given to testing. This can
 *  often be combined with other testing of the network element.
 *  The second largest amount of time is for design issues.
 *  The actual implementation can be done quite quickly.
 *
 *  <table>
 *  <tr><td>Define what data will be generated and how the data will be used</td>
 *      <td>Data is useless if it doesn't fulfill some need or
 *          if it is difficult to use. Multiple uses for the data
 *          should be considered (e.g., for both accounting and
 *          for QoS monitoring).
 *          See also @ref IPDR_cookbookcollectors.</td>
 *  </tr>
 *  <tr><td>Read the overview documentation</td>
 *      <td>The minimum for this is the @ref IPDR_cookbook,
 *          the sample code,
 *          and the overview of the protocol in @ref IPDR_RFC.
 *      </td>
 *  </tr>
 *  <tr><td>Define the data</td>
 *      <td>Create the C/C++ structures for the data
 *          and write the IPDR templates for accessing it.
 *          See @ref IPDR_cookbook_templates.
 *      </td>
 *  </tr>
 *  <tr><td>Define which features of the API will be used</td>
 *      <td>This step includes determining the
 *          sizing parameters (see @ref IPDR_cookbook_sizing),
 *          estimating performance issues, and how the IPDR
 *          exporter will be integrated into the platform.
 *      </td>
 *  </tr>
 *  <tr><td>Write the configuration callback</td>
 *       <td>Typically, this is a lookup into a table
 *           of key/value pairs. We assume that the
 *           network element already provides a function for
 *           reading in the values. See @ref
 *           IPDR_cookbook_integration_configuration.</td>
 *  </tr>
 *  <tr><td>Define the monitoring, event and error handling</td>
 *      <td>See @ref IPDR_cookbook_events and
 *          @ref IPDR_cookbook_integration_monitoring.</td>
 *  </tr>
 *  <tr><td>Add the IPDR start, stop, and "send" calls to the code</td>
 *      <td>The start and stop code must be added at an appropriate
 *          point in the network element's start-up and shut-down
 *          code. The "send" calls may simply be added to the code
 *          when some accounting even occurs; or they may require
 *          a separate thread which outputs data at specified
 *          intervals.</td>
 *  </tr>
 *  <tr><td>Integrate the IPDR MIB</td>
 *      <td>See @ref IPDR_cookbook_integration_MIB</td>
 *  </tr>
 *  <tr><td>Test functionality</td>
 *      <td>Configure the collector and examining
 *          the output to ensure it is what is expected
 *          (this may require writing simple scripts).</td>
 *  </tr>
 *  <tr><td>Stress test and failure test</td>
 *      <td>Extended tests at maximum data rates
 *          and with maximum number of transmitters (clients).
 *          Failure tests can be labor-intensive because
 *          of the need to physically plug cables in and
 *          out, turn power on and off, etc.
 *       </td>
 *  </tr>
 *  </table>
 *
 *  If porting of the platform is needed (see @ref IPDR_cookbook_platform),
 *  the following additional knowledge is needed:
 *  <ul>
 *  <li>The platform's operating system (threads, synchronization,
 *      non-blocking socket I/O, signal handling, time and date handling);
 *  <li>POSIX functions for threads, synchronization,
 *      non-blocking socket I/O, signal handling, time and date handling;
 *  <li>Low-level debugging on the platform;
 *  </ul>
 *
 */


/** @defgroup IPDR_cookbook_terminology Cookbook: Terminology
 *
 *  <p><p>
 *  No piece of software would be complete without its own twist on
 *  terminology. So, here are the words and acronyms to make sense of
 *  the rest of this document. We apologize for any confusion that our
 *  use of "client" and "server" might cause ... they come from
 *  traditional usage for AAA (authentication, authorization, and
 *  accounting) and can be a little confusing to those who aren't
 *  familiar with that world.
 *
 *  To help avoid confusion, throughout this document we have added
 *  the glosses "transmitter" and "collector" to "client" and "server"
 *  respectively.
 *
 *  A more detailed terminology list is given in the section
 *  @ref IPDR_terminology.
 *
 *  <dl>
 *  <dt>IPDR<dd><b>C</b>ommon
 *               <b>R</b>eliable
 *               <b>A</b>ccounting protocol for
 *               <b>N</b>etwork
 *               <b>E</b>lements
 *  <dt>IPDR client<dd>The data <em>transmitter</em>.
 *  <dt>IPDR server<dd>The data <em>collector</em>.
 *  <dt>Platform<dd>Where the IPDR transmitter (client) runs.
 *  <dt>Network element<dd>The device which uses the IPDR transmitter (client)
 *      to send data.
 *  <dt>Template<dd>A single record layout, consisting of a list of "keys" (q.v.).
 *  <dt>Key<dd>A single field in a "template" (q.v.), consisting of the
 *      key name and its type.
 *  </dl>
 *
 */


/** @defgroup IPDR_cookbook_determine_needs Cookbook: Determine needs
 *
 *  <p><p>
 *  Considerations for transmitting data:
 *  <ul>
 *  <li>What kind of data will be transmitted?
 *  <li>Are there better tools to use instead?
 *  <li>Will it also work for future needs?
 *  </ul>
 *
 *  As its name states, IPDR is designed for sending "accounting"
 *  type data from network elements. IPDR is superior to the following
 *  following output mechanisms:
 *  <dl>
 *  <dt>SNMP (MIBs): <dd>IPDR can efficiently send much higher
 *      rates of data, and more reliably (SNMP is based on UDP).
 *      Also, IPDR is push-oriented, so the network element
 *      can control the rate at which data is sent (conventional
 *      SNMP leaves the network element at the mercy of the
 *      network management system's polling).
 *  <dt>Log files: <dd>IPDR avoids the need for disk space on
 *      network element. Also, it is a binary protocol, so precious
 *      processing time is not used in converting data to an ASCII
 *      or XML representation.
 *  <dt>RADIUS accounting: <dd>IPDR avoids some of the scalability
 *      issues of RADIUS; it is more reliable and has a simpler
 *      interface (the IPDR API handles issues of fail-over that
 *      typically must be explicitly programmed when using RADIUS).
 *  <dt>TCP sockets: <dd>IPDR takes care of the assembly and
 *      disaassembly of records and negotiation of content.
 *      It is also provides application-level reliability, to acknowledge
 *      when records have been processsed (TCP reliability only verifies
 *      that records have arrived) and supports fail-over to multiple
 *      collectors.
 *  </dl>
 *
 *  The main requirements that were used to design the protocol were:
 *  <ul>
 *  <li>Aimed at accounting/logging needs: the data are structured
 *      into individual records with well-defined fields. Multiple record
 *      types are allowed.
 *  <li>Real-time: minimal latency to deliver records.
 *  <li>Scalable: support multiple devices at over 1000 records/sec each.
 *  <li>Flexible: the transmitter (client) defines the data records and can change
 *      the content by adding new fields or by defining new record
 *      "templates".
 *  <li>Reliable: ensures that every record is delivered and processed.
 *  <li>Efficient: the protocol is as light-weight as possible
 *      (network elements spend their time doing network-related
 *      work and don't want to "waste" processing power on non-core
 *      tasks.
 *  <li>Easy to deploy: simple API.
 *  <li>Portable: all operating system functions are isolated for
 *      portability, so if you have an esoteric operating system
 *      that nobody else uses, the chances are good that IPDR
 *      can be quickly ported to it.
 *  </ul>
 *
 *  IPDR isn't for everybody. If you don't care about reliability,
 *  just use standard TCP sockets or even UDP. If you don't care about
 *  flexibility, hard-code your record formats. If you want free-form
 *  data transfer, IPDR might not do what you want. If you want
 *  two-way communication, consider some other protocol, such as HTTP
 *  or RADIUS (although IPDR is being extended to handle certain
 *  kinds of two-way communication, mainly additional acknowledgment
 *  from the collector (server), such as authorization).
 *
 *  IPDR's flexibility is important for future-proofing your investment.
 *  You can add new record types and fields at any time, simply by installing
 *  a new version of your software that defines them.
 */


/** @defgroup IPDR_cookbookcollectors Cookbook: Determine downstream processing needs
 *
 *  <p><p>
 *  In an ideal world, you could just output your accounting data in the
 *  form that's most convenient for you. In reality, you need to think a bit
 *  about how the data will be used.
 *
 *  If you're outputting multiple kinds of data, you must have some
 *  way of distinguishing one kind of data from another -- IPDR
 *  multiplexes all records into a single record, with unused data
 *  fields marked as "null".
 *
 *  IPDR handles data with a variable number of fields the same way
 *  that a relational database handles data. For example, if you have
 *  master information and a list of details, you must instead send
 *  the master information by itself, with a separate record for each
 *  detail (and the detail records must have a field that allows
 *  matching them with the master record).
 *
 *  For example, suppose a mail server wishes to output each message's
 *  sender and list of recipients. This can be done by defining two
 *  records:
 *  <dl>
 *  <dt>message: <dd>timestamp, msgID, sender, ...
 *  <dt>recipientDetail: <dd>msgID, receiver
 *  </dl>
 *  This has an obvious implementation in a SQL database, allowing
 *  a query to get the sender and all recipients for a message by
 *  doing a "join" on the msgID:
 *  <br><code>&nbsp;&nbsp;SELECT&nbsp;timestamp,&nbsp;sender,&nbsp;receiver
 *  <br>&nbsp;&nbsp;FROM&nbsp;message,&nbsp;recipientDetail
 *  <br>&nbsp;&nbsp;WHERE&nbsp;message.msgID&nbsp;=&nbsp;receipient.msgID</code>
 *
 *  Other processing needs include:
 *  <ul>
 *  <li>Whether to send timestamps in local time or UTC (GMT) ... UTC is
 *      usually better.
 *  <li>How to identify the sender (e.g., the sender's IP address).
 *  <li>Whether to send absolute values or deltas (if absolute values,
 *      how to handle overflow; if deltas, how to handle situations
 *      of data loss, as described in @ref IPDR_cookbook_events).
 *  <li>How to describe missing data situations (e.g., zero-lengh string,
 *      a separate field that indicates missing data).
 *  <li>How to uniquely identify a record, for deduplication. In general,
 *      this is not a problem because the IPDR protocol provides a
 *      transmitter (client) boot time and sequence number for each record. However,
 *      the sequence number can wrap around, so if deduplication is done
 *      over large sets of records, some additional field(s) must be
 *      used to uniquely identify records.
 *  <li>If multiple records are output at one time (e.g.,
 *      statistics tables), whether these must be locked from
 *      update while the data are being output.
 *  </ul>
 */


/** @defgroup IPDR_cookbook_templates Cookbook: Define templates
 *
 *  <p><p>
 *  After you have determined what data needs to be transmitted and the
 *  broad form of its format (see @ref IPDR_cookbookcollectors).
 *
 *  If you are starting from scratch, you simply define a C or C++
 *  structure which contains the data that you wish to send.
 *  Continuing with the example given in @ref IPDR_cookbookcollectors,
 *  there could be two C structures:
 *  <br><code>&nbsp;&nbsp;struct&nbsp;message&nbsp;{
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;char&nbsp;type;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;'M'&nbsp;for&nbsp;message
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;int&nbsp;&nbsp;timestamp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;seconds&nbsp;since&nbsp;00:00:00&nbsp;UTC&nbsp;1&nbsp;January&nbsp;1970
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;int&nbsp;&nbsp;msgID;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;unique&nbsp;for&nbsp;each&nbsp;message
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;char&nbsp;sender[100];&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;null-terminated
 *  <br>}</code>
 *  <br><code>&nbsp;&nbsp;struct&nbsp;recipientDetail&nbsp;{
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;char&nbsp;type;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;'R'&nbsp;for&nbsp;recipient
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;int&nbsp;&nbsp;msgID;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//&nbsp;unique&nbsp;for&nbsp;each&nbsp;message
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;char&nbsp;recipient[100];&nbsp;//&nbsp;null-terminated
 *  <br>&nbsp;&nbsp;}</code>
 *
 *  Next, a unique identifier must be defined for each field:
 *  <br><code>&nbsp;&nbsp;enum&nbsp;{
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;typeKeyID&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;=&nbsp;1;
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;timestampKeyID&nbsp;=&nbsp;2;&nbsp;
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;msgIDKeyID&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;=&nbsp;3;
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;senderKeyID&nbsp;&nbsp;&nbsp;&nbsp;=&nbsp;4;
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;receipient_id&nbsp;&nbsp;=&nbsp;5;
 *  <br>&nbsp;&nbsp;</code>}
 *
 *  This information must be communicated to the IPDR transmitter (client), using
 *  the ::IPDR_Key data type. For example, the "sender" field
 *  would be defined:
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp;{&nbsp;senderKeyID,&nbsp;XDR_TYPE_STRING,&nbsp;"sender",&nbsp;"",&nbsp;"",&nbsp;0,
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;IPDR_ACCESS_BY_OFFSET,&nbsp;NULL,
 *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(IPDR_KeyAccessor(offsetof(struct&nbsp;message,&nbsp;sender)))&nbsp;</code>}
 *
 *  These key definitions are combined into templates and passed
 *  to the IPDR_CreateTemplate() function. The result is a "template
 *  handle", which is used when calling IPDR_SendData().
 *
 *  Not all situations can be handled so simply. If the internal record
 *  structure does not match the desired output format, there are two
 *  possible solutions:
 *  <ol>
 *  <li>Create records in the desired format, copy the fields into
 *      them, and output directly.
 *  <li>Use the #IPDR_ACCESS_BY_FUNCTION option, with a ::IPDR_KeyAccessor
 *      function to return a pointer to the data.
 *  </ol>
 */


/** @defgroup IPDR_cookbook_send Cookbook: Send data
 *
 *  <p><p>
 *  The network element determines when to send data, and uses the
 *  IPDR_SendData() function, with the appropriate "template handle"
 *  (from IPDR_CreateTemplate()) and the record
 *  data. IPDR_SendData() queues the data for the
 *  sending thread. The sending thread takes care of transmitting to
 *  the primary or secondary collector (server), handling fail-over, and removing
 *  the record when it is positively acknowledged by the collector (server).
 *
 *  IPDR_SendData() is non-blocking (assuming use of the
 *  multi-threaded implementation of the platform-specific functions),
 *  so there is no need for a separate thread for outputting data.
 *
 *  For situations where the copy operation is too expensive,
 *  the API has provision for "external queuing", allowing the
 *  platform to handle the sent data explicitly.
 */


/** @defgroup IPDR_cookbook_events Cookbook: Error and event handling
 *
 *  <p><p>
 *  Even though IPDR is a reliable protocol, it cannot perform magic.
 *  In particular, if all the collectors (servers) are down or the network connection
 *  to them is broken, data will be lost (see @ref IPDR_cookbook_sizing
 *  for how to set the parameters for limiting such data loss).
 *
 *  Because data sending and acknowledgment is asynchronous to the
 *  IPDR_SendData() calls, any errors or collector (server) status change events
 *  are communicated via ::IPDR_EventCallbackDetailed (set by
 *  IPDR_SetExporterEventHandlerDetailed()). The most recent event and
 *  other statistical information can also be examined by
 *  IPDR_GetServerStatisticsAndState().
 *
 *  THe exception to this is if something goes wrong with the main
 *  read/timer loop, which has no ability to communicate its
 *  status. When it has a problem, it writes a log message (if
 *  possible) and sets its status to not running,
 *
 *  The following errors may be handled:
 *  <dl>
 *  <dt>#IPDR_EVENT_RECORD_LOST
 *  <dd>The internal queue filled up and the record
 *      was thrown away.
 *  </dl>
 *
 *  The following events may be handled. Typically, these are used
 *  for status indication.
 *  <dl>
 *  <dt>#IPDR_EVENT_SERVER_CONNECTED
 *  <dd>A collector (server) has become available
 *  <dt>#IPDR_EVENT_SERVER_DISCONNECTED
 *  <dd>A collector (server) has become unavailable, either because
 *      it was stopped or because a network timeout occurred.
 *  <dt>#IPDR_EVENT_ACTIVE_SERVER_CHANGED
 *  <dd>A higher priority collector (server) has become active and the
 *      transmitter (client) has switched back to using it.
 *  <dt>#IPDR_EVENT_QUEUE_EMPTY
 *  <dd>All queued records have been acknowledged by the collector (server)
 *      and it can be safely shut down.
 *  </dl>
 *
 *  The transmitter (client) code also can generate log messages. The verbosity
 *  level is set by IPDR_SetModuleDebugLevel(). For more on this,
 *  see @ref IPDR_cookbook_integration_monitoring.
 */


/** @defgroup IPDR_cookbook_sizing Cookbook: Sizing
 *
 *  <p><p>
 *  The various internal memory areas can have their maximum size
 *  limited. This prevents the IPDR transmitter (client) from using all the memory
 *  resources and stopping other processing on the network element.
 *  Details of all the values are given in IPDR_configuration_values.
 *  The most important is <code>configuration.queueMaxMemory</code>.
 *  If this is allowed to default, the transmitter will use all
 *  available memory. To calculate this value, you must know:
 *  <ul>
 *  <li>Average IPDR record size (including header)
 *  <li>Fastest data rate
 *  <li>Worst-case fail-over time
 *  </ul>
 *
 *  Using these, you can calculate how much memory must be reserved
 *  to avoid losing any records. More details are given in
 *  @ref IPDR_reliability_internal_queue.
 *
 *  Related sizing values allow setting the incremental chunk size
 *  and the memory for some ancilliary queues which are quite small
 *  and can safely be allowed to default because their sizes depend
 *  on the main queue's size.
 */


/** @defgroup IPDR_cookbook_integration Cookbook: Integration issues
 *
 *  <p><p> This section describes the options for integrating the
 *  IPDR transmitter (client) seamlessly into the platform. For
 *  porting issues, see @ref IPDR_cookbook_platform.
 *
 *  <ul>
 *  <li>@ref IPDR_cookbook_integration_configuration
 *  <li>@ref IPDR_cookbook_integration_start_stop
 *  <li>@ref IPDR_cookbook_integration_MIB
 *  <li>@ref IPDR_cookbook_integration_monitoring
 *  </ul>
 *
 *  @{
 */


/** @defgroup IPDR_cookbook_integration_configuration Cookbook: Integrating configuration
 *
 *  <p><p>
 *  The IPDR transmitter (client) API defines all its configuration values by
 *  a name and a MIB OID. There are two mechanisms for configuration:
 *  <ul>
 *  <li>A callback for mapping a name to a value (::IPDR_ParametersHandlerFunc,
 *      set by IPDR_InitExporter()). For example, you might use this if
 *      you do configuration with a Windows-style ".INI" file by simply reading
 *      the name=value pairs into a hash table; the callback is just a few lines
 *      of code, looking up the name to get the associated value.
 *  <li>A MIB value. You could use these if you use SNMP SET on the
 *      configuraiton values.
 *  </ul>
 *
 *  The ::IPDR_ParametersHandlerFunc is called only when the IPDR
 *  transmitter (client) is initialized. If you wish to change a value, you must
 *  stop the transmitter and restart it (see @ref
 *  IPDR_cookbook_integration_start_stop for more on this).
 *
 *  When a MIB value is changed, the transmitter (client) is reconfigured.
 *  This may cause the transmitter to briefly disconnect from the collector (server),
 *  but does not cause any data loss.
 *
 *  The ::IPDR_LoadMIBFunc callback is defined in IPDR_InitExporter() and
 *  can be used to get initial MIB values from some non-volatile memory.
 */


/** @defgroup IPDR_cookbook_integration_start_stop Cookbook: Integrating IPDR transmitter (client) start/stop
 *
 *  <p><p>
 *  When the network element has initialized itself and is ready to send data,
 *  it must first start the IPDR transmitter (client). As described in @ref IPDR_usage_steps,
 *  this consists of:
 *  <ol>
 *  <li>Initialize the platform-specific services (e.g., with GenericPlatform_Initialize()).
 *  <li>Start the transmitter (client) with IPDR_InitExporter() and appropriate callbacks
 *      (::IPDR_ParametersHandlerFunc, ::IPDR_LoadMIBFunc, ::IPDR_SaveMIBFunc).
 *  <li>Create a session with IPDR_CreateSession().
 *  <li>Define the collectors (servers) with IPDR_AddCollector().
 *  <li>Add templates with IPDR_CreateTemplate().
 *  <li>Start the transmitter (client) with IPDR_StartExporter().
 *  </ol>
 *
 *  Once these steps are done, IPDR_SendData() can be used.
 *
 *  You can add or delete collectors (servers) at any time.
 *
 *  When you wish to shut down the IPDR data delivery, you should do:
 *  <ol>
 *  <li>IPDR_DrainQueue(), to ensure that all data has been sent
 *      and acknowledged (this step is optional).
 *  <li>IPDR_DeleteExporter() to stop and remove the IPDR transmitter (client).
 *  <li>IPDR_PlatformServices::Delete() to remove any
 *      platform-specific resources.
 *  </ol>
 *
 *  You can stop and start the transmitter (client) as many times as you want;
 *  there are no "global" values that are kept around between
 *  invocations. You can also run multiple transmitters (clients) if you want,
 *  although this would be rather uncommon (multiple sessions
 *  would usually be a better method).
 */


/** @defgroup IPDR_cookbook_integration_MIB Cookbook: Integrating the IPDR MIB
 *
 *  <p><p>
 *  The IPDR MIB can be integrated with the platform's MIB by using these
 *  callback functions:
 *  <dl>
 *  <dt>IPDR_MIBGet(): <dd>Find a specific MIB value, given the OID.
 *  <dt>IPDR_MIBGetNext(): <dd>Find the next MIB value (used for "walking" the MIB).
 *  <dt>IPDR_MIBSet(): <dd>Set a MIB value, given the OID and value.
 *  </dl>
 *  These can be used, for example, by an "AgentX" extensibility framework
 *  as described in RFC 2741 (http://www.ietf.org/rfc/rfc2741.txt). An
 *  example SNMP daemon that uses this is CMU's AgentX SNMPD
 *  (http://www.net.cmu.edu/groups/netdev/agentx/doc/snmpd.html).
 *
 *  The IPDR API also has utility functions that help in saving the
 *  MIB to non-volatile memory and restoring it.
 *
 *  See @ref IPDR_MIB_related for more details.
 */


/** @defgroup IPDR_cookbook_integration_monitoring Cookbook: Integrating IPDR monitoring
 *
 *  <p><p>
 *  The IPDR transmitter (client) provides three basic mechanisms for monitoring it:
 *  <ul>
 *  <li>Statistics and status call (IPDR_GetServerStatisticsAndState(),
 *      IPDR_GetSessionStatistics()).
 *  <li>Log (both to file/console and in syslog format [RFC 3164
 *       http://www.ietf.org/rfc/rfc3164.txt]).
 *  <li>SNMP MIB (see also @ref IPDR_cookbook_integration_MIB).
 *  </ul>
 *
 *  In addition, if the console is enabled
 *  (<code>configuration.console.consolePort</code> (default port
 *  40000), you can connect via ordinary telnet to examine things.
 *
 */

/*@}*/


/** @defgroup IPDR_cookbook_platform Cookbook: Implementing the platform-dependent layer (porting)
 *
 *  <p><p>
 *  The IPDR transmitter (client) currently supports the following platforms:
 *  <ul>
 *  <li>POSIX.1 (most Unixes, including Linux and Solaris support this)
 *  <li>Solaris (some Solaris-specific functions are used instead of
 *      the POSIX functions).
 *  <li>VxWorks
 *  </ul>
 *
 *  The porting layer details are given in  the section @ref IPDR_platform.
 *
 *  For Unix-like platforms, porting usually is quite easy.
 *  The main issues in porting are:
 *  <ul>
 *  <li>Thread or process model (start/stop thread, synchronization)
 *  <li>Details of the socket read and write functions
 *      in non-blocking mode (e.g., use the poll() or select() functions)
 *  <li>Signal handling
 *  <li>Time and date handling
 *  </ul>
 *
 *  For other platforms, a considerable amount of work may be needed.
 *  The IPDR transmitter (client) makes no assumptions about such issues as thread or
 *  process model, exception handling, I/O, etc.; but if the platform's
 *  model is considerably different from POSIX's, there can be a considerabl
 *  amount of work.
 */


/** @defgroup IPDR_cookbook_testing Cookbook: Testing and using
 *
 *  <p><p>
 *  The IPDR transmitter (client) API comes with full source code and with sample
 *  programs that use it.
 *
 *  When deploying a system, debug log messages can be turned on
 *  by calling IPDR_SetModuleMoreDebugLevel(). These can also be
 *  set by commands at the console.
 *
 *  Various statistis are output at intervals defined by
 *  <code>configuration.statisticsInterval</code>.
 *
 *  If the console is enabled (<code>configuration.console.consolePort</code>
 *  (default port 40000)
 *  and (<code>configuration.console.consolePassword</code>), you can use
 *  ordinary telnet to connect to the transmitter (client) and issue various configuration
 *  and analysis commands. This is also a good way of verifying the
 *  version of the deployed code, by using the "version" command"
 *
 *  If the network element has no "console" output, you can
 *  direct the syslog output can be directed to any IP address using
 *  <code>configuration.logging.loggerAddress</code> and
 *  <code>configuration.logging.loggerPort</code>.
 *
 */

/*@}*/


/** @defgroup IPDR_applicable_documents Applicable Documents
 *
 *  <p><p>
 *  <ol>
 *  <li>IPDR.org v3.5 Document Map Overview
 *  <li>IPDR.org Protocol Encoding: XDR 3.5 IPDR Organization, Inc.
 *  <li>IPDR/SP Protocol Specification Version 2.1
 *  <li>IPDR Streaming Protocol – CRANE Harmonization 1.7
 *  <li>XDR3 5 1.pdf
 *  <li>C Programming language standard: ISO/IEC 9899:1900 (ANSI X3.159-1989)
 *  <li>Portable Operating System Interface(POSIX) - Part 1: System
 *  Application Programming Interface (API) [C Language] ISO/IEC
 *  9945-1:1996, IEEE 5. Open Group; Single Unix Specification,
 *  Version 2
 *  http://www.opengroup.org/public/pubs/online/7908799/index.html
 *  <li>Stream Control Transmission Protocol (SCTP)
 *  http://www.ietf.org/rfc/rfc2960.txt
 *  <li>SCTP Applicability Statement
 *  http://www.ietf.org/rfc/rfc3257.txt
 *  </ol>
 */


/** @defgroup IPDR_terminology Terminology
 *
 *  <p><p>
 *  <dl>
 *  <dt>IPDR Protocol
 *  <dd>The IPDR Exporter implementation. A library of components
 *  used to format accounting data produced by the Network Element and
 *  dispatch it using the IPDR protocol.  This usage of the word
 *  "client" is slightly unusual; it derives from the terminology used
 *  for accounting Exporter/server, where the "client" requests the
 *  "server" to take care of the accounting data.
 *  <dt>IPDR %Collector
 *  <dd>An agent that receives IPDR protocol data (messages) from a
 *  IPDR transmitter (client). Typically a component within a mediation system or
 *  an Operations or Business Support System (OSS/BSS).  This usage of
 *  the workd "server" is slightly unusual; it derives from the
 *  terminology used for accounting Exporter/server, where the "server"
 *  takes care of the accounting data for the "client".
 *  <p>
 *  <dt>DSN
 *  <dd>Data Sequence Number: a number assigned to each record, used
 *  for ensuring that all data have arrived at the receiving end and
 *  for deduplication in fail-over scenarios.
 *  <dt>Host Application
 *  <dd>The NE's (platform's) main software, which communicates with
 *  and controls the IPDR transmitter (client) via the API.
 *  <dt>NE
 *  <dd>Network Element, the platform on which the Host Application
 *  runs and the IPDR transmitter (client) is embedded.
 *  <dt>(RT)OS
 *  <dd>(Real-Time) Operating System.
 *  <dt>Template
 *  <dd>A single record layout, consisting of a list of "keys" (q.v.).
 *  <dt>Key
 *  <dd>A single field in a "template" (q.v.), consisting of the
 *      key name and its type.
 *  </dl>
 */


/** @defgroup IPDR_system_overview IPDR system overview
 *
 *  <p><p>
 *  The IPDR transmitter is a library of functions and structures that
 *  allows a network element to transmit accounting data using the IPDR
 *  protocol. The transmitter is designed to be readily ported to (embedded
 *  in) any platform and host application. It communicates with the
 *  platform's Host Application via the API described herein, and with
 *  the local (RT)OS via an abstraction layer. The schematic structure
 *  of the NE's software, showing the IPDR transmitter's position in the
 *  architecture, is given below:
 *
 *  @htmlonly
 *  <table border=1 align=center>
 *  <tr><th colspan=3>Host Application</th></tr>
 *  <tr><th colspan=3>IPDR API</th></tr>
 *  <tr><th colspan=3>IPDR Data Layer</th></tr>
 *  <tr colspan=1><th><code>&nbsp;&nbsp;&nbsp;</code></th><th colspan=2>IPDR Transport Layer</th></tr>
 *  <tr><th colspan=3>Abstraction Layer</th></tr>
 *  <tr><th colspan=3>Operating System</th></tr>
 *  <tr><th colspan=3>TCP stack</th></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  The transmitter (client) is internally separated into transport
 *  and data layers, which communicate via a separate, internal API
 *  (not described in this document). The initial version of the
 *  transmitter is based on a TCP transport layer (which does not meet
 *  all the protocol requirements). Future versions may be based on
 *  the fully adequate SCTP transport [An initial negotiation over
 *  UDP, which will not vary from version to version, allows, among
 *  other things, specification of the transport. This negotiation
 *  could determine the protocol versions supported by the transmitter
 *  (client) and the collector (server) and result in utilization of
 *  the appropriate version.]
 *
 *  The IPDR transmitter (client) is implemented as an ANSI C library, designed to
 *  run on all the most popular operating systems used on NEs. It
 *  includes an abstraction layer to facilitate porting, and services
 *  accessible from the API, which allow vendors to specify data
 *  transmit fields and formats.
 *
 *  Functions in the API can be divided into two categories. The
 *  first, including platform services, are implemented by the host
 *  application and passed to the IPDR transmitter (client) library during the
 *  initialization process. Functions in the second category are
 *  called by the host application to define templates, keys and
 *  target collectors (servers), and generate and transmit accounting records.
 *
 *  The API is described in more detail in IPDR.h.
 *
 *  The sources are organized as follows:
 *  <ul>
 *    <li>API and data handling
 *    <ul>
 *      <li>Exporter/Exporter.c
 *      <li>Exporter/ExporterConsole.c
 *      <li>Exporter/ExporterMIB.c
 *      <li>Exporter/ExporterMIBConstants.c
 *      <li>Exporter/ExporterMsgs.h
 *      <li>Exporter/ExporterProtocol.c
 *      <li>Exporter/ProtocolDebug.c
 *      <li>Exporter/Templates.c
 *      <li>Exporter/VolatileQueue.c
 *      <li>Exporter/VQHandler.c
 *      <li>Common/Protocol.h
 *    </ul>
 *    <li>Common utilities (memory management, logs, parameters)
 *    <ul>
 *      <li>Common/ChunksManager.c
 *      <li>Common/CommonMsgs.h
 *      <li>Common/CommonUtil.c
 *      <li>Common/Console.c
 *      <li>Common/DynamicArray.c
 *      <li>Common/Log.c
 *      <li>Common/MemoryManagement.c
 *      <li>Common/MIBHandler.c
 *      <li>Common/ParametersHandler.c
 *    </ul>
 *    <li>Platform abstraction
 *    <ul>
 *      <li>Common/Platform.h
 *      <li>Platforms/Generic/GenericPlatform.c
 *    </ul>
 *    <li>Transport layer (currently TCP only)
 *    <ul>
 *      <li>Transports/TCP/TCPMsgs.h
 *      <li>Transports/TCP/TCPTransport.c
 *    </ul>
 *  </ul>
 *
 *  @{
 */


/** @defgroup IPDR_special_considerations Special Considerations
 *  @{
 */


/** @defgroup IPDR_special_considerations_multi_threading Multi-threading
 *
 *  <p><p>
 *  All the functions in the API are multi-thread safe. This means
 *  that separate threads can call functions in the API without any
 *  need to put specific locks around them.
 *
 *  Unless otherwise specified, none of the functions block on I/O (in
 *  general, a socket can block until buffer space is available or
 *  TPC-level acknowledgments have been received). However, this
 *  behavior depends on proper implementation of the platform
 *  abstraction layer, in particular by using non-blocking socket
 *  receive and send functions.
 *
 *  When the IPDR transmitter (client) library operates in a multi-threaded
 *  environment, the following behavior is assumed:
 *  <ul>
 *  <li>
 *  A single lockable entity is provided by the abstraction layer,
 *  which is activated by the Lock, Unlock functions (such as a single
 *  mutex, semaphore or a similar mechanism).
 *  <li>
 *  The above entity is automatically locked by the abstraction
 *  layer before callbacks are triggered by the abstraction layer
 *  (such as timer and read / write notification callbacks).
 *  </ul>
 */


/** @defgroup IPDR_special_considerations_strings Strings
 *
 *  <p><p>
 *  In general, standard ASCII C strings (null-terminated) are
 *  used. For data delivery, additional formats are provided, which
 *  allow handling non-ASCII data. Unless otherwise specified, all
 *  strings that are passed as parameters to API functions are saved
 *  if needed (e.g., the names for transmitters (clients) and sessions).
 */


/** @defgroup IPDR_special_considerations_future_compatitibility Future Compatibility
 *
 *  <p><p>
 *  This API is subject to change with experience gained in its
 *  use. Every attempt will be made to maintain backwards
 *  compatibility, but in some cases that will not be possible. To
 *  minimize the impact of future changes, programmers should:
 *  <ul>
 *  <li>Not depend on the format of log messages or event messages.
 *  <li>Not depend on the absence of particular return codes from API
 *  functions (additional return codes may be added in the future.
 *  <li>Not depend on callbacks being synchronous with API calls.
 *  <li>Not depend on the content of "void pointer" or similar "handle" types.
 *  <li>Not depend on "void *" types remaining the same in the future
 *  (these may be changed to opaque structures in the future, to
 *  provide better compile-type type checking.
 *  <li>Not depend on platform-level implementation details, for
 *  example that a socket is an integer (this is true on Unix or
 *  VxWorks, but not on some other platforms).
 *  <li>The socket definition (::IPDR_SocketHandle) may change in the
 *  future. Also, its implementation location might change; currently,
 *  it is split between Defs.h, GenericCMPlatform.h, and GenericCMPlatform.c.
 *  </ul>
 */

/*@}*/


/** @defgroup IPDR_usage IPDR transmitter (client) usage guidelines
 *  @{
 */


/** @defgroup IPDR_usage_steps Steps for implementing a IPDR transmitter (client)
 *
 *  <p><p>
 *  <ol>
 *  <li>
 *  Write a platform-dependent implementation for the required
 *  services (memory allocation, timers, non-volatile memory)
 *  and set up an appropriate ::IPDR_PlatformServices structure.
 *  This is necessary only if the target platform is not POSIX,
 *  Linux, Solaris, or VxWorks.
 *  <li>
 *  For any template that will be transmitted as accounting data from
 *  the application, define keys corresponding to the structure's
 *  fields. For each key, write suitable sccessor function, if
 *  required. Define the template with a vector IPDR_Key values.
 *  <li>Call the appropriate platform-dependent intialization
 *  routine for the ::IPDR_PlatformServices structure described in #1
 *  (such as GenericPlatform_Initialize()).
 *  <li>
 *  Call IPDR_InitExporter() along with a pointer to the platform
 *  services structure containing references to the functions
 *  implemented in (1). Configuration can be done using
 *  the IPDR_ParametersHandlerFunc callback and the ::IPDR_LoadMIBFunc
 *  callback.
 *  <li>
 *  Create a session using IPDR_CreateSession().
 *  <li>
 *  Associate one or more collectors (servers) with the session (using IPDR_AddCollector()).
 *  <li>
 *  Add templates using IPDR_CreateTemplate(), using keys defined in #2.
 *  <li>
 *  Start the transmitter (client) with IPDR_StartExporter().
 *  <li>
 *  Use IPDR_SendData() to send accounting records.
 *  </ol>
 *
 *  Optional steps, which should be done before calling IPDR_StartExporter():
 *  <ol>
 *  <li>
 *  Define event handling, using IPDR_SetExporterEventHandlerDetailed().
 *  <li>
 *  Define handling of template negotiation, where the collector (server) does not
 *  need all the fields, using IPDR_SetTemplateChangedCallback().
 *  This is needed only if the platform can save processing by not
 *  generating data for unneeded fields.
 *  <li>
 *  Bind IPDR MIB into the platform MIB, using IPDR_MIBGet()
 *  and IPDR_MIBGetNext() and IPDR_MIBSet(). During transmitter (client) startup,
 *  the ::IPDR_LoadMIBFunc callback can be used to populate the
 *  configuration parameters in the MIB.
 *  <li>
 *  Bind logging into the platform logging, using
 *  IPDR_SetLogOutputCallbackDetailed().
 *  </ol>
 *
 *  Optional steps, after IPDR_StartExporter():
 *  <ol>
 *  <li>
 *  Monitor progress using IPDR_GetSessionStatistics() or
 *  IPDR_GetServerStatisticsAndState(). You would do this
 *  if the default statistic messages (set by
 *  <code>configuration.statisticsInterval</code>)
 *  are not to your liking.
 *  </ol>
 *
 *  Shut-down is done by:
 *  <ol>
 *  <li>IPDR_DrainQueue(), if desired.
 *  <li>IPDR_DeleteExporter().
 *  <li>IPDR_PlatformServices::Delete().
 *  </ol>
 */


/** @defgroup IPDR_usage_template Template structure recommendations
 *
 *  <p><p>
 *  The following are only of consideration if there are extreme
 *  performance issues.
 *  <ul>
 *  <li>
 *  For best performance, access keys by offsets and use only fixed
 *  length keys. When using this scenario it's better to group keys
 *  which are mandatory together (i.e. in a consecutive memory
 *  region), which will result in reducing the overhead a bit.
 *  <li>
 *  If possible use a single type of access to all keys.
 *  <li>
 *  Using accessor function implies the overhead of a function call
 *  per key (which was configured to access by function) for each
 *  record.
 *  </ul>
 *
 *  The IPDR transmitter (client) differentiates internally among six types of
 *  templates, depending on the key types and the accessor types of
 *  keys. The template's types are:
 *  <ol>
 *  <li>All keys are accessed by offsets and all keys are fixed length.
 *  <li>All keys are accessed by functions and all keys are fixed length.
 *  <li>There is a mixture of functions and offsets and all keys are
 *  fixed length.
 *  <li>All keys are accessed by offsets and there are variable length keys.
 *  <li>All keys are accessed by functions and there are variable length keys.
 *  <li>There is a mixture of functions and offsets and there are
 *  variable length keys.
 *  </ol>
 *
 */
/*@}*/


/** @defgroup IPDR_used_functions C and system (POSIX) functions used
 *
 *  <p><p>
 *  The following standard C functions are used throughout the code:
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; abort</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; abs</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; atoi</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; atol</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fclose</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fflush</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fgets</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fopen</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fprintf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fread</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; free</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fwrite</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; malloc</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; memcpy</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; memset</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; printf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; snprintf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sprintf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sscanf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strchr</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strcmp</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strcpy</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strerror</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strlen</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strncmp</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strncpy</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtod</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtok</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtol</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtoll</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtoul</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; strtoull</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; vsnprintf</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; vsprintf</code>
 *
 *  <p>
 *  Some of the following are used by the GenericCMPlatform.h abstraction
 *  and are confined to GenericCMPlatform.c.
 *  (This is for a POSIX implementation;
 *  VxWorks and other operating systems are slightly different).
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; accept</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; bind</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; clock_gettime(CLOCK_REALTIME, ...)</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; close</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; connect</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; fcntl</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; getsockopt</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; gettimeofday</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; listen</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; localtime_r</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; poll</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_creat</code>e
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_equal</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_mutex_destroy</code>y
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_mutex_init</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_mutex_lock</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_mutex_unlock</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_self</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; pthread_sigmask</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; recv</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; recvfrom</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; select</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; send</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sendto</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; setsockopt</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; shutdown</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sigaddset</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sigemptyset</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; sleep</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; socket</code>
 *  <br><code>&nbsp;&nbsp;&nbsp;&nbsp; stat</code>
 */


/** @defgroup IPDR_MIB IPDR configuration and monitoring data (MIB)
 *
 *  <p><p>
 *  @see IPDR_MIB_related
 *  @see IPDR_transmitter_MIB
 *  @see IPDR_configuration_values
 *  @see IPDR_ParametersHandlerFunc
 *
 *  @include IPDR.mib
 */
/*@}*/


/** @defgroup IPDR_reliability IPDR reliability mechanism
 *  @{
 */


/** @defgroup IPDR_reliability_base_transport Base transport (TCP)
 *
 *  <p><p>
 *  IPDR is designed to work on top of any reliable transport. The
 *  current implementation uses TCP but advantage could also be taken
 *  of SCTP in the future.
 *
 *  TCP is reliable in the following sense:
 *  <ul>
 *  <li>Check-sums are calculated to ensure data integrity.
 *  <li>TCP handles out-of-order and duplicate packets, to provide a
 *    reliable stream of bytes (i.e., all the packet handling is done by
 *    TCP; this allows IPDR records to be split across packets,
 *    transparently to the application).
 *  <li>TCP retransmits packets if they are not acknowledged in
 *    time. This is transparent to the application, which only sees a
 *    stream of bytes.
 *  <li>If packets cannot be delivered, eventually an error indication
 *    will be generated. However, the time for this is
 *    implementation-defined, and can be hours.
 *  </ul>
 *
 *  In addition, TCP provides congestion control. TCP depends on IP
 *  for alternative routing of packets in an unreliable network.
 *
 *  TCP does not provide application-level reliability. It only
 *  handles delivery of packets to the receiving machine. Applications
 *  that need the highest level of reliability must provide their own
 *  reliability mechanism.
 *
 *  The TCP connection is made from the collector (server) to the
 *  transmitter (client). If there are multiple collectors (servers), there will be
 *  multiple connections to the transmitter; however only one is "active"
 *  at any time (the transmitter's configuration allows setting the
 *  priority of the collectors (servers)).
 *
 *  Whenever a collector (server) loses connectivity with the transmitter (either
 *  because the network connection has broken in such a way that TCP
 *  sees the break; or because the transmitter has switched to another
 *  collector), the collector tries repeatedly to connect to the transmitter.
 *
 *  There is no communication between the various collectors (servers) that are
 *  receiving data for any particular transmitter(s).
 *
 *  The collector (server) can be configured so that a single collector (server) can receive from
 *  one or multiple transmitters. In general, it is recommended to have one
 *  transmitter (client) to one collector (server) unless the data rates are very low, in which
 *  case it is easier to configure multiple transmitters to a single collector (server).
 *
 *  From a performance point of view, there is no significant
 *  difference between having multiple transmitters or a single transmitter to
 *  one collector (server). For configuration, a single collector (server) and multiple
 *  transmitters are easier to set up.
 */


/** @defgroup IPDR_reliability_record_level IPDR record-level reliability (fail-over)
 *
 *  <p><p>
 *  TCP is not sufficient for quick detection of failure; and the
 *  standard implementations do not allow application-level
 *  acknowledgment. Therefore, IPDR provides an application-level
 *  acknowledgment mechanism. The protocol leaves some flexibility in
 *  the implementation. The current implementation batches up
 *  acknowledgments for efficiency as described below.
 *
 *  The IPDR transmitters (clients) and collectors (servers) use the
 *  host systems' TCP stacks. If desired, these can be tuned to handle
 *  situations such as "long fat pipes" or to have increased socket
 *  buffers. However, our experience is that the default values work
 *  well for data rates to at least 5000 records/second on a LAN.
 *
 *  IPDR implementation focuses on time-outs, with the
 *  following main points (most of the timeouts can be configured to
 *  different values, if desired):
 *  <ul>
 *  <li>Data records are sent from the exporter to the collector (server) as soon as
 *    possible (the TCP stack usually will try to combine multiple records into
 *    a single packet using the Nagle algorithm, which typically has a
 *    time-out of 200 milliseconds; the IPDR implementation leaves this
 *    processing enabled).
 *  <li>10 seconds time-out for the transmitter (client) to not get an
 *    acknowledgment from the collector (server).
 *   The timer is started when the first record is sent. The timer
 *      is re-set whenever an acknowledgment is received that acknowledges
 *      at least one new record (that is, "keep-alive" acknowledgments are
 *      ignored by the timer, so that if the collector is too busy to
 *      process records, the behavior is the same as if the collector were
 *      disconnected or crashed).
 *  </ul>
 */


/** @defgroup IPDR_reliability_protocol Details of the protocol's implementation: acknowledgment, fail-over and fail-back
 *
 *  <p><p>
 *  To be more precise, the fail-over and fail-back mechanism is as follows
 *  (this description is somewhat specific to IPDR Collector; other
 *  implementations are possible but should follow this design in its main points):
 *  <ol>
 *  <li>A record is sent (via IPDR_SendData()) and the 10-second
 *    timer is started [10 seconds is the default; it can be
 *    changed]. The sent record is saved in a queue on the transmitter (client) in
 *    case it is not acknowledged.
 *  <li>Subsequent records do not change the timer, which continues to
 *    count down. These records are also saved in a queue on the transmitter (client).
 *  <li>The records are received at the collector (server), batched (for efficiency)
 *    and enqueued to some persistent queue implementation).
 *  <li>Every 1 second, a timer on the collector (server) causes an ACK to be sent
 *    with the sequence number of the record most recently enqueued. [The timer interval can be changed.]
 *  <li>When the transmitter (client) receives the ACK, it deletes all saved
 *    (queued) records up to this sequence number. If there is at least
 *    one saved (enqueued) record on the transmitter, the timer is started
 *    again (as in step 1 and processing continues as at step 2. If
 *    there are no saved (enqueued) records, the timer is turned off and
 *    processing continues as at step 1
 *    <ul><li>This processing happens only the ACK increments the
 *      sequence number (that is, if it isn't a "keep-alive"
 *      ACK). Otherwise, the ACK is ignored and the timer continues to
 *      run.
 *    </ul>
 *  <li>If the acknowledgment timer expires, it is assumed that the
 *    collector (server) has crashed or become "stuck". This causes the
 *    connection to be broken to the primary collector (server). All the saved
 *    (queued) records are sent to the secondary collector (server) and the
 *    10-second timer is started. Processing continues as at step 2.
 *  <li>While sending to the secondary collector (server), if an ACK is received
 *    from the primary collector, the connection to the secondary collector is
 *    dropped and processing continues as at step 5.
 *  </ol>
 *
 *  The 1-second interval by the collector (server) in sending acknowledgments is
 *  an efficiency consideration to avoid sending an acknowledgment for
 *  every record. A record is eligible for acknowledgment only after
 *  it has been persisted.
 *
 *  There is a 1 to 10 second time-out for the collector (server) to
 *  re-try connecting to the transmitter (client) if the TCP
 *  connection is lost. (Initial connection is tried at 1-second
 *  intervals and is gradually increased to 10 seconds if there is no
 *  response). Again, this value is configurable.
 *
 */


/** @defgroup IPDR_reliability_failover Fail-over event display
 *
 *  <p><p>
 *  When a time-out occurs or the TCP connection is lost, the transmitter (client)
 *  fails over to the secondary collector (server). It also generates an "event"
 *  which the host can display and/or write to a log. The transmitter
 *  breaks the TCP connection, so that the collector (server) will reset its
 *  internal state and try to reconnect (e.g., if it has been too
 *  busy to acknowledge).
 *
 *  When fail-over occurs, the IPDR code automatically re-sends all
 *  unacknowledged records to the secondary collector (server) (no special event
 *  is signaled for this; however a signal is generated for the change
 *  from primary to secondary collector). Because it is possible that a
 *  record has been received and processed but not acknowledged (e.g.,
 *  if the connection went down just when the acknowledgment was being
 *  sent), duplicate records may exist. The processing for this is
 *  described in more detail below.
 */


/** @defgroup IPDR_reliability_failback IPDR record-level reliability (fail-back)
 *
 *  <p><p>
 *  When the primary collector (server) comes back online, it connects to the
 *  transmitter (client), which switches back to transmitting to the primary
 *  collector (server). The transmitter treats this situation as if the secondary
 *  collector (server) had failed (that is, it ignores any further acknowledgments
 *  from the secondary) and retransmits all unacknowledged
 *  records. This behavior is slightly inefficient, because it could
 *  retransmit some records that have already processed by the
 *  secondary collector (server) and simply haven't been acknowledged; however,
 *  the decision was made to use simpler and more robust code rather
 *  than try to be of the utmost efficiency in a fairly rare
 *  situation.
 */


/** @defgroup IPDR_reliability_transmitter_failure transmitter (client) failure
 *
 *  <p><p>
 *  The IPDR transmitter (client) maintains a queue (memory based)
 *  of unacknowledged records (TCP buffering may also means that some
 *  of these have not been sent either). If the transmitter crashes, the
 *  contents of this queue will be lost.
 *
 *  If collector (server) fail-over takes too long, the queue can fill up
 *  and records will be lost (the IPDR code detects this and raises
 *  an event).
 *
 *  <!-- &times; is also �(0xfc) but doxygen doesn't understand -->
 *  Proper sizing of the queue is: maximum-records-per-second � *  (average-record-length + 16) �worst-case-failover-time. (The "16"
 *  is for the overhead of each IPDR record).
 *  In calculating average record length, use average string lengths,
 *  plus room for length value or null terminator, as appropriate.
 *
 *  The parameter is <code>configuration.queueMaxMemory</code> in
 *  section @ref IPDR_configuration_values.
 */


/** @defgroup IPDR_reliability_other Other failure conditions
 *
 *  <p><p>
 *  There are a number of more rare failure conditions. Most are
 *  handled by the mechanism given above (e.g., if an ICMP record
 *  indicates that a remote machine is unavailable, the underlying TCP
 *  stack should translate this into an error condition, which will
 *  cause the IPDR transmitter (client) or collector (server) to disconnect).
 *
 *  The transmitter (client) code has been made "robust", by detecting conditions
 *  such as the same collector attempting to create multiple
 *  connections. In this case, all connections to the collector are
 *  broken, allowing the collector to reset and make a single new
 *  connection. (This provides a possible denial-of-service security
 *  hole, but it is assumed that the network elements are inside a
 *  protected network ... the reason for breaking all connections is
 *  that there are some unusual error situations where a "stale"
 *  connection can persist, preventing the collector (server) from making a new
 *  connection.)
 */


/** @defgroup IPDR_reliability_internal_queue Internal queue sizing
 *
 *  <p><p>
 *  The parameter <code>configuration.queueMaxMemory</code>
 *  (see @ref IPDR_configuration_values) controls the size of the
 *  internal queue. It can be set to 0 (meaning <i>unlimited</i>).
 *  If a limit is set, it should be:
 *  <br>
 *  maximum-records-per-second � *         (average-record-length + 16) �worst-case-failover-time.
 *
 *  (16 is the length of the IPDR record header.)
 *
 *  When the queue fills up, each <em>new</em> record is thrown away
 *  and the #IPDR_EVENT_RECORD_LOST event is raised. (The reasons for
 *  throwing away the new record and not removing the oldest record
 *  are (a) it's much more complicated to program and (b) the oldest
 *  record might be partially transmitted and there is no way in
 *  general to abort its transfer.
 *
 *  The queue size affects the configuration of a de-duplicator (see
 *  @ref IPDR_reliability_deduplication for more details).
 */


/** @defgroup IPDR_reliability_deduplication De-duplication
 *
 *  <p><p>
 *  Fail-over can result in duplicate records being sent. This
 *  is because a connection can be lost while an acknowledgment is
 *  being transmitted. For this reason, any robust IPDR collector must
 *  also be used with a de-duplicator (the IPDR protocol provides a
 *  "duplicate" flag, but this is only a hint and is not sufficient
 *  to prevent duplicates in allcases).
 *  The IPDR protocol provides a "boot time" field which,
 *  together with the DSN, can be used as a de-duplicator. Whenever
 *  IPDR_InitExporter() is called, the "boot time" is reset.
 *
 *  The de-duplicator should have room for at least as many records as
 *  the maximum that the internal queue can hold. That is its look-up
 *  table should contain at least this many entries:
 *  <br>queueMaxMemory �(minimum-record-size + 16) <br>The reasoning
 *  behind this is: Suppose all the records in the queue are sent
 *  before an acknowledgment is received.  When the connection is
 *  re-established, these records will all be re-sent, so the
 *  de-duplicator must have room to remember the oldest record in the
 *  queue.  In practice, the de-duplicator can keep fewer entries than
 *  this because of other considerations that will prevent sending all
 *  the records. However, a smaller de-duplication table is not
 *  recommended because of the variety of factors, some of which
 *  depend on how the platform implements TCP/IP.
 *
 */
/*@}*/


/** @defgroup IPDR_sessions Sessions
 *
 *  <p><p>
 *  A session is established to allow formatted records to be sent to
 *  one or more redundant collectors (servers). Records contain data organized
 *  into templates, which are referred to via keys that can be enabled
 *  and disabled. See section @ref IPDR_templates.
 *
 *  Multiple sessions can be created by the host application, for
 *  example to direct different types of data to different
 *  collectors (servers). Conversely, a transmitter (client) can send the same information to
 *  several collectors (servers) within a single session, for redundancy or load
 *  balancing purposes. (Load balancing will be fully supported in a
 *  future version.) In addition, templates are shared between
 *  sessions - the client can transmit data formatted by any template
 *  in any session.
 *
 *  A session has one or more collectors (servers) associated with it. When
 *  IPDR_SendData() is called, the record is sent to each session
 *  (unless template negotiation has turned off the sending for that
 *  session; the template negotiation is controlled by the collector).
 *  Each session has at most one active collector (server); an attempt is made to
 *  send the record to the active collector (server), with failover to alternate
 *  collectors (servers) if that fails. When the record has been acknowledged for
 *  all sessions, it is removed from the internal queue.
 *
 *  The state of sessions (i.e. the set of sessions and their
 *  associated collectors (servers)) can optionally be stored in non-volatile
 *  memory (see @ref IPDR_non_volatile_memory,
 *  @ref IPDR_MIB, @ref IPDR_MIB_related, and
 *  the <code>configurationCommandString</code> in @ref IPDR_transmitter_MIB).
 *  Parameter values can also be set via the
 *  parameters handler function (see
 *  @ref IPDR_initialization_starting_deletion and
 *  @ref IPDR_ParametersHandlerFunc).
 */


/** @defgroup IPDR_alternate_collectors Alternate collectors (servers) policy
 *
 *  <p><p>
 *  collectors (servers) are assigned priorities and different (multiple) collectors (servers)
 *  can have the same priority. Automatic switching to an alternate
 *  collector (server) occurs under the following conditions:
 *  <ul>
 *  <li>The collector (server) sends a STOP message to the transmitter (client).
 *  <li>The transmitter's transport layer notifies the data layer of active
 *      collector (server) disconnection.
 *  <li>The transmitter (client) fails to receive an ACK for a prescribed
 *      duration. Collector(s) may send periodic "keep-alive" ACKs, using
 *      the most recent ACK message. However, the transmitter
 *      will ignore ACKs that do not increment the sequence number.
 *      The collector(s) may also send other messages such as
 *      STATUS REQ to verify that the TCP collection is alive but these
 *      messages do not cause the transmitter to adjust its notion
 *      of whether the collector is working or not. The reason for this
 *      is simple: the collector might be overloaded and able to only
 *      send ACKs but not actually process any of the records.
 *  <li>An ACK message was received from a higher priority collector (server) to
 *      which the transmitter (client) wasn't able to send data, due to reaching the
 *      window size limit.
 *  <li>A higher priority collector (server) acknowledged the reception of the
 *      current set of templates (TMPL DATA ACK / FINAL TMPL DATA ACK
 *      messages).
 *  <li>IPDR_RemoveServer() removes the collector (server).
 *  </ul>
 *  <p>
 *  In case one of the above conditions occur, the transmitter (client) sends data
 *  to the highest priority collector (server) which:
 *  <ul>
 *  <li>has indicated that is started using a START message.
 *  <li>has acknowledged the current templates configuration (using
 *      TMPL DATA ACK / FINAL TMPL DATA ACK).
 *  <li>is connected at the transport layer level.
 *  </ul>
 */
#ifndef _IPDR_H_


/** Macro to prevent double inclusion of this header.
 */
#define _IPDR_H_

#include <IPDR/Config.h>

/** Source code version identifier.
 */
#define IPDR_H_RCSID "@(#) $Id: IPDR.h,v 1.65 2005/05/11 12:38:34 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)

/** Renaming global version identifier (to avoid name clashes).
 */
#define IPDR_h_rcsid IPDR__IPDR_h_rcsid
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

#include <stddef.h>


/** @defgroup IPDR_general_typedefs General types, macros, and "opaque" types
 *
 *  <p><p>
 *  These are used by a variety of API functions. Where possible, void
 *  pointers have been avoided, instead using pointers-to-struct,
 *  which allows the compiler to do some extra type-checking.
 *
 *  @{
 */


/** @def IPDR_API_LINKAGE
 *  The linkage type to add to each external function declaration
 *  (mainly for Windows).
 */
#if defined(_MSC_VER)
#ifdef IPDR_STATIC


/** Linkage for Windows static.
 */
#define IPDR_API_LINKAGE
#elif defined(IPDR_EXPORTS)


/** Linkage for Windows dynamic (DLL) exports.
 */
#define IPDR_API_LINKAGE  __declspec(dllexport)
#else


/** Linkage for Windows dynamic (DLL) imports.
 */
#define IPDR_API_LINKAGE  __declspec(dllimport)
#endif
#else


/** Linkage for non-Windows.
 */
#define IPDR_API_LINKAGE
#endif


/** A convenience macro for defining the return value and linkage for
 *  an API function.
 */
#define IPDR_API IPDR_Status IPDR_API_LINKAGE


/** Value to use for "default" IP address (for type @ref IPDR_IPAddress).
 */
#define IPDR_DEFAULT_ADDRESS  0


/** @brief Type returned by a IPDR API function. Actual values are given by
 *  the enumeration #IPDR_STATUS_RETURN_CODE.
 */
typedef unsigned int IPDR_Status;


/** The type used as a context argument when calling platform function.
 */
typedef struct IPDR_tag_PlatformArg *IPDR_PlatformArg;


/** The information about a timer.
 */
typedef struct IPDR_tag_Timer *IPDR_TimerID;


/** The information about an event that is triggered by a socket
 *  becoming available for I/O. In effect, this is a "callback". If
 *  the socket is handled by non-blocking I/O, the callback is called
 *  when the file descriptor (socket) becomes available for I/O (or
 *  has had something traumatic happen to it, such as being
 *  closed). In the case of blocking I/O, the callback is used by the
 *  generic thread which repeatedly does the I/O. (Note: for the
 *  blocking case - which has a thread - the event is not put into the
 *  list of non-blocking callbacks.)
 */
typedef struct IPDR_tag_SocketEvent *IPDR_SocketEventID;


/** The information for a transmitter (client), as created by IPDR_InitExporter().
 */
typedef struct IPDR_tag_Exporter *IPDR_ExporterHandle;


/** The information for a session, as created by IPDR_CreateSession().
 */
typedef struct IPDR_tag_Session *IPDR_SessionHandle;


/* The information for a Collector, as created by IPDR_AddCollector */
typedef struct IPDR_tag_Collector *IPDR_CollectorHandle ;

/** The information for a template, as created by
 *  IPDR_CreateTemplate().  This should be treated as an opaque type,
 *  with no reliance on the actual structure, which may change in the
 *  future. */
typedef struct IPDR_tag_TemplateHandle { unsigned int i; unsigned char s;} IPDR_TemplateHandle;


/** MAC address definition */

typedef unsigned long long IPDR_MACAddress;

/** An internal IP address (#unint32_t). Following ancient tradition,
 *  it is kept in host byte order, not network byte order.
 */
typedef unsigned int IPDR_IPAddress;


/** A time value as seconds since "epoch" (1970-01-01T00:00:00 UTC
 *  (GMT)) (the number of bits in the type should be the same as or
 *  larger than <code>time_t</code>).
 */
typedef long IPDR_Time;


/** @defgroup IPDR_transitional_typedefs Transitional typedefs and macros
 *
 *  <p><p>
 *  The following definitions are transitional and will be
 *  replaced by definitions that are not so tied to the Berkeley
 *  socket style.
 *
 *  @{
 */


/** An opaque socket, whose contents may change in the future.
 *  @see Defs.h */
typedef struct IPDR_tag_Socket
{
    int socketFd; /**< The socket file descriptor. Do not depend on this; use #IPDR_SOCKET_DISPLAY instead. */
} IPDR_SocketHandle;


/** Set a socket to an invalid value (that is, not open)
 *  @param S Pointer to the socket.
 */
void IPDR_SetInvalidSocket(IPDR_SocketHandle* S);


/** Check whether a socket is valid (opened).
 *  @param S The socket to check.
 *  @return 0 for not valid (not open); 1 for valid (opened).
 */
int IPDR_IsValidSocket(const IPDR_SocketHandle S);


/** Check whether two sockets are the same.
 *  @param s1 The first socket.
 *  @param s2 the second socket.
 *  @return 0 for different; 1 for the same.
 */
int IPDR_SocketEq(const IPDR_SocketHandle s1, const IPDR_SocketHandle s2);

  /*@}*/
  /*@}*/


/** @defgroup IPDR_key_type_attributes Key and type (template) attributes
 *  @ingroup IPDR_templates
 *
 *  <p><p>
 *  Each key is identified in the context of its enclosing
 *  template. For every key there is an optional accessor function
 *  that takes as its argument a pointer to the actual (raw) data, and
 *  returns a pointer to the value of the key.
 *
 *  One of a key's attributes is a flag, which enables / disables the
 *  export of the key
 *
 *  @{
 */


/** Mask for disabling a key in a template (for IPDR_Key::KeyAttributes).
 */
#define IPDR_KEY_DISABLED     0x00000001UL


/** Convention with @ref IPDR_KeyType values for varying length fields: this value
 *  is "or"ed with the key type (e.g., #XDR_TYPE_STRING). */
#define IPDR_TYPE_VAR_LENGTH  0x4000U


/** Access a value from a record by a function callback (possible
 *  value for IPDR_Key::AccessType).
 */
#define IPDR_ACCESS_BY_FUNCTION 0x0001U


/** Access a value from a record by an offset (possible value for
 *  IPDR_Key::AccessType).
 */
#define IPDR_ACCESS_BY_OFFSET   0x0002U


/** Access data using a functional callback. For each field being
 *  output by IPDR, the data field needs to be accessed. For the
 *  #IPDR_ACCESS_BY_OFFSET case, this can be done by computing the
 *  pointer as <code>Data+(IPDR_Offset)key.KeyAccessor</code>.
 *  For the #IPDR_ACCESS_BY_FUNCTION case, the pointer is computed by this
 *  key access callback.
 *  The callback is called only if the key is enabled.
 *
 *  @param Data The data record being output.
 *  @param ExtraData The extra data recorded with IPDR_Key::ExtraData for this field.
 *  @param Value (updated) pointer to the data field to be transmitted.
 *  @see IPDR_Key
 */
typedef void (*IPDR_KeyAccessor)(const void *Data,
                                  void *ExtraData,
                                  const void **Value);


/** The type to use for accessing a field offset.
 */
typedef unsigned int IPDR_Offset;

/** Session stop reason codes*/
enum IPDR_SessionStopReason
{
	IPDR_STOP_REASON_ENDOFDATA 			= 0, /** end of data for session */
	IPDR_STOP_REASON_SWITCHCOLLECTOR	= 1, /** 1 = handing off to higher priority Collector */
	IPDR_STOP_REASON_EXIT				= 2, /** 2 = Exporter terminating */
	IPDR_STOP_REASON_CONGESTION			= 3, /** 3 = congestion detected */
	IPDR_STOP_REASON_RENEGOTIATION		= 4, /** 4 = renegotiation is required */
	IPDR_STOP_REASON_STARTNEGOTIATION	= 5, /** 5 = start negotiation acknowledge */
	IPDR_STOP_REASON_ENDOFDOCUMENT		= 6, /** 6 = end of IPDRDoc */
	IPDR_STOP_REASON_TEMPLATEUPDATED	= 7, /** 7 = Template data was updated */
	IPDR_STOP_REASON_MINVENDOR			= 256/** 256 = minimum vendor code */
} ;


/** Macro that indicates whether UTF8, UTF16, BLOB are implemented.
 */
#if 0
#define IPDR_COMPLEX_VARYING_IMPLEMENTED
#endif


/** Key types identifiers (for IPDR_Key::KeyTypeID).  These are in
 *  IPDR/XDR Encoding format, v. 3.5.1, section 4.3.1.2
 */

enum XDR_KeyType
{
/* Primitive types */
	XDR_TYPE_INT			= 0x0021U, /**< an integer in the range [-2147483648,2147483647] */
	XDR_TYPE_UINT			= 0x0022U, /**< a nonnegative integer in the range [0,4294967295] */
	XDR_TYPE_LONG			= 0x0023U, /**< obvious extensions of integer and unsigned integer defined above. */
	XDR_TYPE_ULONG			= 0x0024U, /**< obvious extensions of integer and unsigned integer defined above. */
	XDR_TYPE_FLOAT			= 0x0025U, /**< 4 bytes single-precision floating-point number */
	XDR_TYPE_DOUBLE			= 0x0026U, /**< 8 bytes double-precision floating-point number */
	XDR_TYPE_BASE64			= 0x0027U | IPDR_TYPE_VAR_LENGTH, /**< */
	XDR_TYPE_HEXBINARY		= 0x0027U | IPDR_TYPE_VAR_LENGTH, /**< 4 byte length indication + the number of bytes of hex data */
	XDR_TYPE_STRING			= 0x0028U | IPDR_TYPE_VAR_LENGTH, /**< 4 byte length followed by string */
	XDR_TYPE_BOOLEAN		= 0x0029U, /**< 1 byte value which contains 0 for FALSE and 1 for TRUE */
	XDR_TYPE_BYTE			= 0x002aU, /**< an integer in the range[-128,127] */
	XDR_TYPE_UBYTE			= 0x002bU, /**< a non-negative integer in the range[0,255] */
	XDR_TYPE_SHORT			= 0x002cU, /**< an integer in the range [-32783,32782] */
	XDR_TYPE_USHORT			= 0x002dU, /**< a non-negative integer in the range [0,65565] */
/* The following are the Type IDs for the derived types used in the protocol:*/
	XDR_TYPE_DATETIME		= 0x0122U, /**< */
	XDR_TYPE_TIMEMSEC		= 0x0224U, /**< */
	XDR_TYPE_IPV4ADDR		= 0x0322U, /**< */
	XDR_TYPE_IPV6ADDR		= 0x0427U, /**< */
	XDR_TYPE_UUID			= 0x0527U, /**< */
	XDR_TYPE_TIMEUSEC		= 0x0623U, /**< */
	XDR_TYPE_MACADDR		= 0x0723U  /**< */
};

/** Key types definitions (for IPDR_Key::KeyTypeID).  These are in
 *  IPDR/XDR Encoding format, v. 3.5.1, section 4.3.1.2
 */
typedef int					XDR_INT;
typedef unsigned int		XDR_UINT;
typedef long long			XDR_LONG;
typedef unsigned long long	XDR_ULONG;
typedef float				XDR_FLOAT;
typedef double				XDR_DOUBLE;

#define DEFINE_BASE64( name, size ) int name##__len_; char name[ size ];
#define DEFINE_HEXBINARY( name, size ) int name##__len_; char name[ size ];
#define DEFINE_STRING( name, size ) int name##__len_; char name[ size ];

#define ACCESS_BASE64( name ) name##__len_
#define ACCESS_HEXBINARY( name ) name##__len_
#define ACCESS_STRING( name ) name##__len_

#define FILL_BASE64( name, str )								\
{																\
	name##__len_ = 0 ;											\
	memset(name, 0, sizeof(name));								\
	if(str != NULL)												\
	{															\
		size_t sz = strlen( str );								\
		strncpy(name, str, sizeof(name));						\
		name##__len_ = (sz>sizeof(name))?(sizeof(name)):(sz);	\
	}															\
}

#define FILL_HEXBINARY( name , data, size )						\
{																\
	name##__len_ = 0 ;											\
	size_t sz = size;											\
	memset(name, 0, sizeof(name));								\
	if(data != NULL)											\
	{															\
		sz = (sz>sizeof(name))?(sizeof(name)):(sz);				\
		memcpy(name, data, sz);									\
		name##__len_ = sz;										\
	}															\
}

#define FILL_STRING( name , str )								\
{																\
	name##__len_ = 0 ;											\
	memset(name, 0, sizeof(name));								\
	if(str != NULL)												\
	{															\
		size_t sz = strlen( str );								\
		strncpy(name, str, sizeof(name));						\
		name##__len_ = (sz>sizeof(name))?(sizeof(name)):(sz);	\
	}															\
}

typedef unsigned char		XDR_BOOLEAN;
typedef char				XDR_BYTE;
typedef unsigned char		XDR_UBYTE;
typedef short				XDR_SHORT;
typedef unsigned short		XDR_USHORT;
typedef unsigned int		XDR_DATETIME;
typedef unsigned long long	XDR_TIMEMSEC;
typedef unsigned int		XDR_IPV4ADDR;
typedef unsigned char		XDR_IPV6ADDR[16];
typedef unsigned char		XDR_UUID[20];
typedef unsigned long long	XDR_TIMEUSEC;
typedef unsigned long long	XDR_MACADDR;


/** The definition of a single key in a template, as passed to
 *  IPDR_CreateTemplate().
 *
 *  If these keys are used to automatically generate field names,
 *  it is recommended to keep the key label length to less than
 *  20 characters to avoid truncation in the database field name.
 */
typedef struct
{
    unsigned int KeyID;     /**< The unique identifier for the key. (Unique within a template.) */
    unsigned int KeyTypeID; /**< The key's type: actually @ref IPDR_KeyType. */
    const char * KeyName;   /**< The key's unique (internal) name. (Unique within a template.) */
    const char * KeyLabel;  /**< The key's unique label (external name, for display in a UI). (Unique within a template.) */
    const char * KeyHelp;   /**< The long description for the key, for display in a UI. */
    unsigned int Enabled; 		/**< Or-ed: #IPDR_KEY_DISABLED. */
    unsigned int AccessType; /**< Or-ed: #IPDR_ACCESS_BY_FUNCTION or #IPDR_ACCESS_BY_OFFSET. */
    void *       ExtraData;  /**< Context argument for \a KeyAccessor as a function. */
    IPDR_KeyAccessor KeyAccessor; /**< either a cast offset (typically set with the C
                                    * <code>offsetof</code> macro) or a function pointer
                                    * (::IPDR_KeyAccessor). */
}
IPDR_Key;


/** The definition of a template with its keys (the parameters to
 *  IPDR_CreateTemplate().
 */
typedef struct
{
    int         TemplateID;   /**< The unique identifier for the template. */
    const char *SchemaName;  /**< The the template's schema name */
    const char *TypeName;  /**< The the template's type name */
    int         NumKeys;      /**< How many entries in the \a Keys vector. */
    IPDR_Key * Keys;         /**< Vector of keys. */
}
IPDR_Template;

/*@}*/


/** The definition of a Collector (the parameters to IPDR_CreateSession()).
 * @ingroup IPDR_sessions
 */
typedef struct
{
    const char *    Name;     /**< The collector's (server's) name. */
    IPDR_IPAddress	Addr;     /**< The collector's (server's) IP address. */
    unsigned short  Port;     /**< The collector's (server's) "port". */
    int             Priority; /**< The collector's (server's) priority (lower number is higher priority). */
}
IPDR_Collector;


/** @defgroup IPDR_event_notification Event notification
 *
 *  <p><p>
 *  The IPDR transmitter (client) may trigger events in specific conditions. The
 *  conditions include a connection or disconnection to / from a
 *  collector (server), a change in the currently active collector (server) or data loss. The
 *  host application may register a callback function which will be
 *  called once an event is triggered within the transmitter (client). This can be
 *  used for example to link the IPDR transmitter (client) to the management
 *  facilities of the network element
 *
 *  @{
 */

/** Event types
 */
enum IPDR_Event_t
{
    /** Connection to a collector occurred; this may be
     *  followed by a #IPDR_EVENT_ACTIVE_SERVER_CHANGED event.
     */
    IPDR_EVENT_SERVER_CONNECTED         = 0x0001,

    /** Disconnection from a collector occurred - this can be
     *  either a real disconnection (TCP close) or a a timeout which
     *  may or may not cause an actual TCP-level disconnection. It is
     *  possible that this event can be triggered multiple times
     *  (usually with different reasons) for the same collector (server), without
     *  an intervening #IPDR_EVENT_SERVER_CONNECTED event.
     */
    IPDR_EVENT_SERVER_DISCONNECTED      = 0x0002,

    /** A record was lost because of no room on the queued; if output
     *  is being done with IPDR_SendData(), you will get both a
     *  #IPDR_ERR_QUEUE_FULL error and a record lost event.
     */
    IPDR_EVENT_RECORD_LOST              = 0x0003,

    /** The active collector (server) was changed to a higher priority one.
     */
    IPDR_EVENT_ACTIVE_SERVER_CHANGED    = 0x0004,

    /** The internal queue has become empty (used to detect when it it
     *  is safe to delete a transmitter (client) because all data have been sent and
     *  acknowledged).
     */
    IPDR_EVENT_QUEUE_EMPTY              = 0x0005,

    /** All the collectors (servers) have been disconnected, so any records that
     *  are sent are guaranteed to queue up (and possibly overflow the
     *  internal queue) until at least one collector (server) starts again (see
     *  #IPDR_EVENT_SERVER_CONNECTED). This event contains no extra
     *  information because an earlier
     *  #IPDR_EVENT_SERVER_DISCONENCTED will have given the reason
     *  for the collector (server) disconnection(s). It is possible that this
     *  event can be triggered multiple times before a
     *  #IPDR_EVENT_SERVER_CONNECTED is generated.
     */
    IPDR_EVENT_ALL_SERVERS_DISCONNECTED = 0x0006,

    /** An unrecoverable processing error has occurred.  Details are
     *  given by the #IPDR_EventError_t enumeration.  In general,
     *  this exception is used only for <em>asynchronous</em> events,
     *  so you <em>must</em> check return codes in addition to
     *  handling error events. (Some events can be synchronous,
     *  depending on the implementation.) Not all error events result
     *  in an event callback; some simply put a message in the log.
     */
    IPDR_EVENT_ERROR                    = 0x0007,

	/** Some Collector sent Flow Start Message */
	IPDR_EVENT_FLOWSTART				= 0x0008,

	/** Some Collector sent Final Template Data Ack Message */
	IPDR_EVENT_FINALTEMPLATEACK			= 0x0009,
};


/** Reason for a disconnection.
 */
enum IPDR_EventDisconnected_t
{
    IPDR_EVENT_DISCONNECTED_INITIAL  = 0, /**< Initial state. */
    IPDR_EVENT_DISCONNECTED_TIMEOUT  = 1, /**< No ACK within timeout. */
    IPDR_EVENT_DISCONNECTED_TCP      = 2, /**< TCP level disconnection (I/O error or remote disconnection). */
    IPDR_EVENT_DISCONNECTED_STOP     = 3, /**< Received a "STOP" from collector (server). */
    IPDR_EVENT_DISCONNECTED_TOO_SLOW = 4  /**< Collector is ACKing too slowly. */
};


/** Type of error event.
 */
enum IPDR_EventError_t
{
    IPDR_EVENT_ERROR_UNKNOWN_SERVER     = 1, /**< An unknown collector (server) has tried to connect. */
    IPDR_EVENT_ERROR_UNKNOWN_SESSION    = 2, /**< An unknown session for a collector (server). */
    IPDR_EVENT_ERROR_UNEXPECTED_CONNECT = 3  /**< Got 2nd connect on an already established connection. */
};

/** Extra information for a #IPDR_EVENT_SERVER_CONNECTED event
 *  (see @ref IPDR_EventData).
 */
struct IPDR_EventConnectedData
{
	IPDR_CollectorHandle	CollectorHandle ;	/**< Handle of Collector connected **/
	IPDR_BOOL				Incoming ;			/**< True if the connection is incoming, False if outgoing **/
};


/** Extra information for a #IPDR_EVENT_SERVER_DISCONNECTED event
 *  (see @ref IPDR_EventData).
 */
struct IPDR_EventDisconnectedData
{
    enum IPDR_EventDisconnected_t Reason; /**< Additional information about the timeout. */
    unsigned long TimeoutMillisec; /**< The timeout (in milliseconds) that can cause a timeout
                                    * disconnection (this value is provided even if the
                                    * disconnection is not because of a timeout). */
};


/** The type for data sequence numbers. Note that an older form of the
 *  protocol used signed numbers and reset the counter back to zero,
 *  reserving -1 for a kind of keep-alive. This has been changed to use
 *  a 64-bit unsigned number, with any keep-alive being done
 *  differently.
 */
typedef unsigned long long IPDR_DSN_TYPE;


/** Extra information for a #IPDR_EVENT_RECORD_LOST event (see @ref IPDR_EventData).
 */
struct IPDR_RecordLostData
{
    IPDR_DSN_TYPE DSN; /**< Data Sequence Number of the record that was lost. */
};


/** Extra information for a #IPDR_EVENT_ACTIVE_SERVER_CHANGED event
 *  (see @ref IPDR_EventData).
 */
struct IPDR_ServerChangedData
{
    const struct IPDR_tag_Connection *PreviousCollector; /**< The collector (server) that was active before; NULL if none was active. */
};


/** Extra information for a #IPDR_EVENT_ERROR event
 *  (see @ref IPDR_EventData).
 */
struct IPDR_ErrorData
{
    enum IPDR_EventError_t Reason; /**< Specific error code. */
};

/** Extra information for a #IPDR_EVENT_FLOWSTART event */
struct IPDR_EventFlowStartData
{
	IPDR_SessionHandle		SessionHandle ;		/**< Handle of Session **/
	IPDR_CollectorHandle	CollectorHandle ;	/**< Handle of Collector **/
} ;

/** Extra information for a #IPDR_EVENT_FINALTEMPLATEACK event */
struct IPDR_EventFinalTemplateDataAck
{
	IPDR_SessionHandle		SessionHandle ;		/**< Handle of Session **/
	IPDR_CollectorHandle	CollectorHandle ;	/**< Handle of Collector **/
} ;


/** The event information, containing what's in the message (see
 *  ::IPDR_EventCallback) but in binary form.
 */
typedef struct
{
    enum IPDR_Event_t EventID; /**< What happened. This value determines
                                 *   how to interpret the Extra field. */
    const struct IPDR_tag_Connection *IC; /**< The active collector (server): NULL if none is active. */
    unsigned long OutstandingDataCount; /**< Number of records on the queue that haven't yet been sent. */
    unsigned long UnacknowledgedDataCount; /**< Number of records on the queue that haven't yet been acknowledged. */
    unsigned char SessionID; /**< Session ID as given to IPDR_CreateSession(). */
    const char*   SessionName; /**< Session description as given to IPDR_CreateSession(). */
    const char*   SessionDescription; /**< Session description as given to IPDR_CreateSession(). */
    union {
		struct IPDR_EventConnectedData    Connected; /**< Extra information for a #IPDR_EVENT_SERVER_CONNECTED event. */
        struct IPDR_EventDisconnectedData Disconnected; /**< Extra information for a #IPDR_EVENT_SERVER_DISCONNECTED event. */
        struct IPDR_RecordLostData        RecordLost; /**< Extra information for a #IPDR_EVENT_RECORD_LOST event. */
        struct IPDR_ServerChangedData     ServerChanged; /**< Extra information for a #IPDR_EVENT_ACTIVE_SERVER_CHANGED event. */
        struct IPDR_ErrorData             Error; /**< Extra information for a #IPDR_EVENT_ERROR event. */
		struct IPDR_EventFlowStartData    FlowStart; /**< Information about flow start event */
    } Extra; /**< Extra data, depending on the \a EventID. */
} IPDR_EventData;


/** Explain an event in English, using the information in the
 *  event data (there will already be a string with the message;
 *  this just uses the \a EventID and sub-ID (if any) to give
 *  a brief string).
 *  @param Event The event, as sent to ::IPDR_EventCallbackDetailed.
 *  @return A C-string corresponding to the \a EventID and sub-ID
 *    (if any).
 */
const char* IPDR_API_LINKAGE
IPDR_EventMsg(const IPDR_EventData* Event);


/** Callback for an event.
 *  @param Event The event.
 *  @param Msg Message associated with the event.
 *             The content of the message is subject to change.
 *             If you wish to get any additional information
 *             about the event, use the Event parameter; the
 *             message is simply a convenience for outputting
 *             som text about the error.
 *  @param Arg Context value given when callback was registered by
 *             calling IPDR_SetExporterEventHandlerDetailed().
 */
typedef void (*IPDR_EventCallbackDetailed)(const IPDR_EventData* Event,
                                            const char *Msg,
                                            void *Arg);


/** Set a callback function which will be used by the
 *  transmitter (client) to notify on various events.
 *
 *  Usually the callback will be triggered from within an API
 *  function; but in some cases, it can occur asynchronously. The
 *  callback is multi-thread safe with all the other API functions;
 *  however, the caller may need to provide an additional mutex in the
 *  callback for protection from other threads outside the IPDR API.
 *
 *  Applications should not depend on the order of connect,
 *  disconnect, or changed events. In particular, it is possible for a
 *  single collector (server) to have multiple connect or disconnect events in a
 *  row because there can be multiple causes for a connection or
 *  disconnection (e.g., a TCP layer reconnection, a "START" message,
 *  another collector (server) disconnecting).
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Callback callback function.
 *  @param Arg argument passed to the @a Callback function.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetExporterEventHandlerDetailed(IPDR_ExporterHandle ExporterHandle,
                                    IPDR_EventCallbackDetailed Callback,
                                    void *Arg);

/*@}*/


/** Callback for looking up a parameter value.
 *  @ingroup IPDR_initialization_starting_deletion
 *  The function receives a parameter name and
 *                the additional argument (\a ParamsHandlerArg) and returns the
 *                parameter value as a string (below is a list of available
 *                parameters).  If the function allocates memory for its return
 *                value, it must free the memory itself (e.g., on the next call to
 *                the parameters handler, or after it calls IPDR_DeleteExporter().  If
 *                the callback returns NULL, the default value is used.  If this
 *                function pointer is NULL, the default values will be used.
 *  @param Name The parameter name.
 *  @param Arg  Context value given when callback was registered by calling IPDR_InitExporter().
 *  @return C-string with the value; or NULL if the default is to be used.
 *  The names correspond to names in the MIB (see @ref IPDR_MIB).
 *
 *  @see IPDR_MIBGet
 *  @see IPDR_MIB
 *  @see IPDR_MIBGetByName
 *  @see IPDR_MIB_related
 *  @see IPDR_transmitter_MIB
 *  @see IPDR_MIB
 *
 * This function is currently used only by IPDR_InitExporter() to set
 * initial MIB values.  However, for future compatibility, it is
 * recommended that the \a Arg parameter contains a pointer to the
 * @ref IPDR_ExporterHandle that is being set by
 * IPDR_InitExporter(). The handle will be NULL if the MIB has not yet
 * been set up (that is, if IPDR_MIBGetByName() cannot be called).
 *
 * The following parameters are retrieved by the transmitter (client) using the
 * parameters handler callback. If the parameters handler callback is
 * not given, or if the callback returns NULL, the default values are
 * used.
 *
 * See @ref IPDR_configuration_values for details.
 *
 */
typedef const char* (*IPDR_ParametersHandlerFunc)(const char *Name,
                                                   void *Arg);


/** Callback for loading the MIB.
 *  @ingroup IPDR_initialization_starting_deletion
 *  The function should limit its activity to calls to
 *  IPDR_MIBRestoreNodeByName().
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param MIBVersion The MIB version (for matching with the saved MIB).
 *  @param Arg  Context value given when callback was registered by calling IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *    If this function fails, IPDR_InitExporter() will fail with the
 *    same return value.
 */
typedef IPDR_Status (*IPDR_LoadMIBFunc)(IPDR_ExporterHandle Exporter,
                                          int MIBVersion,
                                          void *Arg);


/** Callback for saving the MIB.
 *  @ingroup IPDR_initialization_starting_deletion
 *  The function will typically walk the MIB and write it to a file,
 *  so that it can be loaded by ::IPDR_LoadMIBFunc.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param MIBVersion The MIB version (for matching with the load).
 *  @param Arg  Context value given when callback was registered by calling IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SaveMIBFunc)(IPDR_ExporterHandle Exporter,
                                          int MIBVersion,
                                          void *Arg);


/** @defgroup IPDR_logging Output to log
 *
 *  <p><p>
 *  The IPDR transmitter (client) library supports a logging mechanism used for
 *  outputting debug, error or generic information messages. Each log
 *  message can have a Debug Level ranging from fatal errors
 *  (#IPDR_LEVEL_FATAL) to debug messages (#IPDR_LEVEL_DEBUG2). The
 *  host application may control the maximum message level which will
 *  be displayed in the log by each internal module of the IPDR
 *  transmitter (client). In addition the API provides capabilities which allow the
 *  host application to send messages to the IPDR log.
 *
 *  By default the IPDR transmitter (client) outputs its log messages by sending
 *  UDP datagrams (in syslog [RFC 3164] format) or to the process's
 *  standard error output (<code>stderr</code>) if
 *  #STD_FILE_OPERATIONS_SUPPORT is defined.
 *  IPDR_SetLogOutputCallback() or
 *  IPDR_SetLogOutputCallbackDetailed() can be used to override this
 *  default behavior and trigger a callback defined by the host
 *  application for each log message the transmitter (client) wishes to output.
 *
 *  That is, logging behavior is:
 *  <ul>
 *  <li>If log output address and port are specified, send
 *      a UDP packet in syslog format (see IPDR_SetLogOutputAddress()
 *      and the <code>set_logger_addr</code> command to
 *      IPDR_DoConsoleCommand()).
 *  <li>If a callback exists, call it.
 *  <li>If neither a syslog packet were sent nor a callback
 *      was called, output to standard output (<code>stderr</code>).
 *  </ul>
 *
 *  @{
 */


/** Levels for log messages; used both as a level for outputting
 *  (e.g., IPDR_LogWrite()) and for filtering (IPDR_SetModuleDebugLevel()).
 */
typedef enum {
  IPDR_LEVEL_MUTE    = 0, /**< Never used for a message but used to filter out all messages. */
  IPDR_LEVEL_FATAL   = 1, /**< Fatal error, typically the last thing seen before total system failure. */
  IPDR_LEVEL_ERROR   = 2, /**< Error that can't be recovered from but where processing can continue. */
  IPDR_LEVEL_WARNING = 3, /**< Looks suspicious but processing can recover. */
  IPDR_LEVEL_INFO    = 4, /**< Moderately chatty about what's going on. */
  IPDR_LEVEL_DEBUG   = 5, /**< Of interest only to implementors and debuggers. */
  IPDR_LEVEL_DEBUG2  = 6  /**< More than you'll ever want to know about what's going on. */
} IPDR_DebugLevel;


/** Set the debug level for a transmitter (client), but don't decrease it below what
 *  it already is. Other than that, it's the same as IPDR_SetModuleDebugLevel().
 * @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 * @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Exporter</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 * @param  L debug level (messages up to this level will be
 *       output from the module).
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetModuleMoreDebugLevel(IPDR_ExporterHandle ExporterHandle,
                              const char *ModuleName,
                              IPDR_DebugLevel L);


/** Check whether a specific module within the transmitter (client) outputs log
 *   messages at the given level.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Exporter</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 *  @param Lvl the level to compare with.
 *  @return 1 if the module is outputing log messages at the
 *   requested level, 0 otherwise.
 */
IPDR_API
IPDR_CheckLogLevel(IPDR_ExporterHandle ExporterHandle,
                    const char *ModuleName,
                    IPDR_DebugLevel Lvl);


/** Output a log message to the IPDR client log.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Exporter</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 *  @param Level message debug level.
 *  @param Func string describing the function which originated the message.
 *  @param Msg message (can be a printf style format string).
 *  @param ... additional arguments in case of a formatted msg.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
/*PRINTFLIKE5*/
IPDR_API
IPDR_LogWrite(IPDR_ExporterHandle ExporterHandle,
               const char *ModuleName,
               IPDR_DebugLevel Level,
               const char *Func,
               const char *Msg,
               ...);

/** Callback for outputting to a log (detailed).
 *
 *  This overrides the default behavior of writing to standard output
 *  (output in syslog format (IPDR_SetLogOutputAddress()) is in addition
 *  to any callback).
 *
 *  @param Level      the level of the message
 *  @param LevelStr   a string representing the message.
 *  @param ExporterName the value given in IPDR_InitExporter.
 *  @param Module     the module ("<code>Exporter</code>", "<code>Platform</code>",
 *                    etc. as used by IPDR_SetModuleDebugLevel()
 *                    or IPDR_LogWrite()).
 *  @param Func       a string representing where in the code the
 *                    message was generated.
 *  @param Msg        the actual log message.
 *  @param Arg The context that was passed to IPDR_SetLogOutputCallbackDetailed().
 *  @see IPDR_SetLogOutputCallback
 */
typedef void (*IPDR_LogCallbackDetailed)(IPDR_DebugLevel Level,
                                          const char *LevelStr,
                                          const char *ExporterName,
                                          const char *Module,
                                          const char *Func,
                                          const char *Msg,
                                          void *Arg);


/** Set a callback function which will be used to output
 *  log messages (instead of sending them over a UDP socket
 *  to a syslog server or to stdout).
 *
 *  IPDR_SetLogOutputCallback() and IPDR_SetLogOutputCallbackDetailed()
 *  can be called as many times as desired, in any order.
 *  The latest call to either of these overrides all other calls.
 *  The callback can be turned off by setting the callback function
 *  to NULL.
 *
 *  Output via callback is <em>in addition to</em> output in syslog
 *  format (see IPDR_SetLogOutputAddress()). If neither
 *  syslog output nor a callback are provided, log output is to
 *  standard error output (<code>stderr</code>) if
 *  #STD_FILE_OPERATIONS_SUPPORT is defined.
 *
 *  IPDR_SetLogOutputCallback() and
 *  IPDR_SetLogOutputCallabackDetailed() are cancel each other. If
 *  you first call IPDR_SetLogOutputCallbackDetailed() and then call
 *  IPDR_SetLogOutputCallback(), the function specified by
 *  IPDR_SetLogOutputCallbackDetailed() will be disabled. Similarly,
 *  if you call IPDR_SetLogOutputCallback() and then call
 *  IPDR_SetLogOutputputCallbackDetailed(), the function specified by
 *  IPDR_SetLogOutputCallback() will be disabled.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param CB Callback.
 *  @param Arg additional argument attached to the callback.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetLogOutputCallbackDetailed(IPDR_ExporterHandle ExporterHandle,
                                   IPDR_LogCallbackDetailed CB,
                                   void *Arg);


/** Set the log output address and port.
 *  As mentioned in @ref IPDR_logging, the log can be output
 *  by UDP in syslog format. To turn this on, you must
 *  set the log output address and port. Output in syslog format
 *  is <em>in addition to</em> output via callback. If neither
 *  syslog output nor a callback are provided, log output is to
 *  standard error output (<code>stderr</code>) if
 *  #STD_FILE_OPERATIONS_SUPPORT is defined.
 *
 *  You can turn off sending syslog datagrams by setting both
 *  \a Addr and \a Port to zero.
 *
 *  The RFC 3164 message will look something like this (assuming that
 *  facility=19 (local3) and the severity is "error":
 *  <xmp>
 *   &lt;155&gt;&nbsp;Jul&nbsp;&nbsp;1&nbsp;17:23:47&nbsp;192.168.8.88&nbsp;Exporter_TriggerEvent:&nbsp;Connected(START): Server=10.0.0.213, Port=15001, outstanding=0, unACKed=100, Session=101 [session1] (NE/one)Connected: 10.0.0.40
 *  </xmp>
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Addr The IP address to send syslog UDP packets to.
 *  @param Port The IP port to send syslog UDP packets to (port 514 is
 *              the suggested port in RFC 3164).
 *  @param Facility The facility, as defined in RFC 3164. It should be one
 *         of the following (values 2 through 15 should not be used):
 *     <ul>
 *        <li>0: kernel messages
 *        <li>1: user-level messages
 *        <li>16: local use 0  (local0)
 *        <li>17: local use 1  (local1)
 *        <li>18: local use 2  (local2)
 *        <li>19: local use 3  (local3)
 *        <li>20: local use 4  (local4)
 *        <li>21: local use 5  (local5)
 *        <li>22: local use 6  (local6)
 *        <li>23: local use 7  (local7)
 *     </ul>
 *  @param MachineName The machine's name for the message, which
 *      must conform to RFC 1035 (an internal copy is made of this string).
 *      If this is  NULL or zero-length, an attempt will be made to generate
 *      the name (typically using the host name or IP address). If it cannot
 *      be determined, the value "<code>unknown-IPDR</code>"
 *      will be used.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
// TODO
IPDR_API
IPDR_SetLogOutputAddress(IPDR_ExporterHandle ExporterHandle,
                          IPDR_IPAddress Addr,
                          unsigned short Port,
                          unsigned int Facility,
                          const char* MachineName);

/*@}*/


/** Status / Error codes.
 *  @see IPDR_Status
 *  @ingroup IPDR_general_typedefs
 */
enum IPDR_STATUS_RETURN_CODE
{
    IPDR_OK                         = 0u, /**< Everything worked fine (guaranteed to be zero value) */
	IPDR_ERR_MEMORY_FREE			= 1u, /**< It Is Some logic error trying to free already freed memory*/
	IPDR_ERR_SESSION_LOGICALLY_RUN  = 2u, /**<Session was already stoped or not started yet by API*/
	IPDR_ERR_SESSION_LOGICALLY_STOPED=3u, /**<Session was already stoped or not started yet by API*/
    IPDR_ERR_OPERATION_WOULD_BLOCK  = 4u, /**< Non-blocking I/O and operation would block; or operation could not be completed within specified timeout. */
	IPDR_ERR_COLLECTORSESSION_REASSOC=5u, /**< Collecotr Session Reasociation Actually changing priority*/

    IPDR_ERR                        = 128u, /**< A generic error, not otherwise distinguished. */
    IPDR_ERR_INVALID_VALUE          = 129u, /**< A parameter or other value was invalid. */
    IPDR_ERR_ALLOCATION_FAILED      = 130u, /**< Out of memory (possibly caused by a parameterized limitation). */
    IPDR_ERR_NO_AVAILABLE_TIMERS    = 131u, /**< Could not allocate another timer. */
    IPDR_ERR_TIMER_NOT_FOUND        = 132u, /**< Timer ID was invalid: timer does not exist. */
    IPDR_ERR_TIMER_ALREADY_STARTED  = 133u, /**< Attempted an illegal operation on an already started timer. */
    IPDR_ERR_TIMER_NOT_STARTED      = 134u, /**< Attempted an illegal operation on a timer that isn't started. */
    IPDR_ERR_PERSISTENCE_ERROR      = 135u, /**< Could not open or read from non-volatile memory. */
    IPDR_ERR_COMMUNICATION_ERROR    = 136u, /**< I/O error on network, open or close error, or end-of-file on read. */
    IPDR_ERR_ADDRESS_ALREADY_IN_USE = 137u, /**< Could not open specified address/port because already in use, restricted, or invalid. */
    IPDR_ERR_NOT_IMPLEMENTED		= 138u, /**<Function is not implemented. */
    IPDR_ERR_SESSION_ALREADY_EXISTS = 139u, /**< Tried to create a session with an ID that has already been used and not released by IPDR_DeleteSession(). */
    IPDR_ERR_UNABLE_TO_FIND_SESSION = 140u, /**< Tried to operate on a session that has not been created or is not valid for a particular transmitter (client). */
    IPDR_ERR_PORT_ALREADY_IN_USE    = 141u, /**< Could not open specified port because already in use, restricted, or invalid. */
    IPDR_ERR_COLLECTOR_ALREADY_EXISTS= 142u, /**< Tried to add a collector (server) that had already been added. */
    IPDR_ERR_COLLECTOR_NOT_FOUND     = 143u, /**< Tried to operate on a collector (server) that has not been added. */
    IPDR_ERR_TEMPLATE_ALREADY_EXISTS = 144u, /**< Tried to create a template with an ID that has already been used. */
    IPDR_ERR_TEMPLATE_NOT_FOUND      = 145u, /**< Tried to operate on a template that has not been created. */
    IPDR_ERR_KEY_NOT_FOUND           = 146u, /**< Tried to operate on a tamplte key that has not been created. */
    IPDR_ERR_KEY_ALREADY_EXISTS      = 147u, /**< Tried to create a key whose ID, Name, or Label already exists. */
    IPDR_ERR_QUEUE_FULL              = 148u, /**< Could not allocate any more memory to the queue (possibly because of a parameterized limitation. */
    IPDR_ERR_NOT_RUNNING             = 149u, /**< Tried to do an operation that requires
                                             * having IPDR_StartExporter() first (see also
                                             * IPDR_IsRunning(). Can also be produced if
                                             * certain internal errors occurred which
                                             * prevent further processing. If you get this
                                             * when you don't expect it, about the only
                                             * thing you can do is call IPDR_DeleteExporter().
                                             */
    IPDR_ERR_MIB_VERSION_MISMATCH    = 150u, /**< Configuration data (MIB data) was a different version than expected. */
    IPDR_ERR_MIB_NODE_NOT_FOUND      = 151u, /**< MIB NODE not found in lookup. */
    IPDR_ERR_MIB_NODE_NOT_WRITABLE   = 152u, /**< MIB NODE not writable (attempt to "set" failed). */
    IPDR_ERR_NO_ACTIVE_COLLECTOR     = 153u, /**< No active collector (server), or collector (server) is disconnected. */
    IPDR_ERR_ALREADY_RUNNING         = 154u, /**< Attempt to start transmitter (client) when it's already running,
                                             *   or doing an operation that must be done before
                                             *   IPDR_StartExporter(). */
    IPDR_CMD_EXIT                    = 155u, /**< Returned from IPDR_DoConsoleCommand() to indicate an "exit" command. */
    IPDR_CMD_NOT_FOUND               = 156u, /**< Returned from IPDR_DoConsoleCommand() to indicate a command wasn't found. */
    IPDR_CMD_HELP                    = 157u, /**< Returned from IPDR_DoConsoleCommand() to indicate a "help" command. */

    IPDR_ERR_TEMPLATE_DISABLED       = 158u, /**< Tried to send data on a disabled template. */
    IPDR_ERR_ACTIVE_COLLECTOR_EXISTS = 159u, /**< Tried to start a session while an active collector already exists for that session. */
    IPDR_ERR_API_CALL_FROM_CALLBACK  = 160u,

	IPDR_ERR_COLLECTOR_NOT_IN_SESSION= 161u,/**<Collector not associated with any session */
	IPDR_ERR_SESSION_IS_RUNNING		 = 162u, /**<Command may executed when the Session is stoped */
	IPDR_ERR_SESSION_IS_NOT_RUNNING  = 163u, /**<Command may executed when the Session is running */
};

#define SUCCESS(ec) ((ec) < 128u)
#define FAILURE(ec) ((ec) > 127u)


/** Convert a return code into an English string.
 *  @ingroup IPDR_general_typedefs
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *                This may be NULL, which might result in a slightly less
 *                informative message; it also might result in using
 *                a static area for creating the message.
 *  @param Status the result of a call a IPDR API call.
 *  @return A string with the English string, if the \a Status
 *  parameter is one of the Status / Error codes given by
 *  #IPDR_STATUS_RETURN_CODE. If the the status is #IPDR_OK, the
 *  return value is the empty string (""). If the status is not a
 *  valid code, a dynamically-created string is created. This string
 *  is valid only until another call of IPDR_StatusMsg() and is not
 *  thread-safe.
*/
const char* IPDR_API_LINKAGE
IPDR_StatusMsg(IPDR_ExporterHandle ExporterHandle,
                IPDR_Status Status);


/** @defgroup IPDR_platform Platform abstraction
 *
 *  <p><p>
 *  In order to ease the process of porting the IPDR transmitter (client) library to
 *  various platforms, the library relies on functions provided by the
 *  host application, which provide platform specific services.
 *
 *  The platform services are passed by the host application as a set
 *  of structures during the transmitter (client) initialization process
 *  (IPDR_InitExporter).
 *
 *  The platform-specific services are:
 *  <ol>
 *   <li>@ref IPDR_start_stop_services
 *   <li>@ref IPDR_msg_services
 *   <li>@ref IPDR_event_services
 *   <li>@ref IPDR_memory_services
 *   <li>@ref IPDR_lock_services
 *   <li>@ref IPDR_non_volatile_memory
 *   <li>@ref IPDR_network_services
 *     <ul>
 *     <li>@ref IPDR_TCP_services
 *     <li>@ref IPDR_UDP_services
 *     </ul>
 *   <li>@ref IPDR_time_services
 *  </ol>
 *
 *  Some of these can be called by the host application (see @ref IPDR_platform_calls).
 *
 *  @{
 */


/** @defgroup IPDR_start_stop_services Start/stop/delete services
 *
 *  <p><p>
 *  The start/stop/delete services are used to create and destroy
 *  the platform abstraction. The error message routines are here also
 *
 *  @{
 */


/** The Start function is called by the library from within the
 *  IPDR_StartExporter function. After successfully calling this
 *  function, the library proceeds to use event services. The
 *  implementation of the Start function may for example enter a loop
 *  containing a 'select' statement, which monitors a set of file
 *  descriptors and implements timers using the timeout parameter of
 *  'select'. If system has an external select loop, the Start function
 *  should be empty.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Start_t)(IPDR_PlatformArg P);


/** Stop any tasks / threads used for providing the timer / socket
 *  event services. After calling Stop, the transmitter (client) assumes that these
 *  services are not active but the abstraction layer can still be
 *  used.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Stop_t)(IPDR_PlatformArg P);


/** Test whether the transmitter (client) is running.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return 0 for not running, 1 for running.
 */
typedef IPDR_BOOL (*IPDR_IsRunning_t)(IPDR_PlatformArg P);


/** Free any resources internally allocated
 *  by the platform abstraction layer and stop execution of any code
 *  started (if any) by the Start operation.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Delete_t)(IPDR_PlatformArg P);


/** Set the transmitter (client) handle in the platform abstraction (so that logging can be done from the platform abstraction).
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param CH The transmitter (client) handle as returned by IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SetExporterHandle_t)(IPDR_PlatformArg P,
                                                IPDR_ExporterHandle CH);


/** Called by IPDR_DeleteExporter() to unset the transmitter (client) handle in the platform abstraction.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_UnsetExporterHandle_t)(IPDR_PlatformArg P);

/*@}*/

/** @defgroup IPDR_msg_services Message services
 *
 *  <p><p> The message services are used to interpret error codes from
 *  the underlying operating system.
 *
 * @{
 */


/** The longest error message that GetLastErrorMessage can return.
 */
#define IPDR_MAX_ERROR_LENGTH 511


/** Retrieve the error message corresponding to the last error that
 *  occurred in the current thread/task during a platform abstraction
 *  layer call (for example, in case of Unix, this should correspond to
 *  the current <code>errno</code>).
 *  If there is no error (e.g., on Unix errno == 0), then this
 *  should set the string to the #IPDR_NO_ERROR_ERROR_MESSAGE
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Msg (output) A buffer of at least #IPDR_MAX_ERROR_LENGTH bytes, which
 *             receives a null-terminated C-string of the error message.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_GetLastErrorMessage_t)(IPDR_PlatformArg P,
                                                    char *Msg);


/** Value returned by #IPDR_GetLastErrorMessage_t if there was no error. */
#define IPDR_NO_ERROR_ERROR_MESSAGE "No error"

/*@}*/


/** @defgroup IPDR_event_services Timer and asynchronous I/O services
 *
 *  <p><p>
 *  The event service functions are used by the library to implement
 *  timer and asynchronous I/O mechanisms.
 *
 *  @{
 */


/** The callback when a timer expires.
 *  @param Arg The context argument provided with the
 *             callback was registered with ::IPDR_RegisterTimer_t
 *             or ::IPDR_RegisterPeriodicTimer_t.
 *  @see IPDR_RegisterTimer_t
 *  @see IPDR_ReregisterTimer_t
 *  @see IPDR_RegisterPeriodicTimer_t
 */
typedef void (*IPDR_TimerCallback_t)(void *Arg);


/** Register and start a new timer.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for reregistering or
 *                  deleting the timer.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_ReregisterTimer_t
 *  @see IPDR_UnregisterTimer_t
 */
typedef IPDR_Status (*IPDR_RegisterTimer_t)(IPDR_PlatformArg P,
                                              IPDR_TimerCallback_t Callback,
                                              void *Arg,
                                              int Timeout,
                                              IPDR_TimerID *ID);


/** Change the timeout for an existing timer.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (updated) Reference to the timer from ::IPDR_RegisterTimer_t.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_ReregisterTimer_t)(IPDR_PlatformArg P,
                                                IPDR_TimerCallback_t Callback,
                                                void *Arg,
                                                int Timeout,
                                                IPDR_TimerID* ID);


/** Register and start a periodic timer.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for deleting the timer.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_UnregisterTimer_t
 */
typedef IPDR_Status (*IPDR_RegisterPeriodicTimer_t)(IPDR_PlatformArg P,
                                                      IPDR_TimerCallback_t Callback,
                                                      void *Arg,
                                                      int Timeout,
                                                      IPDR_TimerID *ID);


/** Cancel and delete an existing timer.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param ID The timer reference from ::IPDR_RegisterTImer_t
 *            or ::IPDR_RegisterPeriodictTimer_t.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_RegisterTimer_t
 *  @see IPDR_RegisterPeriodictTimer_t
 */
typedef IPDR_Status (*IPDR_UnregisterTimer_t)(IPDR_PlatformArg P,
                                                IPDR_TimerID ID);


/** The callback when a socket is ready to do I/O or
 *  an error has occurred on the socket.
 *  @param S The socket
 *  @param Arg The context argument provided with the
 *             callback was registered with ::IPDR_RegisterReadSocket_t
 *             or ::IPDR_Register_Write_Socket_t.
 *  @see IPDR_RegisterReadSocket_t
 *  @see IPDR_Register_Write_Socket_t
 */
typedef void (*IPDR_SocketCallback_t)(IPDR_SocketHandle S,
                                       void *Arg);


/** Register a socket for reading. A callback is triggered when data
 *  arrives or a communication problem occurs on the socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_RegisterReadSocket_t)(IPDR_PlatformArg P,
                                                   IPDR_SocketHandle S,
                                                   IPDR_SocketCallback_t Callback,
                                                   void *Arg,
                                                   IPDR_SocketEventID *EventID);


/** Register a socket for writing. A callback is triggered when data
 *  arrives or a communication problem occurs on the socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_RegisterWriteSocket_t)(IPDR_PlatformArg P,
                                                    IPDR_SocketHandle S,
                                                    IPDR_SocketCallback_t Callback,
                                                    void *Arg,
                                                    IPDR_SocketEventID *EventID);


/** Unregister a socket callback.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_RegisterReadSocket_t
 *  @see IPDR_RegisterWRiteSocket_t
 */
typedef IPDR_Status (*IPDR_UnregisterSocket_t)(IPDR_PlatformArg P,
                                                 IPDR_SocketEventID EventID);


/** Pointers to functions implementing the @ref IPDR_event_services.
 */
typedef struct
{
    IPDR_RegisterTimer_t RegisterTimer;
    IPDR_ReregisterTimer_t ReregisterTimer;
    IPDR_RegisterPeriodicTimer_t RegisterPeriodicTimer;
    IPDR_UnregisterTimer_t UnregisterTimer;
    IPDR_RegisterReadSocket_t RegisterReadSocket;
    IPDR_RegisterWriteSocket_t RegisterWriteSocket;
    IPDR_UnregisterSocket_t UnregisterSocket;
}
IPDR_EventServices;

/*@}*/


/** @defgroup IPDR_memory_services Memory services
 *
 *  <p><p>
 *  The memory service functions are used by the transmitter (client) to allocate
 *  and de-allocate memory during runtime. Some of the set-up
 *  parameters (such as <code>configuration.queueMaxMemory</code>) can be
 *  used to impose limits on parts of the IPDR memory allocation; you
 *  may wish to impose additional limits on the amount of memory that
 *  the IPDR transmitter (client) can allocate.
 *
 *  The memory used by IPDR transmitter (client) is allocated and de-allocated by
 *  functions supplied by the host application during
 *  initialization. To improve performance, the transmitter (client) uses internal
 *  memory pools that reduce the number of memory
 *  allocation/de-allocation requests. The IPDR transmitter (client) memory pools
 *  are based on pre-allocated, relatively large, memory chunks (more
 *  chunks can be allocated at runtime on a per need basis) out of
 *  which smaller buffers are efficiently allocated.
 *
 *  The following memory pools are used by the transmitter (client):
 *  <dl>
 *  <dt>queue<dd>a per session memory pool used for storing records.
 *  <dt>data_rcv<dd>a pool used for storing received messages (per collector (server) connection)
 *  <dt>data_snd<dd>a pool used for storing sent control messages (per collector (server) connection)
 *  <dt>send_buffer<dd>a small (< 100 bytes in most cases) memory
 *      region used for storing references to memory regions which are
 *     going to be sent over the transport layer (per collector (server) connection)
 *  </dl>
 *  Memory pools are allocated with an initial chunk size,
 *  pre-allocated number of chunks and have an optional size limit.
 *  Each has its own configuration parameter (see the
 *  various <i>xxx</i>MaxMem parameters described with ::IPDR_ParametersHandlerFunc).
 *
 *  @{
 */


/** Function called by the transmitter (client) in order to allocate a block of
 *  memory. The function must allocate at least the amount of memory
 *  requested by the function. The memory must be aligned on the most
 *  restrictive boundary (as for C's <code>malloc()</code>).
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#IPDR_ERR_ALLOCATION_FAILED).
 *  @return #IPDR_OK on success;
 *                 IPDR_ERR_ALLOCATION_FAILED or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_MemoryAllocate_t)(IPDR_PlatformArg P,
                                               size_t Size,
                                               void **Buf);


/** Allocate memory, given a chunk size and a header size.
 *  This is used to allocate "objects" in a safe way.
 *  Same as ::IPDR_MemoryAllocate_t(P, HdrSize+Size, Buf).
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param HdrSize Size of a header that goes in front of the data;
 *                  must be sizeof ObjectHeader in MemoryManagement.h
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#IPDR_ERR_ALLOCATION_FAILED).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_MemoryAllocate_t
 */
typedef IPDR_Status (*IPDR_MemoryAllocateWithHdr_t)(IPDR_PlatformArg P,
                                                      size_t HdrSize,
                                                      size_t Size,
                                                      void **Buf);


/** Free memory allocated by ::IPDR_MemoryAllocate.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Buf The pointer to the memory allocated by ::IPDR_MemoryAllocate.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_MemoryFree_t)(IPDR_PlatformArg P,
                                           void *Buf);


/** Free memory allocated by ::IPDR_MemoryAllocateWithHdr.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_MemoryFree_t
 */
typedef IPDR_Status (*IPDR_MemoryFreeWithHdr_t)(IPDR_PlatformArg P,
                                                  size_t HdrSize,
                                                  void *Buf);


/** Get statistics on memory allocation.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param numAllocs The number of calls to the IPDR_MemoryAllocate_t or IPDR_MemoryAllocateWithHdr_t
 *                   services.
 *  @param numFrees The number of calls to the IPDR_MemoryFree_t or IPDR_MemoryFreeWithHdr_t
 *                   services.
 *  @param bytesAllocated (output) The total number of bytes currently allocated.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_MemoryStatistics_t)(IPDR_PlatformArg P,
                                                 unsigned int *numAllocs,
                                                 unsigned int *numFrees,
                                                 size_t *bytesAllocated);


/** Pointers to functions implementing the @ref IPDR_memory_services.
 */
typedef struct
{
    IPDR_MemoryAllocate_t        MemoryAllocate;
    IPDR_MemoryAllocateWithHdr_t MemoryAllocateWithHdr;
    IPDR_MemoryFree_t            MemoryFree;
    IPDR_MemoryFreeWithHdr_t     MemoryFreeWithHdr;
    IPDR_MemoryStatistics_t      MemoryStatistics;
}
IPDR_MemoryServices;

/*@}*/


/** @defgroup IPDR_lock_services Lock services
 *
 *  <p><p>
 *  The locking service functions are used by the transmitter (client) to lock
 *  critical sections within the code. If the host application is
 *  single threaded, NULL pointers should be passed as the function
 *  pointers. The IPDR transmitter (client) library assumes that a single lockable
 *  entity (such as a single mutex) is locked by the abstraction
 *  layer.
 *
 *  There is single "platform" lock which is kept with the platform
 *  services; additional locks may be used where needed.
 *
 *  @{
 */


/** Create a lock.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param LockPtr (output) The lock (allocated and initialized as necessary).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_CreateLock_t)(IPDR_PlatformArg P, void **LockPtr);


/** Delete a lock.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param LockPtr The lock (as allocated by ::IPDR_CreateLock_t).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_DeleteLock_t)(IPDR_PlatformArg P, void *LockPtr);


/** Obtain a lock. This function must be reentrant and allow the lock
 *  to be obtained recursively (that is, if the same thread tries to
 *  obtain the lock twice, it will not deadlock but will wait until
 *  the other lock is freed). These semantics are required if socket
 *  handling is done by "call backs" (e.g., using the Unix select() or
 *  poll() to wait until a socket has data, then doing a read).
 *  @param P The context argument when calling platform function, as
 *  passed to IPDR_InitExporter().
 *  @param LockPtr The lock (as allocated by ::IPDR_CreateLock_t).
 *  @return #IPDR_OK on success;
 *  #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Lock_t)(IPDR_PlatformArg P, void *LockPtr);


/** Unlock a previously locked lock. Unlocking is recursive and symmetric
 *  with ::IPDR_Lock_t -- that is, if there are <i>n</i> calls to lock,
 *  then there must be <i>n</i> calls to unlock before the lock is released.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param LockPtr The lock (as allocated by ::IPDR_CreateLock_t).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_Lock_t
 */
typedef IPDR_Status (*IPDR_Unlock_t)(IPDR_PlatformArg P, void *LockPtr);


/** Lock the single platform lock. Usually this is implemented with a
 *  pre-allocated lock in the ::IPDR_LockPlatform_t structure. This
 *  lock must be recursive, as described with ::IPDR_Lockt_t.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param LockPtr The lock (as allocated by ::IPDR_CreateLock_t).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_LockPlatform_t)(IPDR_PlatformArg P);


/** Unlock the single platform lock.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_LockPlatform_t
 */
typedef IPDR_Status (*IPDR_UnlockPlatform_t)(IPDR_PlatformArg P);


/** Pointers to functions implementing the @ref IPDR_lock_services.
 */
typedef struct
{
    IPDR_CreateLock_t CreateLock;
    IPDR_DeleteLock_t DeleteLock;
    IPDR_Lock_t Lock;
    IPDR_Unlock_t Unlock;
    IPDR_LockPlatform_t LockPlatform;
    IPDR_UnlockPlatform_t UnlockPlatform;
}
IPDR_LockingServices;

/*@}*/


/** @defgroup IPDR_non_volatile_memory Non-volatile (persistent) memory services
 *
 *  <p><p>
 *  @note Non-volatile memory is not currently used. It was originally
 *  defined as a way of dumping the persistent parts of the
 *  MIB. See @ref IPDR_MIB_related for more details on how to save
 *  and restore the MIB data.
 *
 *  If you do not with to support this, you can provide functions
 *  which return #IPDR_ERR_PERSISTENCE_ERROR.
 *
 *  The host application may supply the library a set of functions for
 *  using non-volatile memory. The interface to this memory assumes
 *  the ability to use multiple, named units of information
 *  simultaneously.
 *
 *  The library uses this memory to store configuration information,
 *  such as the list of collectors (servers) and the current configuration of
 *  templates (enabled or disabled keys).  Data records are not stored
 *  in non-volatile memory, so it is accessed only occasionally.
 *
 *  The transmitter (client) should be able to work properly even when its
 *  configuration cannot be stored in persistent memory. (This
 *  requires manual configuration of collectors (servers), at least, after
 *  re-starting the transmitter (client).)
 *
 *  @see IPDR_MIB_related
 *
 *  @{
 */


/** Store data into non-volatile (persistent) memory.
 *  The function must allow a named unit of memory to be replaced by
 *  another unit with the same \a UnitName but with a different
 *  \a BufferLength. This can happen during re-configuration.
 *  @param UnitName The name of the unit of memory to store,
 *            typically a file name.
 *  @param Buffer Pointer to the data to be stored.
 *  @param BufferLength Number of bytes to store; if 0 then delete
 *          the data from non-volatile memory.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_PersistentStore_t)(IPDR_PlatformArg P,
                                                const char *UnitName,
                                                const char *Buffer,
                                                unsigned int BufferLength);


/** Retrieve data from non-volatile (persistent) memory.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param UnitName The name of the unit of memory to retrieve,
 *            typically a file name.
 *  @param Buffer (output) A pointer to a buffer
 *      that can be freed using the ::IPDR_MemoryFree_t.
 *      This functionstores the retrieved information in this buffer.
 *      The IPDR transmitter (client) will use ::IPDR_MemoryFree_t to free this.
 *  @param BufferLength (output) The number of bytes retrieved.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_PersistentRetrieve_t)(IPDR_PlatformArg P,
                                                   const char *UnitName,
                                                   char **Buffer,
                                                   unsigned int *BufferLength);


/** Pointers to functions implementing the @ref IPDR_non_volatile_memory.
 */
typedef struct
{
    IPDR_PersistentStore_t PersistentStore;
    IPDR_PersistentRetrieve_t PersistentRetrieve;
}
IPDR_PersistenceServices;

/*@}*/


/** @defgroup IPDR_network_services Network services (TCP/UDP sockets)
 *
 *  <p><p>
 *  The network service functions use the Berkeley socket model to do
 *  network I/O using TCP and UDP. For non-blocking I/O, the "event"
 *  model described in @ref IPDR_event_services
 *  (::IPDR_RegisterReadSocket_t, :IPDRRegisterWriteSocket_t) allows
 *  a callback style of programming, which will also work for the
 *  WinSock model.
 *
 *  All the network functions expect the address and port parameters
 *  to be in host order and not in network order.
 *
 *  @{
 */


/** Create a socket that can be used for TCP communication.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S (output) Identifier of the newly created socket.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_CreateTCPSocket_t)(IPDR_PlatformArg P,
                                                IPDR_SocketHandle *S);


/** Create a socket that can be used for UDP communication.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S (output) Identifier of the newly created socket.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_CreateUDPSocket_t)(IPDR_PlatformArg P,
                                                IPDR_SocketHandle *S);


/** Bind a socket to a local address.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t or ::IPDR_CreateUDPSocket_t).
 *  @param LocalAddr The local address in host byte order.  If this
 *      value is #IPDR_LOCAL_ADDRESS, use the address of the transmitter (client) machine
 *      (i.e., equivalent to 127.0.0.1)
 *  @param LocalPort the local port number in host byte order.
 *  @return #IPDR_OK on success; #IPDR_ERR_ADDRESS_ALREADY_IN_USE,
 *     #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Bind_t)(IPDR_PlatformArg P,
                                     IPDR_SocketHandle S,
                                     IPDR_IPAddress LocalAddr,
                                     unsigned short LocalPort);

typedef IPDR_Status (*IPDR_Connect_t)(IPDR_PlatformArg P,
                                     IPDR_SocketHandle S,
                                     IPDR_IPAddress LocalAddr,
                                     unsigned short LocalPort);

typedef IPDR_Status (*IPDR_GetSockName_t)(IPDR_PlatformArg P,
                                     IPDR_SocketHandle S,
                                     IPDR_IPAddress *LocalAddr,
                                     unsigned short *LocalPort);
/** Close a socket and deallocate any resources associated with
 *  the socket. If you need to leave the socket allocated, use
 *  ::IPDR_Shutdown_t.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t or ::IPDR_CreateUDPSocket_t).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Close_t)(IPDR_PlatformArg P, IPDR_SocketHandle S);


/** Shutdown a socket for both reading and writing, leaving the socket allocated.
 *  The socket is still allocated, but any operation other than
 *  ::IPDR_Close_t will get an error status.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t or ::IPDR_CreateUDPSocket_t).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Shutdown_t)(IPDR_PlatformArg P, IPDR_SocketHandle S);


/** @defgroup IPDR_TCP_services TCP-related services
 *
 *  <p><p>
 *  The TCP-related functions are used for connection-oriented data
 *  transmission. There is shared code here for both the transmitter (client) (which
 *  opens sockets as "passive" -- accepting connections) and for the
 *  collector (server) (which opens sockets as "active" -- creating connections).
 *
 *  @{
 */


/** Modify a socket to accept connections.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t.
 *  @param Backlog Maximum number of queued connections.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Listen_t)(IPDR_PlatformArg P,
                                       IPDR_SocketHandle S,
                                       int Backlog);


/** Create a socket for the first pending connection.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t.
 *          prepared by ::IPDR_Listen_T).
 *  @param Addr (output) The address of the new connection in host byte order.
 *  @param Port (output) The port of the new connection in host byte order.
 *  @param AcceptSocket (output) The newly created socket.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_Listen_t
 */
typedef IPDR_Status (*IPDR_Accept_t)(IPDR_PlatformArg P,
                                       IPDR_SocketHandle S,
                                       IPDR_IPAddress *Addr,
                                       unsigned short *Port,
                                       IPDR_SocketHandle *AcceptSocket);


/** Set any platform specific options of a TCP socket opened at the
 *  transmitter (client) side (for example it may change the send and receive buffer
 *  size, or change the socket to non blocking). The function receives
 *  a ::IPDR_ParametersHandlerFunc callback function (the same one
 *  passed to IPDR_InitExporter()) which can be used to retrieve socket
 *  parameters from an external configuration mechanism.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t).
 *  @param ParamFunc Function for retrieving parameter values.
 *  @param ParamArg Context argument as passed to IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SetExporterSocketParams_t)(IPDR_PlatformArg P,
                                                      IPDR_SocketHandle S,
                                                      IPDR_ParametersHandlerFunc ParamFunc,
                                                      void *ParamArg);


/** Send the contents of a buffer onto a TCP socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t).
 *  @param Buf Pointer to the bytes to write.
 *  @param BufLength The number of bytes to be written from the buffer.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Send_t)(IPDR_PlatformArg P,
                                     IPDR_SocketHandle S,
                                     const char *Buf,
                                     unsigned int BufLength,
                                     unsigned int *WrittenBytes);


/** Read into a buffer from a TCP socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateTCPSocket_t).
 *  @param Buf Pointer to the buffer where the bytes will be read into.
 *  @param BufLength Maximum number of bytes to read into the buffer.
 *  @param Timeout Maximum time in milliseconds to wait for data (0 means wait forever);
 *          the function returns #IPDR_ERR_OPERATION_WOULD_BLOCK if the timeout expires.
 *  @param ReadBytes (output) How many bytes were actually read.
 *         This will be greater than zero if the return code is
 *         #IPDR_OK; it will be zero for any other return code.
 *         End-of-input is treated as #IPDR_ERR_COMMUNICATION_ERROR.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR,
 *        #IPDR_ERR_OPERATION_WOULD_BLOCK, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_Recv_t)(IPDR_PlatformArg P,
                                     IPDR_SocketHandle S,
                                     char *Buf,
                                     unsigned int BufLength,
                                     int Timeout,
                                     unsigned int *ReadBytes);

/*@}*/


/** @defgroup IPDR_UDP_services UDP-related services
 *
 *  <p><p>
 *  The UPD-related functions are used for broadcast (for the syslog
 *  facility) and for the collector (server) to do the intial protocol
 *  negotiation.
 *
 *  @{
 */


/** Read into a buffer from a UDP socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateUDPSocket_t).
 *  @param Buf Pointer to the buffer where the bytes will be read into.
 *  @param BufLength Maximum number of bytes to read into the buffer.
 *  @param Timeout Maximum time in milliseconds to wait for data (0 means wait forever);
 *          the function returns #IPDR_ERR_OPERATION_WOULD_BLOCK if the timeout expires.
 *  @param ReadBytes (output) How many bytes were actually read.
 *         This will be greater than zero if the return code is
 *         #IPDR_OK; it will be zero for any other return code.
 *         End-of-input is treated as #IPDR_ERR_COMMUNICATION_ERROR.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_RecvFrom_t)(IPDR_PlatformArg P,
                                         IPDR_SocketHandle S,
                                         char *Buf,
                                         unsigned int BufferLength,
                                         IPDR_IPAddress *RemoteAddr,
                                         unsigned short *RemotePort,
                                         unsigned int *ReadBytes);


/** Send the contents of a buffer onto a TCP socket.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param S Socket (created with ::IPDR_CreateUDPSocket_t).
 *  @param Buf Pointer to the bytes to write.
 *  @param BufLength The number of bytes to be written from the buffer.
 *  @return #IPDR_OK on success; #IPDR_ERR_COMMUNICATION_ERROR, or #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SendTo_t)(IPDR_PlatformArg P,
                                       IPDR_SocketHandle S,
                                       const char *Buf,
                                       unsigned int BufferLength,
                                       IPDR_IPAddress RemoteAddr,
                                       unsigned short RemotePort);


/** Get the hostname, in the form needed for syslog. The result is always
 *  null-terminated; if insufficient space is provided, the result will be truncated.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param HostName The returned host name. On Unix systems, this should be
 *                  at least MAXHOSTNAMELEN long. If the call fails, this
 *                  will be set to the empty string ("").
 *  @param NameLength The size of the \a HostName array.
 *  @return #IPDR_OK on success; #IPDR_ERR otherwise (GetLastErrorMessage ought
 *        to have an indication of the problem).
 *  @see IPDR_SetLogOutputAddress
 */
typedef IPDR_Status (*IPDR_GetHostName_t)(IPDR_PlatformArg P,
                                            char* HostName,
                                            int NameLength);

/*@}*/


/** Pointers to functions implementing the @ref IPDR_network_services.
 */
typedef struct
{
    IPDR_CreateTCPSocket_t CreateTCPSocket;
    IPDR_CreateUDPSocket_t CreateUDPSocket;
    IPDR_Bind_t Bind;
	IPDR_Connect_t Connect;
	IPDR_GetSockName_t GetSockName;
    IPDR_Close_t Close;
    IPDR_Listen_t Listen;
    IPDR_Accept_t Accept;
    IPDR_SetExporterSocketParams_t SetExporterSocketParams;
    IPDR_Send_t Send;
    IPDR_Recv_t Recv;
    IPDR_RecvFrom_t RecvFrom;
    IPDR_SendTo_t SendTo;
    IPDR_Shutdown_t Shutdown;
    IPDR_GetHostName_t GetHostName;
}
IPDR_NetworkServices;

/*@}*/


/** @defgroup IPDR_time_services Time and date services
 *
 *  <p><p>
 *  The time and date services are used to get and set host
 *  time-of-day and to format date/time values for output.
 *
 *  @{
 */


/** The maximum length of the buffer in the time string output functions.
 */
#define IPDR_TIME_FORMAT_LENGTH 26 /* 1900-01-01T01:02:03.123456 (ISO format) */
                                    /* 01234567890123456789012345 */


/** Get the current time of day.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Time (output) seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_GetTime_t)(IPDR_PlatformArg Arg,
                                        IPDR_Time *Time);


/** Set the current time of day. It is allowed that this function
 *  fails, for example by not having sufficient permissions to set
 *  the time of day (the function is only used by some debug
 *  and testing functions).
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SetTime_t)(IPDR_PlatformArg Arg,
                                        IPDR_Time Time);


/** Sleep for a given number of milliseconds.
 *  If the system clock is not sufficiently precise, the time
 *  interval is rounded up to the precision of the clock.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Interval The number of milliseconds to sleep.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*IPDR_SleepMS_t)(IPDR_PlatformArg Arg,
                                        long Interval);


/** Format the current time into ISO format in the local time zone.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see IPDR_FormatTimeLocal_t
 */
typedef char* (*IPDR_FormatTimeLocalNow_t)(IPDR_PlatformArg Arg,
                                            char* Buf);


/** Format the current time into ISO format in the GMT (UTC) time zone.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see IPDR_FormatTimeGMT_t
 */
typedef char* (*IPDR_FormatTimeGMTNow_t)(IPDR_PlatformArg Arg,
                                          char* Buf);


/** Format a time into ISO format in the local time zone.
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef char* (*IPDR_FormatTimeLocal_t)(IPDR_PlatformArg Arg,
                                         IPDR_Time Time,
                                         char* Buf);


/** Format a time into ISO format in the GMT (UTC) time zone.
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef char* (*IPDR_FormatTimeGMT_t)(IPDR_PlatformArg Arg,
                                       IPDR_Time Time,
                                       char* Buf);


/** Format the current time into syslog format.
 *  This formats the time according to RFC 3164, e.g.:
 *  "<code>Jul&nbsp;&nbsp;1&nbsp;17:23:47</code>".
 *  @param P The context argument when calling platform function, as passed to IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef char* (*IPDR_FormatTimeSyslogNow_t)(IPDR_PlatformArg Arg,
                                             char* Buf);


/** Pointers to functions implementing the @ref IPDR_time_services.
 */
typedef struct
{
    IPDR_GetTime_t GetTime;
    IPDR_SetTime_t SetTime;
    IPDR_SleepMS_t SleepMS;
    IPDR_FormatTimeLocalNow_t FormatTimeLocalNow;
    IPDR_FormatTimeGMTNow_t FormatTimeGMTNow;
    IPDR_FormatTimeLocal_t FormatTimeLocal;
    IPDR_FormatTimeGMT_t FormatTimeGMT;
    IPDR_FormatTimeSyslogNow_t FormatTimeSyslogNow;
}
IPDR_TimeServices;

#if defined(SNMP_SUPPORT)
typedef IPDR_Status (*IPDR_MIB_Start_t)(void);
typedef void (*IPDR_MIB_SetExporterHandle_t)(IPDR_ExporterHandle EH);
typedef void (*IPDR_MIB_UnSetExporterHandle_t)(void);
typedef IPDR_BOOL (*IPDR_MIB_IsRunning_t)(void);
typedef void (*IPDR_MIB_Shop_t)(void);

typedef struct
{
	IPDR_MIB_Start_t MIB_Start;
	IPDR_MIB_SetExporterHandle_t MIB_SetExporterHandle;
	IPDR_MIB_UnSetExporterHandle_t MIB_UnSetExporterHandle;
	IPDR_MIB_IsRunning_t MIB_IsRunning;
	IPDR_MIB_Shop_t MIB_Shop;
}
IPDR_MIBServices;
#endif

/*@}*/


/** The platform services structure contains elements specifying the
 *  various platform specific services as well as Start, Stop and
 *  Delete functions called during transmitter (client) start / stop / delete
 *  operations respectively.
 */
typedef struct IPDR_tag_Platform
{
    char eyecatcher[8]; /**< Gets "<code>IPDRPS</code>" */
    IPDR_PlatformArg PlatformArg; /**< Argument which is passed to
                                    *   all platform functions. */
    IPDR_Start_t Start; /**< Called whenever a transmitter (client) is started
                          * using the IPDR_StartExporter() function. */
    IPDR_Stop_t Stop; /**< Called whenever a transmitter (client) is deleted using
                        * the IPDR_DeleteExporter() function in order
                        * to stop any tasks / threads started by the
                        * abstraction layer. */
    IPDR_IsRunning_t IsRunning; /**< Called to test whether
                                  * IPDR_StartExporter() has been done
                                  * successfully. */
    IPDR_Delete_t Delete; /**< Called whenever a transmitter (client) is deleted
                            * using the IPDR_DeleteExporter() function
                            * in order to stop any tasks / threads
                            * started by the abstraction layer. */
    IPDR_GetLastErrorMessage_t GetLastErrorMessage; /**< Called to retrieve the
                                                      * error message corresponding
                                                      * to the last error that
                                                      * occurred in the current
                                                      * thread / task in an platform
                                                      * abstraction layer call (for
                                                      * example, in case of Unix,
                                                      * this should correspond to
                                                      * the current
                                                      * <code>errno</code>). */
    IPDR_SetExporterHandle_t SetExporterHandle;         /**< Called at the end of
                                                      * IPDR_StartExporter() to set the
                                                      * transmitter (client) handle in the platform
                                                      * abstraction (so that logging
                                                      * can be done from the platform
                                                      * abstraction). */
    IPDR_UnsetExporterHandle_t UnsetExporterHandle; /**< Called by IPDR_DeleteExporter()
                                                  * to unset the transmitter (client) handle in
                                                  * the platform abstraction. */
    IPDR_MemoryServices      MemoryServices;
    IPDR_EventServices       EventServices;
    IPDR_PersistenceServices PersistenceServices;
    IPDR_NetworkServices     NetworkServices;
    IPDR_LockingServices     LockingServices;
    IPDR_TimeServices        TimeServices;
#if defined(SNMP_SUPPORT)
	IPDR_MIBServices		 MIBServices;
#endif
}
IPDR_PlatformServices;

/*@}*/


/** @defgroup IPDR_platform_calls Convenience functions for calling some platform services
 *
 *  These functions just call directly to the platform functions.
 *  The main difference is that these use a #IPDR_ExporterHandle" rather than
 *  ::IPDR_PlatformServices.
 *
 *  @{
 */


/** Allocate a chunk of memory.
 *  See ::IPDR_MemoryAllocate_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#IPDR_ERR_ALLOCATION_FAILED).
 *  @return #IPDR_OK on success;
 *                 IPDR_ERR_ALLOCATION_FAILED or #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_MemoryAllocate(IPDR_ExporterHandle ExporterHandle,
                     size_t Size,
                     void **Buf);


/** Free a chunk of memory.
 *  See ::IPDR_MemoryFree_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Buf The pointer to the memory allocated by ::IPDR_MemoryAllocate.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_MemoryFree(IPDR_ExporterHandle ExporterHandle,
                 void *Buf);


/** Get memory usage statistics.
 *  See ::IPDR_MemoryStatistics_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param numAllocs The number of calls to the IPDR_MemoryAllocate_t or IPDR_MemoryAllocateWithHdr_t
 *                   services.
 *  @param numFrees The number of calls to the IPDR_MemoryFree_t or IPDR_MemoryFreeWithHdr_t
 *                   services.
 *  @param bytesAllocated (output) The total number of bytes currently allocated.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_MemoryStatistics(IPDR_ExporterHandle ExporterHandle,
                       unsigned int *numAllocs,
                       unsigned int *numFrees,
                       size_t *bytesAllocated);


/** Register and start a new timer.
 *  See ::IPDR_RegisterTimer_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for reregistering or
 *                  deleting the timer.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_ReregisterTimer
 *  @see IPDR_UnregisterTimer
 */
IPDR_API
IPDR_RegisterTimer(IPDR_ExporterHandle ExporterHandle,
                    IPDR_TimerCallback_t Callback,
                    void *Arg,
                    int Timeout,
                    IPDR_TimerID *ID);


/** Change the timeout for an existing timer.
 *  See ::IPDR_ReregisterTimer_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (updated) Reference to the timer from ::IPDR_RegisterTimer_t.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_ReregisterTimer(IPDR_ExporterHandle ExporterHandle,
                      IPDR_TimerCallback_t Callback,
                      void *Arg,
                      int Timeout,
                      IPDR_TimerID* ID);


/** Register and start a periodic timer.
 *  See ::IPDR_RegisterPeriodicTimer_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for deleting the timer.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_UnregisterTimer_t
 */
IPDR_API
IPDR_RegisterPeriodicTimer(IPDR_ExporterHandle ExporterHandle,
                            IPDR_TimerCallback_t Callback,
                            void *Arg,
                            int Timeout,
                            IPDR_TimerID *ID);


/** Cancel and delete an existing timer.
 *  See ::IPDR_UnregisterTimer_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param ID The timer reference from ::IPDR_RegisterTImer_t
 *            or ::IPDR_RegisterPeriodictTimer_t.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_RegisterTimer_t
 *  @see IPDR_RegisterPeriodictTimer_t
 */
IPDR_API
IPDR_UnregisterTimer(IPDR_ExporterHandle ExporterHandle,
                      IPDR_TimerID ID);


/** Get the current time of day.
*   See ::IPDR_GetTime_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Time (output) seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetTime(IPDR_ExporterHandle ExporterHandle,
              IPDR_Time *Time);


/** Sleep for a given number of milliseconds.
 *  See ::IPDR_SleepMS_t
 *  If the system clock is not sufficiently precise, the time
 *  interval is rounded up to the precision of the clock.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Interval The number of milliseconds to sleep.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SleepMS(IPDR_ExporterHandle ExporterHandle,
              long Interval);


/** Format the current time into ISO format in the local time zone.
 *  See ::IPDR_FormatTimeLocalNow_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see IPDR_FormatTimeLocal_t
 */
IPDR_API_LINKAGE char *
IPDR_FormatTimeLocalNow(IPDR_ExporterHandle ExporterHandle,
                         char* Buf);


/** Format the current time into ISO format in the GMT (UTC) time zone.
 *  See ::IPDR_FormatTimeLocalGMTNow_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see IPDR_FormatTimeGMT_t
 */
IPDR_API_LINKAGE char *
IPDR_FormatTimeGMTNow(IPDR_ExporterHandle ExporterHandle,
                       char* Buf);


/** Format a time into ISO format in the local time zone.
 *  See ::IPDR_FormatTimeLocal_t
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
IPDR_API_LINKAGE char *
IPDR_FormatTimeLocal(IPDR_ExporterHandle ExporterHandle,
                      IPDR_Time Time,
                      char* Buf);


/** Format a time into ISO format in the GMT (UTC) time zone.
 *  See ::IPDR_FormatTimeGMT_t
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Buf The buffer to get the date/time C-string (must be #IPDR_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
IPDR_API_LINKAGE char *
IPDR_FormatTimeGMT(IPDR_ExporterHandle ExporterHandle,
                    IPDR_Time Time,
                    char* Buf);

/*@}*/


/** @defgroup IPDR_initialization_starting_deletion Initializing / starting / deleting a client
 *
 *  <p><p>
 *  All operations performed by the IPDR transmitter (client) library
 *  operate through a handle to a transmitter (client) entity (::IPDR_ExporterHandle).
 *  The following sections describe the API for creating, starting and
 *  deleting a transmitter (client).
 *
 *  The configuration parameters for the transmitter (client) are kept in a "MIB"
 *  (see @ref IPDR_MIB, @ref IPDR_MIB_related, @ref IPDR_transmitter_MIB).
 *
 *  The MIB is initialized to default values using
 *  ::IPDR_ParametersHandlerFunc. The MIB is then optionally loaded,
 *  possibly overriding these values (you can control this behavior by
 *  the way you write the load callback).
 *
 *  @{
 */


/** @defgroup IPDR_configuration_values Configuration values for inialization (programmatically/MIB)
 *
 *  <p><p>
 * The following values are used by @ref IPDR_ParametersHandlerFunc and are
 * also in the MIB (see @ref IPDR_MIB, @ref IPDR_transmitter_MIB).
 * Section @ref IPDR_reliability (especially @ref IPDR_reliability_transmitter_failure) has
 * more details on values that affect behavior under failure.
 *
 * <table border=1>
 * <tr valign=top><td><b>Parameter Name</b></td><td><b>OID</b></td><td><b>Description</b></td><td><b>Default Value</b></td></tr>
 * <tr valign=top><td><code>configuration.</code><code>exporterAddress</code></td>
 *    <td><code>6.1</code></td>
 *     <td>IP address of the transmitter (client) (as a string).</td>
 *     <td>Default interface address</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>exporterPort</code></td>
 *    <td><code>6.2</code></td>
 *     <td>transmitter (client) local port number: the collector (server) connects to this port.</td>
 *     <td>15000 (This may change in the future because
 *         the port is already assigned by IANA).</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>protocolInfoAddr</code></td>
 *    <td><code>6.16</code></td>
 *     <td>IP address used for the UDP-based protocol information which
 *         supplies the IPDR version number and data port to the collector (server).</td>
 *     <td><code>configuration.</code> <code>exporterAddress</code>
 *         (which defaults to default interface address)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>protocolInfoPort</code></td>
 *    <td><code>6.17</code></td>
 *     <td>UDP port number used for receiving protocol information requests. </td>
 *     <td>Value in <code>configuration.</code> <code>exporterPort</code>
 *        (which defaults to 15000). If zero, the "protocol version negotiation"
 *        port is not opened.</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>windowSize</code></td>
 *    <td><code>6.7</code></td>
 *     <td><em>Obsolete and unused</em>. (Was: Maximum number of outstanding
 *         records in the queue. When this value is reached, no more
 *         records are sent until at least one acknowledgment is received
 *         from the collector (server).)</td>
 *     <td>1000000 (records)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>windowTimeout</code></td>
 *    <td><code>6.8</code></td>
 *     <td>Timeout after which the transmitter (client) makes a transition to an
 *     alternate collector (server) when no new data acknowledgments have been
 *     received (keep-alives and repeat of the same DSN are ignored in this
 *     time-out, which is used only if new data have been sent).</td>
 *     <td>10000 (milliseconds)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tmplDataTimeout</code></td>
 *    <td><code>6.9</code></td>
 *     <td>Interval between retransmission of TMPL_DATA messages</td>
 *     <td>5000 (milliseconds)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>statisticsInterval</code></td>
 *    <td><code>6.10</code></td>
 *     <td>Interval between statistics reports to the log.
 *     If zero, no periodic statistics will be output.
 *     </td>
 *     <td>20000 (milliseconds)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>vendorString</code></td>
 *    <td><code>6.4</code></td>
 *     <td>Vendor specific string attached to the exporter.</td>
 *     <td>"<code>IPDR Exporter</code>"</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>flushInterval</code></td>
 *    <td><code>6.12.1.13</code></td>
 *
 *     <td><em>Obsolete and unused</em>. (Was:
 *        The interval of time after which data is sent. The data is
 *     sent if one of the following occurs (as of 2002-03-14, this
 *     value is ignored and all data are sent immediately): previously,
 *     data was sent if either a "chunk" is full (see <code>queueChunkSize</code>)
 *     or this interval has expired.)
 *     <td>500(milliseconds)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>queueChunksize</code></td>
 *    <td><code>6.13</code></td>
 *     <td>Initial memory chunk size for records queue.</td>
 *     <td>16384 (bytes)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>queueInitialNumChunks</code></td>
 *    <td><code>6.15</code></td>
 *     <td>Number of chunks initially allocated for the records queue. If less than 2, it is set to 2.</td>
 *     <td>2</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>queueMaxMemory</code></td>
 *    <td><code>6.14</code></td>
 *     <td>Maximum amount of memory allocated for the records queue (0 means <i>unlimited</i>)
 *         <em>for each session</em>. See
 *         @ref IPDR_reliability_transmitter_failure
 *         (especially @ref IPDR_reliability_internal_queue)
 *         for more information: basically, the
 *         value must be maximum-records-per-second � *         (average-record-length + 16) �worst-case-failover-time.</td>
 *     <td>0 (bytes) - <i>unlimited</i></td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataRcvChunkSize</code></td>
 *    <td><code>6.12.1.1</code></td>
 *     <td>Initial chunk size used for received data </td>
 *     <td>16384 (bytes)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataRcvInitialNumChunks</code></td>
 *    <td><code>6.12.1.3</code></td>
 *     <td>Number of chunks initially allocated for received data. If less than 2, it is set to 2.</td>
 *     <td>2</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataRcvMaxMemory</code></td>
 *    <td><code>6.12.1.2</code></td>
 *     <td>Maximum amount of memory allocated for received data (0 means <i>unlimited</i>). Recommended to not set a maximum value - the value will be fairly small and is determined by the number of records in the queue; but if you need a maximum, choose someting like 200000.</td>
 *     <td>0 (bytes) - <i>unlimited</i></td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataSndChunkSize</code></td>
 *    <td><code>6.12.1.7</code></td>
 *     <td>Initial memory chunk size for sent data.</td>
 *     <td>4096 (bytes)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataSndInitialNumChunks</code></td>
 *    <td><code>6.12.1.6</code></td>
 *     <td>Initial number of chunks allocated for sent data (this applies only to control messages). If less than 2, it is set to 2.</td>
 *     <td>2</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>dataSndMaxMemory</code></td>
 *    <td><code>6.12.1.5</code></td>
 *     <td>Maximum amount of memory allocated for sent data (0 means <i>unlimited</i>). Recommended to not set a maximum value - the value will be fairly small and is determined by the number of records in the queue; but if you need a maximum, choose someting like 200000.</td>
 *     <td>0 (bytes) - <i>unlimited</i></td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>sendBuffersChunkSize</code></td>
 *    <td><code>6.12.1.7</code></td>
 *     <td>Initial memory chunk size for storing references to buffers to be sent.</td>
 *     <td>4096 (bytes)</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>sendBuffersInitialNumChunks</code></td>
 *    <td><code>6.12.1.9</code></td>
 *     <td>Initial number of chunks allocated for buffer references. If less than 2, it is set to 2.</td>
 *     <td>2</td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>tcpTransport.</code><br><code>sendBuffersMaxMemory</code></td>
 *    <td><code>6.12.1.8</code></td>
 *     <td>Maximum amount of memory allocated for references to buffers to be sent (0 means <i>unlimited</i>). Recommended to not set a maximum value - the value will be fairly small and is determined by the number of records in the queue; but if you need a maximum, choose someting like 200000.</td>
 *     <td>0 (bytes) - <i>unlimited</i></td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>console.</code><br><code>consolePort</code></td>
 *    <td><code>6.5.1</code></td>
 *     <td>Port number of the IPDR transmitter (client) console (0 means disable console)</td>
 *     <td>40000</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>console.</code><br><code>consolePassword</code></td>
 *    <td><code>6.5.2</code></td>
 *     <td>Password used for logging to the IPDR console</td>
 *     <td>No password
 *     <br><em>Not in the MIB: can be set only from parameters</em></td></tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleExporter</code></td>
 *    <td><code> </code></td>
 *     <td>Values and related module debug levels:<br>
 *		0:=IPDR_LEVEL_MUTE; 1:=IPDR_LEVEL_FATAL; 2:=IPDR_LEVEL_ERROR; 
 *		3:=IPDR_LEVEL_WARNING; 4:=IPDR_LEVEL_INFO; 5:=IPDR_LEVEL_DEBUG; 
 *		6:=IPDR_LEVEL_DEBUG2</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>modulePlatform</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleTCP</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleHost</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleQueue</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleMemory</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleProtocol</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleJNI</code></td>
 *    <td><code> </code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * <tr valign=top><td><code>configuration.</code><code>logging.</code><code>levels.</code><br><code>moduleSERVER</code></td>
 *    <td><code></code></td>
 *     <td>Same as previous description.</td>
 *     <td>4</td>
 *     </tr>
 *
 * </table>
 */


/** Initialize a new transmitter (client) given the platform services and parameters.
 *
 *  The IPDR transmitter (client) initialization process involves passing a
 *  function pointer which will be used by the transmitter (client) to retrieve its
 *  parameters and a structure that contains function pointer to
 *  platform specific handlers. These implement socket, timer, and
 *  memory allocation operations. Initial parameters should be set
 *  before the transmitter (client) is activated or may be modified by setting
 *  different values to the MIB's relevant OIDs. Changing the
 *  parameters at run time may cause the connections to be restarted.
 *
 *  The implementation is free to use any method it wishes to store
 *  the parameter values. Examples are:
 *  <ul>
 *  <li>As a pre-defined list (the sample code uses this method).
 *  <li>As an external file which is scanned for values.
 *  <li>As internal values which are set via a separate command line
 *  interface or via SNMP (as part of a MIB).
 *  </ul>
 *
 *  The \a ParamsHandlerArg parameter to the parameters handler
 *  function contains the necessary context for the implementation to
 *  fetch the values.
 *
 *  @note
 *
 *  If any subsequent function returns #IPDR_ERR_NOT_RUNNING,
 *  you must call IPDR_DeleteExporter() to clean things up before
 *  calling this function again.
 *
 *  It is undefined when the implementation calls the \a ParamsHandler.
 *  It may choose to collect all parameter the values during
 *  initialization; if your \a ParamsHandler changes its values after
 *  initialization, you must stop and start the transmitter (client) to make them
 *  take effect.
 *
 *  If you wish to give precedence to the values in the MIB, you can
 *  get at them with IPDR_MIBGetByName(). You must create the
 *  \a ParamsHandlerArg to contain a pointer to the \a transmitter (client) handle
 *  and test to ensure that the handle is non-NULL before using it
 *  (IPDR_InitExporter() will sufficiently instantiate the transmitter (client) to
 *  allow MIB lookups.
 *
 *  @param PS handle to a platform services structure supplied by the
 *                host application, containing pointers to service functions. The
 *                contents of this structure will depend on platform specifics.
 *  @param ExporterName name of the transmitter (client) (used for logging
 *                purposes). A copy is made of the string. If this
 *                parameter is NULL or zero-length, a default name is supplied.
 *  @param ParamsHandler callback function for initial values of parameters
 *                needed by the transmitter (client). Any values set by this callback
 *                can be changed before IPDR_StartExporter() by using
 *                IPDR_MIBSet().
 *                See additional comments with
 *                @ref IPDR_ParametersHandlerFunc.
 *                If this parameter is NULL, default values are used.
 *  @param ParamsHandlerArg argument to the \a ParamsHandler callback function.
 *  @param LoadMIB callback function for initializing the values in the MIB.
 *                 The function should limit its activity to calls to
 *                 IPDR_MIBRestoreNodeByName().
 *                 If this function fails (returns anything other than #IPDR_OK),
 *                 IPDR_InitExporter() will fail with the
 *                 same return value.
 *                 If this parameter is NULL, nothing is done for loading the MIB.
 *  @param LoadMIBArg passed to \a LoadMIB.
 *  @param SaveMIB callback function for saving the values in the MIB.
 *                 If this parameter is NULL, nothing is done for saving the MIB.
 *  @param SaveMIBArg passed to \a SaveMIB.
 *  @param CH (output) handle to the newly created transmitter (client); NULL on failure.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_PersistenceServices
 *  @see IPDR_MIB_related
 *  @see IPDR_ParametersHandlerFunc
 *  @see IPDR_StartExporter
 *  @see IPDR_IsRunning
 */
IPDR_API
IPDR_InitExporter(IPDR_PlatformServices *PS,
                 const char *ExporterName,
                 IPDR_ParametersHandlerFunc ParamsHandler,
                 void *ParamsHandlerArg,
                 IPDR_LoadMIBFunc LoadMIB,
                 void *LoadMIBArg,
                 IPDR_SaveMIBFunc SaveMIB,
                 void *SaveMIBArg,
                 IPDR_ExporterHandle *CH);


/** Start a previously created transmitter (client).
 *  After the transmitter (client) has been initialized and sessions and templates
 *  have been created, the transmitter (client) can be started. For <em>some</em>
 *  implementations of the platform services, this will result in
 *  starting a new thread.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_IsRunning()
 *  @see IPDR_InitExporter()
 *  @see IPDR_DeleteExporter()
 */
IPDR_API
IPDR_StartExporter(IPDR_ExporterHandle ExporterHandle);


/** Test whether the transmitter (client) is running.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @return 0 for not running, 1 for running.
 *  @see IPDR_ERR_NOT_RUNNING
 */
int IPDR_API_LINKAGE
IPDR_IsRunning(IPDR_ExporterHandle ExporterHandle);


/** Statistics for a session / collector (server).
 *  These can be used for a session or a collector (server): for a session, they
 *  sum all the collectors (servers); for a collector (server), the give the values when the
 *  collector was active. For example, if 100 records wer enqueued when
 *  "collector-1" was active and 200 records were enqueued when
 *  "collector-2" was active (and both are part of "session-1") and 50
 *  records were enqueued when no collector (server) was active, then the session
 *  will show a total of 350.
 */
typedef struct
{
    unsigned long TransmittedRecords; /**< total number of transmitted
                                       * records (not necessarily acknowleged). */
    unsigned long AcknowledgedRecords; /**< total number of acknowledged records. */
    unsigned long EnqueuedRecords; /**< total number of records
                                    * enqueued by IPDR_SendData(). */
    unsigned long LostRecords; /**< total number of lost records
                                * (internal queue overflow). */
}
IPDR_Statistics;


/** Get statistics for a session.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Session handle returned from IPDR_CreateSession().
 *  @param Now (output) the current time (seconds since 1970-01-01T00:00:00Z).
 *            Use NULL if you don't want this value.
 *  @param Statistics (output) the statistics for the session.
 *  @param QueuedOutstandingRecords (output) the number of records
 *            currently on the output queue (unsent or unacknowledged).
 *            Use NULL if you don't want this value.
 *  @param QueuedUnacknowledgedRecords (output) the number of records
 *            that currently have not been acknowledged. Use NULL
 *            if you don't want this value.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetSessionStatistics(IPDR_ExporterHandle ExporterHandle,
                           IPDR_SessionHandle SessionHandle,
                           IPDR_Time *Now,
                           IPDR_Statistics *Statistics,
                           unsigned long *QueuedOutstandingRecords,
                           unsigned long *QueuedUnacknowledgedRecords);


/** Get statistics for a collector (server) within a session and get information
 *  about its state.
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Session handle returned from IPDR_CreateSession().
 *  @param CollectorHandle The collector handle
 *  @param Now (output) the current time (seconds since 1970-01-01T00:00:00Z).
 *            Use NULL if you don't want this value.
 *  @param Statistics (output) the statistics for the collector (server).
 *            Use NULL if you don't want this value.
 *  @param LastEvent (output) the last state-change "event" that was
 *            triggered;
 *            This can be used to get the state and other current
 *            information about the collector (server). [Non-state events,
 *            such as #IPDR_EVENT_RECORD_LOST, are ignored.]
 *            Use NULL if you don't want this value.
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetCollectorStatisticsAndState(IPDR_ExporterHandle ExporterHandle,
                                  IPDR_SessionHandle SessionHandle,
                                  IPDR_CollectorHandle CollectorHandle,
                                  IPDR_Time *Now,
                                  IPDR_Statistics *Statistics,
                                  IPDR_EventData *LastEvent);


/** Stop and delete a IPDR transmitter (client), and release all resources.
 *  Any connections to a collector (server) are broken.
 *
 *  If you get an unexpected response of #IPDR_ERR_NOT_RUNNING,
 *  you must call this function before calling IPDR_InitExporter().
 *
 *  This does <em>not</em> verify that all the data waiting in the
 *  output queue has been written or acknowledged.
 *  To ensure that all data has been acknowledged, use
 *  IPDR_DrainQueue() or use IPDR_GetSessionStatistics() and
 *  ::IPDR_EventCallbackDetailed with the #IPDR_EVENT_QUEUE_EMPTY
 *  event.
 *
 *  This function is <em>not</em> thread-safe; you must ensure that
 *  no other IPDR_<i>xxx</i> function is active or called before
 *  this function returns.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_InitExporter
 */
IPDR_API
IPDR_DeleteExporter(IPDR_ExporterHandle ExporterHandle);


IPDR_API
IPDR_ReconfigExporter(IPDR_ExporterHandle ExporterHandle);

/** Wait until all the records in the queue have been sent and acknowledged.
 *  Because there is no guarantee that the queue will ever drain (all the
 *  collectors (servers) could be disconnected and they won't reconnect), a time limit
 *  can also be specified. If the queue did not drain in time, this function
 *  returns #IPDR_ERR_OPERATION_WOULD_BLOCK -- it is always possible that
 *  the queue has drained asynchronously so you should also check the
 *  \a QueuedUnacknowledgedRecords value from IPDR_GetSessionStatistics().
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Session Handle as returned by IPDR_CreateSession().
 *  @param Timeout Wait at most this long (in milliseconds) for the queue to
 *                 drain; a value of zero means wait forever.
 *  @return #IPDR_OK on success; #IPDR_ERR_OPERATION_WOULD_BLOCK
 *            #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_DeleteExporter
 */
IPDR_API
IPDR_DrainQueue(IPDR_ExporterHandle ExporterHandle,
                 IPDR_SessionHandle SessionHandle,
                 IPDR_Time Timeout);

/*@}*/


/** Create a new session on the transmitter (client).
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Name session name (an internal copy is made of this string).
 *  @param ID Session ID.
 *  @param Description session description (an internal copy is made of this string).
 *  @param SH (output) handle to the newly created session.
 *  @return #IPDR_OK on success; #IPDR_ERR_SESSION_ALREADY_EXISTS,
 *          or
 *          #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_CreateSession(IPDR_ExporterHandle ExporterHandle,
                    const char *Name,
                    unsigned char ID,
                    const char *Description,
                    IPDR_SessionHandle *SH);


/** Delete a previously created session and release
 *  all resources associated with it.
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session as returned by IPDR_CreateSession().
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION,
 *   or #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_DeleteSession(IPDR_ExporterHandle ExporterHandle,
                    IPDR_SessionHandle SessionHandle);


/** Get the handle to a session given the session's ID.
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param ID Session ID to find.
 *  @param SH (output) Handle to the session as returned by IPDR_CreateSession().
 *  @param Name (output) Name of the session as provided to IPDR_CreateSession().
 *               This parameter can be NULL if you don't want the value.
 *  @param Description (output) Description of the session as provided to IPDR_CreateSession().
 *               This parameter can be NULL if you don't want the value.
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION or
 *           #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetSession(IPDR_ExporterHandle ExporterHandle,
                 unsigned char ID,
                 IPDR_SessionHandle *SH,
                 const char** Name,
                 const char** Description);


/** Get a list of all the session IDs.
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param NumSessions (output) number of sessions.
 *  @param Sessions (output) array of collector (server) IDs, which is allocated internally
 *           and remains valid until the transmitter (client) is deleted by a call to
 *           IPDR_DeleteExporter(); or until another call to IPDR_GetSessionsList().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetSessionsList(IPDR_ExporterHandle ExporterHandle,
                      int *NumSessions,
                      unsigned char **Sessions);

/** Add a collector to a collectors list, so that data can be sent to it.
 *  @ingroup IPDR_sessions
 *
 *  collectors are assigned a priority level regarding the specific session. Data is always sent to the
 *  active collector with the highest priority (if two collectors (servers) have the
 *  same priority, it is undefined which will be chosen to be active).
 *  Zero is highest priority. See also @ref IPDR_alternate_collectors.
 *
 *  collectors can be added dynamically. Data will be sent to a collector (server) as
 *  soon as a (transport layer) connection has been established and
 *  the collector (server) has indicated that it is ready to receive data by
 *  sending a START message and there is no higher priority collector (server)
 *  active.
 *
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Name Name of collector (an internal copy is made of this string).
 *  @param Addr IP address of the collector.
 *  @param Port Port number of the collector - used only in case than Exporter connecting
 *  to the collector
 *  @param NumberOfActive - number of connection Exporter need to create. If value of this parameter
 *  is 0, the Exporter will not attempt to connect to the Collector
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
IPDR_API
IPDR_AddCollector(IPDR_ExporterHandle ExporterHandle,
                const char *Name,
                IPDR_IPAddress Addr,
                unsigned short Port,
                unsigned short NumberOfActive);

/** Create a collector to a collectors list, so that data can be sent to it.
 *  @ingroup IPDR_sessions
 *
 *  collectors are assigned a priority level regarding the specific session. Data is always sent to the
 *  active collector with the highest priority (if two collectors (servers) have the
 *  same priority, it is undefined which will be chosen to be active).
 *  Zero is highest priority. See also @ref IPDR_alternate_collectors.
 *
 *  collectors can be added dynamically. Data will be sent to a collector (server) as
 *  soon as a (transport layer) connection has been established and
 *  the collector (server) has indicated that it is ready to receive data by
 *  sending a START message and there is no higher priority collector (server)
 *  active.
 *
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Name Name of collector (an internal copy is made of this string).
 *  @param Addr IP address of the collector.
 *  @param Port Port number of the collector - used only in case than Exporter connecting
 *  to the collector
 *  @param NumberOfActive - number of connection Exporter need to create. If value of this parameter
 *  is 0, the Exporter will not attempt to connect to the Collector
 *	@param CH pointer to CollectorHandle will receive new created collecotr
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
IPDR_API
IPDR_CreateCollector(IPDR_ExporterHandle ExporterHandle,
					const char *Name,
					IPDR_IPAddress Addr,
					unsigned short Port,
					unsigned short NumberOfActive,
					IPDR_CollectorHandle* CH);

/** Add a collector (server) to a session's collectors (servers) list, so that data can be sent to it.
 *  @ingroup CRANE_sessions
 *
 *  collectors (servers) are assigned a priority level. Data is always sent to the
 *  active collector (server) with the highest priority (if two collectors (servers) have the
 *  same priority, it is undefined which will be chosen to be active).
 *  Zero is highest priority.
 *
 *  collectors (servers) can be added dynamically. Data will be sent to a collector (server) as
 *  soon as a (transport layer) connection has been established and
 *  the collector (server) has indicated that it is ready to receive data by
 *  sending a START message and there is no higher priority collector (server)
 *  active.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to session to add collector (server) to.
 *  @param Name Name of collector (server) (an internal copy is made of this string).
 *  @param Addr IP address of the collector (server).
 *  @param Port Port number of the collector (server).
 *  @param Priority collector's (server's) priority (0 - highest priority; behavior
 *            undefined for negative values).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *
 * IPDR Compatibility:
 * This function implemented as calling
 *
*/
IPDR_API
IPDR_AddCollectorCompat(IPDR_ExporterHandle ExporterHandle,
						IPDR_SessionHandle SessionHandle,
						const char *Name,
						IPDR_IPAddress Addr,
						unsigned short Port,
						int Priority);

/** Get the handle to a collector given the collecor's name.
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a exporter previously created by IPDR_InitExporter().
 *  @param Name Collector Name to find.
 *  @param CH (output) Handle to the collector as returned by IPDR_AddCollector().
 *  @param Addr (output) IP Address of the Collector
 *  @param Port (output)_IP Port of the Collector
 *  @param NumberOfActive (output)_number of active connections as specified by IPDR_AddCollector()
 *  @return #IPDR_OK on success; #IPDR_ERR_COLLECTOR_NOT_FOUND or
 *           #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetCollector(IPDR_ExporterHandle ExporterHandle,
				const char* Name,
                IPDR_CollectorHandle *CH,
                IPDR_IPAddress *Addr,
		 		unsigned short* Port,
                unsigned short* NumberOfActive);


/** Associates a collector to session
 *  @ingroup IPDR_sessions
 *
 *  Collectors are assigned a priority level in specific session. Data is always sent to the
 *  active collector (server) with the highest priority (if two collectors (servers) have the
 *  same priority, it is undefined which will be chosen to be active).
 *  Zero is highest priority. See also @ref IPDR_alternate_collectors.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session to add collector to.
 *  @param CollectorHandle Handle to  collector
 *  @param Priority collector's (server's) priority (0 - highest priority; behavior
 *            undefined for negative values).
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/

IPDR_API
IPDR_AssocCollectorSession(
			IPDR_ExporterHandle ExporterHandle,
			IPDR_SessionHandle SessionHandle,
			IPDR_CollectorHandle CollectorHandle,
			int Priority
		);


/** Remove a previously added collector (server).
 *
 *  collectors (servers) can be deleted dynamically, causing lower priority collectors (servers)
 *  to be activated. Removal of a collector (server) causes immediate
 *  disconnection, potentially losing acknowledgments from the collector (server)
 *  and duplicate transmission to the alternate collector (server).
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session to remove collector (server) from.
 *  @param CollectorHandle Handle to the Collector
 *  @return #IPDR_OK on success; #IPDR_ERR_SESSION_ALREADY_EXISTS or
 *     #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_RemoveCollectorFromSession(IPDR_ExporterHandle ExporterHandle,
                   IPDR_SessionHandle SessionHandle,
                   IPDR_CollectorHandle CollectorHandle );


/** Remove a previously added collector (server).
 *
 *  collectors (servers) can be deleted dynamically, causing lower priority collectors (servers)
 *  to be activated. Removal of a collector (server) causes immediate
 *  disconnection, potentially losing acknowledgments from the collector (server)
 *  and duplicate transmission to the alternate collector (server).
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param CollectorHandle Handle to the Collector
 *  @return #IPDR_OK on success; #IPDR_ERR_SESSION_ALREADY_EXISTS or
 *     #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_RemoveCollector(IPDR_ExporterHandle ExporterHandle,
                   IPDR_CollectorHandle CollectorHandle);

/** Retrieve the current list of collectors
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param NumCollectors (output) number of collectors (servers).
 *  @param Collectors (output) array of collector handles, which is allocated internally
 *           and remains valid until the session is deleted by a call to
 *           IPDR_DeleteSession() or IPDR_DeleteExporter(); or until
 *           another call to IPDR_GetServersList() is done.
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION or
 *       #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetCollectorList(IPDR_ExporterHandle ExporterHandle,
					unsigned int *NumCollectors,
					IPDR_Collector **Collectors);


/** Retrieve the current list of collectors (servers) configured for a given session.
 *  @ingroup IPDR_sessions
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *  @param NumCollectors (output) number of collectors (servers).
 *  @param Collectors (output) array of collector (server) structures, which is allocated internally
 *           and remains valid until the session is deleted by a call to
 *           IPDR_DeleteSession() or IPDR_DeleteExporter(); or until
 *           another call to IPDR_GetServersList() is done.
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION or
 *       #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_GetCollectorInSessionList(IPDR_ExporterHandle ExporterHandle,
					IPDR_SessionHandle SessionHandle,
					unsigned int *NumCollectors,
					IPDR_Collector **Collectors);

/** Allows to start the current session in order to renegotiate templates
 *  or set another document ID. This function will return error if the session
 *  is not in appropriate state to be started
 *  @ingroup IPDR_sessions
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *  @param Negotiate If this parameter is true, the templates will be renegotiated
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION, #IPDR_STATUS_RETURN_CODE
 *  or #IPDR_ERR_SESSION_NOTREADY otherwise.
 */
IPDR_API
IPDR_StartSession( 	IPDR_SessionHandle SessionHandle,
					IPDR_BOOL Negotiate );

/** Allows to stop the current session in order to renegotiate templates
 *  or set another document ID. This function will return error if the session
 *  is not active. The function will attempt to drain the message queue, using specified
 *	timeout, in order to prevent messages with prior document id to be sent
 *  @ingroup IPDR_sessions
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *	@param Reason reason of session termination to be sent to the collector. Only allowed values
 *	@param Timeout timeout for using whendraining a queue
 * 	of this parameter are #IPDR_STOP_REASON_ENDOFDOCUMENT and #IPDR_STOP_REASON_TEMPLATEUPDATED
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION, #IPDR_STATUS_RETURN_CODE
 *  or #IPDR_ERR_SESSION_NOTACTIVE otherwise.
 */
IPDR_API
IPDR_StopSession( 	IPDR_SessionHandle SessionHandle,
					enum IPDR_SessionStopReason Reason,
                 	IPDR_Time Timeout );

/** Allows specify if the templates will be negotiated in this session at
 * 	closest opportunity.
 *  @ingroup IPDR_sessions
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *	@param Negotiable true is the templates must be negotiated, false otherwise
 *  @return #IPDR_OK on success
 */
IPDR_API
IPDR_SetSessionNegotiable( 	IPDR_SessionHandle SessionHandle,
							IPDR_BOOL Negotiable );


/** Allows to set the document id to be send along with next SESSION START message
 *  This function will succeed only in case that the appropriate Session is not Active
 * 	and Session data queue is empty.
 *  @ingroup IPDR_sessions
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *	@param DocumentID document id (UUID) to be used within the Session
 *  @return #IPDR_OK on success; #IPDR_ERR_UNABLE_TO_FIND_SESSION, #IPDR_ERR_QUEUE_NOT_EMPTY
 *  or #IPDR_ERR_SESSION_ACTIVE otherwise.
 */
IPDR_API
IPDR_SetDocumentID( IPDR_SessionHandle SessionHandle, const char* DocumentID );

/** @defgroup IPDR_templates Templates and keys
 *
 *  <p><p>
 *  A host application can format data for transmission
 *  with a set of templates. Templates contain keys that can be
 *  dynamically enabled or disabled. They are a shared resource
 *  available to all sessions; the host application can define
 *  multiple templates to meet various BSS/OSS needs, and use them
 *  concurrently.  At the start of a session, the IPDR collector and
 *  transmitter 'negotiate' the data to be sent. The aim is to arrive at a
 *  'best fit' between data the platform can make available and the
 *  requirements of the business/support application(s) being used in
 *  a network.
 *
 *  Thus, the IPDR collector indicates which of the available templates
 *  are required and which keys within them are to be enabled or
 *  disabled. This can be done strictly with the XML configuraiton,
 *  or by some interaction by querying from the collector to the
 *  transmitter and then using a GUI to select which fields are actually wanted.
 *  The transmitter, based on the configuration information received
 *  from the collector, sends the collector an indication of the data
 *  formats (templates) it will use and after the collector confirms,
 *  sends only 'raw' data. The collector interprets this compact data
 *  using the templates it has agreed with the transmitter.
 *
 *  The host application uses the IPDR_SendData() function to
 *  populate data records in accordance with the template
 *  structures. The exporter filters out disabled keys and sends the
 *  record in the context of the appropriate sessions. The exporter code
 *  is free to reorder the fields in the template to something
 *  different from the ordering within the host application's records.
 *
 *  @{
 */


/** Create a new template within the exporter.
 *
 *  This function is used to create a new template, with or without
 *  keys. Additional keys can be added to a template after its
 *  creation.
 *
 *  A template applies to specific session. The collector may declare that
 *  it is not interested in receiving a template by disabling all the
 *  keys, in which case any ouptut of the template via
 *  IPDR_SendData() will do nothing. (For efficiency, the host
 *  application might decide not to even call IPDR_SendData() -- it
 *  will know that all the keys have been disabled by the
 *  ::IPDR_TemplateChangedCallback being called with all keys having
 *  #IPDR_KEY_DISABLED set in the IPDR_Key.KeyAttributes field.
 *
 *  During the exporter initialization process, all key data structures
 *  passed as arguments to this function are defined and
 *  activated. Therefore, a platform's core templates should be
 *  created in this way.
 *
 *  Templates can also be created,KeyHelp and keys may be added to templates,
 *  dynamically (in run time). Templates and keys so created will
 *  become active only when the session is restarted. They
 *  will not persist after power-down and will not be re-created in
 *  the next initialization.
 *
 *  When all keys in a template are accessed using offsets and all
 *  have a fixed length, it is recommended to group all mandatory keys
 *  at the beginning for efficiency purposes.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session to contain this template
 *  @param TemplateID ID of template.
 *  @param SchemaName name of the template
 *  @param TypeName template type
 *  @param NumKeys Number of keys within the template (may be zero for an empty template).
 *  @param Keys Array of keys associated with the template (an internal
 *                copy is made of this array and all its contents).
 *  @param TH (output) Handle to the newly created template.
 *  @return #IPDR_OK on success; #IPDR_ERR_TEMPLATE_ALREADY_EXISTS or
 *             #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_ApplyTemplates
 *  @see IPDR_AddKey
 *  @see IPDR_DeleteKey
 */
IPDR_API
IPDR_CreateTemplate(IPDR_ExporterHandle ExporterHandle,
					IPDR_SessionHandle SessionHandle,
                    int TemplateID,
                    const char *SchemaName,
				    const char *TypeName,
                    int NumKeys,
                    const IPDR_Key *Keys,
                    IPDR_TemplateHandle *TH);


/** Create a new template within the exporter.
 *
 * Previous (CRANE compatible) version of IPDR_CreateTemplate function
 * performs corresponding operation for all sessions
 *
 *  This function is used to create a new template, with or without
 *  keys. Additional keys can be added to a template after its
 *  creation.
 *
 *  A template applies to specific session. The collector may declare that
 *  it is not interested in receiving a template by disabling all the
 *  keys, in which case any ouptut of the template via
 *  IPDR_SendData() will do nothing. (For efficiency, the host
 *  application might decide not to even call IPDR_SendData() -- it
 *  will know that all the keys have been disabled by the
 *  ::IPDR_TemplateChangedCallback being called with all keys having
 *  #IPDR_KEY_DISABLED set in the IPDR_Key.KeyAttributes field.
 *
 *  During the exporter initialization process, all key data structures
 *  passed as arguments to this function are defined and
 *  activated. Therefore, a platform's core templates should be
 *  created in this way.
 *
 *  Templates can also be created, and keys may be added to templates,
 *  dynamically (in run time). Templates and keys so created will
 *  become active only when the session is restarted. They
 *  will not persist after power-down and will not be re-created in
 *  the next initialization.
 *
 *  When all keys in a template are accessed using offsets and all
 *  have a fixed length, it is recommended to group all mandatory keys
 *  at the beginning for efficiency purposes.
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param TemplateID ID of template.
 *  @param SchemaName name of the template
 *  @param TypeName template type
 *  @param NumKeys Number of keys within the template (may be zero for an empty template).
 *  @param Keys Array of keys associated with the template (an internal
 *                copy is made of this array and all its contents).
 *  @return #IPDR_OK on success; #IPDR_ERR_TEMPLATE_ALREADY_EXISTS or
 *             #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_ApplyTemplates
 *  @see IPDR_AddKey
 *  @see IPDR_DeleteKey
 */
IPDR_API
IPDR_CreateTemplate_All(IPDR_ExporterHandle ExporterHandle,
					int TemplateID,
                    const char *SchemaName,
				    const char *TypeName,
                    int NumKeys,
                    const IPDR_Key *Keys);

/** Retrieve a template handle given a template ID.
 *  @param SessionHandle Handle to session containing this template
 *  @param TemplateID ID of template.
 *  @param SchemaName (output) name of the template
 *  @param TypeName (output) template type
 *  @param NumKeys (output) number of keys at the template
 *  @param TH (output) Handle to the found template.
 *  @return #IPDR_OK on success; #IPDR_ERR_TEMPLATE_NOT_FOUND or
 *               #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_CreateTemplate
 */
IPDR_API
IPDR_GetTemplate(IPDR_SessionHandle SessionHandle,
                	int TemplateID,
                    char **SchemaName,
				    char **TypeName,
                    int *NumKeys,
                    IPDR_TemplateHandle *TH);

/** Delete a previously created template.
 *  @param SessionHandle Handle to session containing this template
 *  @param TH Handle of template (created by IPDR_CreateTemplate()) to remove.
 *  @return #IPDR_OK on success; #IPDR_ERR_TEMPLATE_NOT_FOUND or
 *                         #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_DeleteTemplate(IPDR_SessionHandle SessionHandle,
                     IPDR_TemplateHandle TH);

/** Delete a previously created template in all sessions of the exporter
 *  @param ExporterHandle Handle to the exporter
 *  @param TemplateID ID of template.
 *  @return #IPDR_OK on success; #IPDR_ERR_TEMPLATE_NOT_FOUND or
 *                         #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_DeleteTemplate_All(IPDR_ExporterHandle ExporterHandle,
					int TemplateID);

/** Add a key to a previously created template.
 *
 *  If this function is called after the exporter has already been
 *  started, the key will be added only after IPDR_ApplyTemplates()
 *  is called.
 *  @param SessionHandle Handle to session containing this template
 *  @param TH Template handle previously created by IPDR_CreateTemplate().
 *  @param CKey Key to add (an internal copy is made of this structure and
 *              all its contents).
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_ALREADY_EXISTS,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 *  @see IPDR_ApplyTemplates
 */
IPDR_API
IPDR_AddKey(IPDR_SessionHandle SessionHandle,
            IPDR_TemplateHandle TH,
            const IPDR_Key *CKey);


/** Add a key to a previously created template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be added only after IPDR_ApplyTemplates()
 *  is called.
 *
 *  @param ExporterHandle Handle to the exporter
 *  @param TemplateID ID of the template, used in call to IPDR_CreateTemplate
 *  @param CKey Key to add (an internal copy is made of this structure and
 *              all its contents).
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_ALREADY_EXISTS,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 *  @see IPDR_ApplyTemplates
 */
IPDR_API
IPDR_AddKey_All(IPDR_ExporterHandle ExporterHandle,
			int TemplateID,
            const IPDR_Key *CKey);

/** Remove a key from a template.
 *
 *  If this function is called after the exporter has already been
 *  started, the key will be added only after IPDR_ApplyTemplates()
 *  is called.
 *  @param SessionHandle Handle to session containing this template
 *  @param TH Template handle previously created by IPDR_CreateTemplate().
 *  @param KeyID ID of key to remove from the template.
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_NOT_FOUND,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 */
IPDR_API
IPDR_RemoveKey(IPDR_SessionHandle SessionHandle,
                IPDR_TemplateHandle TH,
                unsigned int KeyID);

/** Remove a key from a template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be added only after IPDR_ApplyTemplates()
 *  is called.
 *  @param ExporterHandle Handle to the exporter
 *  @param TemplateID ID of the template used for IPDR_CreateTemplate().
 *  @param KeyID ID of key to remove from the template.
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_NOT_FOUND,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 */
IPDR_API
IPDR_RemoveKey_All(IPDR_ExporterHandle ExporterHandle,
				int TemplateID,
                unsigned int KeyID);

/** Enable or disable a key within a template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be acutally enabled or diabled only after
 *  IPDR_ApplyTemplates() is called.
 *  @param SessionHandle Handle to session containing this template
 *  @param TH Template handle previously created by IPDR_CreateTemplate().
 *  @param KeyID ID of key to remove from the template.
 *  @param Enabled - enables the key if true, disables otherwise
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_NOT_FOUND,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 */

IPDR_API
IPDR_EnableKey(IPDR_SessionHandle SessionHandle,
                IPDR_TemplateHandle TH,
                unsigned int KeyID,
				IPDR_BOOL Enabled);


/** Enable or disable a key within a template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be acutally enabled or diabled only after
 *  IPDR_ApplyTemplates() is called.
 *  @param SessionHandle Handle to session containing this template
 *  @param TH Template handle previously created by IPDR_CreateTemplate().
 *  @param KeyID ID of key to remove from the template.
 *  @param Enabled - enables the key if true, disables otherwise
 *  @return #IPDR_OK on success; #IPDR_ERR_KEY_NOT_FOUND,
 *       #IPDR_ERR_TEMPLATE_NOT_FOUND, or #IPDR_STATUS_RETURN_CODE
 *       otherwise.
 */

IPDR_API
IPDR_EnableKey_All(IPDR_SessionHandle SessionHandle,
					int TemplateID,
					unsigned int KeyID,
					IPDR_BOOL Enabled);


/** Apply a new template configuration during exporter runtime.
 *
 *  Changes to templates, including template creation and add/remove
 *  keys, take effect only after this function runs to successful
 *  completion and also IPDR_SessionStart function is called
 *	Internally, this function will send TEMPLATE DATA message
 *  @param SessionHandle Handle to session to apply the templates
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  This function can not be called from within active session and
 *	will result in error. Before calling this function, IPDR_SessionStop
 *	must be called.
 *
 *  @see IPDR_AddKey
 *  @see IPDR_RemoveKey
 */
IPDR_API
IPDR_ApplyTemplates(IPDR_SessionHandle SessionHandle);

/** Apply a new template configuration during exporter runtime to all existing sessions.
 *
 *  @param ExporterHandle Handle to a Exporter previously created by IPDR_InitExporter().
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *  @see IPDR_AddKey
 *  @see IPDR_RemoveKey
 */
IPDR_API
IPDR_ApplyTemplates_All(IPDR_ExporterHandle ExporterHandle);

/** Callback for when a template is changed by the collector.
 *   During the template negotiation, the collector can request that
 *  certain fields be not transmitted. The IPDR exporter software will
 *  agree to this and not transmit these fields; this callback is
 *  called to inform the host application that certain fields are not
 *  needed (possibly improving its performance). The unneeded fields
 *  can safely be left unitialized when a call to IPDR_SendData() is
 *  done.
 *
 *  The host application will have an option to change the modified fields
 * 	back if necessary, as demonstrated below
 *
 *  There is no record of before/after for fields; processing should be
 *  something like this:
 *  @verbatim
 *     for (i = 0; i < NumTemplates; i++) {
 *       const IPDR_Template *T = Templates[i];
 *       for (j = 0; j < T->NumKeys; j++) {
 *         const IPDR_Key *K = T->Keys[j];
 *         if (K->Enabled = true;
 *       }
 *     }
 *  @endverbatim
 *
 *  Or - for debugging purposes
 *  @verbatim
 *     for (i = 0; i < NumTemplates; i++) {
 *       const IPDR_Template *T = Templates[i];
 *       for (j = 0; j < T->NumKeys; j++) {
 *         const IPDR_Key *K = T->Keys[j];
 *		   if( j % 2 )	K->Enabled = true;
 *			else K->Enabled = false;
 *       }
 *     }
 *  @endverbatim
 *
 *  @param SessionHandle Handle to session where the template has been changed
 *  @param NumTemplates The number of templates that have been changed.
 *  @param Templates The templates that have changed.
 *  @param Arg The \a Arg parameter set by IPDR_SetTemplateChangedCallback().
 *  @see IPDR_SetTemplateChangedCallback
 */
typedef void (*IPDR_TemplateChangedCallback)(IPDR_SessionHandle SessionHandle,
											 IPDR_Template *T,
											 int NumTemplates,
                                             void *Arg);


/** Set a callback which will be called whenever the templates configuration changes.
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetTemplateChangedCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_TemplateChangedCallback Callback,
                                 void *Arg);

/**
 *  @param SessionID Session ID where we want to get new configuration ID
 *  @param NewConfigurationID poiner to new Configuration ID
 *  @param Arg The \a Arg parameter set by IPDR_SetNewConfigurationIDCallback().
 *  @see IPDR_SetNewConfigurationIDCallback
 */
typedef void (*IPDR_NewConfigurationIDCallback)(unsigned char SessionID,
												unsigned short* NewConfigurationID,
												void *Arg);

/** Set a callback which will be called the first time we set configuration ID.
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetNewConfigurationIDCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_NewConfigurationIDCallback Callback,
                                 void *Arg);

/**
 *  @param SessionID Session ID where we configuration ID has been changed
 *  @param NewConfigurationID poiner to new Configuration ID
 *  @param Arg The \a Arg parameter set by IPDR_SetConfigurationIDChangedCallback().
 *  @see IPDR_SetConfigurationIDChangedCallback
 */
typedef void (*IPDR_ConfigurationIDChangedCallback)(unsigned char SessionID,
													unsigned short NewConfigurationID,
													void *Arg);

/** Set a callback which will be called whenever the configuration ID changes.
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetConfigurationIDChangedCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_ConfigurationIDChangedCallback Callback,
                                 void *Arg);

/** Callback for when start negotiation message is received at the session
 *	This callback allows to the host application to decide if it willing
 *	to perform negotiation at this time
 *  @param SessionHandle Handle to session where the template has been changed
 *  @param CollectorHanlde Handle to Collector which sent the message
 *  @param Negotiate the output boolean value. If set to false, the negotiation
 *					will be rejected
 *  @param Arg The \a Arg parameter set by IPDR_SetStartNegotiationCallback().
 *  @see IPDR_SetStartNegotiationCallback
 */
typedef void (*IPDR_StartNegotiationCallback)(IPDR_SessionHandle SessionHandle,
												IPDR_CollectorHandle CollectorHandle,
												IPDR_BOOL *Negotiate,
												void *Arg);

/** Set a callback which will be called whenever the collector sends
 * 	START NEGOTIATION message.
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetStartNegotiationCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_StartNegotiationCallback Callback,
                                 void *Arg);

/** Callback for when flow start message is received at the session
 *	This callback allows to the host application to force choice of specific Collector
 *	The flag defaults to the value, specified at capabilities
 *  @param SessionHandle Handle to session where the template has been changed
 *  @param CollectorHanlde Handle to Collector which sent the message
 *  @param ChooseThisOne the output boolean value. Allows to force choosing this collector
 *			for active session
 *	at "TEMPLATE DATA" message
 *  @param Arg The \a Arg parameter set by IPDR_SetFlowStartCallback().
 *  @see IPDR_SetFlowStartCallback
 */

typedef void (*IPDR_FlowStartCallback)(IPDR_SessionHandle SessionHandle,
												IPDR_CollectorHandle CollectorHandle,
												IPDR_BOOL *ChooseThisOne,
												void *Arg);

/** Set a callback which will be called whenever the collector sends
 * 	FLOW START message.
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SetFlowStartCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_FlowStartCallback Callback,
                                 void *Arg);

/*@}*/


/** @defgroup IPDR_data_delivery Data delivery
 *
 *  <p><p>
 *  Data is passed to the IPDR transmitter (client) in host-native format,
 *  i.e. either little- or big-endian depending on the platform. The
 *  collector (server) is notified of the format by a flag in the TMPL DATA
 *  message and if byte order swapping is required, it should occur
 *  in the collector (server) side.
 *
 *  @{
 */


/** Send data using the IPDR protocol.
 *
 *  This function is called by the host application when the data to
 *  be delivered is ready to be transmitted. It copies data suitable to
 *  populate a specific template, to specific session which was configured
 *  to receive this template. (If all keys in the
 *  given template were disabled for a specific session, the record
 *  will not be sent in that session.) Once the data is enqueued it is
 *  sent over to the transport layer to the active collector the session
 *  with appropriate fail-over processing within sessions.
 *
 *  The #IPDR_EVENT_RECORD_LOST event will be triggered for the
 *  session for which there is no space in the output queue (see
 *  IPDR_SetExporterEventHandler()). The record-lost event can occur
 *  even if a #IPDR_OK return code is returned from this function. The
 *  record-lost event may also occur asynchornously from the
 *  IPDR_SendData() function.
 *
 *  As with all other API functions, it is safe for multiple threads to
 *  call this function simultaneously; mutexes will ensure
 *  multi-thread safety.  If the underlying platform supports
 *  non-blocking I/O, this function will not block on I/O: if the data
 *  cannot be sent immediately, it will be sent asynchronously later.

 can not be sent after swtop session


 *
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param SessionHandle Handle to session to send the data
 *  @param TemplateHandle Handle of template to which the data comform.
 *  @param Data Pointer to the actual data (which serves as the
 *          base address when using offsets or passed to
 *          accessor functions
 *  @return #IPDR_OK on success; #IPDR_ERR_ALLOCATION_FAILED
 *     (sometimes, when the queue is full), #IPDR_ERR_QUEUE_FULL, or
 *     #IPDR_STATUS_RETURN_CODE otherwise.
 *	This function will fail when StopSession was called and StopSession
 *	was not called yet. This is due to document ID, which may be unknown in this
 *	situation.Return code for this situation is TBD.
 */
IPDR_API
IPDR_SendData(IPDR_ExporterHandle ExporterHandle,
			  IPDR_SessionHandle SessionHandle,
               IPDR_TemplateHandle TemplateHandle,
               const void *Data);

/** This function will attempt to send the data to each session available at the exporter
 * 	This is mostly useful for compatibility purposes
 *  @param ExporterHandle Handle to an exporter previously created by IPDR_InitExporter().
 *  @param TemplateID ID of template to which the data comform.
 *  @param Data Pointer to the actual data (which serves as the
 *          base address when using offsets or passed to
 *          accessor functions
 *  @return #IPDR_OK on success; #IPDR_ERR_ALLOCATION_FAILED
 *     (sometimes, when the queue is full), #IPDR_ERR_QUEUE_FULL, or
 *     #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_SendData_All(IPDR_ExporterHandle ExporterHandle,
               int TemplateID,
               const void *Data);

/*@}*/


/** @defgroup IPDR_MIB_related SNMP agent support: MIB-related functions and types
 *
 *  <p><p>
 *  The various initialization parameters of the transmitter (client) are
 *  kept in "MIB" data. See @ref IPDR_initialization_starting_deletion
 *  for more on how these are used at start-up.
 *
 *  The MIB also contains information about the transmitter's state: sessions,
 *  collectors (servers), templates, and statistics.
 *
 *  Integration between the IPDR transmitter (client) and an SNMP Agent allows
 *  transmitting information about the transmitter and dynamic
 *  reconfiguration of it.
 *
 *  The IPDR transmitter (client) API exposes three functions used to integrate
 *  with the Agent - IPDR_MIBGet(), IPDR_MIBGetNext(), and
 *  IPDR_MIBSet(). These functions can be called by an external agent
 *  when the corresponding SNMP request is received.
 *
 *  The API also allows saving all the parameters to non-volatile memory
 *  and restoring them on restart. The data can be saved by "walking"
 *  the MIB (using IPDR_MIBGetNext()) and saving the nodes that
 *  are marked as persistent; the values can be restored by calling
 *  IPDR_MIBSet() [IPDR_MIBGetByName() can be useful for handling
 *  OIDs that are saved as strings]. Sample code is suplied for this.
 *
 *  @see IPDR_MIB
 *  @see IPDR_transmitter_MIB
 *  @see IPDR_ParametersHandlerFunc
 *
 *  @{
 */


/** @defgroup IPDR_transmitter_MIB IPDR transmitter (client) MIB overview
 *
 *  <p><p>
 *  The IPDR transmitter (client) library can be integrated with an external SNMP
 *  Agent, in order to support configuration and monitoring
 *  through an SNMP based management system.
 *  <p>
 *  See also @ref IPDR_MIB and @ref IPDR_ParametersHandlerFunc.
 *  <p>
 *  <dl>
 *  <dt>sessionTable<dd>a read only table containing information about each
 *  configured session and statistics concerning records transfer
 *  through this session (total number of records, average rate at
 *  which records were sent by the application and number of lost
 *  records).
 *  <p>
 *  <dt>serversTable<dd>a read only table containing information about
 *  each configured collector (server) along with its corresponding session id. In
 *  addition the table contains statistical information about the total
 *  number of transmitted records, number of acknowledged records, the
 *  average rate of transmission and the average rate of acknowledged
 *  records.
 *  <p>
 *  <dt>templatesTable<dd>a read only table containing information
 *  about each template (template id and description).
 *  <p>
 *  <dt>keysTable<dd>a read only table containing information about
 *  each configured key and Its associated template id.
 *  <p>
 *  <dt>keysAttributesTable<dd>a read only table containing
 *  information about the key attributes (i.e. whether a key is
 *  enabled or disabled). The table indicates the attributes
 *  associated with every key for every configured session.
 *  <p>
 *  <dt>configuration<dd>read/write table containing transmitter (client)
 *  configuration parameters. The table contains the parameters
 *  described in the IPDR_InitExporter function description as well as
 *  parameters controlling the log address and logging level of
 *  various components within the transmitter (client). Note that modifying
 *  parameters in this section might result in network connections
 *  being restarted (for example modifying the transmitter (client) port number or
 *  the console port number). The initial values for these parameters
 *  are set by the \a ParamsHandler callback to IPDR_InitExporter()
 *  (see also @ref IPDR_ParametersHandlerFunc); then values
 *  can be overriden with the \a LoadMIB callback to
 *  IPDR_InitExporter() --- this also allows settin up initial sessions
 *  and collectors (servers) (presumably from values saved using the \a SaveMIB
 *  callback, which would have been created by "walking" the MIB tree
 *  with IPDR_MIBGetNext()).
 *  <p>
 *  <dt>commands<dd>a section allowing the user to send commands for
 *  adding / removing sessions and adding / removing collectors (servers). Each
 *  command has a corresponding sub section containing the command's
 *  parameters and an execute parameter. In order to invoke a command,
 *  the user should fill the parameters and set the corresponding
 *  'execute' parameter to 1.
 *  </dl>
 *
 *  @{
 */


/** The root OID for all IPDR MIB items; a @ref IPDR_OID is taken
 *  relative to this value
 *  (<code>iso.org.dod.internet.private.ipdr.exorter</code> or <code>1.3.6.1.4.1.9334.1</code>).
 *  @see IPDR_MIB_ROOT_OID_INTS
 */
/*#define IPDR_MIB_ROOT_OID "1.3.6.1.4.4316.2"*/


/** The root node name for all IPDR MIB items; it has the
 *  same meaning as #IPDR_MIB_ROOT_OID.
 */
#define IPDR_MIB_ROOT_NODE_NAME "iso.org.dod.internet.private.ipdr.exporter"


/** The root OID as a list of integers (see @ref IPDR_MIB_ROOT_OID_LENGTH).
 */
/*#define IPDR_MIB_ROOT_OID_INTS 1, 3, 6, 1, 4, 9334, 1*/


/** The root OID's number of dotted items (see @ref IPDR_MIB_ROOT_OID_INTS).
 */
#define IPDR_MIB_ROOT_OID_LENGTH 7


/** The maximum number of integers in a @ref IPDR_OID.
 */
#define IPDR_MAX_OID_SIZE 6


/** The MIB-related functions use Object Identifiers (OIDs) in order
 *  to specify an object within the IPDR transmitter (client) MIB; all object
 *  identifiers are relative to the IPDR transmitter (client) MIB root OID
 *  #IPDR_MIB_ROOT_OID.
 */
typedef struct
{
    int Length; /**< Length of the OID vector of numbers. */
    unsigned int OIDArray[IPDR_MAX_OID_SIZE]; /**< The OID numbers (relative to #IPDR_MIB_ROOT_OID). */
}
IPDR_OID;


/** The "zero" OID for starting a "walk" of the MIB (e.g., with IPDR_MIBGetNext()).
 */
extern const IPDR_OID IPDR_ZeroOID;


/** For translating from an OID to a name, there is a structure
 *  similar to a @ref IPDR_OID, except it has names instead of OIDs.
 */
typedef struct
{
    int Length; /**< Length of the OID vector of numbers. */
    const char* NameArray[IPDR_MAX_OID_SIZE]; /**< The node names. */
}
IPDR_OIDname;


/** Get the MIB version identifier (for synchronizing store and load).
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @return Version (as passed to ::IPDR_LoadMIBFunc and ::IPDR_SaveMIBFunc);
 *          -1 is returned if there was an error (e.g., invalid \a Exporter).
 */
IPDR_API_LINKAGE
int IPDR_MIBVersion(IPDR_ExporterHandle ExporterHandle);

IPDR_API
IPDR_MIB_Start(IPDR_PlatformServices *PS);

IPDR_API
IPDR_MIB_Stop(IPDR_PlatformServices *PS);

IPDR_BOOL
IPDR_MIB_IsRunning(IPDR_PlatformServices *PS);

IPDR_API
IPDR_ParamGet(	/* IN */ IPDR_ExporterHandle ExporterHandle,
				/* IN */ const char *Name,
				/* OUT */ char *Value,	/* IN */ int valueLen);

IPDR_API
IPDR_GetFirstParam(	/* IN */ IPDR_ExporterHandle ExporterHandle,
					/* IN/OUT */ unsigned long *Index,
					/* OUT */ char** Name,		/* IN */ int nameLen,
					/* OUT */ char** Value, 	/* IN */ int valueLen,
					/* OUT */ char** DefValue,	/* IN */ int defValueLen);

IPDR_API
IPDR_GetNextParam(	/* IN */ IPDR_ExporterHandle ExporterHandle,
					/* IN/OUT */ unsigned long *Index,
					/* OUT */char** Name,		/* IN */ int nameLen,
					/* OUT */char** Value,		/* IN */ int valueLen,
					/* OUT */char** DefValue,	/* IN */ int defValueLen);


IPDR_API
IPDR_ParamSet(	/* IN */ IPDR_ExporterHandle ExporterHandle,
				/* IN */ const char *Name,
				/* IN */ const char *Value);


/*
 **********************
 * CRANE API function *
 **********************
 */
IPDR_API
IPDR_GetServerStatisticsAndState(IPDR_ExporterHandle ExporterHandle,
                                  IPDR_SessionHandle SessionHandle,
                                  IPDR_IPAddress Addr,
                                  unsigned short  Port,
                                  IPDR_Time *Now,
                                  IPDR_Statistics *Statistics,
                                  IPDR_EventData* LastEvent);


/*@}*/
/*@}*/


/** @defgroup IPDR_utilities Utility and debugging functions
 *
 *  <p><p>
 *  These functions are provided for convenience, mainly for
 *  debugging. As such, they are <em>not</em> part of the official API
 *  for sending data via the IPDR protocol.
 *
 *  Because these use extra parts of the C standard library (such as
 *  stdio), they must be activated by a "<code>#define IPDR_UTILITIES</code>"
 *  (see Config.h)
 *
 *  @{
 */
#if defined(IPDR_UTILITIES) || defined(DOXYGEN_PROCESSING)

#include <stdio.h>


/** Execute a console command and output the result to a file.
 *  Typically, the caller will read a line, then call this
 *  function to evaluate it and output its results. This is the
 *  same function as is used by the console, so the command
 *  "<code>help</code>" will generate a list of commands.
 *  Comments can be added to a line by preceding them with
 *  a "<code>;</code>". Blank lines and comment lines generate
 *  no output. It is undefined what happens if this function is
 *  simultaneously by multiple threads (e.g., by a debugging
 *  main thread and the console reader).
 *
 *  "#" is the comment symbol: anything after "#" is ignored.
 *
 *  Multiple commands can be separated by ";".
 *
 *  Parameters with embedded white space can have be quoted with
 *  either the single or the double quote symbol.
 *
 *  If there are multiple commands in a line (separated
 *  by ";"), \a ParsedCommand, \a ParsedArgc, and \a ParsedArgs
 *  will be for the last command that was executed.
 *
 *  If a command fails, further commands in the line are not executed.
 *
 * <table>
 * <tr><td><b>console commands</b></td></tr>
 * <tr><td>help</td>
 *      <td>Print out brief help information for the commands.</td></tr>
 * <tr><td>helpm <i>module</i></td>
 *      <td>Print out brief help information for a module's commands.</td></tr>
 * <tr><td>helpc <i>command</i></td>
 *      <td>Print out brief help information for a command.</td></tr>
 * <tr><td>exit</td>
 *      <td>Exit from the console.</td></tr>
 * <tr><td>stop</td>
 *      <td>Exit from the console.</td></tr>
 * <tr><td><b>log commands</b></td></tr>
 * <tr><td>get_debug_level [ <i>module</i> ]</td>
 *      <td>Display the logging level for one or all the modules
 *          (see IPDR_SetModuleDebugLevel()).</td></tr>
 * <tr><td>set_debug_level <i>module</i>|* <i>level</i></td>
 *      <td>Set the logging level for one or all modules (the "*"
 *          argument means "all").
 *          See IPDR_SetModuleDebugLevel().</td></tr>
 * <tr><td>get_logger_addr</td>
 *      <td>Display the UDP (syslog) address and port.
 *          See IPDR_SetLogOutputAddress().</td></tr>
 * <tr><td>set_logger_addr <address> <i>port</i></td>
 *      <td>Set the UDP (syslog) address and port.
 *          See IPDR_SetLogOutputAddress().</td></tr>
 * <tr><td><b>transmitter (client) commands:</b></td></tr>
 * <tr><td>add_session <i>id</i> <i>name</i> <i>description</i></td>
 *      <td>See IPDR_CreateSession().</td></tr>
 * <tr><td>delete_session <id></td>
 *      <td>See IPDR_DeleteSession().</td></tr>
 * <tr><td>add_server <i>session_id</i> <i>name</i> <i>ip</i> <i>port</i> <i>priority</i></td>
 *      <td>See IPDR_AddCollector().</td></tr>
 * <tr><td>delete_server <i>session_id</i> <i>ip</i> <i>port</i></td>
 *      <td>See IPDR_RemoveServer().</td></tr>
 * <tr><td>get_sessions</td>
 *      <td>Displays all the sessions, their collectors (servers), and statistics for
            them.</td></tr>
 * <tr><td>apply_templates</td>
 *      <td>See IPDR_ApplyTemplates().</td></tr>
 * <tr><td>get_templates [ <i>session_id</i> ]</td>
 *      <td>Display all the tmplates for all sessions, or for
 *           a single session.</td></tr>
 * <tr><td>version</td>
 *      <td>Show version information.</td></tr>
 * <tr><td>time</td>
 *      <td>Show the current time, when the transmitter (client) was started (the
 *          "boot" time in the protocol START ACK record) and how
 *          long it has been running.</td></tr>
 * <tr><td>set_time <i>epoch_seconds</i></td>
 *      <td>Set the time using a value from 1970-01-01 in seconds.
 *          This is intended for hosts which do not have a mechanism
 *          for setting the clock; and would be envoked by something
 *          like this (assuming that the host is on 192.168.8.88 and
 *          port 40000 is the console port):
 *  <xmp>
 *   perl -e '$|=1; sleep 1;
 *         print "time\nset_time ", time(), "\n";
 *         sleep 1' |
 *     telnet 192.168.8.88 40000
 *  </xmp>
 *      </td></tr>
 * <tr><td>parameters</td>
 *      <td>Show the parameter values as returned by
 *          IPDR_ParametersHandlerFunc().</td></tr>
 * <tr><td>mib [ <i>node</i> ]</td>
 *      <td>Display the MIB below a particular node (or all the
 *          MIB if the node is omitted).</td></tr>
 * <tr><td>mibset <i>node</i> <i>value</i></td>
 *      <td>Set one node in the MIB (see IPDR_MIBSet()).</td></tr>
 * <tr><td>log [ <i>print</i> | <i>callback</i> ]</td>
 *      <td>With no arguments, show the output status of the log; otherwise
 *          set the status (see IPDR_SetLogOutputCallbackDetailed()):
 *          <dl><dt>print</dt><dd>Override the callback
 *              and instead output to <code>stderr</code> (this
 *              is maingly for debugging).</dd>
 *              <dt>callback</dt><dd>Normal callback behavior.</dd>
 *          </dl>
 *      </td></tr>
 *  </table>
 *
 *  @param ExporterHandle Handle to a transmitter (client) previously created by IPDR_InitExporter().
 *  @param Output The output file, which must be already opened.
 *  @param Cmd The command line (null-terminated C string). This is
 *     modified as a by-product of parsing, so if you should make a copy
 *     of it first if you wish to use it afterwards.
 *  @param ParsedCommand (output) Gets the command (if any)
 *     This parameter can be NULL if you don't want the value.
 *  @param ParsedArgc (output) Gets the number of arguments (zero if none).
 *     This parameter can be NULL if you don't want the value.
 *  @param ParsedArgs (output) Gets the arguments as a vector of C-strings.
 *     This parameter can be NULL if you don't want the value.
 *  @param RemainingCmd (output) If there is more in the input to process,
 *     then this will point to the unknown command; otherwise it will be NULL
 *     or an empty string.
 *     This parameter can be NULL if you don't want the value.
 *  @return #IPDR_OK on success; #IPDR_CMD_EXIT if an "exit" command was found;
 *     #IPDR_CMD_HELP if a "help" command was found (its output will have already been done);
 *     #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_API
IPDR_DoConsoleCommand(IPDR_ExporterHandle ExporterHandle, FILE *Output, char* Cmd,
                       const char **ParsedCommand,
                       int *ParsedArgc,
                       const char ***ParsedArgs,
                       char **RemainingCmd,
						char *CmdRes,
						int CmdResLen);


#ifdef __cplusplus
}
#endif

#endif

#endif
