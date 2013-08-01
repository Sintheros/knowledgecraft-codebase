package com.gmail.sintheros.hephaestus;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;

import net.aufdemrand.denizen.exceptions.CommandExecutionException;
import net.aufdemrand.denizen.exceptions.InvalidArgumentsException;
//import net.aufdemrand.denizen.flags.FlagManager;
import net.aufdemrand.denizen.flags.FlagManager.Flag;
import net.aufdemrand.denizen.scripts.ScriptEntry;
//import net.aufdemrand.denizen.scripts.ScriptRegistry;
import net.aufdemrand.denizen.scripts.commands.AbstractCommand;
//import net.aufdemrand.denizen.scripts.containers.core.FormatScriptContainer;
//import net.aufdemrand.denizen.utilities.arguments.aH;
import net.aufdemrand.denizen.objects.aH;
import net.aufdemrand.denizen.utilities.debugging.dB;
//import net.aufdemrand.denizen.utilities.debugging.dB.Messages;
//import org.bukkit.Bukkit;
//import org.bukkit.entity.Player;
import org.jsoup.Jsoup;

/**
 * <p>Retrieves the plain-text content from a given URL and saves it into a flag.n.</p>
 *
 * <b>dScript Usage:</b><br>
 * <pre>URL ["URL Address"] ({npc}/global/player) [flagname]</pre>
 *
 * <ol><tt>Arguments: [""] [] - Required, () - Optional</ol></tt>
 *
 * <ol><tt>["URL Address"]</tt><br> 
 *         The website for the denizen to retrieve information from.</ol>
 *  
 *  <ol><tt>[flag name]</tt><br>
 *  	   The name of the flag.n where the plain text will be placed.
 *  
 *  <ol><tt>(npc/global)</tt><br>
 *  		Can be an NPC, player, or global flag. Defaults to NPC.
 *
 *
 * <br><b>Example Usage:</b><br>
 * <ol><tt>
 *  - URL "http://isitthursday.org/" Thursday <br>
 *  The NPC will be given a flag.n named Thursday containing the string "Is it Thursday? MAYBE" 
 * </ol></tt>
 *
 * @author Ian Siczewicz
 *
 */
public class UrlCommand extends AbstractCommand {

	public enum Type   { GLOBAL, NPC, PLAYER}
	
    @Override
    public void parseArgs(ScriptEntry scriptEntry) throws InvalidArgumentsException {
    	
    	//Init
    	String url = null;
    	String flag = null;
    	Type type = Type.NPC;
    	
    	//Parse Args
    	for (String arg : scriptEntry.getArguments()) {
    		if (aH.matchesArg("GLOBAL, NPC, DENIZEN, GLOBAL", arg))
    			type = Type.valueOf(arg.toUpperCase().replace("DENIZEN", "NPC"));
    		else if(arg.contains("."))
    			url = arg;
    		else
    			flag = arg.toUpperCase();
    	}
    	
    	if(url == null)
    		dB.echoError("URL missing - URLs always have a period in them somewhere.");
    	
    	if(flag == null)
    		dB.echoError("Flag missing - Did you put a period in your flag name?");
    	
    	
    	//Pass objects to execute
    	scriptEntry.addObject("url", url);
    	scriptEntry.addObject("flag", flag);
    	scriptEntry.addObject("type", type);
    }

    @Override
    public void execute(ScriptEntry scriptEntry) throws CommandExecutionException {
    	
    	//Get objects
    	String urlAddress = (String) scriptEntry.getObject("url");
    	String flagName = (String) scriptEntry.getObject("flag");
    	Type type = (Type) scriptEntry.getObject("type");
    	
    	//Report to dB
    	dB.report(getName(),
    			aH.debugObj("URL", urlAddress)
    					+ aH.debugObj("Flag", flagName)
    					+ aH.debugObj("Type", type.toString()));
    	
    	//Init flag
    	Flag flag = null;
    	String player = (String) scriptEntry.getObject("player");
    	
    	switch (type) {
        case NPC:
            flag = denizen.flagManager().getNPCFlag(scriptEntry.getNPC().getId(), flagName);
            break;
        case GLOBAL:
            flag = denizen.flagManager().getGlobalFlag(flagName);
            break;
        case PLAYER:
        	flag = denizen.flagManager().getPlayerFlag(player, flagName);
    }
    	
		// Go to URL, retrieve all data, including HTML.
		String urlContent = null;
		try {
			URL url = new URL(urlAddress);
			try {
				BufferedReader reader = new BufferedReader(
						new InputStreamReader(url.openStream()));
				String nextLine;
				StringBuffer buffer = new StringBuffer();
				while ((nextLine = reader.readLine()) != null)
					buffer.append(nextLine);
				urlContent = buffer.toString();
				reader.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
			}
		} catch (MalformedURLException e) {
			e.printStackTrace();
		}
		
		//Parse out HTML from urlContent into plain text only using Jsoup package.
		urlContent = Jsoup.parse(urlContent).text();
		
		//Set the flag.
		flag.set(urlContent);
		
    }

}
