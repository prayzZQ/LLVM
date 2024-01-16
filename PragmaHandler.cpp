#include "PragmaHandler.h"

using namespace clang;

//#pragma code_clip("/path/to/target/file")
//对以上代码进行解析

void CodeClipPragma::HandlePragma(Preprocessor &PP, PragmaIntroducer Introducer,
                                    Token &PragmaTok) 
{

  llvm::outs() << "Token Text: " << PragmaTok.getName() << "\n";

    // 创建 SmallVector 以存储 #pragma 指令的令牌
    SmallVector<Token, 16> Tokens;

    // 手动调用 Lex 函数逐个获取令牌，直到语句的结束
    Token Tok;
    do {
        PP.Lex(Tok);
        Tokens.push_back(Tok);
    } while (Tok.isNot(tok::eod));

    SmallString<64> SpellingBuffer;
    for (const Token &Tok : Tokens) {
        llvm::StringRef Spelling = PP.getSpelling(Tok, SpellingBuffer);
        SpellingBuffer.append(Spelling);
    }

    llvm::StringRef Spelling = SpellingBuffer.str();  
    llvm::outs() << Spelling << "----------------\n";

    // 判断是否以预期的 (" 开头
    if (Spelling.startswith("(")) {
        // 提取路径字符串
        size_t StartPos = 2; // 跳过前缀
        size_t EndPos = Spelling.find_last_of('"');
        if (EndPos != llvm::StringRef::npos) {
            llvm::StringRef PathString = Spelling.slice(StartPos, EndPos);
            llvm::outs() << "Path String: " << PathString << "\n";
            parseFile(PathString.str());
        } else {
           llvm::outs() << "Failed to extract path from #pragma\n";
        }
    } else {
        llvm::outs() << "Unexpected #pragma format\n";
    }

    


  // Reset the to-be-enclosed argument list
  // ArgsToEnclosed.clear();

  // Token Tok;
  // PP.Lex(Tok);
  // while (Tok.isNot(tok::eod)) {
  //   if (auto *II = Tok.getIdentifierInfo()) {
  //     ArgsToEnclosed.push_back(II);
  //   }
  //   PP.Lex(Tok);
  // }

  // if (!IsValidatorRegistered) {
  //   // Register the validator PPCallbacks
  //   auto Validator = std::make_unique<MacroGuardValidator>(PP.getSourceManager());
  //   PP.addPPCallbacks(std::move(Validator));
  //   IsValidatorRegistered = true;
  // }


}

//std::unorderd_map<std::string, int> CodeClipPragma::parseFile(const std::string& filename)
int CodeClipPragma::parseFile(const std::string& filename)
{
    std::unordered_map<std::string, int> config;

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        int value;
        std::cout << "key :" << key << " value :" <<value << "\n";
        if (iss >> key >> value)
        {
            config[key] = value;
        }
    }

    return 1;

}


void CodeClipPragma::MarcoLogicHandler()  //  宏定义组合逻辑处理函数
{

  // ExternalConfigParser configParser;
  // std::unordered_map<std::string, int> config = configParser.parseFile("config.txt");

  // // Use MY_CUSTOM_PRAGMA to conditionally include or exclude code
  // if (config["FEATURE_A_ENABLED"] == 1) {
  //     MY_CUSTOM_PRAGMA(FEATURE_A_ENABLED)
  //     {
  //         std::cout << "Feature A is enabled.\n";
  //     }
  // }

  // if (config["FEATURE_B_ENABLED"] == 1) {
  //     MY_CUSTOM_PRAGMA(FEATURE_B_ENABLED)
  //     {
  //         std::cout << "Feature B is enabled.\n";
  //     }
  // }


}

static PragmaHandlerRegistry::Add<CodeClipPragma> X("code_clip", "");