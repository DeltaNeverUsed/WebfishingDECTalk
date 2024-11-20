using GDWeave.Godot;
using GDWeave.Modding;

namespace DECTalkMod.Patches;

public class SoundManagerPatcher : IScriptMod
{
    public bool ShouldRun(string path) => path == "res://Scenes/Entities/Player/sound_manager.gdc";

    public IEnumerable<Token> Modify(string path, IEnumerable<Token> tokens)
    {
        var voiceWaiter = new MultiTokenWaiter([
            t => t.Type == TokenType.PrFunction,
            t => t is IdentifierToken { Name: "_construct_voice" },
            t => t.Type == TokenType.ParenthesisOpen,
            t => t is IdentifierToken,
            t => t.Type == TokenType.Comma,
            t => t is IdentifierToken,
            t => t.Type == TokenType.Comma,
            t => t is IdentifierToken,
            t => t.Type == TokenType.ParenthesisClose,
            t => t.Type == TokenType.Colon,
            t => t.Type == TokenType.Newline
        ]);
        
        foreach (var token in tokens)
        {
            if (voiceWaiter.Check(token))
            {
                yield return token;

                yield return new Token(TokenType.CfReturn);
                yield return new Token(TokenType.Newline, 1);
            }
            else
            {
                yield return token;
            }
        }
    }
}