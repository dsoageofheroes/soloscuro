package soloscuro

import (
    "google.golang.org/protobuf/proto"
    "net"
)

import pb "soloscuro/proto"

func SendToken(ps *PlayerState, conn net.Conn) bool {
    data, err := ps.UUID.MarshalBinary()

    tp := &pb.Token {
        Token: data,
        Username: ps.Username,
    }

    packet := &pb.Packet{
        Op: &pb.Packet_TokenOp{ tp },
    }

    // Serialize the Packet message to a byte slice
    data, err = proto.Marshal(packet)
    if err != nil {
        println("marshaling error: ", err)
        return false
    }

    n, err := conn.Write(data)
    if err != nil {
        println(n, err)
        //log.Println(n, err)
        return false
    }

    return true
}

func SendError(msg string, error_packet *pb.Packet, conn net.Conn) bool {
    ep := &pb.Error { Msg: msg, Packet: error_packet }

    packet := &pb.Packet{
        Op: &pb.Packet_ErrorOp{ ep },
    }

    // Serialize the Packet message to a byte slice
    data, err := proto.Marshal(packet)
    if err != nil {
        println("marshaling error: ", err)
        return false
    }

    n, err := conn.Write(data)
    if err != nil {
        println(n, err)
        //log.Println(n, err)
        return false
    }

    return true
}
