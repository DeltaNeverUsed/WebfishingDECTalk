using System.Reflection;
using GDWeave.Godot;
using GDWeave.Godot.Variants;
using GDWeave.Modding;

namespace DECTalkMod.Patches;

public class ModPatcher : IScriptMod
{
    public bool ShouldRun(string path) => path == "res://mods/deltaneverused.dectalk/main.gdc";

    public IEnumerable<Token> Modify(string path, IEnumerable<Token> tokens)
    {
        foreach (var token in tokens) {
            if (token is ConstantToken {Value: StringVariant {Value: "%LIBDECTALKPATH%"} str}) {
                str.Value = Path.Combine(
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!,
                    "libGodotDecTalk.dll"
                );
            }

            yield return token;
        }
    }
}