#ifndef CODE_CLIP_PLUGIN_H
#define CODE_CLIP_PLUGIN_H
#include "clang/Lex/PPCallbacks.h"
#include "llvm/ADT/SmallVector.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Pragma.h"

namespace clang {
// Forward declarations
class Token;
class MacroDirective;
class IdentifierInfo;
class SourceManager;
class Preprocessor;
class SourceLocation;
class MacroDefinition;
} // end namespace clang

class CodeClipCallbacks: public clang::PPCallbacks {
 // clang::SourceManager &SM;

public:
    //       CodeClipCallbacks() : SM(nullptr) {
    //     // 构造函数的逻辑...
    // }

      // void If(clang::Preprocessor &PP, clang::PragmaIntroducerKind Introducer,
      //                       clang::Token &FirstToken) override ;

      void Ifdef(clang::SourceLocation loc, const clang::Token& id,
             const clang::MacroDefinition& definition) override;

};

#endif
