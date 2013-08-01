package com.gmail.sintheros.hephaestus;

import org.bukkit.entity.LivingEntity;
import org.bukkit.entity.Player;

import net.aufdemrand.denizen.exceptions.CommandExecutionException;
import net.aufdemrand.denizen.exceptions.InvalidArgumentsException;
import net.aufdemrand.denizen.npc.traits.HungerTrait;
import net.aufdemrand.denizen.scripts.ScriptEntry;
import net.aufdemrand.denizen.scripts.commands.AbstractCommand;
import net.aufdemrand.denizen.objects.aH;
import net.aufdemrand.denizen.objects.aH.ArgumentType;
import net.aufdemrand.denizen.utilities.debugging.dB;
import net.aufdemrand.denizen.utilities.debugging.dB.Messages;
import net.citizensnpcs.api.CitizensAPI;
import net.citizensnpcs.api.npc.NPC;

/**
 * Saturates a player.
 * 
 * @author Ian Siczewicz, modified FEED command by Jeremy Schroeder, Mason Adkins
 */

public class SaturateCommand extends AbstractCommand {

    @Override
    public void onEnable() {
        // nothing to do here
    }

    /* SATURATE (AMT:#) */

    /* 
     * Arguments: [] - Required, () - Optional 
     * (AMT:#) 1-20, usually.
     *   
     * Example Usage:
     * SATURATE AMT:20
     * SATURATE AMT:5
     * SATURATE
     *
     */

    private enum TargetType {PLAYER } 

    private int amount;
    private LivingEntity target;
    private TargetType targetType;

    @Override
    public void parseArgs(ScriptEntry scriptEntry) throws InvalidArgumentsException {

        // Must reset ALL private variables, else information left over from last time
        // might be used.
        targetType = TargetType.PLAYER;
        amount = Integer.MAX_VALUE;
        // Set target to Player by default, if available
        if (scriptEntry.getPlayer() != null) target = scriptEntry.getPlayer().getPlayerEntity();
        else target = null;
        
        for (String arg : scriptEntry.getArguments()) {

            if (aH.matchesQuantity(arg) || aH.matchesValueArg("amt", arg, ArgumentType.Integer)) {
                amount = aH.getIntegerFrom(arg);
                dB.echoDebug(Messages.DEBUG_SET_QUANTITY, String.valueOf(amount));

            } else throw new InvalidArgumentsException(Messages.ERROR_UNKNOWN_ARGUMENT, arg);
        }

        // If TARGET is NPC/PLAYER and no NPC/PLAYER available, throw exception.
        if (targetType == TargetType.PLAYER && scriptEntry.getPlayer() == null) throw new InvalidArgumentsException(Messages.ERROR_NO_PLAYER);

    }

    
    @Override
    public void execute(ScriptEntry scriptEntry) throws CommandExecutionException {
        
        // Target is a Player
           // Set to max saturation level
           if (amount == Integer.MAX_VALUE){
        	   ((Player)target).setSaturation(20);
           }
           // else, increase saturation levels
           else{
        	   ((Player) target).setSaturation(((Player) target).getSaturation() + amount);
           }

    }
    
    
}