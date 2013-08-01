package com.gmail.sintheros.hephaestus;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;

import org.bukkit.Bukkit;
import org.bukkit.plugin.java.JavaPlugin;

public final class Hephaestus extends JavaPlugin {
	@Override
	public void onEnable() {
		try {
			UrlCommand.class.newInstance().activate().as("URL").withOptions("url ({npc}/global/player) [\"url address\"] [name]", 2);
			TweetCommand.class.newInstance().activate().as("TWEET").withOptions("tweet [\"message\"] [hashtags]", 1);
			SaturateCommand.class.newInstance().activate().as("SATURATE").withOptions("saturate (amt:#)", 0);
		} catch (InstantiationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		//External .jar importing by fletch_to_99
		try {
            final File[] libs = new File[] {
                    new File(getDataFolder(), "jsoup-1.7.2.jar"), 
                    new File(getDataFolder(), "twitter4j-core-3.0.3.jar") };
            for (final File lib : libs) {
                if (!lib.exists()) {
                    JarUtils.extractFromJar(lib.getName(),
                            lib.getAbsolutePath());
                }
            }
            for (final File lib : libs) {
                if (!lib.exists()) {
                    getLogger().warning(
                            "There was a critical error loading Hephaestus! Could not find lib: "
                                    + lib.getName());
                    Bukkit.getServer().getPluginManager().disablePlugin(this);
                    return;
                }
                addClassPath(JarUtils.getJarUrl(lib));
            }
        } catch (final Exception e) {
            e.printStackTrace();
        }
	        
		getLogger().info("Hephaestus enabled -- the forge is lit!");
	}
	
	//External .jar importing by fletch_to_99
	 private void addClassPath(final URL url) throws IOException {
         final URLClassLoader sysloader = (URLClassLoader) ClassLoader
                 .getSystemClassLoader();
         final Class<URLClassLoader> sysclass = URLClassLoader.class;
         try {
             final Method method = sysclass.getDeclaredMethod("addURL",
                     new Class[] { URL.class });
             method.setAccessible(true);
             method.invoke(sysloader, new Object[] { url });
         } catch (final Throwable t) {
             t.printStackTrace();
             throw new IOException("Error adding " + url
                     + " to system classloader");
         }
     }

	@Override
	public void onDisable() {
		getLogger().info("Hephaestus disabled.");
	}
}
