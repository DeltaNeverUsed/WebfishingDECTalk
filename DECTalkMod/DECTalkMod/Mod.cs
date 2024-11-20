using DECTalkMod.Patches;
using GDWeave;

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
        
        Interface.RegisterScriptMod(new PlayerPatcher());
        Interface.RegisterScriptMod(new SoundManagerPatcher());

        LogInformation("Haaii!");
    }

    public void Dispose() { }
}