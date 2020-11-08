//   TOPOLOGIA DUMBBELL:
//
//   nodo0                    nodo1
//
//   nodo1 --- r0-----r1------nodo2
//
//   nodo2                    nodo0


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

#include "funciones.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE ("TP2-Dumbbell-SOR2");


int main (int argc, char *argv[])
{

  uint32_t  cantNodosCliente = 3;
  uint32_t  cantNodosServidores = 3;
  uint32_t puerto = 300;
  bool tracing = true;

  CommandLine cmd;
  cmd.AddValue ("nLeaf",     "Numero de nodos de cliente y servidores", cantNodosCliente);
  cmd.AddValue ("tracing", "Enable/Disable Tracing", tracing);
  cmd.Parse (argc, argv);

/////////////////////////////////////////////////////
//Facilita  la creacion de redes punto a punto.
  PointToPointHelper redDumbbell;
  redDumbbell.SetDeviceAttribute  ("DataRate", StringValue ("150kb/s"));
  redDumbbell.SetChannelAttribute ("Delay", StringValue ("100ms"));
  redDumbbell.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("50p"));

  //Facilita  la creacion de nodos routers.
  PointToPointHelper redBottleneck;
  redBottleneck.SetDeviceAttribute  ("DataRate", StringValue ("50kb/s"));
  redBottleneck.SetChannelAttribute ("Delay", StringValue ("100ms"));
  redBottleneck.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("50p"));
/////////////////////////////////////////////////////

  //Facilita la creación de una topología dumbbell con enlaces p2p.
  PointToPointDumbbellHelper dumbbell (cantNodosCliente, redDumbbell, cantNodosServidores, redDumbbell, redBottleneck);

  //Utiliza una combinación de enrutamiento estático y enrutamiento global de forma predeterminada.
  InternetStackHelper stack;
  //Se usa para instalar en cada nodo de la topologia dumbbell.
  dumbbell.InstallStack (stack);
  //Asigna direcciones IPV4 a la topologia.
  dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));
  //Facilita la tarea de administrar el enrutamiento global. Crea una base de datos de enrutamiento e inicializa las tablas de 
  //enrutamiento de los nodos en la simulación. Convierte todos los nodos de la simulación en enrutadores.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ApplicationContainer clienteApps;
  ApplicationContainer servidorApps;

  uint32_t nodo0 = 0;
  uint32_t nodo1 = 1;
  uint32_t nodo2 = 2;

  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  // Facilita el trabajo con OnOffApplications (Envio de paquetes).
  OnOffHelper clienteHelper ("ns3::TcpSocketFactory", Address ());
  clienteHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  //Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
  AddressValue remoteAddress (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
  clienteHelper.SetAttribute ("Remote", remoteAddress);
  //Agrego a mi contenedor de APPs-Cliente, el nuevo cliente junto al puntero del nodo Origen.
  clienteApps.Add (clienteHelper.Install (dumbbell.GetLeft (nodo0)));
  //Crea la aplicacion de tipo servidor/receptor, con nodo Destino y puerto.
  PacketSinkHelper servidor ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
  //Agrego a mi contenedor de APPs-Servidor, el nuevo servidor junto al puntero y nodo Origen (En este caso = a nodo Destino).
  servidorApps.Add (servidor.Install (dumbbell.GetRight (nodo1)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  OnOffHelper clienteHelper2 ("ns3::TcpSocketFactory", Address ());
  clienteHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  AddressValue remoteAddress2 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
  clienteHelper2.SetAttribute ("Remote", remoteAddress2);
  clienteApps.Add (clienteHelper2.Install (dumbbell.GetLeft (nodo1)));

  PacketSinkHelper servidor2 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
  servidorApps.Add (servidor2.Install (dumbbell.GetRight (nodo2)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application UDP
  OnOffHelper clienteHelper3 ("ns3::UdpSocketFactory", Address ());
  clienteHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  AddressValue remoteAddress3 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  clienteHelper3.SetAttribute ("Remote", remoteAddress3);
  clienteApps.Add (clienteHelper3.Install (dumbbell.GetLeft (nodo2)));

  PacketSinkHelper servidor3 ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  servidorApps.Add (servidor3.Install (dumbbell.GetRight (nodo0)));
  //--------------------------------------------------------------------------------------------------------------------------
  //Comienzo y parada de paquetes enviados (Clientes(emisores) -> Servidores(receptores)).
  clienteApps.Start (Seconds (0.0));
  clienteApps.Stop (Seconds (30.0));
  servidorApps.Start (Seconds (0.0));
  servidorApps.Stop (Seconds (100.0));

  //Generacion de pcap's. - Captura de los paquetes de cada enlace (aristas).
    if (tracing)
    {
      redDumbbell.EnablePcapAll ("dumbbell-tp2", false);

      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd20, prefix_file_name + "-20-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh20, prefix_file_name + "-20-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt20, prefix_file_name + "-20-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto20, prefix_file_name + "-20-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight20, prefix_file_name + "-20-inflight.data");


      Simulator::Schedule (Seconds (0.00001), &TraceCwnd30, prefix_file_name + "-30-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh30, prefix_file_name + "-30-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt30, prefix_file_name + "-30-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto30, prefix_file_name + "-30-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight30, prefix_file_name + "-30-inflight.data");
   
    }


  NS_LOG_INFO ("Comienzo de Simulación.");
  Simulator::Run ();
  NS_LOG_INFO ("Listo.");
  Simulator::Destroy ();

}