package main

import (
 "fmt"
 "github.com/golang/protobuf/proto"
 "log"
 "unsafe"
)

import pb "soloscuro/soloscuro"

func main() {

 op1 := &pb.Packet_Auth { Username: "test", Password: "1234" }
 packet := &pb.Packet{
  Op: &pb.Packet_Auth_Op{ op1 },
 }

 // Serialize the Packet message to a byte slice
 data, err := proto.Marshal(packet)
 if err != nil {
  log.Fatal("marshaling error: ", err)
 }

 fmt.Printf("Sizeof marshalled data: %d\n", unsafe.Sizeof(data));

 // Deserialize the byte slice back into a new Player message
 newPacket := &pb.Packet{}
 err = proto.Unmarshal(data, newPacket)
 if err != nil {
  log.Fatal("unmarshaling error: ", err)
 }

 np := &pb.Packet_Status { NumPlayers: 1234 }
 packet_status := &pb.Packet{
     Op: &pb.Packet_Status_Op{ np },
 }
 // Print the original and deserialized messages
 fmt.Printf("Original Packet: %T size: %d\n", packet, unsafe.Sizeof(*packet))
 fmt.Printf("status Packet: %T size: %d\n", packet_status, unsafe.Sizeof(*np))
 fmt.Println("New Packet: ", newPacket)
 /*
 */
}
