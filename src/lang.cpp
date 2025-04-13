#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "scan/scanner.h"
#include "tools/log.h"
#include "expr/expr.h"
#include "tools/ast_printer.h"
#include "tools/prn_visitor.h"
#include "tools/token_print.h"
#include "parse/parser.h"

namespace opt = boost::program_options;

using namespace std;

int main(int argc, char* argv[]) {
    opt::options_description desc("All options");

    Scanner sc;
    string src = sc.read_file("/home/gcreep/github.local/lang-cpp/src/test.txt");
    
    printf("src='%s'\n", src.c_str());

    auto toks = sc.get_tokens(src);
    tools::print_tokens(toks, cout, ' ');

    Parser ps(toks);

    Expr* ex = ps.parse();
    if (ex != NULL) {
        AstPrinter as;
        auto s = as.print(ex);
        cout << s.c_str() << '\n';
    }

    


    //AstPrinter pr;

        /*
    PrnVisitor v;
    string sss = v.make_prn(ex);    
    cout << sss.c_str();

    string prm_name;
    
    desc.add_options()
        ("file", opt::value<string>(), "file to be executed.")
        ("help", "help msg");

    // parsing opts
    opt::variables_map opt_map;
    opt::store(opt::parse_command_line(argc, argv, desc), opt_map);
    opt::notify(opt_map);

    if (opt_map.count("file")) {
        //printf("bad!\n");
    }
    
    if (opt_map.count("help")) {
        std::cout << desc;
    }
*/
}