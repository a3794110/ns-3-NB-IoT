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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo  <nbaldo@cttc.es>
 */


#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>

#include "lte-amc.h"
#include "lte-control-messages.h"
#include "lte-enb-net-device.h"
#include "lte-ue-net-device.h"

#include <ns3/lte-enb-mac.h>
#include <ns3/lte-radio-bearer-tag.h>
#include <ns3/lte-ue-phy.h>

#include "ns3/lte-mac-sap.h"
#include <ns3/lte-common.h>

#include <ns3/lte-enb-rrc.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteEnbMac");

NS_OBJECT_ENSURE_REGISTERED (LteEnbMac);



// //////////////////////////////////////
// member SAP forwarders
// //////////////////////////////////////


class EnbMacMemberLteEnbCmacSapProvider : public LteEnbCmacSapProvider
{
public:
  EnbMacMemberLteEnbCmacSapProvider (LteEnbMac* mac);

  // inherited from LteEnbCmacSapProvider
  virtual void ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  virtual void AddUe (uint16_t rnti);
  virtual void RemoveUe (uint16_t rnti);
  virtual void AddLc (LcInfo lcinfo, LteMacSapUser* msu);
  virtual void ReconfigureLc (LcInfo lcinfo);
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid);
  virtual void UeUpdateConfigurationReq (UeConfig params);
  virtual RachConfig GetRachConfig ();
  //for nb-iot
  virtual NpdcchConfig GetNpdcchConfig ();
  virtual AllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti);
  

private:
  LteEnbMac* m_mac;
};


EnbMacMemberLteEnbCmacSapProvider::EnbMacMemberLteEnbCmacSapProvider (LteEnbMac* mac)
  : m_mac (mac)
{
}

void
EnbMacMemberLteEnbCmacSapProvider::ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  m_mac->DoConfigureMac (ulBandwidth, dlBandwidth);
}

void
EnbMacMemberLteEnbCmacSapProvider::AddUe (uint16_t rnti)
{
  m_mac->DoAddUe (rnti);
}

void
EnbMacMemberLteEnbCmacSapProvider::RemoveUe (uint16_t rnti)
{
  m_mac->DoRemoveUe (rnti);
}

void
EnbMacMemberLteEnbCmacSapProvider::AddLc (LcInfo lcinfo, LteMacSapUser* msu)
{
  m_mac->DoAddLc (lcinfo, msu);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReconfigureLc (LcInfo lcinfo)
{
  m_mac->DoReconfigureLc (lcinfo);
}

void
EnbMacMemberLteEnbCmacSapProvider::ReleaseLc (uint16_t rnti, uint8_t lcid)
{
  m_mac->DoReleaseLc (rnti, lcid);
}

void
EnbMacMemberLteEnbCmacSapProvider::UeUpdateConfigurationReq (UeConfig params)
{
  m_mac->DoUeUpdateConfigurationReq (params);
}

LteEnbCmacSapProvider::RachConfig 
EnbMacMemberLteEnbCmacSapProvider::GetRachConfig ()
{
  return m_mac->DoGetRachConfig ();
}

//for nb-iot
LteEnbCmacSapProvider::NpdcchConfig
EnbMacMemberLteEnbCmacSapProvider::GetNpdcchConfig ()
{
  return m_mac->DoGetNpdcchConfig();
}
 
LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue 
EnbMacMemberLteEnbCmacSapProvider::AllocateNcRaPreamble (uint16_t rnti)
{
  return m_mac->DoAllocateNcRaPreamble (rnti);
}


class EnbMacMemberFfMacSchedSapUser : public FfMacSchedSapUser
{
public:
  EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac);


  virtual void SchedDlConfigInd (const struct SchedDlConfigIndParameters& params);
  virtual void SchedUlConfigInd (const struct SchedUlConfigIndParameters& params);
private:
  LteEnbMac* m_mac;
};


EnbMacMemberFfMacSchedSapUser::EnbMacMemberFfMacSchedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}


void
EnbMacMemberFfMacSchedSapUser::SchedDlConfigInd (const struct SchedDlConfigIndParameters& params)
{
  m_mac->DoSchedDlConfigInd (params);
}



void
EnbMacMemberFfMacSchedSapUser::SchedUlConfigInd (const struct SchedUlConfigIndParameters& params)
{
  m_mac->DoSchedUlConfigInd (params);
}



class EnbMacMemberFfMacCschedSapUser : public FfMacCschedSapUser
{
public:
  EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac);

  virtual void CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params);
  virtual void CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params);
  virtual void CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params);
  virtual void CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params);
  virtual void CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params);
  virtual void CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params);
  virtual void CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params);

private:
  LteEnbMac* m_mac;
};


EnbMacMemberFfMacCschedSapUser::EnbMacMemberFfMacCschedSapUser (LteEnbMac* mac)
  : m_mac (mac)
{
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params)
{
  m_mac->DoCschedCellConfigCnf (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params)
{
  m_mac->DoCschedUeConfigCnf (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params)
{
  m_mac->DoCschedLcConfigCnf (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params)
{
  m_mac->DoCschedLcReleaseCnf (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params)
{
  m_mac->DoCschedUeReleaseCnf (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params)
{
  m_mac->DoCschedUeConfigUpdateInd (params);
}

void
EnbMacMemberFfMacCschedSapUser::CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params)
{
  m_mac->DoCschedCellConfigUpdateInd (params);
}



// ---------- PHY-SAP


class EnbMacMemberLteEnbPhySapUser : public LteEnbPhySapUser
{
public:
  EnbMacMemberLteEnbPhySapUser (LteEnbMac* mac);

  // inherited from LteEnbPhySapUser
  virtual void ReceivePhyPdu (Ptr<Packet> p);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  virtual void ReceiveRachPreamble (uint32_t prachId);
  virtual void UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi);
  virtual void UlInfoListElementHarqFeeback (UlInfoListElement_s params);
  virtual void DlInfoListElementHarqFeeback (DlInfoListElement_s params);

private:
  LteEnbMac* m_mac;
};

EnbMacMemberLteEnbPhySapUser::EnbMacMemberLteEnbPhySapUser (LteEnbMac* mac) : m_mac (mac)
{
}


void
EnbMacMemberLteEnbPhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p);
}

void
EnbMacMemberLteEnbPhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  m_mac->DoSubframeIndication (frameNo, subframeNo);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveLteControlMessage (Ptr<LteControlMessage> msg)
{
  m_mac->DoReceiveLteControlMessage (msg);
}

void
EnbMacMemberLteEnbPhySapUser::ReceiveRachPreamble (uint32_t prachId)
{
  m_mac->DoReceiveRachPreamble (prachId);
}

void
EnbMacMemberLteEnbPhySapUser::UlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi)
{
  m_mac->DoUlCqiReport (ulcqi);
}

void
EnbMacMemberLteEnbPhySapUser::UlInfoListElementHarqFeeback (UlInfoListElement_s params)
{
  m_mac->DoUlInfoListElementHarqFeeback (params);
}

void
EnbMacMemberLteEnbPhySapUser::DlInfoListElementHarqFeeback (DlInfoListElement_s params)
{
  m_mac->DoDlInfoListElementHarqFeeback (params);
}


// //////////////////////////////////////
// generic LteEnbMac methods
// //////////////////////////////////////


TypeId
LteEnbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteEnbMac")
    .SetParent<Object> ()
    .SetGroupName("Lte")
    .AddConstructor<LteEnbMac> ()
    .AddAttribute ("NumberOfRaPreambles",
                   "how many random access preambles are available for the contention based RACH process",
                   UintegerValue (48), //48 for NB-IoT,original is 50
                   MakeUintegerAccessor (&LteEnbMac::m_numberOfRaPreambles),
                   MakeUintegerChecker<uint8_t> (4, 48)) //48 for NB-IoT,original is 64
    .AddAttribute ("PreambleTransMax",
                   "Maximum number of random access preamble transmissions",
                   UintegerValue (50),
                   MakeUintegerAccessor (&LteEnbMac::m_preambleTransMax),
                   MakeUintegerChecker<uint8_t> (3, 200))
    .AddAttribute ("RaResponseWindowSize",
                   "length of the window (in TTIs) for the reception of the random access response (RAR); the resulting RAR timeout is this value + 3 ms",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteEnbMac::m_raResponseWindowSize),
                   MakeUintegerChecker<uint8_t> (2, 10))
    .AddTraceSource ("DlScheduling",
                     "Information regarding DL scheduling.",
                     MakeTraceSourceAccessor (&LteEnbMac::m_dlScheduling),
                     "ns3::LteEnbMac::DlSchedulingTracedCallback")
    .AddTraceSource ("UlScheduling",
                     "Information regarding UL scheduling.",
                     MakeTraceSourceAccessor (&LteEnbMac::m_ulScheduling),
                     "ns3::LteEnbMac::UlSchedulingTracedCallback")
  ;

  return tid;
}

// for nb-iot
bool
LteEnbMac::CCtrigger = false; 

bool
LteEnbMac::SCtrigger = false;

std::vector<uint16_t>
LteEnbMac::m_rnti;

std::vector<int>
LteEnbMac::m_rep;

std::vector<int> 
LteEnbMac::m_startsf; 

std::vector<double> 
LteEnbMac::m_offset;

std::vector<LteEnbMac::DCI_N0>
LteEnbMac::m_dci_n0;

std::vector<LteEnbMac::DCI_N1>
LteEnbMac::m_dci_n1;

std::vector<uint16_t>
LteEnbMac::m_ue;

std::vector<int> 
LteEnbMac::m_kb; 

std::vector<int> 
LteEnbMac::m_k; 

LteEnbMac::LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
  m_macSapProvider = new EnbMacMemberLteMacSapProvider<LteEnbMac> (this);
  m_cmacSapProvider = new EnbMacMemberLteEnbCmacSapProvider (this);
  m_schedSapUser = new EnbMacMemberFfMacSchedSapUser (this);
  m_cschedSapUser = new EnbMacMemberFfMacCschedSapUser (this);
  m_enbPhySapUser = new EnbMacMemberLteEnbPhySapUser (this);
}


LteEnbMac::~LteEnbMac ()
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_dlCqiReceived.clear ();
  m_ulCqiReceived.clear ();
  m_ulCeReceived.clear ();
  m_dlInfoListReceived.clear ();
  m_ulInfoListReceived.clear ();
  m_miDlHarqProcessesPackets.clear ();
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_schedSapUser;
  delete m_cschedSapUser;
  delete m_enbPhySapUser;
}


void
LteEnbMac::SetFfMacSchedSapProvider (FfMacSchedSapProvider* s)
{
  m_schedSapProvider = s;
}

FfMacSchedSapUser*
LteEnbMac::GetFfMacSchedSapUser (void)
{
  return m_schedSapUser;
}

void
LteEnbMac::SetFfMacCschedSapProvider (FfMacCschedSapProvider* s)
{
  m_cschedSapProvider = s;
}

FfMacCschedSapUser*
LteEnbMac::GetFfMacCschedSapUser (void)
{
  return m_cschedSapUser;
}



void
LteEnbMac::SetLteMacSapUser (LteMacSapUser* s)
{
  m_macSapUser = s;
}

LteMacSapProvider*
LteEnbMac::GetLteMacSapProvider (void)
{
  return m_macSapProvider;
}

void
LteEnbMac::SetLteEnbCmacSapUser (LteEnbCmacSapUser* s)
{
  m_cmacSapUser = s;
}

LteEnbCmacSapProvider*
LteEnbMac::GetLteEnbCmacSapProvider (void)
{
  return m_cmacSapProvider;
}

void
LteEnbMac::SetLteEnbPhySapProvider (LteEnbPhySapProvider* s)
{
  m_enbPhySapProvider = s;
}


LteEnbPhySapUser*
LteEnbMac::GetLteEnbPhySapUser ()
{
  return m_enbPhySapUser;
}

//for nb-iot
// for enb mac univeral subframe and frame  
static uint32_t u_frameNo;
static uint32_t u_subframeNo;

//for nb-iot
// for enb mac mark msg4 scheduled subframeNo and frameNo
static uint32_t sch_frameNo;
static uint32_t sch_subframeNo;

//for nb-iot
// for change offset for scheduling 0, 1/8, 2/8, 3/8
// i_offset for hopping offset value for different UEs
static double offset[4];
static int i_offset = 0;

//for nb-iot
// for how many UE we have to scheduling, scaleable
// initailize 0 UE for scheduling
static int i_ue = 0;

//for nb-iot
// choose simulation scenario downlink or uplink
// NPDSCH (channel = 1) or NPUSCH (channel = 0)
static int channel;
static int ch;

//for nb-iot
// N is resource allocation for scheduling UE in SCH (the unit of N is subframe)
// start_ stands is the first frame and subframe of N for the scheduling UE
// end_ stands for the end frame and subframe of N for the scheduling UE
static int N;
static int start_frameNo, start_subframeNo;
static int end_frameNo, end_subframeNo;

//for nb-iot
static uint32_t sib1_frameNo=0;

//for nb-iot
// these parameters for simulation statistic results 
static int npdcch_subframe;
static int npdcch_mib;
static int npdcch_sib1;
static int npdcch_npss;
static int npdcch_nsss;
static int npdcch_dci;

static int npdsch_subframe;
static int npdsch_mib;
static int npdsch_sib1;
static int npdsch_npss;
static int npdsch_nsss;
static int npdsch_user;

static int npusch_subframe;
static int npusch_user;

static int sch_ue;

void
LteEnbMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << " EnbMac - frame " << frameNo << " subframe " << subframeNo << "--------------------------------------------------");
  u_frameNo = frameNo;
  u_subframeNo = subframeNo;

  sch_frameNo = frameNo;
  sch_subframeNo = subframeNo;

  // Store current frame / subframe number
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;

  if(m_rnti.empty() == true){
    CCtrigger = false;
  }

  // --- NPDCCH Scheduler ---
  if(CCtrigger == true && SCtrigger == false){
    
    // show parameters information of every UEs in the scheduling queue
    std::cout << "-----------------RNTI table size: " << m_rnti.size() << "-----------------"<< std::endl;
    for(int i = 0;i<int(m_rnti.size()); i++)
    {
      int j = i%4;
      std::cout << "RNTI: " << m_rnti[i] << " Rep: " << m_rep[i] << " StartSf: " << m_startsf[i] << " Offset: " << offset[j] << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;

    std::cout << "Search Space UE Specific Searching for rnti: " << m_rnti[i_ue] << " Start..." << std::endl;
    std::cout << "rnti: " << m_rnti[i_ue] << " rep: " << m_rep[i_ue] << " startsf: " << m_startsf[i_ue] << " offset: " << offset[i_offset] << std::endl;

    // m_rep stands for R_max
    // m_startsf stands for G
    // offset stands for alpha 
    double cond1,cond2;
    int T,R;
    T = m_rep[i_ue]*m_startsf[i_ue];
    cond1 = (10*(m_frameNo-1)+(m_subframeNo-1)) % T;
    cond2 = offset[i_offset]*T;


    // the R here stand for the repetition number of DCI, it's scalealbe accroding to spec
    // if R_max = 1, R = 1
    // if R_max = 2, R = 1 or 2
    // if R_max = 4, R = 1 or 2 or 4
    // if R_max >= 8, R = R_max/8 or R_max/4 or R_max/2 or R_max
    if(m_rep[i_ue] == 1)
    {
      if(m_R == 1)
        R = 1;
      else
        NS_FATAL_ERROR("Wrong DCI Repetition Configuration");
    }
    else if(m_rep[i_ue] == 2)
    {
      if(m_R == 1 || m_R == 2)
        R = m_R;
      else
        NS_FATAL_ERROR("Wrong DCI Repetition Configuration");
    }
    else if(m_rep[i_ue] == 4)
    {
      if(m_R == 1 || m_R == 2 || m_R == 4)
        R = m_R;
      else
        NS_FATAL_ERROR("Wrong DCI Repetition Configuration");
    }
    else if(m_rep[i_ue] > 4)
    {
      if(m_R == m_rep[i_ue] || m_R == m_rep[i_ue]/2 || m_R == m_rep[i_ue]/4 || m_R == m_rep[i_ue]/8)
        R = m_R;
      else
        NS_FATAL_ERROR("Wrong DCI Repetition Configuration");
    }

    if(cond1 == cond2){
        //for nb-iot
        // NPDCCH schedule one UE per subframe
        std::cout << "Search Space UE Specific Searching for rnti: " << m_rnti[i_ue] << " Complete" << std::endl;
        CCtrigger = false;
        DoSearchSpace(m_frameNo, m_subframeNo, m_rnti[i_ue], R);
      }
    else
    {
      if(m_subframeNo == 1)
      {
        npdcch_mib++;
      }
      else if(m_subframeNo == 6)
      {
        npdcch_npss++;
      }
      else if((m_frameNo-1)%2 == 0 && m_subframeNo == 10)
      {
        npdcch_nsss++;
      }
      else if(m_frameNo == sib1_frameNo && m_subframeNo == 5)
      {
        npdcch_sib1++;
      }
      npdcch_subframe++;
    }
    
  }

  // --- NPDSCH/NPUSCH Scheduler ---
  if(SCtrigger == true)
  {
    std::cout << "-------------NPDSCH/NPUSCH Scheduler-------------" << std::endl;
    switch (channel)
    {
    case 0:
    {
      int N_RU0[8] = {1, 2, 3, 4, 5, 6, 8, 10};// NPUSCH resource assignment candidates
      int N_Rep0[8] = {1, 2, 4, 8, 16, 32, 64, 128};// NPUSCH repetition number candidates
      int N_slot0 = 2; // NPUSCH slots of resource unit, here, our simulation is single tone with 15khz ,therefore N_slot = 2, can check in spec 36.331

      start_frameNo = (m_kb[0] + m_k[0])/10;
      start_subframeNo = (m_kb[0] + m_k[0])%10;
      if(start_subframeNo == 0)
      {
        start_frameNo--;
        start_subframeNo=10;
      }

      // special case for last UE in SCH
      // we give last UE N = 1, scaleable
      if(int(m_ue.size()) == 1)
      {
        end_frameNo = start_frameNo;
        end_subframeNo = start_subframeNo;
        N = N_RU0[0]*N_Rep0[0]*N_slot0/2;
        std::cout << "RNTI: " << m_ue[0] << std::endl;
        std::cout << "startframe: " << start_frameNo << std::endl;
        std::cout << "startsubframe: " << start_subframeNo << std::endl;
        std::cout << "endframe: " << end_frameNo << std::endl;
        std::cout << "endsubframe: " << end_subframeNo << std::endl;
        std::cout << "N: " << N << " N_RU: " << N_RU0[0] << " N_Rep: " << N_Rep0[0] << " N_slot: " << N_slot0 << std::endl;
      }
      else
      {
        // use next scheduled UE start_subframe -1 to calculate current scheduled UE enb_subframe
        end_frameNo = (m_kb[1] + m_k[1])/10;
        end_subframeNo = (m_kb[1] + m_k[1])%10;
        if(end_subframeNo == 0)
        {
          end_frameNo--;
          end_subframeNo=10;
        }

        if(end_subframeNo-1 == 0)
        {
          end_subframeNo = 10;
          end_frameNo = end_frameNo-1;
        }
        else
        {
          end_subframeNo--;
        }
        
        // calculate N
        int i = 0;
        while( ((m_kb[1] + m_k[1])-(m_kb[0] + m_k[0])) >= N_RU0[0]*N_Rep0[i]*N_slot0/2 )
        {
          i++;
        }i--;
        //for nb-iot testing
        // if(i>1)
        // i=1;
        N = N_RU0[0]*N_Rep0[i]*N_slot0/2;

        std::cout << "RNTI: " << m_ue[0] << std::endl;
        std::cout << "startframe: " << start_frameNo << std::endl;
        std::cout << "startsubframe: " << start_subframeNo << std::endl;
        std::cout << "endframe: " << end_frameNo << std::endl;
        std::cout << "endsubframe: " << end_subframeNo << std::endl;
        std::cout << "N: " << N << " N_RU: " << N_RU0[0] << " N_Rep: " << N_Rep0[i] << " N_slot: " << N_slot0 << std::endl;
      }
      
      //avoid other UE enter this switch function
      ch = channel;
      channel = 2;
      break;
    }

    case 1:
    {
      int N_SF1[8] = {1, 2, 3, 4, 5, 6, 8, 10};// NPDSCH resource assignment candidates
      int N_Rep1[16] = {1, 2, 4, 8, 16, 32, 64, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};// NPDSCH repetition number candidates

      start_frameNo = (m_kb[0] + m_k[0] + 5)/10;
      start_subframeNo = (m_kb[0] + m_k[0] + 5)%10;
      if(start_subframeNo == 0)
      {
        start_frameNo--;
        start_subframeNo=10;
      }

      // special case for last UE in SCH
      // we give last UE N = 1, scaleable
      if(int(m_ue.size()) == 1)
      {
        end_frameNo = start_frameNo;
        end_subframeNo = start_subframeNo;
        N = N_SF1[0]*N_Rep1[0];
        std::cout << "RNTI: " << m_ue[0] << std::endl;
        std::cout << "startframe: " << start_frameNo << std::endl;
        std::cout << "startsubframe: " << start_subframeNo << std::endl;
        std::cout << "endframe: " << end_frameNo << std::endl;
        std::cout << "endsubframe: " << end_subframeNo << std::endl;
        std::cout << "N: " << N << " N_SF: " << N_SF1[0] << " N_Rep: " << N_Rep1[0] << std::endl;
      }
      else
      {
        // use next scheduled UE start_subframe -1 to calculate current scheduled UE enb_subframe
        end_frameNo = (m_kb[1] + m_k[1] + 5)/10;
        end_subframeNo = (m_kb[1] + m_k[1] + 5)%10;
        if(end_subframeNo == 0)
        {
          end_frameNo--;
          end_subframeNo=10;
        }

        if(end_subframeNo-1 == 0)
        {
          end_subframeNo = 10;
          end_frameNo = end_frameNo-1;
        }
        else
        {
          end_subframeNo--;
        }
        
        // calculate N
        int i = 0;
        while( ((m_kb[1] + m_k[1] + 5)-(m_kb[0] + m_k[0] + 5)) >= N_SF1[0]*N_Rep1[i] )
        {
          i++;
        }i--;
        //for nb-iot testing
        // if(i>1)
        // i=1;
        N = N_SF1[0]*N_Rep1[i];

        std::cout << "RNTI: " << m_ue[0] << std::endl;
        std::cout << "startframe: " << start_frameNo << std::endl;
        std::cout << "startsubframe: " << start_subframeNo << std::endl;
        std::cout << "endframe: " << end_frameNo << std::endl;
        std::cout << "endsubframe: " << end_subframeNo << std::endl;
        std::cout << "N: " << N << " N_SF: " << N_SF1[0] << " N_Rep: " << N_Rep1[i] << std::endl;
      }

      //avoid other UE enter this switch function
      ch = channel;
      channel = 2;
      break;
    }
    
    default:
      break;
    }

    if(start_frameNo == (int)m_frameNo && start_subframeNo == (int)m_subframeNo)
    {
      if(m_subframeNo == 1 && ch == 1)
      {
        std::cout << "------------" << std::endl;
        std::cout << "|  MIB-NB  |" << std::endl;
        std::cout << "------------" << std::endl;
        npdsch_mib++;
        npdsch_subframe++;
      }
      else if(m_subframeNo == 6 && ch == 1)
      {
        std::cout << "------------" << std::endl;
        std::cout << "|   NPSS   |" << std::endl;
        std::cout << "------------" << std::endl;
        npdsch_npss++;
        npdsch_subframe++;
      }
      else if((m_frameNo-1)%2 == 0 && m_subframeNo == 10 && ch == 1)
      {
        std::cout << "------------" << std::endl;
        std::cout << "|   NSSS   |" << std::endl;
        std::cout << "------------" << std::endl;
        npdsch_nsss++;
        npdsch_subframe++;
      }
      else if(m_frameNo == sib1_frameNo && m_subframeNo == 5 && ch ==1)
      {
        std::cout << "------------" << std::endl;
        std::cout << "|  SIB1-NB |" << std::endl;
        std::cout << "------------" << std::endl;
        npdsch_sib1++;
        npdsch_subframe++;
      }
      else if(ch == 0)
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "|  NPUSCH for RNTI: "  << m_ue[0] << "  |" << std::endl;
        std::cout << "---------------------------" << std::endl;
        N--;
        npusch_user++;
        npusch_subframe++;
      }
      else if(ch == 1)
      {
        std::cout << "---------------------------" << std::endl;
        std::cout << "|  NPDSCH for RNTI: "  << m_ue[0] << "  |" << std::endl;
        std::cout << "---------------------------" << std::endl;
        N--;
        npdsch_user++;
        npdsch_subframe++;
      }

      if((start_frameNo == end_frameNo && start_subframeNo == end_subframeNo) || N == 0)
      {
        channel = ch;
        m_ue.erase(m_ue.begin());
        m_kb.erase(m_kb.begin());
        m_k.erase(m_k.begin());
      }
      if(start_subframeNo+1>10)
        {
          start_frameNo++;
          start_subframeNo = 1;
        }
      else
      {
        start_subframeNo++;
      }
      // end the SCH scheduler
      if(m_ue.empty() == true){
        if(channel == 0)
        {
          std::cout << "------------NPUSCH Statistical Results------------" << std::endl;
          std::cout << "|UEs            |" << sch_ue << std::endl;
          std::cout << "|NPUSCH         |" << npusch_user << std::endl;
          std::cout << "|Used subframes |" << npusch_user << std::endl;
          std::cout << "|Total subframes|" << npusch_subframe << std::endl;
          std::cout << "|Efficiency     |" << ((double)npusch_user/(double)npusch_subframe)*100 << "%" << std::endl;
          std::cout << "--------------------------------------------------" << std::endl;
          npusch_user= npusch_subframe = sch_ue = 0;
        }
        else if(channel == 1)
        {
          std::cout << "------------NPDSCH Statistical Results------------" << std::endl;
          std::cout << "|UEs            |" << sch_ue << std::endl;
          std::cout << "|MIB-NB         |" << npdsch_mib << std::endl;
          std::cout << "|SIB1-NB        |" << npdsch_sib1 << std::endl;
          std::cout << "|NPSS           |" << npdsch_npss << std::endl;
          std::cout << "|NSSS           |" << npdsch_nsss << std::endl;
          std::cout << "|NPDSCH         |" << npdsch_user << std::endl;
          std::cout << "|Used subframes |" << npdsch_mib+npdsch_sib1+npdsch_npss+npdsch_nsss+npdsch_user << std::endl;
          std::cout << "|Total subframes|" << npdsch_subframe << std::endl;
          std::cout << "|Efficiency     |" << ((double)(npdsch_mib+npdsch_sib1+npdsch_npss+npdsch_nsss+npdsch_user)/(double)npdsch_subframe)*100 << "%" << std::endl;
          std::cout << "--------------------------------------------------" << std::endl;
          npdsch_mib = npdsch_sib1 = npdsch_npss = npdsch_nsss = npdsch_user = npdsch_subframe = sch_ue = 0;
        }
        SCtrigger = false;
      }
    }
    // for output results
    else if(ch == 0)
    {
      npusch_subframe++;
    }
    else if(ch == 1)
    {
      if(m_subframeNo == 1)
      {
        npdsch_mib++;
      }
      else if(m_subframeNo == 6)
      {
        npdsch_npss++;
      }
      else if((m_frameNo-1)%2 == 0 && m_subframeNo == 10)
      {
        npdsch_nsss++;
      }
      else if(m_frameNo == sib1_frameNo && m_subframeNo == 5)
      {
        npdsch_sib1++;
      }
      npdsch_subframe++;
    }
  }

  // --- DOWNLINK ---
  // Send Dl-CQI info to the scheduler
  if (m_dlCqiReceived.size () > 0)
    {
      FfMacSchedSapProvider::SchedDlCqiInfoReqParameters dlcqiInfoReq;
      dlcqiInfoReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);

      int cqiNum = m_dlCqiReceived.size ();
      if (cqiNum > MAX_CQI_LIST)
        {
          cqiNum = MAX_CQI_LIST;
        }
      dlcqiInfoReq.m_cqiList.insert (dlcqiInfoReq.m_cqiList.begin (), m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
      m_dlCqiReceived.erase (m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
      m_schedSapProvider->SchedDlCqiInfoReq (dlcqiInfoReq);
    }

  if (!m_receivedRachPreambleCount.empty ())
    {
      // process received RACH preambles and notify the scheduler
      FfMacSchedSapProvider::SchedDlRachInfoReqParameters rachInfoReqParams;
      NS_ASSERT (subframeNo > 0 && subframeNo <= 10); // subframe in 1..10
      for (std::map<uint8_t, uint32_t>::const_iterator it = m_receivedRachPreambleCount.begin ();
           it != m_receivedRachPreambleCount.end ();
           ++it)
        {
          NS_LOG_INFO (this << " preambleId " << (uint32_t) it->first << ": " << it->second << " received");
          NS_ASSERT (it->second != 0);
          if (it->second > 1)
            {
              NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": collision");
              // in case of collision we assume that no preamble is
              // successfully received, hence no RAR is sent 
            }
          else
            {
              uint16_t rnti;
              std::map<uint8_t, NcRaPreambleInfo>::iterator jt = m_allocatedNcRaPreambleMap.find (it->first);
              if (jt != m_allocatedNcRaPreambleMap.end ())
                {
                  rnti = jt->second.rnti;
                  NS_LOG_INFO ("preambleId previously allocated for NC based RA, RNTI =" << (uint32_t) rnti << ", sending RAR");
                  
                }
              else
                {
                  rnti = m_cmacSapUser->AllocateTemporaryCellRnti ();
                  NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": allocated T-C-RNTI " << (uint32_t) rnti << ", sending RAR");
                }

              RachListElement_s rachLe;
              rachLe.m_rnti = rnti;
              rachLe.m_estimatedSize = 144; // to be confirmed
              rachInfoReqParams.m_rachList.push_back (rachLe);
              m_rapIdRntiMap.insert (std::pair <uint16_t, uint32_t> (rnti, it->first));
            }
        }
      m_schedSapProvider->SchedDlRachInfoReq (rachInfoReqParams);
      m_receivedRachPreambleCount.clear ();
    }
  // Get downlink transmission opportunities
  uint32_t dlSchedFrameNo = m_frameNo;
  uint32_t dlSchedSubframeNo = m_subframeNo;
  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
  if (dlSchedSubframeNo + m_macChTtiDelay > 10)
    {
      dlSchedFrameNo++;
      dlSchedSubframeNo = (dlSchedSubframeNo + m_macChTtiDelay) % 10;
    }
  else
    {
      dlSchedSubframeNo = dlSchedSubframeNo + m_macChTtiDelay;
    }
  FfMacSchedSapProvider::SchedDlTriggerReqParameters dlparams;
  dlparams.m_sfnSf = ((0x3FF & dlSchedFrameNo) << 4) | (0xF & dlSchedSubframeNo);

  // Forward DL HARQ feebacks collected during last TTI
  if (m_dlInfoListReceived.size () > 0)
    {
      dlparams.m_dlInfoList = m_dlInfoListReceived;
      // empty local buffer
      m_dlInfoListReceived.clear ();
    }

  m_schedSapProvider->SchedDlTriggerReq (dlparams);


  // --- UPLINK ---
  // Send UL-CQI info to the scheduler
  std::vector <FfMacSchedSapProvider::SchedUlCqiInfoReqParameters>::iterator itCqi;
  for (uint16_t i = 0; i < m_ulCqiReceived.size (); i++)
    {
      if (subframeNo > 1)
        {        
          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & (subframeNo - 1));
        }
      else
        {
          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & (frameNo - 1)) << 4) | (0xF & 10);
        }
      m_schedSapProvider->SchedUlCqiInfoReq (m_ulCqiReceived.at (i));
    }
    m_ulCqiReceived.clear ();
  
  // Send BSR reports to the scheduler
  if (m_ulCeReceived.size () > 0)
    {
      FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters ulMacReq;
      ulMacReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
      ulMacReq.m_macCeList.insert (ulMacReq.m_macCeList.begin (), m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_ulCeReceived.erase (m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_schedSapProvider->SchedUlMacCtrlInfoReq (ulMacReq);
    }


  // Get uplink transmission opportunities
  uint32_t ulSchedFrameNo = m_frameNo;
  uint32_t ulSchedSubframeNo = m_subframeNo;
  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
  if (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY) > 10)
    {
      ulSchedFrameNo++;
      ulSchedSubframeNo = (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY)) % 10;
    }
  else
    {
      ulSchedSubframeNo = ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY);
    }
  FfMacSchedSapProvider::SchedUlTriggerReqParameters ulparams;
  ulparams.m_sfnSf = ((0x3FF & ulSchedFrameNo) << 4) | (0xF & ulSchedSubframeNo);

  // Forward DL HARQ feebacks collected during last TTI
  if (m_ulInfoListReceived.size () > 0)
    {
     ulparams.m_ulInfoList = m_ulInfoListReceived;
      // empty local buffer
      m_ulInfoListReceived.clear ();
    }

  m_schedSapProvider->SchedUlTriggerReq (ulparams);

}

//for nb-iot
// frameNo and subframeNo stand for current frame number
// n_frameNo and n_subframeNo stand for next frame number
// R stand for DCI repetition 
// n stand for counting n-th DCI
static int n = 1;
void
LteEnbMac::DoSearchSpace(uint32_t frameNo, uint32_t subframeNo, uint16_t rnti, int R)
{
  double tti = 0.00101;
  uint32_t n_frameNo, n_subframeNo;

  if(subframeNo == 10){
    n_frameNo = frameNo+1;
    n_subframeNo = 1;
  }
  else{
    n_frameNo = frameNo;
    n_subframeNo = subframeNo+1;
  }

  // cannot schedule in subframe = 1 (NS3) due to MIB
  // cannot schedule in subframe = 6 (NS3) due to NPSS
  // cannot schedule in subframe = 10 and odd frameNo (NS3) due to NSSS
  if(subframeNo == 1)
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace,
                       this,
                       n_frameNo,
                       n_subframeNo,
                       rnti,
                       R);
    npdcch_mib++;
    npdcch_subframe++;
  }
  else if(subframeNo == 6)
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace,
                       this,
                       n_frameNo,
                       n_subframeNo,
                       rnti,
                       R);
    npdcch_npss++;
    npdcch_subframe++;
  }
  else if(subframeNo == 10 && (frameNo-1) % 2 == 0)
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace,
                       this,
                       n_frameNo,
                       n_subframeNo,
                       rnti,
                       R);
    npdcch_nsss++;
    npdcch_subframe++;
  }
  else if(subframeNo == 5 && frameNo == sib1_frameNo)
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace,
                       this,
                       n_frameNo,
                       n_subframeNo,
                       rnti,
                       R);
    npdcch_sib1++;
    npdcch_subframe++;
  }
  else
  {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "|  DCI(" << n << ") for RNTI: "  << rnti << " in frame " << frameNo << " subframe " << subframeNo << "  |" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    n++;
    npdcch_dci++;
    npdcch_subframe++;

    // if R = 1 mean that then current UE DCI scheduling is done
    // CCtrigger = ture for scheduling DCI for next UE
    if(R == 1)
    {
      n=1;
      i_ue++;
      i_offset++;
      if(i_offset==4)
      i_offset=0;
      CCtrigger = true;
      m_kb.push_back(10*frameNo+subframeNo);

      // for nb-iot
      // NPDCCH schedule 4 or less UEs in a NPDCCH period, scaleble 
      // scheduling 4 UEs one time show the best performance of resource ultilization
      // first erase scheduled 4 or less UEs in rnti table
      // than we have to close NPDCCH and start NPDSCH
      if(i_ue == int(m_rnti.size()))
      {
        std::cout << "i_ue: " << i_ue << std::endl;
        std::cout << "m_rnti size: " << int(m_rnti.size()) << std::endl;
        for(int i = 0;i<i_ue; i++)
        {
          // empty rnti
          m_ue.push_back(m_rnti[0]);
          std::cout << "rnti: " << m_ue[i] << std::endl;
          m_rnti.erase(m_rnti.begin());
          m_rep.erase(m_rep.begin());
          m_startsf.erase(m_startsf.begin());
          m_offset.erase(m_offset.begin());
        }

        std::cout << "------------NPDCCH Statistical Results------------" << std::endl;
        std::cout << "|UEs            |" << i_ue << std::endl;
        std::cout << "|MIB-NB         |" << npdcch_mib << std::endl;
        std::cout << "|SIB1-NB        |" << npdcch_sib1 << std::endl;
        std::cout << "|NPSS           |" << npdcch_npss << std::endl;
        std::cout << "|NSSS           |" << npdcch_nsss << std::endl;
        std::cout << "|DCI            |" << npdcch_dci << std::endl;
        std::cout << "|Used subframes |" << npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci << std::endl;
        std::cout << "|Total subframes|" << npdcch_subframe << std::endl;
        std::cout << "|Efficiency     |" << ((double)(npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci)/(double)npdcch_subframe)*100 << "%" << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        npdcch_mib = npdcch_sib1 = npdcch_npss = npdcch_nsss = npdcch_dci = npdcch_subframe = 0;
        sch_ue = i_ue;
        i_offset = 0;
        i_ue = 0;
        SCtrigger = true;
        DoPreAllocation();
      }

      // if(i_ue == int(m_rnti.size()) && i_ue < 4)
      // {
      //   std::cout << "i_ue: " << i_ue << std::endl;
      //   std::cout << "m_rnti size: " << int(m_rnti.size()) << std::endl;
      //   for(int i = 0;i<i_ue; i++)
      //   {
      //     // empty rnti
      //     m_ue.push_back(m_rnti[0]);
      //     std::cout << "rnti: " << m_ue[i] << std::endl;
      //     m_rnti.erase(m_rnti.begin());
      //     m_rep.erase(m_rep.begin());
      //     m_startsf.erase(m_startsf.begin());
      //     m_offset.erase(m_offset.begin());
      //   }

      //   std::cout << "------------NPDCCH Statistical Results------------" << std::endl;
      //   std::cout << "|UEs            |" << i_ue << std::endl;
      //   std::cout << "|MIB-NB         |" << npdcch_mib << std::endl;
      //   std::cout << "|SIB1-NB        |" << npdcch_sib1 << std::endl;
      //   std::cout << "|NPSS           |" << npdcch_npss << std::endl;
      //   std::cout << "|NSSS           |" << npdcch_nsss << std::endl;
      //   std::cout << "|DCI            |" << npdcch_dci << std::endl;
      //   std::cout << "|Used subframes |" << npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci << std::endl;
      //   std::cout << "|Total subframes|" << npdcch_subframe << std::endl;
      //   std::cout << "|Efficiency     |" << ((double)(npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci)/(double)npdcch_subframe)*100 << "%" << std::endl;
      //   std::cout << "--------------------------------------------------" << std::endl;
      //   npdcch_mib = npdcch_sib1 = npdcch_npss = npdcch_nsss = npdcch_dci = npdcch_subframe = 0;
      //   sch_ue = i_ue;
      //   i_offset = 0;
      //   i_ue = 0;
      //   SCtrigger = true;
      //   DoPreAllocation();
      // }
      // else if(i_ue == 4)
      // {
      //   std::cout << "i_ue: " << i_ue << std::endl;
      //   std::cout << "m_rnti size: " << int(m_rnti.size()) << std::endl;
      //   for(int i = 0;i<i_ue; i++)
      //   {
      //     // empty rnti
      //     m_ue.push_back(m_rnti[0]);
      //     std::cout << "rnti: " << m_ue[i] << std::endl;
      //     m_rnti.erase(m_rnti.begin());
      //     m_rep.erase(m_rep.begin());
      //     m_startsf.erase(m_startsf.begin());
      //     m_offset.erase(m_offset.begin());
      //   }

      //   std::cout << "------------NPDCCH Statistical Results------------" << std::endl;
      //   std::cout << "|UEs            |" << i_ue << std::endl;
      //   std::cout << "|MIB-NB         |" << npdcch_mib << std::endl;
      //   std::cout << "|SIB1-NB        |" << npdcch_sib1 << std::endl;
      //   std::cout << "|NPSS           |" << npdcch_npss << std::endl;
      //   std::cout << "|NSSS           |" << npdcch_nsss << std::endl;
      //   std::cout << "|DCI            |" << npdcch_dci << std::endl;
      //   std::cout << "|Used subframes |" << npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci << std::endl;
      //   std::cout << "|Total subframes|" << npdcch_subframe << std::endl;
      //   std::cout << "|Efficiency     |" << ((double)(npdcch_mib+npdcch_sib1+npdcch_npss+npdcch_nsss+npdcch_dci)/(double)npdcch_subframe)*100 << "%" << std::endl;
      //   std::cout << "--------------------------------------------------" << std::endl;
      //   npdcch_mib = npdcch_sib1 = npdcch_npss = npdcch_nsss = npdcch_dci = npdcch_subframe = 0;
      //   sch_ue = i_ue;
      //   i_offset = 0;
      //   i_ue = 0;
      //   SCtrigger = true;
      //   DoPreAllocation();
      // }
    }
    else
    {
      Simulator::Schedule (Seconds (tti),
                        &LteEnbMac::DoSearchSpace,
                        this,
                        n_frameNo,
                        n_subframeNo,
                        rnti,
                        R-1);
    }
  }
}

// for nb-iot
// the function for calculating scheduling delay of UEs
void
LteEnbMac::DoPreAllocation()
{
  NS_LOG_FUNCTION (this);
  int ue = int(m_ue.size());
  std::cout << "UE: " << ue << std::endl;
  int delay1 = m_kb[ue-1]-m_kb[0]; // delay1 for the calculating the scheduling delay of the first UE
  int k0[4] = {8, 16, 32, 64}; // NPUSCH scheduling delay candidates
  int k1[8] = {0, 4, 8, 12, 16, 32, 64, 128}; // NPDSCH scheduling delay candidates
  int i;
  switch (channel)
  {
  case 0:
    i = 0;   
    if(delay1 < k0[i])
    {
      k0[i] = k0[0];
    }
    else
    {
      while(delay1 >= k0[i])
      {
        i++;
      }
    }

    if(k0[i]>64)
    {
      std::cout<< "No fitted k (scheduling delay)" << std::endl;
      break;
    }
    m_k.push_back(k0[i]);

    for(int j=1;j<ue;j++)
    {
      int i=0;
      while( (m_kb[j-1]+m_k[j-1]) >= (m_kb[j]+k0[i]) )
      {
        i++;
      }
      if(k0[i]>64)
      {
        break;
      }
      m_k.push_back(k0[i]);
    }
    for (int k=0;k<ue;k++)
    {
      std::cout << "RNTI: "  << m_ue[k] << " scheduling delay k: " << m_k[k]  << " kb: " << m_kb[k] << std::endl;
    }
    break;
  
  case 1:
    i = 0;
    if(delay1 < k1[i]+5)
    {
      k1[i] = k1[0];
    }
    else
    {
      while(delay1 >= k1[i]+5)
      {
        i++;
      }
    }
    if(k1[i]>128)
    {
      std::cout<< "No fitted k (scheduling delay)" << std::endl;
      break;
    }
    m_k.push_back(k1[i]);
    for(int j=1;j<ue;j++)
    {
      int i=0;
      while( (m_kb[j-1]+m_k[j-1]+5) >= (m_kb[j]+k1[i]+5) )
      {
        i++;
      }
      if(k1[i]>128)
      {
        break;
      }
      m_k.push_back(k1[i]);
    }
    for (int k=0;k<ue;k++)
    {
      std::cout << "RNTI: "  << m_ue[k] << " scheduling delay k: " << m_k[k] << " kb: " << m_kb[k] << std::endl;
    }
    break;

  default:
    break;
  }
}

void
LteEnbMac::DoSearchSpace_msg4(uint32_t frameNo, uint32_t subframeNo, int u, int R)
{
  double tti = 0.001;
  uint32_t ss_frameNo, ss_subframeNo;
  uint32_t n_frameNo, n_subframeNo;

  if(subframeNo == 10){
    ss_frameNo = frameNo+1;
    ss_subframeNo = 1;
  }
  else{
    ss_frameNo = frameNo;
    ss_subframeNo = subframeNo+1;
  }

  if(subframeNo+R > 10){
    n_subframeNo = (subframeNo+R) % 10;
    n_frameNo = frameNo + (subframeNo+R)/10;
  }
  else{
    n_subframeNo = subframeNo+R;
    n_frameNo = frameNo;
  }

  // if u == 0 then DCI scheduling is done
  if(u == 0){
    std::cout << "DCI for msg4 allocated" << std::endl;
  }
  // cannot schedule in subframe = 1 (NS3) due to MIB
  else if((subframeNo == 1))
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace_msg4,
                       this,
                       ss_frameNo,
                       ss_subframeNo,
                       u,
                       R);
  }
  // cannot schedule in subframe = 6 (NS3) due to NPSS
  else if((subframeNo == 6))
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace_msg4,
                       this,
                       ss_frameNo,
                       ss_subframeNo,
                       u,
                       R);
  }
  // cannot schedule in subframe = 10 and odd frameNo (NS3) due to NSSS
  else if((subframeNo == 10) && ((frameNo % 2) == 1))
  {
    Simulator::Schedule (Seconds (tti),
                       &LteEnbMac::DoSearchSpace_msg4,
                       this,
                       ss_frameNo,
                       ss_subframeNo,
                       u,
                       R);
  }
  else{
    // std::cout << "------------------------------------------" << std::endl;
    // std::cout << "|  DCI for msg4 "  << " in frame " << frameNo << " subframe " << subframeNo << "  |" << std::endl;
    // std::cout << "------------------------------------------" << std::endl;

    Simulator::Schedule (Seconds (tti*R),
                       &LteEnbMac::DoSearchSpace_msg4,
                       this,
                       n_frameNo,
                       n_subframeNo,
                       u-1,
                       R);
  }
}

static uint16_t msg4_rep;
static uint16_t msg4_startsf;
static uint16_t msg4_offset;

//for nb-iot
// this function for calculate which subframe for msg4 first DCI
void
LteEnbMac::DoSearchSpace_pre_msg4(uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this);

  double cond1,cond2;
  int T,R;

  if(msg4_rep == 1 || msg4_rep == 2 || msg4_rep == 4)
      R = 1;
  else
      R = msg4_rep/8;
  
  while(true)
  {
    T = msg4_rep*msg4_startsf;
    cond1 = (10*(frameNo-1)+(subframeNo-1)) % T;
    cond2 = msg4_offset*T;

    // std::cout << "u_frame " << u_frameNo << std::endl;
    // std::cout << "u_subframe " << u_subframeNo << std::endl;
    // std::cout << "cond1 " << cond1 << std::endl;
    // std::cout << "cond2 " << cond2 << std::endl;

    if(cond1 == cond2)
    {
      // Do search space is a function for real schedule DCI
      // we have to sync dci_frameNo and u_frameNo(universal frameNo stand for current frameNo)
      int sync_time;
      sync_time = (10*frameNo+subframeNo)-(10*u_frameNo+u_subframeNo);
      Simulator::Schedule (MilliSeconds (sync_time),
                       &LteEnbMac::DoSearchSpace_msg4,
                       this,
                       frameNo,
                       subframeNo,
                       R,
                       R);
      //DoSearchSpace(frameNo, subframeNo, R, R, false);
      std::cout << "DCI N1 (msg4) repetition times: " << R << std::endl;

      break;
    }
    else
    {
      if(subframeNo == 10){
        frameNo = frameNo+1;
        subframeNo = 1;
      }
      else{
        subframeNo = subframeNo+1;
      }
      //std::cout << "delay" << std::endl;
    }
    
  }
}

//for nb-iot
// when enb-rrc receive msg3 would trigger this
// and input some parameter and frameNo
void
LteEnbMac::Sendmsg4Info(uint16_t rep, uint16_t startsf, double offset)
{
  msg4_rep = rep;
  msg4_startsf = startsf;
  msg4_offset = offset;
  DoSearchSpace_pre_msg4(sch_frameNo, sch_subframeNo);
}

//for nb-iot
// when enb-rrc receive msg3 would trigger this
// this function calculate delay for what time enb-rrc send msg4
// sch_frameNo and sch_subfrmaeNo stand for previous UE last allocated frameNo
// so the frame between and u_frame and sch_frame cannot allocate resources 
int
LteEnbMac::DoConfigureRrcDelay(uint16_t rep, uint16_t startsf, double offset)
{
  NS_LOG_FUNCTION (this);
  uint32_t frameNo, subframeNo;
  uint32_t last_dci_frameNo, last_dci_subframeNo;
  double cond1,cond2;
  int dci_delay, dci_time;
  int T,R;
  dci_time = 0;
  frameNo = sch_frameNo;
  subframeNo = sch_subframeNo;
  T = msg4_rep*msg4_startsf;

  if(msg4_rep == 1 || msg4_rep == 2 || msg4_rep == 4)
    R = 1;
  else
    R = msg4_rep/8;

  while(true)
  {
    cond1 = (10*(frameNo-1)+(subframeNo-1)) % T;
    cond2 = msg4_offset*T;

    if(cond1 == cond2)
    {
      int i = 0;
      last_dci_frameNo = frameNo;
      last_dci_subframeNo = subframeNo;

      dci_delay = (10*frameNo+subframeNo)-(10*u_frameNo+u_subframeNo);
      
      //std::cout << "(" << last_dci_frameNo << "," << last_dci_subframeNo << ")" << std::endl;

      while(true)
      {
        if(last_dci_subframeNo == 1 || last_dci_subframeNo == 6 || (last_dci_subframeNo == 10 && last_dci_frameNo%2 == 1))
        {
          if(last_dci_subframeNo + 1 > 10)
          {
            last_dci_subframeNo = 1;
            last_dci_frameNo = last_dci_frameNo + 1;
          }
          else
          {
            last_dci_subframeNo = last_dci_subframeNo + 1;
          }
        }
        else
        {
          i++;
          if(i == R)
          {
            break;
          }
          if(last_dci_subframeNo + R > 10)
          {
            last_dci_frameNo = last_dci_frameNo+1;
            last_dci_subframeNo = last_dci_subframeNo+R-10;
          }
          else
          {
            last_dci_subframeNo = last_dci_subframeNo + R;
          }
        }
      }

      //std::cout << "(" << last_dci_frameNo << "," << last_dci_subframeNo << ")" << std::endl;

      dci_time = (10*last_dci_frameNo + last_dci_subframeNo) - (10*frameNo + subframeNo);
      break;
    }
    else
    {
      if(subframeNo == 10){
        frameNo = frameNo+1;
        subframeNo = 1;
      }
      else{
        subframeNo = subframeNo+1;
      }
    }
  }

  DCI_N1 dci;
  dci = DoConfigureDCI_N1(3, false);
  //for nb-iot 
  // next ue must wait the TIME between recv msg3(10*u_frameNo+u_subframeNo) and pervious UE send last harq feedback 
  sch_frameNo = ((10*u_frameNo + u_subframeNo) + (dci_delay + dci_time) + 5 + dci.Scheduling_delay + (dci.Resource_assignment*dci.Repetition_number) + 13 - 1 + (4*2/2) + 1) /10;
  sch_subframeNo = ((10*u_frameNo + u_subframeNo) + (dci_delay + dci_time) + 5 + dci.Scheduling_delay + (dci.Resource_assignment*dci.Repetition_number) + 13 - 1 + (4*2/2) + 1) %10;
  std::cout << "msg3 coming frame: (" << u_frameNo << "," << u_subframeNo << ")" << std::endl;
  std::cout << "last DCI frame: (" << last_dci_frameNo << "," << last_dci_subframeNo << ")" << std::endl;
  std::cout << "msg4 last scheduled frame: (" << sch_frameNo << "," << sch_subframeNo << ")" << std::endl;

  return dci_delay+dci_time;
}

std::vector<int>
LteEnbMac::DoConfigureMsg4Info()
{
  NS_LOG_FUNCTION (this);
  DCI_N1 dci;
  std::vector<int> msg4_info;

  dci = DoConfigureDCI_N1(3, false);
  msg4_info.push_back(dci.Scheduling_delay);
  msg4_info.push_back(dci.Resource_assignment);
  msg4_info.push_back(dci.Repetition_number);

  return msg4_info;
}

//for nb-iot
LteEnbMac::DCI_N0
LteEnbMac::DoConfigureDCI_N0(int CE)
{
  DCI_N0 dci;
  if(CE == 0)
  {
    dci.Scheduling_delay = 8;
    dci.Resource_assignment = 2;
    dci.Repetition_number = 2;
  }
  else if(CE == 1)
  {
    dci.Scheduling_delay = 16;
    dci.Resource_assignment = 4;
    dci.Repetition_number = 8;
  }
  else if(CE == 2)
  {
    dci.Scheduling_delay = 32;
    dci.Resource_assignment = 6;
    dci.Repetition_number = 32;
  }

  return dci;
}

//for nb-iot
// 3 for msg4 , other normal 
LteEnbMac::DCI_N1
LteEnbMac::DoConfigureDCI_N1(int CE, bool flag)
{
  DCI_N1 dci;
  if(CE == 3 && flag == false)
  {
    dci.Scheduling_delay = 0;
    dci.Resource_assignment = 2;
    dci.Repetition_number = 4;
  }

  return dci;
}

void
LteEnbMac::SIB1Indication (uint32_t m_nrFrames)
{
  NS_LOG_FUNCTION (this);
  sib1_frameNo = m_nrFrames;
  std::cout << "SIB1 FrameNo: " << sib1_frameNo << std::endl;
}

//for nb-iot
//senduepara would do multiple times no how
//so we have to check whether there is repeat rnti 
void
LteEnbMac::SendUePara(uint16_t rnti, int rep, int startsf, double offset)
{
  int c = 0;
  NS_LOG_FUNCTION (this);
  for(int i=0; i<int(m_rnti.size()); i++){
    if(m_rnti[i] == rnti){
      c = 1;
      std::cout << "RNTI: " << m_rnti[i] << " existed..." << std::endl;
    }
  }
  std::cout << "rnti: " << rnti << " rep: " << rep << " startsf: " << startsf << " offset: " << offset << std::endl;
  if(c == 0){
    m_rnti.push_back(rnti);
    m_rep.push_back(rep);
    m_startsf.push_back(startsf);
    m_offset.push_back(offset);
    // check whether there is rnti in the queue 
    // if not empty, trigger the scheduler for search space
    std::cout << "rnti: " << rnti << " Queuing..." << std::endl;
    if(int(m_rnti.size()) == 1)
    {
      CCtrigger = true;
    }
  }
}

void
LteEnbMac::SetChannel(int ch)
{
   NS_LOG_FUNCTION (this);
   channel = ch;
}

void
LteEnbMac::SetRepetition(int R)
{
  NS_LOG_FUNCTION (this);
  m_R = R;
}

void
LteEnbMac::SetAlpha(int alpha)
{
  NS_LOG_FUNCTION (this);
  if(alpha == 0)
  {
    offset[0] = (double)0;
    offset[1] = (double)1/8;
    offset[2] = (double)2/8;
    offset[3] = (double)3/8;
  }
  else
  {
    offset[0] = (double)0;
    offset[1] = (double)0;
    offset[2] = (double)0;
    offset[3] = (double)0;
  }
}

void
LteEnbMac::DoReceiveLteControlMessage  (Ptr<LteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);
  if (msg->GetMessageType () == LteControlMessage::DL_CQI)
    {
      Ptr<DlCqiLteControlMessage> dlcqi = DynamicCast<DlCqiLteControlMessage> (msg);
      ReceiveDlCqiLteControlMessage (dlcqi);
    }
  else if (msg->GetMessageType () == LteControlMessage::BSR)
    {
      Ptr<BsrLteControlMessage> bsr = DynamicCast<BsrLteControlMessage> (msg);
      ReceiveBsrMessage (bsr->GetBsr ());
    }
  else if (msg->GetMessageType () == LteControlMessage::DL_HARQ)
    {
      Ptr<DlHarqFeedbackLteControlMessage> dlharq = DynamicCast<DlHarqFeedbackLteControlMessage> (msg);
      DoDlInfoListElementHarqFeeback (dlharq->GetDlHarqFeedback ());
    }
  else
    {
      NS_LOG_LOGIC (this << " LteControlMessage type " << msg->GetMessageType () << " not recognized");
    }
}

void
LteEnbMac::DoReceiveRachPreamble  (uint8_t rapId)
{
  NS_LOG_FUNCTION (this << (uint32_t) rapId);
  // just record that the preamble has been received; it will be processed later
  ++m_receivedRachPreambleCount[rapId]; // will create entry if not exists
}

void
LteEnbMac::DoUlCqiReport (FfMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi)
{ 
  if (ulcqi.m_ulCqi.m_type == UlCqi_s::PUSCH)
    {
      NS_LOG_DEBUG (this << " eNB rxed an PUSCH UL-CQI");
    }
  else if (ulcqi.m_ulCqi.m_type == UlCqi_s::SRS)
    {
      NS_LOG_DEBUG (this << " eNB rxed an SRS UL-CQI");
    }
  m_ulCqiReceived.push_back (ulcqi);
}


void
LteEnbMac::ReceiveDlCqiLteControlMessage  (Ptr<DlCqiLteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);

  CqiListElement_s dlcqi = msg->GetDlCqi ();
  NS_LOG_LOGIC (this << "Enb Received DL-CQI rnti" << dlcqi.m_rnti);
  NS_ASSERT (dlcqi.m_rnti != 0);
  m_dlCqiReceived.push_back (dlcqi);

}


void
LteEnbMac::ReceiveBsrMessage  (MacCeListElement_s bsr)
{
  NS_LOG_FUNCTION (this);

  m_ulCeReceived.push_back (bsr);
}


void
LteEnbMac::DoReceivePhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag;
  p->RemovePacketTag (tag);

  // store info of the packet received

//   std::map <uint16_t,UlInfoListElement_s>::iterator it;
//   u_int rnti = tag.GetRnti ();
//  u_int lcid = tag.GetLcid ();
//   it = m_ulInfoListElements.find (tag.GetRnti ());
//   if (it == m_ulInfoListElements.end ())
//     {
//       // new RNTI
//       UlInfoListElement_s ulinfonew;
//       ulinfonew.m_rnti = tag.GetRnti ();
//       // always allocate full size of ulReception vector, initializing all elements to 0
//       ulinfonew.m_ulReception.assign (MAX_LC_LIST+1, 0);
//       // set the element for the current LCID
//       ulinfonew.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       ulinfonew.m_receptionStatus = UlInfoListElement_s::Ok;
//       ulinfonew.m_tpc = 0; // Tx power control not implemented at this stage
//       m_ulInfoListElements.insert (std::pair<uint16_t, UlInfoListElement_s > (tag.GetRnti (), ulinfonew));
// 
//     }
//   else
//     {
//       // existing RNTI: we just set the value for the current
//       // LCID. Note that the corresponding element had already been
//       // allocated previously.
//       NS_ASSERT_MSG ((*it).second.m_ulReception.at (tag.GetLcid ()) == 0, "would overwrite previously written ulReception element");
//       (*it).second.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       (*it).second.m_receptionStatus = UlInfoListElement_s::Ok;
//     }



  // forward the packet to the correspondent RLC
  uint16_t rnti = tag.GetRnti ();
  uint8_t lcid = tag.GetLcid ();
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
  //NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);

  //Receive PDU only if LCID is found
  if (lcidIt != rntiIt->second.end ())
    {
      (*lcidIt).second->ReceivePdu (p);
    }
}



// ////////////////////////////////////////////
// CMAC SAP
// ////////////////////////////////////////////

void
LteEnbMac::DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
  FfMacCschedSapProvider::CschedCellConfigReqParameters params;
  // Configure the subset of parameters used by FfMacScheduler
  params.m_ulBandwidth = ulBandwidth;
  params.m_dlBandwidth = dlBandwidth;
  m_macChTtiDelay = m_enbPhySapProvider->GetMacChTtiDelay ();
  std::cout << "[myprint] TTI: " << int(m_macChTtiDelay) << std::endl;
  // ...more parameters can be configured
  m_cschedSapProvider->CschedCellConfigReq (params);
}


void
LteEnbMac::DoAddUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  std::map<uint8_t, LteMacSapUser*> empty;
  std::pair <std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator, bool> 
    ret = m_rlcAttached.insert (std::pair <uint16_t,  std::map<uint8_t, LteMacSapUser*> > 
                                (rnti, empty));
  NS_ASSERT_MSG (ret.second, "element already present, RNTI already existed");

  FfMacCschedSapProvider::CschedUeConfigReqParameters params;
  params.m_rnti = rnti;
  params.m_transmissionMode = 0; // set to default value (SISO) for avoiding random initialization (valgrind error)

  m_cschedSapProvider->CschedUeConfigReq (params);

  // Create DL trasmission HARQ buffers
  std::vector < Ptr<PacketBurst> > dlHarqLayer0pkt;
  dlHarqLayer0pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer0pkt.at (i) = pb;
    }
  std::vector < Ptr<PacketBurst> > dlHarqLayer1pkt;
  dlHarqLayer1pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer1pkt.at (i) = pb;
    }
  DlHarqProcessesBuffer_t buf;
  buf.push_back (dlHarqLayer0pkt);
  buf.push_back (dlHarqLayer1pkt);
  m_miDlHarqProcessesPackets.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf));
}

void
LteEnbMac::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  FfMacCschedSapProvider::CschedUeReleaseReqParameters params;
  params.m_rnti = rnti;
  m_cschedSapProvider->CschedUeReleaseReq (params);
  m_rlcAttached.erase (rnti);
  m_miDlHarqProcessesPackets.erase (rnti);
}

void
LteEnbMac::DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);

  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;
  
  LteFlowId_t flow (lcinfo.rnti, lcinfo.lcId);
  
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (lcinfo.rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "RNTI not found");
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcinfo.lcId);
  if (lcidIt == rntiIt->second.end ())
    {
      rntiIt->second.insert (std::pair<uint8_t, LteMacSapUser*> (lcinfo.lcId, msu));
    }
  else
    {
      NS_LOG_ERROR ("LC already exists");
    }

  // CCCH (LCID 0) is pre-configured 
  // see FF LTE MAC Scheduler
  // Interface Specification v1.11, 
  // 4.3.4 logicalChannelConfigListElement
  if (lcinfo.lcId != 0)
    {
      struct FfMacCschedSapProvider::CschedLcConfigReqParameters params;
      params.m_rnti = lcinfo.rnti;
      params.m_reconfigureFlag = false;

      struct LogicalChannelConfigListElement_s lccle;
      lccle.m_logicalChannelIdentity = lcinfo.lcId;
      lccle.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle.m_direction = LogicalChannelConfigListElement_s::DIR_BOTH;
      lccle.m_qosBearerType = lcinfo.isGbr ? LogicalChannelConfigListElement_s::QBT_GBR : LogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle.m_qci = lcinfo.qci;
      lccle.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params.m_logicalChannelConfigList.push_back (lccle);

      m_cschedSapProvider->CschedLcConfigReq (params);
    }
}

void
LteEnbMac::DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo)
{
  NS_FATAL_ERROR ("not implemented");
}

void
LteEnbMac::DoReleaseLc (uint16_t rnti, uint8_t lcid)
{
  NS_LOG_FUNCTION (this);

  //Find user based on rnti and then erase lcid stored against the same
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  rntiIt->second.erase (lcid);

  struct FfMacCschedSapProvider::CschedLcReleaseReqParameters params;
  params.m_rnti = rnti;
  params.m_logicalChannelIdentity.push_back (lcid);
  m_cschedSapProvider->CschedLcReleaseReq (params);
}

void
LteEnbMac::DoUeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  FfMacCschedSapProvider::CschedUeConfigReqParameters req;
  req.m_rnti = params.m_rnti;
  req.m_transmissionMode = params.m_transmissionMode;
  req.m_reconfigureFlag = true;
  m_cschedSapProvider->CschedUeConfigReq (req);
}

LteEnbCmacSapProvider::RachConfig 
LteEnbMac::DoGetRachConfig ()
{
  struct LteEnbCmacSapProvider::RachConfig rc;
  rc.numberOfRaPreambles = m_numberOfRaPreambles;
  rc.preambleTransMax = m_preambleTransMax;
  rc.raResponseWindowSize = m_raResponseWindowSize;

  //for NB-IoT
  //for repetitions
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.numRepetitionsPerPreambleAttempt_r13 = 2;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.numRepetitionsPerPreambleAttempt_r13 = 8;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.numRepetitionsPerPreambleAttempt_r13 = 32;
  //for maxTransmissionNum
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.maxNumPreambleAttempt_r13 = 3;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.maxNumPreambleAttempt_r13 = 3;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.maxNumPreambleAttempt_r13 = 3;
  //for periodicity
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.periodicity_r13 = 40;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.periodicity_r13 = 160;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.periodicity_r13 = 640;
  //for startTime
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.startTime_r13 = 8;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.startTime_r13 = 32;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.startTime_r13 = 256;

  //for NPDCCH-numRepetitions-RA
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.npdcch_numRepetitions_RA_r13 = 4;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.npdcch_numRepetitions_RA_r13 = 32;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.npdcch_numRepetitions_RA_r13 = 256;

  //for NPDCCH-startSF-CSS-RA
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_0.npdcch_StartSF_CSS_RA_r13 = 2;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_1.npdcch_StartSF_CSS_RA_r13 = 2;
  rc.nprachConfig.nprach_ConfigSIB.nprach_ParametersList.CE_2.npdcch_StartSF_CSS_RA_r13 = 2;

  //for NRSRP thresholds
  rc.nprachConfig.nprach_ConfigSIB.rsrp_ThresholdsPrachInfoList.NRSRP_thresholds_first_value = 3;
  rc.nprachConfig.nprach_ConfigSIB.rsrp_ThresholdsPrachInfoList.NRSRP_thresholds_second_value = 13;

  return rc;
}

//for nb-iot
LteEnbCmacSapProvider::NpdcchConfig
LteEnbMac::DoGetNpdcchConfig ()
{
  struct LteEnbCmacSapProvider::NpdcchConfig nc;
  
  nc.npdcch_ParametersList.CELV0.npdcch_NumRepetitions = 4;
  nc.npdcch_ParametersList.CELV1.npdcch_NumRepetitions = 32;
  nc.npdcch_ParametersList.CELV2.npdcch_NumRepetitions = 256;
  
  nc.npdcch_ParametersList.CELV0.npdcch_StartSF_USS = 2;
  nc.npdcch_ParametersList.CELV1.npdcch_StartSF_USS = 2;
  nc.npdcch_ParametersList.CELV2.npdcch_StartSF_USS = 2;

  nc.npdcch_ParametersList.CELV0.npdcch_Offset_USS = 0;
  nc.npdcch_ParametersList.CELV1.npdcch_Offset_USS = 0;
  nc.npdcch_ParametersList.CELV2.npdcch_Offset_USS = 0;

  return nc;
}
 
LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue 
LteEnbMac::DoAllocateNcRaPreamble (uint16_t rnti)
{
  bool found = false;
  uint8_t preambleId;
  for (preambleId = m_numberOfRaPreambles; preambleId < 64; ++preambleId)
    {
      std::map<uint8_t, NcRaPreambleInfo>::iterator it = m_allocatedNcRaPreambleMap.find (preambleId);
      if ((it ==  m_allocatedNcRaPreambleMap.end ())
          || (it->second.expiryTime < Simulator::Now ()))
        {
          found = true;
          NcRaPreambleInfo preambleInfo;
          uint32_t expiryIntervalMs = (uint32_t) m_preambleTransMax * ((uint32_t) m_raResponseWindowSize + 5); 
          
          preambleInfo.expiryTime = Simulator::Now () + MilliSeconds (expiryIntervalMs);
          preambleInfo.rnti = rnti;
          NS_LOG_INFO ("allocated preamble for NC based RA: preamble " << preambleId << ", RNTI " << preambleInfo.rnti << ", exiryTime " << preambleInfo.expiryTime);
          m_allocatedNcRaPreambleMap[preambleId] = preambleInfo; // create if not exist, update otherwise
          break;
        }
    }
  LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue ret;
  if (found)
    {
      ret.valid = true;
      ret.raPreambleId = preambleId;
      ret.raPrachMaskIndex = 0;
    }
  else
    {
      ret.valid = false;
      ret.raPreambleId = 0;
      ret.raPrachMaskIndex = 0;
    }
  return ret;
}



// ////////////////////////////////////////////
// MAC SAP
// ////////////////////////////////////////////


void
LteEnbMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag (params.rnti, params.lcid, params.layer);
  params.pdu->AddPacketTag (tag);
  // Store pkt in HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  NS_LOG_DEBUG (this << " LAYER " << (uint16_t)tag.GetLayer () << " HARQ ID " << (uint16_t)params.harqProcessId);
  
  //(*it).second.at (params.layer).at (params.harqProcessId) = params.pdu;//->Copy ();
  (*it).second.at (params.layer).at (params.harqProcessId)->AddPacket (params.pdu);
  m_enbPhySapProvider->SendMacPdu (params.pdu);
}

void
LteEnbMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this);
  FfMacSchedSapProvider::SchedDlRlcBufferReqParameters req;
  req.m_rnti = params.rnti;
  req.m_logicalChannelIdentity = params.lcid;
  req.m_rlcTransmissionQueueSize = params.txQueueSize;
  req.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  req.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  req.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  req.m_rlcStatusPduSize = params.statusPduSize;
  m_schedSapProvider->SchedDlRlcBufferReq (req);
}



// ////////////////////////////////////////////
// SCHED SAP
// ////////////////////////////////////////////



void
LteEnbMac::DoSchedDlConfigInd (FfMacSchedSapUser::SchedDlConfigIndParameters ind)
{
  NS_LOG_FUNCTION (this);
  // Create DL PHY PDU
  Ptr<PacketBurst> pb = CreateObject<PacketBurst> ();
  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;

  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
    {
      for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
        {
          if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
            {
              // new data -> force emptying correspondent harq pkt buffer
              std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
              NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
              for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
                {
                  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                  (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
                }
            }
        }
      for (unsigned int j = 0; j < ind.m_buildDataList.at (i).m_rlcPduList.size (); j++)
        {
          for (uint16_t k = 0; k < ind.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
            {
              if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
                {
                  // New Data -> retrieve it from RLC
                  uint16_t rnti = ind.m_buildDataList.at (i).m_rnti;
                  uint8_t lcid = ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
                  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
                  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
                  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
                  NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);
                  NS_LOG_DEBUG (this << " rnti= " << rnti << " lcid= " << (uint32_t) lcid << " layer= " << k);
                  (*lcidIt).second->NotifyTxOpportunity (ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                }
              else
                {
                  if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
                    {
                      // HARQ retransmission -> retrieve TB from HARQ buffer
                      std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
                      NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
                      Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                      for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                        {
                          Ptr<Packet> pkt = (*j)->Copy ();
                          m_enbPhySapProvider->SendMacPdu (pkt);
                        }
                    }
                }
            }
        }
      // send the relative DCI
      Ptr<DlDciLteControlMessage> msg = Create<DlDciLteControlMessage> ();
      msg->SetDci (ind.m_buildDataList.at (i).m_dci);
      m_enbPhySapProvider->SendLteControlMessage (msg);
    }

  // Fire the trace with the DL information
  for (  uint32_t i  = 0; i < ind.m_buildDataList.size (); i++ )
    {
      // Only one TB used
      if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
        {
          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                          0, 0
                          );

        }
      // Two TBs used
      else if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
        {
          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (1),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
                          );
        }
      else
        {
          NS_FATAL_ERROR ("Found element with more than two transport blocks");
        }
    }

  // Random Access procedure: send RARs
  Ptr<RarLteControlMessage> rarMsg = Create<RarLteControlMessage> ();
  // see TS 36.321 5.1.4;  preambles were sent two frames ago
  // (plus 3GPP counts subframes from 0, not 1)
  uint16_t raRnti;
  if (m_subframeNo < 3)
    {
      raRnti = m_subframeNo + 7; // equivalent to +10-3
    }
  else
    {
      raRnti = m_subframeNo - 3;
    }
  rarMsg->SetRaRnti (raRnti);
  for (unsigned int i = 0; i < ind.m_buildRarList.size (); i++)
    {
      std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap.find (ind.m_buildRarList.at (i).m_rnti);
      if (itRapId == m_rapIdRntiMap.end ())
        {
          NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildRarList.at (i).m_rnti);
        }
      RarLteControlMessage::Rar rar;
      rar.rapId = itRapId->second;
      rar.rarPayload = ind.m_buildRarList.at (i);
      rarMsg->AddRar (rar);
      NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildRarList.at (i).m_rnti << " rapId " << itRapId->second);
    }
  if (ind.m_buildRarList.size () > 0)
    {
      m_enbPhySapProvider->SendLteControlMessage (rarMsg);
    }
  m_rapIdRntiMap.clear ();
}


void
LteEnbMac::DoSchedUlConfigInd (FfMacSchedSapUser::SchedUlConfigIndParameters ind)
{
  NS_LOG_FUNCTION (this);

  for (unsigned int i = 0; i < ind.m_dciList.size (); i++)
    {
      // send the correspondent ul dci
      Ptr<UlDciLteControlMessage> msg = Create<UlDciLteControlMessage> ();
      msg->SetDci (ind.m_dciList.at (i));
      m_enbPhySapProvider->SendLteControlMessage (msg);
    }

  // Fire the trace with the UL information
  for (  uint32_t i  = 0; i < ind.m_dciList.size (); i++ )
    {
      m_ulScheduling (m_frameNo, m_subframeNo, ind.m_dciList.at (i).m_rnti,
                      ind.m_dciList.at (i).m_mcs, ind.m_dciList.at (i).m_tbSize);
    }



}




// ////////////////////////////////////////////
// CSCHED SAP
// ////////////////////////////////////////////


void
LteEnbMac::DoCschedCellConfigCnf (FfMacCschedSapUser::CschedCellConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeConfigCnf (FfMacCschedSapUser::CschedUeConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedLcConfigCnf (FfMacCschedSapUser::CschedLcConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
  // Call the CSCHED primitive
  // m_cschedSap->LcConfigCompleted();
}

void
LteEnbMac::DoCschedLcReleaseCnf (FfMacCschedSapUser::CschedLcReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeReleaseCnf (FfMacCschedSapUser::CschedUeReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoCschedUeConfigUpdateInd (FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
  // propagates to RRC
  LteEnbCmacSapUser::UeConfig ueConfigUpdate;
  ueConfigUpdate.m_rnti = params.m_rnti;
  ueConfigUpdate.m_transmissionMode = params.m_transmissionMode;
  m_cmacSapUser->RrcConfigurationUpdateInd (ueConfigUpdate);
}

void
LteEnbMac::DoCschedCellConfigUpdateInd (FfMacCschedSapUser::CschedCellConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
LteEnbMac::DoUlInfoListElementHarqFeeback (UlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this);
  m_ulInfoListReceived.push_back (params);
}

void
LteEnbMac::DoDlInfoListElementHarqFeeback (DlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this);
  // Update HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.m_rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  for (uint8_t layer = 0; layer < params.m_harqStatus.size (); layer++)
    {
      if (params.m_harqStatus.at (layer) == DlInfoListElement_s::ACK)
        {
          // discard buffer
          Ptr<PacketBurst> emptyBuf = CreateObject <PacketBurst> ();
          (*it).second.at (layer).at (params.m_harqProcessId) = emptyBuf;
          NS_LOG_DEBUG (this << " HARQ-ACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else if (params.m_harqStatus.at (layer) == DlInfoListElement_s::NACK)
        {
          NS_LOG_DEBUG (this << " HARQ-NACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else
        {
          NS_FATAL_ERROR (" HARQ functionality not implemented");
        }
    }
  m_dlInfoListReceived.push_back (params);
}


} // namespace ns3
