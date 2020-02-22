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
    * Author: Jaume Nin <jaume.nin@cttc.cat>
    * Modified by: Saulo da Mata <damata.saulo@gmail.com>
    */

    #include "ns3/lte-helper.h"
    #include "ns3/epc-helper.h"
    #include "ns3/core-module.h"
    #include "ns3/network-module.h"
    #include "ns3/ipv4-global-routing-helper.h"
    #include "ns3/internet-module.h"
    #include "ns3/mobility-module.h"
    #include "ns3/lte-module.h"
    #include "ns3/applications-module.h"
    #include "ns3/point-to-point-helper.h"
    #include "ns3/config-store.h"

    #include "ns3/header.h"
    #include "ns3/packet.h"
    #include "ns3/buffer.h"
    #include "ns3/type-id.h"

    using namespace ns3;
    using namespace std;

    NS_LOG_COMPONENT_DEFINE ("LteEpc");

    void
    NotifyConnectionEstablishedUe (std::string context,
                                  uint64_t imsi,
                                  uint16_t cellid,
                                  uint16_t rnti)
    {
      std::cout << "at "<<Simulator::Now ().GetSeconds () << "s "<< context
                << " UE IMSI " << imsi
                << ": connected to CellId " << cellid
                << " with RNTI " << rnti
                << std::endl;
    }
    int
    main (int argc, char *argv[])
    {

    //LogComponentEnable ("LteEpc", LOG_LEVEL_INFO);
    //LogComponentEnable ("LteEnbPhy", LOG_LEVEL_ALL);
    //LogComponentEnable ("LteEnbMac", LOG_LEVEL_ALL);
    //LogComponentEnable ("LteHelper", LOG_LEVEL_ALL);
    //LogComponentEnable ("EpcHelper", LOG_LEVEL_ALL);
    //LogComponentEnable ("PfFfMacScheduler", LOG_LEVEL_ALL);
    //LogComponentEnable ("LteEnbRrc", LOG_LEVEL_ALL);
    //LogComponentEnable ("LteUeMac", LOG_LEVEL_ALL);
    LogComponentEnable ("LteUeRrc", LOG_LEVEL_ALL);
    //LogComponentEnable ("LteUePhy", LOG_LEVEL_ALL);
    //LogComponentEnable ("EpcEnbApplication", LOG_LEVEL_ALL);
    //LogComponentEnable ("EpcMme", LOG_LEVEL_ALL);
    //LogComponentEnable ("EpcSgwPgwApplication", LOG_LEVEL_ALL);
    
     //number of UE
    static uint16_t numberOfNodes = 2;
    double simTime = 10;//default is 0.1
    //int radius = 6800;//default is 50
    double interPacketInterval = 10;
    string scheduler = "ns3::PfFfMacScheduler";

    // Command line arguments
    CommandLine cmd;
    cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
    cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
    //cmd.AddValue("radius", "Radius of eNBs [m]", radius);
    cmd.AddValue("interPacketInterval", "Inter ;,abcdehilmoprst- interval [ms])", interPacketInterval);
    cmd.AddValue("Scheduler", "Scheduler", scheduler);
    cmd.Parse(argc, argv);


    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
    Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);
    lteHelper->SetSchedulerType(scheduler);

    //ConfigStore inputConfig;
    //inputConfig.ConfigureDefaults();

    // parse again so you can override default values from the command line
    cmd.Parse(argc, argv);

    Ptr<Node> pgw = epcHelper->GetPgwNode ();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    InternetStackHelper internet;
    internet.Install (remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
    // interface 0 is localhost, 1 is the p2p device

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
    remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

    NodeContainer ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(1);
    ueNodes.Create(numberOfNodes);


    // Install Mobility Model
    Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
    positionAllocEnb->Add (Vector(0, 0, 0));
    MobilityHelper mobilityEnb;
    //mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityEnb.SetPositionAllocator(positionAllocEnb);
    mobilityEnb.Install(enbNodes);

    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    for (uint16_t i = 0; i < numberOfNodes; i++)
        {
         /*
        int random_rad = rand() % radius ;
        int random_alpha = rand() % 360;
        double x = random_rad * cos(random_alpha*3.14159265/180);
        double y = random_rad * sin(random_alpha*3.14159265/180);
        positionAlloc->Add (Vector(x, y, 0));
        */
        // CELV0 -> 3000
        // CELV1 -> 30000
        // CELV2 -> 60000

        positionAlloc->Add (Vector(3000, 0, 0));
        }

    MobilityHelper mobility;
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(ueNodes);

    for (uint16_t i = 0; i < numberOfNodes; i++)
        {
        Ptr<MobilityModel> mob = ueNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        NS_LOG_INFO("UE "<<(i+1)<<" position: x=" << pos.x << " y=" << pos.y);
        }

    // Install LTE Devices to the nodes
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

    // Install the IP stack on the UEs
    internet.Install (ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
    // Assign IP address to UEs, and install applications
    for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
        {
        Ptr<Node> ueNode = ueNodes.Get (u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
        ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
        }

    //set eNB transmission power
    Ptr<LteEnbPhy> enbPhy = enbLteDevs.Get(0)->GetObject<LteEnbNetDevice>()->GetPhy();
    enbPhy->SetTxPower(1.0);

    // Attach one UE per eNodeB
    for (uint16_t i = 0; i < numberOfNodes; i++)
        {
            lteHelper->Attach(ueLteDevs.Get(i));
        }
    
     Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                  MakeCallback (&NotifyConnectionEstablishedUe));
    
    lteHelper->EnableTraces ();

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    Simulator::Destroy();
    return 0;

    }
