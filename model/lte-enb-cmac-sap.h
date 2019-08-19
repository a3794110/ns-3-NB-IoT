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
 *         Marco Miozzo <mmiozzo@cttc.es>
 */

#ifndef LTE_ENB_CMAC_SAP_H
#define LTE_ENB_CMAC_SAP_H

#include <ns3/packet.h>
#include <ns3/ff-mac-common.h>
#include <ns3/eps-bearer.h>
#include <ns3/lte-common.h>

namespace ns3 {


class LteMacSapUser;

/**
 * Service Access Point (SAP) offered by the eNB MAC to the eNB RRC
 * See Femto Forum MAC Scheduler Interface Specification v 1.11, Figure 1
 *
 * This is the MAC SAP Provider, i.e., the part of the SAP that contains the MAC methods called by the RRC
 */
class LteEnbCmacSapProvider
{
public:
  virtual ~LteEnbCmacSapProvider ();
  /**
   *
   *
   * @param ulBandwidth
   * @param dlBandwidth
   */
  virtual void ConfigureMac (uint8_t ulBandwidth,
                             uint8_t dlBandwidth) = 0;


  virtual void AddUe (uint16_t rnti) = 0;

  /** 
   * remove the UE, e.g., after handover or termination of the RRC connection
   * 
   * \param rnti 
   */
  virtual void RemoveUe (uint16_t rnti) = 0;

  /**
   * Logical Channel information to be passed to CmacSapProvider::ConfigureLc
   *
   */
  struct LcInfo
  {
    uint16_t rnti;   /**< C-RNTI identifying the UE */
    uint8_t  lcId;    /**< logical channel identifier */
    uint8_t  lcGroup; /**< logical channel group */
    uint8_t  qci;     /**< QoS Class Identifier */
    bool     isGbr;   /**< true if the bearer is GBR, false if the bearer is NON-GBR */
    uint64_t mbrUl;   /**< maximum bitrate in uplink */
    uint64_t mbrDl;   /**< maximum bitrate in downlink */
    uint64_t gbrUl;   /**< guaranteed bitrate in uplink */
    uint64_t gbrDl;   /**< guaranteed bitrate in downlink */
  };

  /**
   * Add a new logical channel
   *
   * \param lcinfo
   * \param msu
   */
  virtual void AddLc (LcInfo lcinfo, LteMacSapUser* msu) = 0;


  /**
   * Reconfigure an existing logical channel
   *
   * \param lcinfo
   */
  virtual void ReconfigureLc (LcInfo lcinfo) = 0;


  /**
   * release an existing logical channel
   *
   * \param rnti
   * \param lcid
   */
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid) = 0;
  
  /**
   * \brief Parameters for [re]configuring the UE 
   */
  struct UeConfig
  {
    /**
     * UE id within this cell
     */
    uint16_t  m_rnti;
    /**
     * Transmission mode [1..7] (i.e., SISO, MIMO, etc.)
     */
    uint8_t   m_transmissionMode;
  };

  /** 
   * update the configuration of the UE
   * 
   * \param params 
   */
  virtual void UeUpdateConfigurationReq (UeConfig params) = 0;
  


  /**
   * struct defining the RACH configuration of the MAC
   * 
   */

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
  struct CELV0_Info{
    int npdcch_NumRepetitions;
    int npdcch_StartSF_USS;
    double npdcch_Offset_USS;
  };

  struct CELV1_Info{
    int npdcch_NumRepetitions;
    int npdcch_StartSF_USS;
    double npdcch_Offset_USS;
  };

  struct CELV2_Info{
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
    CELV0_Info CELV0;
    CELV1_Info CELV1;
    CELV2_Info CELV2;
  };

  struct RSRP_ThresholdsNPRACH_InfoList{
    int NRSRP_thresholds_first_value;
    int NRSRP_thresholds_second_value;
  };

  struct NPRACH_ConfigSIB{
    RSRP_ThresholdsNPRACH_InfoList rsrp_ThresholdsPrachInfoList;
    NPRACH_ParametersList nprach_ParametersList;
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
  struct NpdcchConfig{
    NPDCCH_ParametersList npdcch_ParametersList;
  };


  /** 
   * 
   * \return the current RACH configuration of the MAC
   */
  virtual RachConfig GetRachConfig () = 0;

  virtual NpdcchConfig GetNpdcchConfig () = 0;

  /**
   * 
   * 
   */
  struct AllocateNcRaPreambleReturnValue
  {
    bool valid; ///< true if a valid RA config was allocated, false otherwise
    uint8_t raPreambleId; ///< random access preamble id
    uint8_t raPrachMaskIndex; /// PRACH mask index
  };

  /** 
   * Allocate a random access preamble for non-contention based random access (e.g., for handover).
   * 
   * \param rnti the RNTI of the UE who will perform non-contention based random access
   * 
   * \return  the newly allocated random access preamble 
   */
  virtual AllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti) = 0;

};



/**
 * Service Access Point (SAP) offered by the MAC to the RRC
 * See Femto Forum MAC Scheduler Interface Specification v 1.11, Figure 1
 *
 * This is the MAC SAP User, i.e., the part of the SAP that contains the RRC methods called by the MAC
 */
class LteEnbCmacSapUser
{
public:
  virtual ~LteEnbCmacSapUser ();

  /** 
   * request the allocation of a Temporary C-RNTI 
   *
   * \return the T-C-RNTI
   */
  virtual uint16_t AllocateTemporaryCellRnti () = 0;

  /**
   * notify the result of the last LC config operation
   *
   * \param rnti the rnti of the user
   * \param lcid the logical channel id
   * \param success true if the operation was successful, false otherwise
   */
  virtual void NotifyLcConfigResult (uint16_t rnti, uint8_t lcid, bool success) = 0;

  /**
   * \brief Parameters for [re]configuring the UE 
   */
  struct UeConfig
  {
    /**
     * UE id within this cell
     */
    uint16_t  m_rnti;
    /**
     * Transmission mode [1..7] (i.e., SISO, MIMO, etc.)
     */
    uint8_t   m_transmissionMode;
  };

  /** 
   * Notify the RRC of a UE config updated requested by the MAC (normally, by the scheduler)
   * 
   * \param params 
   */
  virtual void RrcConfigurationUpdateInd (UeConfig params) = 0;
};







} // namespace ns3


#endif // MAC_SAP_H
