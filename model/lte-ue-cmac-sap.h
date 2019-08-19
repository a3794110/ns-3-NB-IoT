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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef LTE_UE_CMAC_SAP_H
#define LTE_UE_CMAC_SAP_H

#include <ns3/packet.h>
#include <ns3/ff-mac-common.h>
#include <ns3/eps-bearer.h>
#include <ns3/lte-common.h>

namespace ns3 {



class LteMacSapUser;

/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP Provider, i.e., the part of the SAP that contains the MAC methods called by the RRC
 */
class LteUeCmacSapProvider
{
public:
  virtual ~LteUeCmacSapProvider ();


  //for NB-IoT
  struct EnhancementCoverageLevel_0{
    int numRepetitionsPerPreambleAttempt_r13;
    int maxNumPreambleAttempt_r13;
    int periodicity_r13;
    int startTime_r13;
    int npdcch_numRepetitions_RA_r13;
    int npdcch_StartSF_CSS_RA_r13;
  };

  struct EnhancementCoverageLevel_1{
    int numRepetitionsPerPreambleAttempt_r13;
    int maxNumPreambleAttempt_r13;
    int periodicity_r13;
    int startTime_r13;
    int npdcch_numRepetitions_RA_r13;
    int npdcch_StartSF_CSS_RA_r13;
  };

  struct EnhancementCoverageLevel_2{
    int numRepetitionsPerPreambleAttempt_r13;
    int maxNumPreambleAttempt_r13;
    int periodicity_r13;
    int startTime_r13;
    int npdcch_numRepetitions_RA_r13;
    int npdcch_StartSF_CSS_RA_r13;
  };

  //for nb-iot
  struct CELV_0{
    int npdcch_NumRepetitions;
    int npdcch_StartSF_USS;
    double npdcch_Offset_USS;
  };

  struct CELV_1{
    int npdcch_NumRepetitions;
    int npdcch_StartSF_USS;
    double npdcch_Offset_USS;
  };

  struct CELV_2{
    int npdcch_NumRepetitions;
    int npdcch_StartSF_USS;
    double npdcch_Offset_USS;
  };

  struct NPRACH_ParametersList{
   //for NB-IoT
    EnhancementCoverageLevel_0 CE_0;
    EnhancementCoverageLevel_1 CE_1;
    EnhancementCoverageLevel_2 CE_2;
  };

  //for nb-iot
  struct NPDCCH_ParametersList{
    CELV_0 CELV0;
    CELV_1 CELV1;
    CELV_2 CELV2;
  };

  struct RSRP_ThresholdsNPRACH_InfoList{
    int NRSRP_thresholds_first_value;
    int NRSRP_thresholds_second_value;
  };

  struct NPRACH_ConfigSIB{
    RSRP_ThresholdsNPRACH_InfoList rsrp_ThresholdsPrachInfoList;
    NPRACH_ParametersList nprach_ParametersList;
  };

  //for nb-iot
  struct NPDCCH_ConfigSIB{
    NPDCCH_ParametersList npdcch_ParametersList;
  };

  struct NprachConfig{
    NPRACH_ConfigSIB nprach_ConfigSIB;
  };

  struct RachConfig
  {
    uint8_t numberOfRaPreambles;
    uint8_t preambleTransMax;
    uint8_t raResponseWindowSize;

    //for NB-IoT
    NprachConfig nprachConfig;
  };

  //for nb-iot
  struct NpdcchConfig
  {
    NPDCCH_ConfigSIB npdcch_ConfigSIB;
  };
  
  virtual void ConfigureRach (RachConfig rc) = 0;

  //for nb-iot
  virtual void ConfigureNpdcch (NpdcchConfig nc) = 0;

  /** 
   * tell the MAC to start a contention-based random access procedure,
   * e.g., to perform RRC connection establishment 
   * 
   */
  virtual void StartContentionBasedRandomAccessProcedure () = 0;
  virtual void StartContentionBasedRandomAccessProcedure (uint64_t m_imsi,int repetitionOfPreamble_UE,int preambleTransmissionAttempt_UE,int periodicity_UE,int startTime_UE) = 0;

  /** 
   * tell the MAC to start a non-contention-based random access
   * procedure, e.g., as a consequence of handover
   * 
   * \param rnti
   * \param rapId Random Access Preamble Identifier
   * \param prachMask 
   */
  virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask) = 0;


  struct LogicalChannelConfig
  {
    uint8_t priority;
    uint16_t prioritizedBitRateKbps;
    uint16_t bucketSizeDurationMs;
    uint8_t logicalChannelGroup;
  };
  
  /** 
   * add a new Logical Channel (LC) 
   * 
   * \param lcId the ID of the LC
   * \param lcConfig the LC configuration provided by the RRC
   * \param msu the corresponding LteMacSapUser
   */
  virtual void AddLc (uint8_t lcId, LogicalChannelConfig lcConfig, LteMacSapUser* msu) = 0;

  /** 
   * remove an existing LC
   * 
   * \param lcId 
   */
  virtual void RemoveLc (uint8_t lcId) = 0;

  /** 
   * reset the MAC
   * 
   */
  virtual void Reset () = 0;
  
};



/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP User, i.e., the part of the SAP that contains the RRC methods called by the MAC
 */
class LteUeCmacSapUser
{
public:

  virtual ~LteUeCmacSapUser ();

  /** 
   * 
   * 
   * \param rnti the T-C-RNTI, which will eventually become the C-RNTI after contention resolution
   */
  virtual void SetTemporaryCellRnti (uint16_t rnti) = 0;

  /** 
   * Notify the RRC that the MAC Random Access procedure completed successfully
   * 
   */
  virtual void NotifyRandomAccessSuccessful () = 0;

  /** 
   * Notify the RRC that the MAC Random Access procedure failed
   * 
   */
  virtual void NotifyRandomAccessFailed () = 0;
};




} // namespace ns3


#endif // LTE_UE_CMAC_SAP_H
