Antlr runtime:  sudo apt-get install libantlr3c-dev

Run AntlrWorks: java -jar antlrworks-1.5.2-complete.jar

Compile Grammar: java -jar antlr-3.2.jar ExprCppTree.g

Compile code:  g++ -o expr ExprCppTree.cpp ExprCppTreeLexer.c ExprCppTreeParser.c -lantlr3c

