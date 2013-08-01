package com.gmail.sintheros.hephaestus;


import net.aufdemrand.denizen.exceptions.CommandExecutionException;
import net.aufdemrand.denizen.exceptions.InvalidArgumentsException;
import net.aufdemrand.denizen.scripts.ScriptEntry;
import net.aufdemrand.denizen.scripts.commands.AbstractCommand;
import net.aufdemrand.denizen.objects.aH;
import net.aufdemrand.denizen.utilities.debugging.dB;
import twitter4j.Twitter;
import twitter4j.TwitterFactory;
import twitter4j.conf.ConfigurationBuilder;

/**
 * <p>Sends a tweet from the server. The server must have an active Twitter account.</p>
 *
 * <b>dScript Usage:</b><br>
 * <pre>TWEET ["message"] [hashtags]</pre>
 *
 * <ol><tt>Arguments: [""] - Required []</ol></tt>
 *
 * <ol><tt>["message"]</tt><br> 
 *         The body of the tweet to send.</ol>
 *  
 *  <ol><tt>[hashtags]</tt><br>
 *  	   The tags you want to add, separated by commas.
 *
 *
 * <br><b>Example Usage:</b><br>
 * <ol><tt>
 *  - TWEET "This is a test message" test,message <br>
 *  The NPC will give the tweet "This is a test message #test #message"
 * </ol></tt>
 *
 * @author Ian Siczewicz
 *
 */
public class TweetCommand extends AbstractCommand {

    @Override
    public void parseArgs(ScriptEntry scriptEntry) throws InvalidArgumentsException {
    	
    	//Init
    	String message = null;
    	String tempTags = null;
    	
    	//Parse Args
    	int ticker = 0;
    	for (String arg : scriptEntry.getArguments()) {
    		if(ticker == 0)
    			message = arg;
    		else
    			tempTags = arg;
    		ticker++;
    	}
    	
    	if(message == null)
    		dB.echoError("Message missing.");
    	
    	//Parse hashtags apart.
    	if(tempTags!=null)
    	{
    		String temp = "";
    		for(int i=0; i<tempTags.length(); i++)
    		{
    			if(tempTags.charAt(i)!=',')
    				temp += tempTags.charAt(i);
    			else
    			{
    				temp = " #"+temp+" ";
    				message = message + temp;
    				temp = "";
    			}
    		}
    		temp = " #"+temp;
    		message = message + temp;
    		
    	}
    	
    	//Pass objects to execute
    	scriptEntry.addObject("message", message);

    }

    @Override
    public void execute(ScriptEntry scriptEntry) throws CommandExecutionException {
    	
    	//Get objects
    	String message = (String) scriptEntry.getObject("message");
    	
    	//Report to dB
    	dB.report(getName(),
    			aH.debugObj("Message", message));
    	
    	
		ConfigurationBuilder cb = new ConfigurationBuilder();
		cb.setDebugEnabled(true)
				.setOAuthConsumerKey("uIWTV4ExF57koc2zPLjog")
				.setOAuthConsumerSecret(
						"FkzZs4WsuMQNG86uD6bOs5f1WewF051cJP8Rrdfg2KI")
				.setOAuthAccessToken(
						"1482326880-TL8zVznYjbKRF0IMp8ylRp0UwkFYTk9ITiKN03t")
				.setOAuthAccessTokenSecret(
						"tsMA7E0YOYohdVyXEHL2rsP7GdbKKyVtqi7I2iJBXDw");
		TwitterFactory tf = new TwitterFactory(cb.build());
		Twitter twitter = tf.getInstance();
		try {
			twitter.updateStatus(message);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} // ThrowsTwitterException
		
    }

}
