#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "scan/scanner.h"
#include "lang.h"
#include "expr/expr.h"
#include "tools/ast_printer.h"
#include "tools/prn_visitor.h"
#include "tools/token_print.h"

namespace opt = boost::program_options;

using namespace std;

int main(int argc, char* argv[]) {
    opt::options_description desc("All options");

    string src("123-50;g=apple-50\n");
    Scanner sc;
    auto toks = sc.get_tokens(src);
    tools::print_tokens(toks, cout, ' ');


    Expr* ex = new Binary(new Literal(new Token("123", LITERAL_INT, nullptr, 0)),
        new Literal(new Token("402", LITERAL_INT, nullptr, 0)),
        new Token("-", MINUS, nullptr, 0));

    AstPrinter as;
    auto s = as.print(ex);
    cout << s.c_str();

    PrnVisitor v;
    string sss = v.make_prn(ex);    
    cout << sss.c_str();
    //AstPrinter pr;

        /*
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