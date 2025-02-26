/*
 * javac Ring.java
 * example run- 2 processes and 3 rounds 
 * java 0 2 3
 * java 1 2 3
 */
import java.io.*;
import java.net.*;
import java.util.Random;

public class Ring {
    private int id;
    private int numproc;
    private int rounds;
    private int idnxt;
    private int value;
    private int min;
    private int max;
    private PrintWriter out;
    private BufferedReader in;

    public Ring(int id,int numproc,int rounds){
        this.id=id;
        this.numproc=numproc;
        this.rounds=rounds;
        this.idnxt=(id +1)%numproc;
    }

    public static void main(String[] args) throws IOException,InterruptedException{
        int id,n,r;
        id=Integer.parseInt(args[0]);
        n=Integer.parseInt(args[1]);
        r=Integer.parseInt(args[2]);

        long start = System.currentTimeMillis();
        Ring peer= new Ring(id, n, r);
        peer.start();
        long end = System.currentTimeMillis();
        System.out.println("execution time: "+(end-start)+"ms");

        Thread.sleep(100000);
    }

    public void start() throws IOException,InterruptedException{
        Random rnd=new Random();

        //server ,right and left peers
        ServerSocket server = new ServerSocket(8000+id);
        Socket left=nxtconn(); // next one
        Socket right=server.accept(); //previous one
        
        //output and inputs
        OutputStream output = left.getOutputStream();
        out=new PrintWriter(output,true);
        InputStream input=right.getInputStream();
        InputStreamReader rdr=new InputStreamReader(input);
        in=new BufferedReader(rdr);

        for(int i=0;i<rounds;i++){
            value=rnd.nextInt(1000);
            if(i==0){
                min=value;
                max=value;
            }
            else{
                min = Math.min(min,value);
                max = Math.max(max,value);
            }

            exchangeValues();
            System.out.println("peer"+ id +" min: "+min+" max: "+max+" R: " +i);

        }

        right.close();
        left.close();
        server.close();
        System.out.println("peer"+id+" Final min: "+min+" max: "+max);
    }

    private Socket nxtconn() throws InterruptedException{
        while (true) { 
          try {
              return new Socket("localhost",8000+idnxt);
          } catch (IOException e){}  
        }
    }

    /*first round-(i) start exchnage of values 
                 (ii) reads upadated values after it travles the entire ring
                (iii) update min and max
                steps i, ii,ii for the first peer
                (iv) other peers receive values from right peer, update min and max, send to left peer

     second round-(i) first peer sends final value of max and min to the entire ring
                  (ii) nth-2 peer reads min and max values againand updates and passes to left peer
                  (iii)nth-1 peer retains final min and max from round 1(ring so no next peer to comm with)


    */
    private void exchangeValues() throws IOException{
        //first round
        if(id==0){
            out.println(min+","+ max);
            String[] temp=in.readLine().split(",");
            min=Math.min(min,Integer.parseInt(temp[0]));
            max=Math.max(max,Integer.parseInt(temp[1]));
        }
        else{
            String[] temp=in.readLine().split(",");
            min=Math.min(min,Integer.parseInt(temp[0]));
            max=Math.max(max,Integer.parseInt(temp[1]));
            out.println(min+","+ max);
        }

        //second round
        if(id==0){
            out.println(min+","+ max);
        }
        else if(id<numproc-1){
            String[] temp=in.readLine().split(",");
            min=Math.min(min,Integer.parseInt(temp[0]));
            max=Math.max(max,Integer.parseInt(temp[1]));
            if(id<numproc-2){
                out.println(min+","+ max);
            }
        }
    }
}





