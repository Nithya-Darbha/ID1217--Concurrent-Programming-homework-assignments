/*
 * one coordinator - multiple peers 
 * cooridnator sends value to peers - centre 
 * peer sends value to coordinator and receives values of gloabal min and max 
 * 
 * example run:
 * javac Centralized.java
 * java Centralized <peernum> <numPeers> <numRounds> - peernu 0 for coordinator and 1-n for n peers
 * java Centralized 0 2 3 - coordinator 
 * java Centralized 1 2 3 - peer 1
 * java Centralized 2 2 3 - peer 2
 */

import java.io.*;
import java.net.*;
import java.util.*;

public class Centralized{
    public static void main(String[] args) {
        try {
            if (args.length<3) {
                System.out.println("enter valid details");
                return;
            }
            int count=Integer.parseInt(args[0]);
            int numPeers=Integer.parseInt(args[1]); 
            int numRounds=Integer.parseInt(args[2]); 

            long startTime=System.currentTimeMillis();
            if (count==0) {
                new coordinator(numPeers, numRounds).start();
            } else {
                new peer(count, numRounds).start();
            }
            long endTime=System.currentTimeMillis();
            System.out.println("execution time: "+(endTime-startTime)+"ms"); 
        }
         catch (IOException e){}
    }
}


class peer{
    private int count;
    private int rounds;

    public peer(int count, int rounds){
        this.count=count;
        this.rounds=rounds;
    }
    
    public void start() throws IOException{
        Random rnd=new Random();

        for(int i=0;i<rounds;i++){
            Socket sock= new Socket("localhost",8888);
            DataInputStream input=new DataInputStream(sock.getInputStream());
            DataOutputStream output=new DataOutputStream(sock.getOutputStream());

            int v=rnd.nextInt(1000); // generate values
            output.writeInt(v); // send value to coordinator
            // receive min and max values
            int localmin=input.readInt();
            int localmax=input.readInt();

            System.out.println("Peer " + count + " for round " + (i + 1) + ": Min = " + localmin + ", Max = " + localmax + " (received values)");
            sock.close();
        }
    }
}

class coordinator{
    private int peernum;
    private int rounds;

    public coordinator(int peernum,int rounds){
        this.peernum=peernum;
        this.rounds=rounds;
    }

    public void start() throws IOException{
        ServerSocket server = new ServerSocket(8888);
        System.out.println("server starting on port 8888" );

        for(int i=0;i<rounds;i++){
            int localmin=Integer.MAX_VALUE;
            int localmax=Integer.MIN_VALUE;
            int [] values=new int[peernum];
            List<Socket> socket= new ArrayList<>();
            List<Integer> value= new ArrayList<>();

            for(int j=0;j<peernum;j++){
                //waits for all peers to connect , stores and updates
                Socket sock= server.accept();
                DataInputStream input=new DataInputStream(sock.getInputStream());
                int v =input.readInt();
                value.add(v);
                socket.add(sock);
                localmin=Math.min(localmin, v);
                localmax=Math.max(localmax, v);
            }
            System.out.println("For round " + (i + 1) + ": Min = " + localmin + ", Max = " + localmax);

            for(Socket sock:socket){
                DataOutputStream output=new DataOutputStream(sock.getOutputStream());
                //send values of min and mac to all peers
                output.writeInt(localmin);
                output.writeInt(localmax);
                sock.close();

            }
        }
        server.close();
    }
}


