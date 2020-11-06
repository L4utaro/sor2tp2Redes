#include <string>
#include <fstream>
#include <stddef.h>                    
#include <iomanip>                     
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-dumbbell.h"


using namespace ns3;

std::string prefix_file_name = "dumbbell-tp2";

static bool firstCwnd20 = true;
static bool firstSshThr20 = true;
static bool firstRtt20 = true;
static bool firstRto20 = true;

static Ptr<OutputStreamWrapper> cWndStream20;
static Ptr<OutputStreamWrapper> ssThreshStream20;
static Ptr<OutputStreamWrapper> rttStream20;
static Ptr<OutputStreamWrapper> rtoStream20;
static Ptr<OutputStreamWrapper> inFlightStream20;

static uint32_t cWndValue20;
static uint32_t ssThreshValue20;


static bool firstCwnd30 = true;
static bool firstSshThr30 = true;
static bool firstRtt30 = true;
static bool firstRto30 = true;

static Ptr<OutputStreamWrapper> cWndStream30;
static Ptr<OutputStreamWrapper> ssThreshStream30;
static Ptr<OutputStreamWrapper> rttStream30;
static Ptr<OutputStreamWrapper> rtoStream30;
static Ptr<OutputStreamWrapper> inFlightStream30;

static uint32_t cWndValue30;
static uint32_t ssThreshValue30;


static void
CwndTracer20 (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd20)
    {
      *cWndStream20->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd20 = false;
    }
  *cWndStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue20 = newval;

  if (!firstSshThr20)
    {
      *ssThreshStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue20 << std::endl;
    }
}

static void
CwndTracer30 (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd30)
    {
      *cWndStream30->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd30 = false;
    }
  *cWndStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue30 = newval;

  if (!firstSshThr30)
    {
      *ssThreshStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue30 << std::endl;
    }
}

static void
SsThreshTracer20 (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr20)
    {
      *ssThreshStream20->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr20 = false;
    }
  *ssThreshStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue20 = newval;

  if (!firstCwnd20)
    {
      *cWndStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue20 << std::endl;
    }
}

static void
SsThreshTracer30 (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr30)
    {
      *ssThreshStream30->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr30 = false;
    }
  *ssThreshStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue30 = newval;

  if (!firstCwnd30)
    {
      *cWndStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue30 << std::endl;
    }
}

static void
RttTracer20 (Time oldval, Time newval)
{
  if (firstRtt20)
    {
      *rttStream20->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt20 = false;
    }
  *rttStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RttTracer30 (Time oldval, Time newval)
{
  if (firstRtt30)
    {
      *rttStream30->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt30 = false;
    }
  *rttStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer20 (Time oldval, Time newval)
{
  if (firstRto20)
    {
      *rtoStream20->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto20 = false;
    }
  *rtoStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer30 (Time oldval, Time newval)
{
  if (firstRto30)
    {
      *rtoStream30->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto30 = false;
    }
  *rtoStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
InFlightTracer20 (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
InFlightTracer30 (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
TraceCwnd20 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream20 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer20));
}

static void
TraceSsThresh20 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream20 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer20));
}

static void
TraceRtt20 (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream20 = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer20));
}

static void
TraceRto20 (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream20 = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer20));
}

static void
TraceInFlight20 (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream20 = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer20));
}


static void
TraceCwnd30 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream30 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer30));
}

static void
TraceSsThresh30 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream30 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer30));
}

static void
TraceRtt30 (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream30 = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer30));
}

static void
TraceRto30 (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream30 = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer30));
}

static void
TraceInFlight30 (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream30 = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer30));
}