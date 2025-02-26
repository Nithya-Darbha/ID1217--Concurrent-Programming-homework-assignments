/*
 * javac Symmetric.java
 * example run- 2 processes and 3 rounds 
 * java 0 2 3
 * java 1 2 3
 */
import java.io.*;
import java.net.*;
import java.util.Random;

public class Symmetric {
    private int id;
    private int numproc;
    private int rounds;
    private int min;
    private int max;
    private ServerSocket server;
    private static final int PORT=8000;

    public Symmetric(int id,int numproc,int rounds){
        this.id=id;
        this.numproc=numproc;
        this.rounds=rounds;
        Random rnd=new Random();
        min=rnd.nextInt(100);
        max=rnd.nextInt(100);
    }

    public void start(){
        startservers();
        stallforprocs();
        exchangevalues();
    }

    public void startservers(){
        try {
            server=new ServerSocket(PORT+id);
            System.out.println("process "+id+" started on port "+(PORT+id));    
           } catch (IOException e){}
    }

    public void stallforprocs(){
        Thread signal=new Thread(new serverready());
        signal.start();
        for(int i=0;i<numproc;i++){
            if(i!=id){
                boolean conn=false;
                while(!conn){
                    try (Socket sock=new Socket("localhost",PORT+i+numproc);
                        DataOutputStream output=new DataOutputStream(sock.getOutputStream())) {
                        output.writeInt(id);
                        output.flush();
                        conn=true;
                    } catch (IOException e) {
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException ignored){}
                    }
                }
            }
        }
    }

    private class serverready implements Runnable{
        public void run(){
            try(ServerSocket ready = new ServerSocket(PORT+id+numproc)){
                for(int i=0;i<numproc-1;i++){
                    try (Socket socket=ready.accept();
                        DataInputStream input=new DataInputStream(socket.getInputStream())) {
                        input.readInt();
                    }  
                }  
            } catch (IOException e){}   
        }
    }

    public void exchangevalues(){
        for (int r=1;r<=rounds;r++) {
            send(r);
            try { 
                Thread.sleep(500); 
            } catch (InterruptedException e){}
            receive();
        }
        System.out.println("final values for process "+id+": min = "+min+", max = "+max);
        
    }

    private void send(int round){
        for(int i=0;i<numproc;i++){
            if(i!=id){
                try {
                    int rndmin=new Random().nextInt(10);
                    int rndmax=new Random().nextInt(10);
                    min+=rndmin;
                    max+=rndmax;  
                    if (min>max) {
                        int temp=min;
                        min=max;
                        max=temp;
                    }
                    System.out.println("Round "+round+": Process "+id+" sent min: "+min+", max: "+max+" to Process "+i);
                    Socket socket=new Socket("localhost",PORT+i);
                    DataOutputStream output=new DataOutputStream(socket.getOutputStream());
                    output.writeInt(min);
                    output.writeInt(max);
                    output.flush();
                    socket.close(); 
                } catch (IOException e){}
            }
        }
    }

    private void receive(){
        int counter=0;
        while(counter<numproc-1){
            try {
                Socket sock=server.accept();
                DataInputStream input=new DataInputStream(sock.getInputStream());
                int rcvmin=input.readInt();
                int rcvmax=input.readInt();
                System.out.println("process"+id+"received min: "+rcvmin+", max: "+rcvmax);
                System.out.println("current min: "+min+" ,max :"+max);
                min=Math.min(min,rcvmin);
                max=Math.max(max,rcvmax);
                counter++;
                System.out.println("updated Min: "+min+", Updated Max: "+max);
                sock.close();     
            } catch (IOException e){}
        }
    }

    public static void main(String[] args) {
        if (args.length!=3) {
            System.out.println("enter valid details");
            return;
        }
        int id=Integer.parseInt(args[0]);
        int procnum=Integer.parseInt(args[1]);
        int Round=Integer.parseInt(args[2]);

        long start = System.currentTimeMillis();
        Symmetric process=new Symmetric(id,procnum,Round);
        process.start();
        long end = System.currentTimeMillis();
        System.out.println("execution time: "+(end-start)+"ms");
    }
}
