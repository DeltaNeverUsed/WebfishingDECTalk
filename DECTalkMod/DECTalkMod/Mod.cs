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

#pragma warning disable CA1416
        var baseKey = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Default);
        
        var key = baseKey.OpenSubKey(@"SOFTWARE\DECtalk Software\DECtalk\4.99\US", true);
        if (key == null)
            key = baseKey.CreateSubKey(@"SOFTWARE\DECtalk Software\DECtalk\4.99\US", true);
        
        key.SetValue("MainDict", Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)! + @"\dic\dtalk_us.dic");
        key?.Close();
        
        baseKey.Close();
#pragma warning restore CA1416
        
        Interface.RegisterScriptMod(new ModPatcher());
        Interface.RegisterScriptMod(new PlayerHUDPatcher());
        Interface.RegisterScriptMod(new PlayerLabelPatcher());
        Interface.RegisterScriptMod(new SoundManagerPatcher());
    }

    public void Dispose()
    { }
}