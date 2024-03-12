#include "CodeClipPlugin.h"

using namespace clang;
using namespace clang::tooling;

// ASTFrontendAction类的实现
// EndSourceFileAction BeginSourceFileAction源文件处理前后动作 CreateASTConsumer自定义动作 实现
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}

  static std::string InputValue;  //  输入文件
  static std::string OutputValue;  //  输出文件
  static std::string ConfigValue;  //  配置文件
  static std::string MarcoValue;  //  待处理宏定义
  static std::vector<std::string> lines;  //  待处理文件内容
  static bool        ClipFlag;  //  待处理标志

  //  自定义处理函数
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
 
  //  获取当前处理的文件名
  std::string currentFilePath = file.str();

  InputValue = currentFilePath;

  llvm::outs() << "当前处理文件路径: " << currentFilePath << "\n";

  //  读取配置文件
  std::unordered_map<std::string, int>  ConfigMap = parseFile(ConfigValue); //  home/zq/桌面/test0305/MarcoConfig.txt

  //  配置文件数据处理
  MyFrontendAction::ClipFlag = MarcoLogicHandler(ConfigMap);

  llvm::outs() << "待处理宏: " << MyFrontendAction::MarcoValue << "\n";
  llvm::outs() << "待处理宏定义修改值: " << MyFrontendAction::ClipFlag << "\n";

  SourceManager &SM = CI.getSourceManager();

  LangOptions &langOptions = CI.getLangOpts();

  ASTContext &astContext = CI.getASTContext();

  Preprocessor &PP = CI.getPreprocessor();
  
  //  编译前端回调函数注册
  PP.addPPCallbacks(std::make_unique<CodeClipCallbacks>(PP , &astContext, SM, langOptions));

  return std::make_unique<ASTConsumer>();

  }

  //  源文件处理前动作--对输入文件内容进行读取
  bool BeginSourceFileAction(CompilerInstance &CI) {

    llvm::outs() << "--------- BeginSourceFileAction  ---------\n";

    // 检查 vector 是否为空
    if (!lines.empty()) {
        lines.clear();
    }

    //  读取目标文件
    // std::string infilename = "/home/zq/桌面/test0229/fzz.cpp";
    char current_path[256];
    getcwd(current_path, sizeof(current_path));
    std::string current_path_str(current_path);

    std::string infilename = MyFrontendAction::OutputValue; 

    std::ifstream inputFile(infilename);

    // 检查文件是否成功打开
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << infilename << std::endl;
        return false;
    }
    
    std::string linedata;

    //  逐行读取文件内容并保存到vector中
    while (std::getline(inputFile, linedata)) 
    {
          lines.push_back(linedata);
    }

    std::cout << " lines.size(): " << lines.size()<< std::endl;

    //  关闭文件
    inputFile.close();

    return true;
  }

  //  源文件处理后动作--对输出文件内容进行修改
  void EndSourceFileAction() override {

    llvm::outs() << "--------- EndSourceFileAction  ---------\n";
 
    //  打开输出文件并写入更新后的内容
    std::string outinfilename = MyFrontendAction::OutputValue;

    std::ofstream outputFile(outinfilename);
    std::ostringstream outputline;
    llvm::outs() << "当前输出文件: " << outinfilename << "\n";

    //  检查文件是否打开
    if (outputFile.is_open())
    {
        for (int i = 0; i < lines.size(); i++)
        {
          //  逐行写入文件
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


};

void clang::CodeClipCallbacks::If(SourceLocation Loc, SourceRange ConditionRange,
          ConditionValueKind ConditionValue) 
{
  // 获取条件编译#if所包含的源代码片段
  clang::SourceLocation StartLoc = ConditionRange.getBegin();
  clang::SourceLocation EndLoc = ConditionRange.getEnd();
  clang::SourceLocation StartFileLoc = SM.getSpellingLoc(StartLoc);
  clang::SourceLocation EndFileLoc = SM.getSpellingLoc(EndLoc);
  
  //  转化为字符串
  StringRef conditionCode = Lexer::getSourceText(CharSourceRange::getTokenRange(ConditionRange), SM, LangOpts, 0);

  //  读取静态变量的目标宏定义值和待修改值
  std::string targetFeature = MyFrontendAction::MarcoValue; // todo 改成各个目标的判断
  std::string replacement = std::to_string(MyFrontendAction::ClipFlag);

  size_t found = conditionCode.str().find(targetFeature);

  // 使用 find 函数查找目标宏定义
  if (found != std::string::npos) {
    // 计算 targetFeature 所在的行数
    unsigned line = SM.getSpellingLineNumber(StartFileLoc) +
                    conditionCode.substr(0, found).count('\n');

    std::cout << "目标宏定义行数 " << line << std::endl;

    // 输出源代码片段
    std::cout << "处理前代码:  " << conditionCode.str() << std::endl;

    std::string newconditionCode = conditionCode.str().replace(found, targetFeature.length(), replacement);

    std::cout << "处理后预期:  " << newconditionCode << std::endl;
   
  // 检查要替换的行数是否有效
  if (line <= 0 || line > MyFrontendAction::lines.size()) {
      std::cerr << "Invalid line number to replace." << std::endl;
      return ;
  }              

  //  替换第N行的单词
  std::string updatedLine;
  std::string word;

  //  第N行是否存在目标宏定义
  int found_flag = MyFrontendAction::lines[line - 1].find(targetFeature);

  //  存在即替换
  updatedLine = MyFrontendAction::lines[line - 1].replace(found_flag, targetFeature.length(), replacement);

  //  更新替换后的行
  MyFrontendAction::lines[line - 1] = updatedLine;

  } 
  else {

//      std::cout << "不包含特性 " << targetFeature << std::endl;

  }
}


//  配置文件参数读取
std::unordered_map<std::string, int> parseFile(const std::string& filename)
{
    //  待读取键值对
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
        if (iss >> key >> value)  //  逐行读取键值
        {
            std::cout << "key :" << key << " value :" <<value << "\n";
            config[key] = value;
        }
    }

    return config;
}

//  配置文件参数解析
bool MarcoLogicHandler(const std::unordered_map<std::string, int>& config) {

  bool MajorFlag = 1;
  bool MinorFlag = 1;

  // 遍历map中的值
  for (const auto& pair : config) {

      if (pair.first == "FEATURE")  //  读取总体配置值
      {
        // std::cout << "pair.first == FEATURE" <<  "\n";
        if (pair.second == 1)
        {
          MajorFlag = 1;
          // std::cout << "MajorFlag : 1" << "\n";
        }
        else
        {
          MajorFlag = 0;
          // std::cout << "MajorFlag : 0" << "\n";
        }
      }

      if (pair.first != "FEATURE")  //  读取目标宏定义配置值
      {
        MyFrontendAction::MarcoValue = pair.first;
        // std::cout << "MarcoValue : "  << MyFrontendAction::MarcoValue<<  "\n";

        if (pair.second == 1)
        {
          MinorFlag = 1;
          // std::cout << "MinorFlag : 1" << "\n";
        }
        else
        {
          MinorFlag = 0;
          // std::cout << "MinorFlag : 0" << "\n";
        }

      }    
  }

  if (MajorFlag == 1) //  总体配置值
  {
    if (MinorFlag == 1) //  宏定义配置值
    {
      return 1;
    }else
    {
      return 0;
    }
  }else // 全部裁剪
  {
    return 0;

  }

}


// 在全局范围定义
std::string MyFrontendAction::InputValue;
std::string MyFrontendAction::OutputValue;
std::string MyFrontendAction::ConfigValue;
std::string MyFrontendAction::MarcoValue;
std::vector<std::string> MyFrontendAction::lines;
bool        MyFrontendAction::ClipFlag = 1;

//  自定义命令行类
static  llvm::cl::OptionCategory CodeClipCategory("CodeClip Matcher Sample");

// // 添加自定义命令行选项
static llvm::cl::opt<std::string> Output("out-put", llvm::cl::desc("Description of input") , llvm::cl::cat(CodeClipCategory));

// // llvm::cl::list该选型接受多个字符串参数 并将这些值存储在一个列表中 位置参数 并且至少提供一个参数 
static llvm::cl::opt<std::string> ConfigFiles("config-path", llvm::cl::desc("config file path"), llvm::cl::cat(CodeClipCategory));

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
  MyFrontendAction::ConfigValue = ConfigFiles.getValue();  

  std::cout << "输出文件： " << MyFrontendAction::OutputValue << std::endl;
  std::cout << "配置文件： " << MyFrontendAction::ConfigValue << std::endl;

  return tool.run(newFrontendActionFactory<MyFrontendAction>().get());

}




