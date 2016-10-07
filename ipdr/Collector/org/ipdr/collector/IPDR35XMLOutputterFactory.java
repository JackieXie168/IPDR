package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterFactory;
import java.net.InetAddress;

/**
 * Factory for IPDR35XMLOutputter objects
 */
public final class IPDR35XMLOutputterFactory
    implements OutputterFactory {
  private IPDR35XMLDocDataHandler mDocDataHandler;
  private StateMachineListenerManager mStateMachineListenerManager;
  private String mConfiFileName;
  private String mOutputPrefix;

  /**
   * Constructor
   *
   * @param confiFileName
   *            The configuration file name
   * @param outputPrefix
   *            The prefix of output file name
   * @param stateMachineListenerManager
   *            The manager of StateMachineListeners
   */
  public IPDR35XMLOutputterFactory(String confiFileName, String outputPrefix,StateMachineListenerManager stateMachineListenerManager) {
    mConfiFileName = confiFileName;
    mOutputPrefix = outputPrefix;
    mDocDataHandler = new IPDR35XMLDocDataHandler();
    mStateMachineListenerManager = stateMachineListenerManager;
  }

  /**
   * @see org.ipdr.api.OutputterFactory#create()
   */
  public Outputter create(InetAddress transmitterAddress,
                          int transmitterPort) {
    return new IPDR35XMLOutputter(mConfiFileName, mOutputPrefix,
                                  transmitterAddress, transmitterPort,
                                  mDocDataHandler,mStateMachineListenerManager);
  }
}
