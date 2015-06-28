/**
 * Time-of-day server listening to port 6013.
 *
 * Figure 3.21
 *
 * @author Silberschatz, Gagne, and Galvin. 
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013.
 */
 
import java.net.*;
import java.io.*;

public class DateServer
{
    public static void main(String[] args)  {
        try {
    	    ServerSocket sock = new ServerSocket(6017);

            // now listen for connections
            while (true) {
                Socket client = sock.accept();
                
                InputStream in = client.getInputStream();
                BufferedReader bin = new BufferedReader(new InputStreamReader(in));

                // we have a connection		
                PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

                String line;
                line = bin.readLine();
                pout.println(line);
                
                // close the socket and resume listening for more connections
          	client.close();
            }
    	}
    	catch (IOException ioe) {
    	    System.err.println(ioe);
    	}
    }
}
