#ifndef CODE_CLIP_PLUGIN_H
#define CODE_CLIP_PLUGIN_H
#include "clang/Lex/PPCallbacks.h"
#include "llvm/ADT/SmallVector.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Pragma.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/ModuleLoader.h"
#include "llvm/ADT/StringRef.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <filesystem> // 添加这个头文件以使用C++17文件系统支持
#include <unistd.h>  // 包含用于获取当前工作目录的头文件

namespace clang {
// Forward declarations
class Token;
class MacroDirective;
class IdentifierInfo;
class SourceManager;
class Preprocessor;
class SourceLocation;
class MacroDefinition;
class SourceRange;
class ConditionValueKind;


class CodeClipCallbacks: public clang::PPCallbacks {
 // clang::SourceManager &SM;
private:
  clang::Preprocessor &PP;

  clang::ASTContext *ASTContextPtr;
  clang::SourceManager &SM;
  clang::LangOptions &LangOpts;

  // clang::Rewriter Rewrite;

public:

CodeClipCallbacks(clang::Preprocessor &PP, clang::ASTContext *ASTContextPtr,
                  clang::SourceManager &SM, clang::LangOptions &LangOpts)
    : PP(PP),  ASTContextPtr(ASTContextPtr), SM(SM), LangOpts(LangOpts) {
    // 在构造函数中进行初始化
}

  // void MacroDefined(const clang::Token &MacroNameTok,
  //                   const clang::MacroDirective *MD) override;

                      // Override EndOfMainFile callback


  /// Hook called whenever an \#if is seen.
  /// \param Loc the source location of the directive.
  /// \param ConditionRange The SourceRange of the expression being tested.
  /// \param ConditionValue The evaluated value of the condition.
  ///
  // FIXME: better to pass in a list (or tree!) of Tokens.
  void If(SourceLocation Loc, SourceRange ConditionRange,
          ConditionValueKind ConditionValue) override;


};

} // end namespace clang
#endif
