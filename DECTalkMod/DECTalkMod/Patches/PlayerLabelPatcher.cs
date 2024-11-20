using GDWeave.Godot;
using GDWeave.Godot.Variants;
using GDWeave.Modding;

namespace DECTalkMod.Patches;

public class PlayerLabelPatcher : IScriptMod
{
    public bool ShouldRun(string path) => path == "res://Scenes/Entities/Player/player_label.gdc";

    public IEnumerable<Token> Modify(string path, IEnumerable<Token> tokens)
    {
        var speechWaiter = new MultiTokenWaiter([
            t => t is IdentifierToken { Name: "SwearFilter" },
            t => t.Type == TokenType.Period,
            t => t is IdentifierToken { Name: "_filter_string" },
            t => t.Type == TokenType.ParenthesisOpen,
            t => t is IdentifierToken { Name: "text" },
            t => t.Type == TokenType.ParenthesisClose,
            t => t.Type == TokenType.Newline
        ]);
        
        foreach (var token in tokens)
        {
            if (speechWaiter.Check(token))
            {
                yield return token;
                
                yield return new Token(TokenType.PrVar);
                yield return new IdentifierToken("voice");
                yield return new Token(TokenType.OpAssign);
                
                foreach (var t in Helpers.GetMain())
                    yield return t;
                
                yield return new Token(TokenType.Period);
                yield return new IdentifierToken("speak");
                yield return new Token(TokenType.ParenthesisOpen);
                yield return new IdentifierToken("text");
                yield return new Token(TokenType.ParenthesisClose);
                yield return new Token(TokenType.Newline, 1);
                
                foreach (var t in ScriptTokenizer.Tokenize(@"
get_parent().get_parent().add_child(voice)
voice.transform.origin.x = 0
voice.transform.origin.y = 0
voice.transform.origin.z = 0
voice.pitch_scale = 1
voice.play()
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