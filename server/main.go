package main

import (
    "fmt"
    "log"
    "google.golang.org/protobuf/proto"
    "crypto/tls"
    "net"
    "os"
    "bytes"
    "encoding/gob"
    "github.com/google/uuid"
)

import pb "soloscuro/proto"
import "soloscuro/internal"

var pstate map[uuid.UUID]*soloscuro.PlayerState = make(map[uuid.UUID]*soloscuro.PlayerState)
var players_file = "players.bin"

func main() {
    log.SetFlags(log.Lshortfile)

    b, err := os.ReadFile(players_file)
    if err == nil {
        // Decoding the serialized data
        //buf := bytes.Buffer(b)
        fmt.Printf("%T\n", b)
        d := gob.NewDecoder(bytes.NewBuffer(b))
        err = d.Decode(&pstate)
        if err != nil {
            log.Printf("Unable to load players: '%s'\n", err.Error())
        }
        log.Printf("Loaded players from '%s'\n", players_file);
    }

    cer, err := tls.LoadX509KeyPair("cert/notsecure.crt", "cert/notsecure.key")
    if err != nil {
        log.Println(err)
        return
    }

    config := &tls.Config{Certificates: []tls.Certificate{cer}}
    ln, err := tls.Listen("tcp", ":6789", config) 
    if err != nil {
        log.Println(err)
        return
    }
    defer ln.Close()

    for {
        conn, err := ln.Accept()
        if err != nil {
            log.Println(err)
            continue
        }
        go handleConnection(conn)
    }
}

func handleConnection(conn net.Conn) {
    buf := make([]byte, 65536)
    defer conn.Close()
    for {
        mlen, err := conn.Read(buf);
        if err != nil {
            log.Println(err)
            return
        }

        newPacket := &pb.Packet{}
        err = proto.Unmarshal(buf[:mlen], newPacket)
        if err != nil {
         log.Printf("unmarshaling error: %s\n", err)
         return
        }

        switch newPacket.GetOp().(type) {
        case *pb.Packet_TokenOp:
            handleToken(newPacket, conn);
        case *pb.Packet_CreateOp:
            handleCreate(newPacket, conn);
        case *pb.Packet_AuthOp:
            handleAuth(newPacket, conn);
            break;
        default:
            fmt.Printf("%T\n", newPacket.GetOp())
        }
    }
}


func findPlayerState(username string, password string) *soloscuro.PlayerState {
    for _, ps := range pstate {
        if ps.Username == username && ps.Password == password {
            return ps
        }
    }

    return nil
}

func allocatePlayerState(username string, password string) *soloscuro.PlayerState {
    ret := &soloscuro.PlayerState{}
    ret.Username = username
    ret.Password = password
    ret.UUID = uuid.New()

    pstate[ret.UUID] = ret

    b := new(bytes.Buffer)

    e := gob.NewEncoder(b)

    // Encoding the map
    err := e.Encode(pstate)
    if err != nil {
        panic(err)
    }

    err = os.WriteFile(players_file, b.Bytes(), 0644)
    if err != nil {
        log.Printf("Unable to open player file...\n");
        return ret
    }


    return ret
}

func findOrAllocatePlayerState(username string, password string) *soloscuro.PlayerState {
    for _, ps := range pstate {
        if ps.Username == username && ps.Password == password {
            return ps
        }
    }

    ret := &soloscuro.PlayerState{}
    ret.Username = username
    ret.Password = password
    ret.UUID = uuid.New()

    pstate[ret.UUID] = ret

    return ret
}

func handleAuth(packet *pb.Packet, conn net.Conn) bool {
    auth := packet.GetAuthOp()
    ps := findPlayerState(auth.Username, auth.Password)

    if ps == nil {
        soloscuro.SendError("Unable to find player!", packet, conn)
        return false;
    }

    return soloscuro.SendToken(ps, conn)
}

func handleToken(packet *pb.Packet, conn net.Conn) bool {
    token := packet.GetTokenOp()
    u, err := uuid.FromBytes(token.Token)

    if err != nil {
        soloscuro.SendError(err.Error(), packet, conn)
        return false;
    }

    ps := pstate[u]

    if ps == nil {
        soloscuro.SendError("Unable to find player!", packet, conn)
        return false
    }

    return soloscuro.SendToken(ps, conn)
}

func handleCreate(packet *pb.Packet, conn net.Conn) bool {
    create := packet.GetCreateOp()
    ps := findPlayerState(create.Username, create.Password)

    if ps != nil {
        soloscuro.SendError("Player already exists!", packet, conn)
        return false;
    }

    ps = allocatePlayerState(create.Username, create.Password);
    return soloscuro.SendToken(ps, conn)
}
