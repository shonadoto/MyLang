#include "Compiler.h"

int main(int argc, char* argv[]) {

    try {
        if (argc > 1) throw "Too many arguments. 1 needed";
        if (argc < 1) throw "Not enough arguments. 1 needed";
        std::string compiler_path = argv[0];
        while (compiler_path[compiler_path.size() - 1] != '\\') compiler_path.pop_back();
        compiler_path.pop_back();

        std::string path;
        std::cout << "Enter path to file:\n";
        std::cin >> path;
        Compiler compiler(new Path(path, 1), new Path(compiler_path, 1));
        compiler.workLecsical();
        compiler.workSyntax();
        compiler.workSema();
        compiler.workRpnCreator();
        compiler.workExecutor();
    }
    catch (std::string error) {
        std::cout << error;
    }
    std::cout << '\n';
    system("pause");
    return 0;
}
