using System.Reflection;
using DECTalkMod.Patches;
using GDWeave;
using Microsoft.Win32;

namespace DECTalkMod;

public class Mod : IMod
{
    public static Config Config;
    public static IModInterface Interface;

    public static void LogInformation(object msg) => Interface.Logger.Information(": DECTalkMod: " + msg);
    
    public Mod(IModInterface modInterface)
    {
        Interface = modInterface;
        Config = modInterface.ReadConfig<Config>();
        
        LogInformation("Haaii!");
        
        Interface.RegisterScriptMod(new ModPatcher());
        Interface.RegisterScriptMod(new PlayerHUDPatcher());
        Interface.RegisterScriptMod(new PlayerLabelPatcher());
        Interface.RegisterScriptMod(new SoundManagerPatcher());
    }

    public void Dispose()
    { }
}