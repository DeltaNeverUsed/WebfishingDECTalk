using GDWeave.Godot;
using GDWeave.Godot.Variants;

namespace DECTalkMod;

public static class Helpers
{
    public static IEnumerable<Token> GetMain()
    {
        yield return new IdentifierToken("get_node");
        yield return new Token(TokenType.ParenthesisOpen);
        yield return new ConstantToken(new StringVariant("/root/deltaneveruseddectalk"));
        yield return new Token(TokenType.ParenthesisClose);
    }
}