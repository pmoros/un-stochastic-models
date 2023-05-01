#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpEchoExample");

int main(int argc, char *argv[])
{
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create(2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);

  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  UdpEchoServerHelper echoServer(9);

  ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));

  ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  // Create a packet sink application on each node to measure traffic received by that node
  PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
  ApplicationContainer sinkApps = sinkHelper.Install(nodes);

  // Send a UDP packet from node zero to node one
  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
  InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 9);
  ns3UdpSocket->Connect(remote);
  // ns3UdpSocket->Send(Create<Packet>(1024));

  // Calculate and output the average traffic received by each node
  for (uint32_t i = 0; i < nodes.GetN(); ++i)
  {
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(i));
    uint64_t totalBytes = sink->GetTotalRx();
    double avgBytes = totalBytes;
    std::cout << "Node " << i << " received an average of " << avgBytes << " bytes of traffic." << std::endl;
  }

  Simulator::Run();
  Simulator::Destroy();
  return 0;
}