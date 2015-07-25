
/**
 * Time-of-day server listening to port 6013.
 *
 * Figure 3.21
 *
 * @author Silberschatz, Gagne, and Galvin. 
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013.
 */
 
import java.net.*;
import java.io.*;

public class DateServer
{
    public static void main(String[] args)  {
        try {
            ServerSocket sock = new ServerSocket(6013);

            // now listen for connections
            while (true) {
                Socket client = sock.accept();
                Thread thread = new Thread(new DatePrinter(client));
                thread.start();
                
            //    try {
           //         thread.join();
                    System.out.println("Thread terminated, date sent.");
           //     }
           //     catch (InterruptedException ie) {}
            }
        }
        catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}

class DatePrinter implements Runnable
{
    private Socket client;

    public DatePrinter(Socket client) {
        this.client = client;
    }

    public void run() {
        try {
            // we have a connection
            PrintWriter pout = new PrintWriter(client.getOutputStream(), true);
    
            // write the Date to the socket
            pout.println(new java.util.Date().toString());

            // close the socket and resume listening for more connections
            client.close();
        }
        catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}
