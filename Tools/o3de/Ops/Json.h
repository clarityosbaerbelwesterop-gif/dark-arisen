#pragma once

// Minimal strict JSON reader and deterministic writer for tooling manifests and glTF.
// Numbers are kept as double; objects are written in key order (std::map), so output is stable.

#include <charconv>
#include <cmath>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools
{
    struct JsonValue
    {
        enum class Kind { Null, Bool, Number, String, Array, Object };
        Kind Type = Kind::Null;
        bool Boolean = false;
        double Number = 0.0;
        std::string Text;
        std::vector<JsonValue> Items;
        std::map<std::string, JsonValue> Members;

        const JsonValue* Find(const std::string& Key) const
        {
            const auto Found = Members.find(Key);
            return Found == Members.end() ? nullptr : &Found->second;
        }
        bool IsString() const { return Type == Kind::String; }
        bool IsNumber() const { return Type == Kind::Number; }
        bool IsObject() const { return Type == Kind::Object; }
        bool IsArray() const { return Type == Kind::Array; }
    };

    class JsonReader
    {
    public:
        static bool Parse(std::string_view Text, JsonValue& Out, std::string& OutError)
        {
            JsonReader Reader(Text);
            if (!Reader.ParseValue(Out, 0))
            {
                OutError = Reader.Error.empty() ? "invalid JSON" : Reader.Error;
                return false;
            }
            Reader.SkipWhitespace();
            if (Reader.Position != Reader.Source.size())
            {
                OutError = "trailing characters after JSON value";
                return false;
            }
            return true;
        }

    private:
        explicit JsonReader(std::string_view Text) : Source(Text) {}

        std::string_view Source;
        std::size_t Position = 0;
        std::string Error;
        static constexpr int MaximumDepth = 128;

        void SkipWhitespace()
        {
            while (Position < Source.size() &&
                (Source[Position] == ' ' || Source[Position] == '\n' || Source[Position] == '\r' || Source[Position] == '\t'))
            {
                ++Position;
            }
        }

        bool Fail(const char* Message)
        {
            Error = std::string(Message) + " at offset " + std::to_string(Position);
            return false;
        }

        bool Consume(const std::string_view Literal)
        {
            if (Source.substr(Position, Literal.size()) != Literal)
            {
                return false;
            }
            Position += Literal.size();
            return true;
        }

        bool ParseValue(JsonValue& Out, const int Depth)
        {
            if (Depth > MaximumDepth)
            {
                return Fail("JSON nested too deeply");
            }
            SkipWhitespace();
            if (Position >= Source.size())
            {
                return Fail("unexpected end of JSON");
            }
            const char Next = Source[Position];
            if (Next == '{') return ParseObject(Out, Depth);
            if (Next == '[') return ParseArray(Out, Depth);
            if (Next == '"')
            {
                Out.Type = JsonValue::Kind::String;
                return ParseString(Out.Text);
            }
            if (Consume("true")) { Out.Type = JsonValue::Kind::Bool; Out.Boolean = true; return true; }
            if (Consume("false")) { Out.Type = JsonValue::Kind::Bool; Out.Boolean = false; return true; }
            if (Consume("null")) { Out.Type = JsonValue::Kind::Null; return true; }
            return ParseNumber(Out);
        }

        bool ParseObject(JsonValue& Out, const int Depth)
        {
            Out.Type = JsonValue::Kind::Object;
            ++Position;
            SkipWhitespace();
            if (Position < Source.size() && Source[Position] == '}')
            {
                ++Position;
                return true;
            }
            while (true)
            {
                SkipWhitespace();
                std::string Key;
                if (Position >= Source.size() || Source[Position] != '"' || !ParseString(Key))
                {
                    return Fail("expected object key");
                }
                SkipWhitespace();
                if (!Consume(":"))
                {
                    return Fail("expected ':'");
                }
                JsonValue Value;
                if (!ParseValue(Value, Depth + 1))
                {
                    return false;
                }
                if (!Out.Members.emplace(std::move(Key), std::move(Value)).second)
                {
                    return Fail("duplicate object key");
                }
                SkipWhitespace();
                if (Consume("}")) return true;
                if (!Consume(",")) return Fail("expected ',' or '}'");
            }
        }

        bool ParseArray(JsonValue& Out, const int Depth)
        {
            Out.Type = JsonValue::Kind::Array;
            ++Position;
            SkipWhitespace();
            if (Consume("]"))
            {
                return true;
            }
            while (true)
            {
                JsonValue Value;
                if (!ParseValue(Value, Depth + 1))
                {
                    return false;
                }
                Out.Items.push_back(std::move(Value));
                SkipWhitespace();
                if (Consume("]")) return true;
                if (!Consume(",")) return Fail("expected ',' or ']'");
            }
        }

        static void AppendUtf8(std::string& Out, const unsigned Code)
        {
            if (Code < 0x80) { Out += static_cast<char>(Code); }
            else if (Code < 0x800) { Out += static_cast<char>(0xC0 | (Code >> 6)); Out += static_cast<char>(0x80 | (Code & 0x3F)); }
            else if (Code < 0x10000)
            {
                Out += static_cast<char>(0xE0 | (Code >> 12));
                Out += static_cast<char>(0x80 | ((Code >> 6) & 0x3F));
                Out += static_cast<char>(0x80 | (Code & 0x3F));
            }
            else
            {
                Out += static_cast<char>(0xF0 | (Code >> 18));
                Out += static_cast<char>(0x80 | ((Code >> 12) & 0x3F));
                Out += static_cast<char>(0x80 | ((Code >> 6) & 0x3F));
                Out += static_cast<char>(0x80 | (Code & 0x3F));
            }
        }

        bool ParseHex4(unsigned& Out)
        {
            if (Position + 4 > Source.size()) return false;
            Out = 0;
            for (int Index = 0; Index < 4; ++Index)
            {
                const char Digit = Source[Position++];
                Out <<= 4;
                if (Digit >= '0' && Digit <= '9') Out |= static_cast<unsigned>(Digit - '0');
                else if (Digit >= 'a' && Digit <= 'f') Out |= static_cast<unsigned>(Digit - 'a' + 10);
                else if (Digit >= 'A' && Digit <= 'F') Out |= static_cast<unsigned>(Digit - 'A' + 10);
                else return false;
            }
            return true;
        }

        bool ParseString(std::string& Out)
        {
            ++Position; // opening quote
            while (Position < Source.size())
            {
                const char Character = Source[Position++];
                if (Character == '"') return true;
                if (static_cast<unsigned char>(Character) < 0x20) return Fail("control character in string");
                if (Character != '\\') { Out += Character; continue; }
                if (Position >= Source.size()) break;
                const char Escape = Source[Position++];
                switch (Escape)
                {
                case '"': Out += '"'; break;
                case '\\': Out += '\\'; break;
                case '/': Out += '/'; break;
                case 'b': Out += '\b'; break;
                case 'f': Out += '\f'; break;
                case 'n': Out += '\n'; break;
                case 'r': Out += '\r'; break;
                case 't': Out += '\t'; break;
                case 'u':
                {
                    unsigned Code = 0;
                    if (!ParseHex4(Code)) return Fail("invalid unicode escape");
                    if (Code >= 0xD800 && Code <= 0xDBFF)
                    {
                        unsigned Low = 0;
                        if (!Consume("\\u") || !ParseHex4(Low) || Low < 0xDC00 || Low > 0xDFFF) return Fail("invalid surrogate pair");
                        Code = 0x10000 + ((Code - 0xD800) << 10) + (Low - 0xDC00);
                    }
                    AppendUtf8(Out, Code);
                    break;
                }
                default: return Fail("invalid escape");
                }
            }
            return Fail("unterminated string");
        }

        bool ParseNumber(JsonValue& Out)
        {
            const std::size_t Start = Position;
            if (Position < Source.size() && Source[Position] == '-') ++Position;
            bool Digits = false;
            while (Position < Source.size() && ((Source[Position] >= '0' && Source[Position] <= '9') ||
                       Source[Position] == '.' || Source[Position] == 'e' || Source[Position] == 'E' ||
                       Source[Position] == '+' || Source[Position] == '-'))
            {
                Digits = Digits || (Source[Position] >= '0' && Source[Position] <= '9');
                ++Position;
            }
            if (!Digits) return Fail("invalid value");
            try
            {
                std::size_t Used = 0;
                const std::string Text(Source.substr(Start, Position - Start));
                Out.Number = std::stod(Text, &Used);
                if (Used != Text.size()) return Fail("invalid number");
            }
            catch (...)
            {
                return Fail("invalid number");
            }
            Out.Type = JsonValue::Kind::Number;
            return true;
        }
    };

    class JsonWriter
    {
    public:
        /** Compact when Indent < 0, otherwise pretty-printed with Indent spaces per level. */
        static std::string Write(const JsonValue& Value, const int Indent = -1)
        {
            std::string Out;
            WriteValue(Value, Out, Indent, 0);
            if (Indent >= 0) Out += '\n';
            return Out;
        }

        static void AppendString(std::string& Out, const std::string_view Text)
        {
            static constexpr char Hex[] = "0123456789abcdef";
            Out += '"';
            for (const char Character : Text)
            {
                switch (Character)
                {
                case '"': Out += "\\\""; break;
                case '\\': Out += "\\\\"; break;
                case '\n': Out += "\\n"; break;
                case '\r': Out += "\\r"; break;
                case '\t': Out += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(Character) < 0x20)
                    {
                        Out += "\\u00";
                        Out += Hex[(Character >> 4) & 0x0F];
                        Out += Hex[Character & 0x0F];
                    }
                    else
                    {
                        Out += Character;
                    }
                }
            }
            Out += '"';
        }

        /** Integers print without a fraction; everything else uses the shortest round-trip form. */
        static void AppendNumber(std::string& Out, const double Number)
        {
            if (!std::isfinite(Number))
            {
                Out += "null";
                return;
            }
            char Buffer[64];
            std::to_chars_result Result{};
            if (std::fabs(Number) < 9007199254740992.0 && Number == std::floor(Number))
                Result = std::to_chars(Buffer, Buffer + sizeof(Buffer), static_cast<long long>(Number));
            else
                Result = std::to_chars(Buffer, Buffer + sizeof(Buffer), Number);
            Out.append(Buffer, Result.ptr);
        }

    private:
        static void NewLine(std::string& Out, const int Indent, const int Depth)
        {
            if (Indent < 0) return;
            Out += '\n';
            Out.append(static_cast<std::size_t>(Indent * Depth), ' ');
        }

        static void WriteValue(const JsonValue& Value, std::string& Out, const int Indent, const int Depth)
        {
            switch (Value.Type)
            {
            case JsonValue::Kind::Null: Out += "null"; break;
            case JsonValue::Kind::Bool: Out += Value.Boolean ? "true" : "false"; break;
            case JsonValue::Kind::Number: AppendNumber(Out, Value.Number); break;
            case JsonValue::Kind::String: AppendString(Out, Value.Text); break;
            case JsonValue::Kind::Array:
            {
                Out += '[';
                bool First = true;
                for (const JsonValue& Item : Value.Items)
                {
                    if (!First) Out += ',';
                    First = false;
                    NewLine(Out, Indent, Depth + 1);
                    WriteValue(Item, Out, Indent, Depth + 1);
                }
                if (!Value.Items.empty()) NewLine(Out, Indent, Depth);
                Out += ']';
                break;
            }
            case JsonValue::Kind::Object:
            {
                Out += '{';
                bool First = true;
                for (const auto& [Key, Member] : Value.Members)
                {
                    if (!First) Out += ',';
                    First = false;
                    NewLine(Out, Indent, Depth + 1);
                    AppendString(Out, Key);
                    Out += Indent >= 0 ? ": " : ":";
                    WriteValue(Member, Out, Indent, Depth + 1);
                }
                if (!Value.Members.empty()) NewLine(Out, Indent, Depth);
                Out += '}';
                break;
            }
            }
        }
    };
}
