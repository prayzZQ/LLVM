#ifndef PRAGMA_HANDLER_H
#define PRAGMA_HANDLER_H
#include "clang/Lex/PPCallbacks.h"
#include "llvm/ADT/SmallVector.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Pragma.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/TokenLexer.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <fstream>
#include <iostream>
#include <sstream>



namespace clang {
// Forward declarations
class Token;
class MacroDirective;
class IdentifierInfo;
class SourceManager;
class Preprocessor;
class PragmaIntroducer;
} // end namespace clang

//llvm::SmallVector<const clang::IdentifierInfo*, 2> ArgsToEnclosed;

class CodeClipPragma : public clang::PragmaHandler {
  bool IsValidatorRegistered;

public:
  CodeClipPragma() : clang::PragmaHandler("code_clip"),
                       IsValidatorRegistered(false) {}

  void HandlePragma(clang::Preprocessor &PP, clang::PragmaIntroducer Introducer,
                    clang::Token &PragmaTok) override;

  int parseFile(const std::string& filename);   //  外部文件解析函数

  void MarcoLogicHandler();  //  宏定义组合逻辑处理函数
};


#endif
