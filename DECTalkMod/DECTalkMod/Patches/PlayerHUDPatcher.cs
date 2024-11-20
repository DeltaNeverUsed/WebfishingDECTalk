using GDWeave.Godot;
using GDWeave.Modding;

namespace DECTalkMod.Patches;

public class PlayerHUDPatcher : IScriptMod
{
    public bool ShouldRun(string path) => path == "res://Scenes/HUD/playerhud.gdc";

    public IEnumerable<Token> Modify(string path, IEnumerable<Token> tokens)
    {
        var speechWaiter = new MultiTokenWaiter([
            t => t.Type == TokenType.PrFunction,
            t => t is IdentifierToken { Name: "_send_message" },
            t => t.Type == TokenType.ParenthesisOpen,
            t => t is IdentifierToken { Name: "text" },
            t => t.Type == TokenType.ParenthesisClose,
            t => t.Type == TokenType.Colon,
            t => t.Type == TokenType.Newline
        ]);
        
        foreach (var token in tokens)
        {
            if (speechWaiter.Check(token))
            {
                yield return token;
                
                foreach (var t in ScriptTokenizer.Tokenize(@"
text = text.replace(""["", ""{"")
text = text.replace(""]"", ""}"")
", 1))
                    yield return t;
            }
            else
            {
                yield return token;
            }
        }
    }
}