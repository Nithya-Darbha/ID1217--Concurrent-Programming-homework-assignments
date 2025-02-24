public class HangryBird {

    final static int BABY_BIRDS = 6;
    final static int WORMS = 10;
    static Bowl bowl;

    public static void main(String[] args) {
        bowl=new Bowl(WORMS); // shared resourse 

        for (int i = 1;i<=BABY_BIRDS;i++) { // baby bird thread
            new babyBird(i).start();
        }
        new parentBird().start();// parent bird thread
    }
    
    private static class babyBird extends Thread{
        int id;
        public babyBird(int id){
            this.id=id;
            this.setName("bird "+id);
        }
        @Override
        public void run(){
            while(true){
                try {
                    bowl.eatWorm(id);
                    Thread.sleep((int)(Math.random()*1000));
                } catch (InterruptedException e){}
            }
        }
    }
    
    private static class parentBird extends Thread{
        @Override
        public void run(){
            while(true){
                try {
                    bowl.refillBowl();
                    Thread.sleep((int)(Math.random()*2000));
                } catch (InterruptedException e){}
            }
      }

    }

    //eating and reffiling handled here,shared resource i.e bowl is the monitor 
    private static class Bowl{
        int wormsPresent;
        final int total;

        private final Object lock=new Object();
        private final int LIMIT;
        private int current=1;

        public Bowl(int startingworms){
            this.wormsPresent=startingworms;
            this.total=startingworms;
            this.LIMIT=total;
        }

        public void eatWorm(int id) {
            synchronized (lock) {
                while (wormsPresent == 0) { 
                    try {
                        lock.notifyAll();  
                        lock.wait(); 
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                }
                while (id!=current) {
                    try {
                        lock.wait();
                    } catch (InterruptedException e){}
                }
                if (wormsPresent>0) {  
                    wormsPresent--;
                    System.out.println(Thread.currentThread().getName() + " eats a worm, bowl has " + wormsPresent + " worms left.");
                }
                current=(current % BABY_BIRDS)+1;
                lock.notifyAll();
            }
        }
        
        public void refillBowl() {
            synchronized (lock) {
                while(wormsPresent>0) {
                    try {
                        lock.wait();  
                    } catch (InterruptedException e){}
                }
                wormsPresent=LIMIT;
                System.out.println("parent bird refills the bowl with " + LIMIT + " worms.");
                //current=1;
                lock.notifyAll();
            }
        }  
    } 
}
