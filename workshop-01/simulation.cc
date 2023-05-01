#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{

  double simulationTime = 30; // seconds
  // Enable logging
  LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(2);

  // Create network devices
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
  NetDeviceContainer devices = pointToPoint.Install(nodes);

  // Assign IP addresses to network devices
  InternetStackHelper stack;
  stack.Install(nodes);
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Create applications
  uint16_t port = 9; // Discard port (RFC 863)
  OnOffHelper onOff("ns3::UdpSocketFactory", Address(InetSocketAddress(interfaces.GetAddress(1), port)));
  onOff.SetAttribute("DataRate", StringValue("1Mbps"));
  onOff.SetAttribute("PacketSize", UintegerValue(1024));
  onOff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onOff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer apps = onOff.Install(nodes.Get(0));
  apps.Start(Seconds(1.0));
  apps.Stop(Seconds(simulationTime));

  // Create a sink application to receive the data
  PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
  apps = sink.Install(nodes.Get(1));
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(simulationTime));

  // Run the simulation
  Simulator::Run();

  // Calculate and print the average amount of data sent
  uint64_t totalBytes = DynamicCast<PacketSink>(apps.Get(0))->GetTotalRx();
  double avgBytesPerSec = totalBytes / (simulationTime - 1.0);
  std::cout << "Average amount of data sent: " << avgBytesPerSec << " bytes/sec" << std::endl;

  // Cleanup
  Simulator::Destroy();

  return 0;
}
