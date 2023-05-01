#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
// #include "ns3/point-to-point-module.h"
#include <fstream>
#include <iostream>
#include "ns3/mobility-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/yans-wifi-helper.h"

#include "ns3/flow-monitor-helper.h"

using namespace ns3;
using namespace dsr;

int main(int argc, char *argv[])
{
  int nWifis = 2;
  double simulationTime = 10; // seconds
  // Enable logging
  LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);

  double txp = 7.5; // transmission power in dBm
  std::string rate("512000bps");
  std::string phyMode("DsssRate11Mbps");
  std::string tr_name("OLSR");
  std::string m_protocolName;
  int nodeSpeed = 1; // in m/s
  int nodePause = 0; // in s
  m_protocolName = "protocol";

  Config::SetDefault("ns3::OnOffApplication::PacketSize", StringValue("64"));
  Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(rate));

  // Set Non-unicastMode rate to unicast mode
  Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

  NodeContainer adhocNodes;
  adhocNodes.Create(nWifis);

  // Create network devices
  // setting up wifi phy and channel using helpers
  WifiHelper wifi;
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  // YANS - Yet Another NEtwork Simulator
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel(wifiChannel.Create());

  // Add a mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue(phyMode),
                               "ControlMode", StringValue(phyMode));
  wifiPhy.Set("TxPowerStart", DoubleValue(txp));
  wifiPhy.Set("TxPowerEnd", DoubleValue(txp));

  wifiMac.SetType("ns3::AdhocWifiMac");
  NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

  MobilityHelper mobilityAdhoc;
  int64_t streamIndex = 0; // used to get consistent mobility across scenarios

  ObjectFactory pos;
  pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
  pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"));
  pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"));
  // 500x500metres

  Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
  streamIndex += taPositionAlloc->AssignStreams(streamIndex);

  std::stringstream ssSpeed;
  ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
  std::stringstream ssPause;
  ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
  mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                 "Speed", StringValue(ssSpeed.str()),
                                 "Pause", StringValue(ssPause.str()),
                                 "PositionAllocator", PointerValue(taPositionAlloc));
  mobilityAdhoc.SetPositionAllocator(taPositionAlloc);
  mobilityAdhoc.Install(adhocNodes);
  streamIndex += mobilityAdhoc.AssignStreams(adhocNodes, streamIndex);
  NS_UNUSED(streamIndex); // From this point, streamIndex is unused
                          // protocol section

  AodvHelper aodv;
  OlsrHelper olsr;
  DsdvHelper dsdv;
  DsrHelper dsr;
  DsrMainHelper dsrMain;
  Ipv4ListRoutingHelper list;
  InternetStackHelper internet;

  list.Add(olsr, 100);
  m_protocolName = "OLSR";

  internet.SetRoutingHelper(list);
  internet.Install(adhocNodes);

  Ipv4AddressHelper addressAdhoc;
  addressAdhoc.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer adhocInterfaces;
  adhocInterfaces = addressAdhoc.Assign(adhocDevices);

  // Create applications
  uint16_t port = 9; // Discard port (RFC 863)
  OnOffHelper onOff1("ns3::UdpSocketFactory", Address(InetSocketAddress(adhocInterfaces.GetAddress(1), port)));
  onOff1.SetAttribute("DataRate", StringValue("512000bps"));
  onOff1.SetAttribute("PacketSize", UintegerValue(1024));

  double sendDuration = 0.5; // send data 50% of the time
  Ptr<UniformRandomVariable> onTime = CreateObject<UniformRandomVariable>();
  onTime->SetAttribute("Min", DoubleValue(0.0));
  onTime->SetAttribute("Max", DoubleValue(simulationTime * sendDuration));
  onOff1.SetAttribute("OnTime", PointerValue(onTime));

  Ptr<UniformRandomVariable> offTime = CreateObject<UniformRandomVariable>();
  offTime->SetAttribute("Min", DoubleValue(0.0));
  offTime->SetAttribute("Max", DoubleValue(simulationTime * (1.0 - sendDuration)));
  onOff1.SetAttribute("OffTime", PointerValue(offTime));

  ApplicationContainer apps = onOff1.Install(adhocNodes.Get(0));
  apps.Start(Seconds(1.0));
  // apps.Stop(Seconds(simulationTime));

  // Create a sink application to receive the data
  PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(adhocInterfaces.GetAddress(1), port)));
  apps = sink.Install(adhocNodes.Get(1));
  apps.Start(Seconds(0.0));
  // apps.Stop(Seconds(simulationTime));

  // Run the simulation
  Simulator::Stop(Seconds(simulationTime));
  Simulator::Run();

  // Calculate and print the average amount of data sent
  uint64_t totalBytes = DynamicCast<PacketSink>(apps.Get(0))->GetTotalRx();
  double avgBytesPerSec = totalBytes / (simulationTime - 1.0);
  std::cout << "Average amount of data sent: " << avgBytesPerSec << " bytes/sec" << std::endl;

  // Cleanup
  Simulator::Destroy();

  return 0;
}
