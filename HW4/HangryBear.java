public class HangryBear {
     static int BEES = 3;
     static int CAPACITY = 10;
     static honeyPot pot;

     public static void main(String[] args) {
        if(args.length!=2){
            System.out.println("invalid values");
            System.exit(1);
        }
        BEES=Integer.parseInt(args[0]);
        CAPACITY=Integer.parseInt(args[1]);
        pot=new honeyPot(CAPACITY); // shared reaource 
        for (int i = 1;i<=BEES;i++) {  // honey bee threads
            new honeyBees(i).start(); 
        }
        new bear().start();// bear thread
     }

     private static class bear extends Thread{
        @Override
        public void run(){
            while(true){
                try {
                    pot.eatHoney();
                    Thread.sleep((int)(Math.random()*2000));
                } catch (InterruptedException e){}
            }
      }
     }

     private static class honeyBees extends Thread{
        int id;
        public honeyBees(int id){
            this.id=id;
            this.setName("honeybee "+id);
        }
        @Override
        public void run(){
            while(true){
                try {
                    pot.refillPot(id);
                    Thread.sleep((int)(Math.random()*1000));
                } catch (InterruptedException e){}
            }
        }

     }

     private static class honeyPot{
        int presentHoney;
        final int capacity;
        private final Object lock = new Object();

        public honeyPot(int capacity){
            this.presentHoney=0;
            this.capacity=capacity;
        }

        public void eatHoney() {
            synchronized (lock) {
                while (presentHoney!=capacity) {
                    try {
                        lock.wait();
                    } catch (InterruptedException e) {}
                }
                System.out.println("bear wakes up, eats all honey.");
                presentHoney = 0;
                System.out.println("bear goes back to sleep.");
                lock.notifyAll(); 
            }
        }
        

        public void refillPot(int id){
            synchronized (lock) {
                while(presentHoney==capacity) {
                    try {
                        lock.wait(); 
                    } catch (InterruptedException e){}
                }
                presentHoney++;
                System.out.println("honeybee " + id + " added honey, pot now has " + presentHoney + " portions.");
                if (presentHoney==capacity) {
                    System.out.println("pot reached its capacity, honeybee " + id + " is waking up the bear!");
                    lock.notifyAll(); 
                }
            }

        }

     }

}
