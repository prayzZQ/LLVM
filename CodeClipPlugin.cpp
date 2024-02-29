#include "CodeClipPlugin.h"

using namespace clang;
using namespace clang::tooling;

#define CLIPTHRESHOLD 1

//  配置文件解析功能
std::unordered_map<std::string, int> parseFile(const std::string& filename)
{

    std::unordered_map<std::string, int> config;
    //  创建文件流
    std::ifstream file(filename);
    std::string line;

    //  逐行读取文件内容
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        int value;        
        if (iss >> key >> value)
        {
            std::cout << "key :" << key << " value :" <<value << "\n";
            config[key] = value;
        }
    }

    return config;
}

bool MarcoLogicHandler(const std::unordered_map<std::string, int>& config) {

  int CodeClipFlag = 0;

  // 遍历map中的值
  for (const auto& pair : config) {
      //  暂定：累加宏定义值
      CodeClipFlag += pair.second;
   
  }

  std::cout << "CodeClipFlag: " << CodeClipFlag  << "\n";

  if (CodeClipFlag >= CLIPTHRESHOLD) //  暂定：对大于阈值的进行裁剪 小于则不进行裁剪
  {
    return 1;
  }
  else
  {
    return 0;
  }

}





// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}

  static std::string InputValue;
  static std::string OutputValue;

  static bool        ClipFlag;


  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
 
  //  获取当前处理的文件名
  currentFilePath = file.str();

  InputValue = currentFilePath;

  llvm::outs() << "当前处理文件路径: " << currentFilePath << "\n";

  std::unordered_map<std::string, int>  ConfigMap = parseFile(MyFrontendAction::InputValue);

  MyFrontendAction::ClipFlag = MarcoLogicHandler(ConfigMap);

  SourceManager &SM = CI.getSourceManager();

  LangOptions &langOptions = CI.getLangOpts();

  ASTContext &astContext = CI.getASTContext();

  Preprocessor &PP = CI.getPreprocessor();
  
  PP.addPPCallbacks(std::make_unique<CodeClipCallbacks>(PP , &astContext, SM, langOptions));

  return std::make_unique<ASTConsumer>();

}


private:
  std::string currentFilePath;

};





void clang::CodeClipCallbacks::If(SourceLocation Loc, SourceRange ConditionRange,
          ConditionValueKind ConditionValue) 
{
  // 获取 ConditionRange 的源代码片段
  clang::SourceLocation StartLoc = ConditionRange.getBegin();
  clang::SourceLocation EndLoc = ConditionRange.getEnd();
  clang::SourceLocation StartFileLoc = SM.getSpellingLoc(StartLoc);
  clang::SourceLocation EndFileLoc = SM.getSpellingLoc(EndLoc);
  

  StringRef conditionCode = Lexer::getSourceText(CharSourceRange::getTokenRange(ConditionRange), SM, LangOpts, 0);

  std::string targetFeature = "FEATURE_A";
  std::string replacement = "1";

  size_t found = conditionCode.str().find(targetFeature);

  // 使用 find 函数查找目标宏定义
  if (found != std::string::npos) {
    // 计算 targetFeature 所在的行数
    unsigned line = SM.getSpellingLineNumber(StartFileLoc) +
                    conditionCode.substr(0, found).count('\n');

    std::cout << "行数 " << line << std::endl;

    // 输出源代码片段
    std::cout << " #if conditionCod before:" << conditionCode.str() << std::endl;

    std::cout << "包含特性 " << targetFeature << std::endl;

    std::string newconditionCode = conditionCode.str().replace(found, targetFeature.length(), replacement);

    std::cout << " #if conditionCod after:" << newconditionCode << std::endl;

    //  读取目标文件
    // std::string infilename = "/home/zq/桌面/test0229/fzz.cpp";
    char current_path[256];
    getcwd(current_path, sizeof(current_path));
    std::string current_path_str(current_path);

    std::cout << "当前工作路径 " << current_path_str << std::endl;

    std::string infilename = MyFrontendAction::OutputValue;

    std::ifstream inputFile(infilename);

    // 检查文件是否成功打开
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << infilename << std::endl;
        return ;
    }
    std::vector<std::string> lines;
    std::string linedata;

    //  逐行读取文件内容并保存到vector中
    while (std::getline(inputFile, linedata)) 
    {
          lines.push_back(linedata);
    }

      std::cout << " lines.size(): " << lines.size()<< std::endl;

    // 检查要替换的行数是否有效
    if (line <= 0 || line > lines.size()) {
        std::cerr << "Invalid line number to replace." << std::endl;
        return ;
    }              

  //  替换第N行的单词
  std::string updatedLine;
  std::string word;

  //  第N行是否存在目标宏定义
  int found_flag = lines[line - 1].find(targetFeature);

  //  存在即替换
  updatedLine = lines[line - 1].replace(found_flag, targetFeature.length(), replacement);

  std::cout << " replaced string L" << updatedLine<< std::endl;


  //  更新替换后的行
  lines[line - 1] = updatedLine;

  //  关闭文件
  inputFile.close();

  //  重新打开文件并写入更新后的内容
   
  // std::string outinfilename = "/home/zq/桌面/test0229/fzz.cpp";
  std::string outinfilename = MyFrontendAction::OutputValue;

  std::ofstream outputFile(outinfilename);
  std::ostringstream outputline;
  llvm::outs() << "当前输出文件: " << outinfilename << "\n";

  if (outputFile.is_open())
  {
      for (int i = 0; i < lines.size(); i++)
      {
        outputFile << lines[i] << '\n';

      }
    std::cout << "File updated successfully" << std::endl;
    outputFile.close();

  }
  else
  {
    std::cout << "Unable to open output file" << std::endl;

    return;
  }



  } 
  else {

      std::cout << "不包含特性 " << targetFeature << std::endl;

  }
}


// 在全局范围定义
std::string MyFrontendAction::InputValue;
std::string MyFrontendAction::OutputValue;
bool        MyFrontendAction::ClipFlag;

static  llvm::cl::OptionCategory CodeClipCategory("CodeClip Matcher Sample");

// // 添加自定义命令行选项
 static llvm::cl::opt<std::string> Output("out-put", llvm::cl::desc("Description of input") , llvm::cl::cat(CodeClipCategory));

// // llvm::cl::list该选型接受多个字符串参数 并将这些值存储在一个列表中 位置参数 并且至少提供一个参数 
// static llvm::cl::list<std::string> InputFiles("InputFiles", llvm::cl::Positional, llvm::cl::desc("<input files or folders>"), llvm::cl::cat(CodeClipCategory));

int main(int argc, const char **argv) {

  //使用 CommonOptionsParser 类创建一个解析器对象，该解析器用于处理常见的命令行选项（例如 -help 和 -version），
  //并将剩余的命令行参数与自定义的 MatcherSampleCategory 进行关联
  auto ExpectedOptionsParser = CommonOptionsParser::create(argc, argv, CodeClipCategory);

  // 添加自定义命令行选项到命令行解析器
  CommonOptionsParser optionsParser = std::move(*ExpectedOptionsParser);

   //创建一个 ClangTool 对象，该对象用于运行Clang工具，并将编译配置和源文件路径列表传递给它
  ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    
  // 设置自定义参数
  MyFrontendAction::OutputValue = Output.getValue();

  std::cout << "输出文件： " << MyFrontendAction::OutputValue << std::endl;

  return tool.run(newFrontendActionFactory<MyFrontendAction>().get());

}




