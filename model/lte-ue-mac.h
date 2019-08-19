/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo  <nbaldo@cttc.es>
 * Author: Marco Miozzo <mmiozzo@cttc.es>
 */

#ifndef LTE_UE_MAC_ENTITY_H
#define LTE_UE_MAC_ENTITY_H



#include <map>

#include <ns3/lte-mac-sap.h>
#include <ns3/lte-ue-cmac-sap.h>
#include <ns3/lte-ue-phy-sap.h>
#include <ns3/nstime.h>
#include <ns3/event-id.h>
#include <vector>
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include <ns3/lte-ue-rrc.h>


namespace ns3 {

class UniformRandomVariable;

class LteUeMac :   public Object
{
  friend class UeMemberLteUeCmacSapProvider;
  friend class UeMemberLteMacSapProvider;
  friend class UeMemberLteUePhySapUser;

public:
  static TypeId GetTypeId (void);

  LteUeMac ();
  virtual ~LteUeMac ();
  virtual void DoDispose (void);

  LteMacSapProvider*  GetLteMacSapProvider (void);
  void  SetLteUeCmacSapUser (LteUeCmacSapUser* s);
  LteUeCmacSapProvider*  GetLteUeCmacSapProvider (void);

  /**
  * \brief Get the PHY SAP user
  * \return a pointer to the SAP user of the PHY
  */
  LteUePhySapUser* GetLteUePhySapUser ();

  /**
  * \brief Set the PHY SAP Provider
  * \param s a pointer to the PHY SAP Provider
  */
  void SetLteUePhySapProvider (LteUePhySapProvider* s);
  
  /**
  * \brief Forwarded from LteUePhySapUser: trigger the start from a new frame
  *
  * \param frameNo frame number
  * \param subframeNo subframe number
  */
  void DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo);

 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  int64_t AssignStreams (int64_t stream);

private:
  // forwarded from MAC SAP
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params);
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params);

  // forwarded from UE CMAC SAP
  void DoConfigureRach (LteUeCmacSapProvider::RachConfig rc);
  //for nb-iot
  void DoConfigureNpdcch (LteUeCmacSapProvider::NpdcchConfig nc);
  void DoStartContentionBasedRandomAccessProcedure ();
  void DoStartContentionBasedRandomAccessProcedure (uint64_t m_imsi, int repetitionOfPreamble_UE,int preambleTransmissionAttempt_UE,int periodicity_UE,int startTime_UE);//for NB-IoT
  void DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask);
  void DoAddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  void DoRemoveLc (uint8_t lcId);
  void DoReset ();

  // forwarded from PHY SAP
  void DoReceivePhyPdu (Ptr<Packet> p);
  void DoReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  
  // internal methods
  void RandomlySelectAndSendRaPreamble ();
  void RandomlySelectAndSendRaPreamble (int repetitionOfPreamble_UE,int preambleTransmissionAttempt_UE,int periodicity_UE,int startTime_UE);//for NB-IoT
  void SendRaPreamble (bool contention);
  void SendRaPreamble (int repetitionOfPreamble_UE,int preambleTransmissionAttempt_UE,int periodicity_UE,int startTime_UE);//for NB-IoT
  void StartWaitingForRaResponse ();
  void RecvRaResponse (BuildRarListElement_s raResponse);
  void RaResponseTimeout (bool contention);
  void RaResponseTimeout_NB (int repetitionOfPreamble_UE,int preambleTransmissionAttempt_UE,int periodicity_UE,int startTime_UE);//for NB-IoT
  void SendReportBufferStatus (void);
  void RefreshHarqProcessesPacketBuffer (void);

private:

  //for nb-iot
  // these parameter is for msg4 scheduling
  struct NPRACH_Parameters_NB_r13
  {
    uint16_t npdcch_NumRepetitions_RA_r13;
    uint16_t npdcch_StartSF_CSS_RA_r13;
    double npdcch_Offset_RA_r13;
  };

  //for nb-iot
  // we first configure parameters for msg4 scheduling before RAR
  // so we do it in SendRaPreamble() and waiting in RecvRaResponse()
  uint16_t m_rep;
  uint16_t m_startsf;
  double m_offset;

  struct LcInfo
  {
    LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
    LteMacSapUser* macSapUser;
  };

  //for nb-iot
  // use function in ue_rrc to send msg4 scheduling parameter
  LteUeRrc* m_ueRrc = new LteUeRrc;

  std::map <uint8_t, LcInfo> m_lcInfoMap;

  LteMacSapProvider* m_macSapProvider;

  LteUeCmacSapUser* m_cmacSapUser;
  LteUeCmacSapProvider* m_cmacSapProvider;

  LteUePhySapProvider* m_uePhySapProvider;
  LteUePhySapUser* m_uePhySapUser;
  
  std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters> m_ulBsrReceived; // BSR received from RLC (the last one)
  
  
  Time m_bsrPeriodicity;
  Time m_bsrLast;
  
  bool m_freshUlBsr; // true when a BSR has been received in the last TTI

  uint8_t m_harqProcessId;
  std::vector < Ptr<PacketBurst> > m_miUlHarqProcessesPacket; // Packets under trasmission of the UL HARQ processes
  std::vector < uint8_t > m_miUlHarqProcessesPacketTimer; // timer for packet life in the buffer

  uint16_t m_rnti;

  bool m_rachConfigured;
  LteUeCmacSapProvider::RachConfig m_rachConfig;
  //for nb-iot
  bool m_npdcchConfigured;
  LteUeCmacSapProvider::NpdcchConfig m_npdcchConfig;

  uint8_t m_raPreambleId;
  uint8_t m_preambleTransmissionCounter;

  //for NB-IoT
  uint64_t UE_IMSI;
  int m_preambleTransmissionRepetitionCounter;
  int transmissionLastTime;
  int testCounter;
  int startSendTime;
  int totalSpendTime;

  uint16_t m_backoffParameter;
  EventId m_noRaResponseReceivedEvent;
  Ptr<UniformRandomVariable> m_raPreambleUniformVariable;

  uint32_t m_frameNo;
  uint32_t m_subframeNo;
  uint8_t m_raRnti;
  bool m_waitingForRaResponse;
};

} // namespace ns3

#endif // LTE_UE_MAC_ENTITY
