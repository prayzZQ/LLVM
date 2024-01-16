#include "CodeClipPlugin.h"
#include "PragmaHandler.h"

using namespace clang;

//extern llvm::SmallVector<const clang::IdentifierInfo*, 2> ArgsToEnclosed;

void CodeClipCallbacks::Ifdef(clang::SourceLocation loc, const clang::Token& id,
             const clang::MacroDefinition& definition)
{

  if (1)
  {
     llvm::errs() << "Code Clipping\n";

  }
  else
  {
     llvm::errs() << "Code Saving\n";

  }
}

class AlwaysBeforeConsumer :public clang::ASTConsumer, public CodeClipCallbacks {
public:

  void HandleTranslationUnit(ASTContext &) override {
    llvm::errs() << "always-before\n";
  }
};


class MyPlugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    // 返回自定义的ASTConsumer，其中包含了你编写的PPCallbacks
    return std::make_unique<AlwaysBeforeConsumer>();
    
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {

    // 创建一个 PragmaNamespace 对象
    clang::PragmaNamespace NS("my_custom_pragma_namespace");

// /// Add the specified pragma handler to this preprocessor.
// ///
// /// If \p Namespace is non-null, then it is a token required to exist on the
// /// pragma line before the pragma string starts, e.g. "STDC" or "GCC".
// void AddPragmaHandler(StringRef Namespace, PragmaHandler *Handler);
// void AddPragmaHandler(PragmaHandler *Handler) {
//   AddPragmaHandler(StringRef(), Handler);
// }

    CI.getPreprocessor().AddPragmaHandler(new CodeClipPragma());

    // 返回 true 表示参数解析成功
    return true;
  }
};

static FrontendPluginRegistry::Add<MyPlugin>
    X("code-clip-plugin", "My Clang Plugin");

// void MacroGuardValidator::MacroDefined(const Token &MacroNameTok,
//                                        const MacroDirective *MD) {
//   const auto *MI = MD->getMacroInfo();
//   if (MI->tokens_empty()) return;

//   for (const auto *ArgII : ArgsToEnclosed) {
//     // First, check if this macro really has this argument
//     if (llvm::none_of(MI->params(), [ArgII](const IdentifierInfo *II) {
//                                       return ArgII == II;
//                                     })) {
//       auto *MacroNameII = MacroNameTok.getIdentifierInfo();
//       assert(MacroNameII);
//       auto NameTokLoc = MacroNameTok.getLocation();
//       llvm::errs() << "[WARNING] Can't find argument '" << ArgII->getName() << "' ";
//       llvm::errs() << "at macro '" << MacroNameII->getName() << "'(";
//       llvm::errs() << NameTokLoc.printToString(SM) << ")\n";
//       continue;
//     }

//     for (auto TokIdx = 0U, TokSize = MI->getNumTokens();
//          TokIdx < TokSize; ++TokIdx) {
//       auto CurTok = *(MI->tokens_begin() + TokIdx);
//       if (CurTok.getIdentifierInfo() == ArgII) {
//         // Check if previous and successor Tokens are parenthesis
//         if (TokIdx > 0 && TokIdx < TokSize - 1) {
//           auto PrevTok = *(MI->tokens_begin() + TokIdx - 1),
//                NextTok = *(MI->tokens_begin() + TokIdx + 1);
//           if (PrevTok.is(tok::l_paren) && NextTok.is(tok::r_paren))
//             continue;
//         }
//         // The argument is not enclosed
//         auto TokLoc = CurTok.getLocation();
//         llvm::errs() << "[WARNING] In " << TokLoc.printToString(SM) << ": ";
//         llvm::errs() << "macro argument '" << ArgII->getName()
//                      << "' is not enclosed by parenthesis\n";
//       }
//     }
//   }

//   // Also clear the storage after one check
//   ArgsToEnclosed.clear();
// }



