#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include <vector>

static const std::vector<std::string> formula = {
    "Src -> Imports Global FunctionDefines Main",
    // 定义开始
    "Imports -> Import Imports",
    "Imports -> ε",
    "Import -> import < identifier >",
    // import
    "Main -> main ( Parameters ) { Statements }",
    "Global -> global ( Parameters ) { Statements }",
    "Global -> ε",
    // main和global
    "FunctionDefines -> FunctionDefine FunctionDefinePrime",
    "FunctionDefine -> ε",
    "FunctionDefinePrime -> ε",
    "FunctionDefinePrime -> FunctionDefine",

    "FunctionDefine -> fn identifier ( Parameters ) { Statements }",
    // funcdef func返回值由return定义
    "Statements -> ε",
    "Statements -> Statement Statements",

    "Statement -> return Expression ;",
    // statement结构
    "Parameters -> ε",
    "Parameters -> ParametersFactor ParameterPrime",
    "ParameterPrime -> ε",
    "ParameterPrime -> , ParameterFactor",

    "ParametersFactor -> TypeFull Assignment ",
    "ParametersFactor -> Container Assignment ",
    // parameters
    "Statement ->let Declear ;",
    "Declear -> TypeFull Assignments ",
    "Declear -> Container Assignments ",
    // 声明 declear
    "Statement -> Assignments ;",
    "Assignments -> Assignment AssignmentsPrime",
    "AssignmentsPrime -> ε",
    "AssignmentsPrime -> , Assignment",

    "Assignment -> identifier = Expression",
    // 赋值 assignments 不自动空值
    "Statement -> using identifier ;",
    // 定义命名规则
    "TypeFull -> const TypeBase",
    "TypeFull -> static TypeBase",
    "TypeFull -> TypeBase",
    "TypeBase -> identifier TypeBasePrime",
    "TypeBase -> Type TypeBasePrime",
    "TypeBasePrime -> ε",
    "TypeBasePrime -> &",
    "TypeBasePrime -> *",

    "Type -> byte",
    "Type -> char",
    "Type -> short",
    "Type -> int",
    "Type -> long",
    "Type -> float",
    "Type -> double",
    "Type -> boolean",
    // 定义类型规则
    "Container -> ContainerType < TypeFull > ",
    "ContainerType -> vector",
    "ContainerType -> map",
    "ContainerType -> set",
    "ContainerType -> stack",
    "ContainerType -> queue",
    "ContainerType -> deque",
    // 定义容器规则
    "Statement -> ostream :: identifier OStreams ;",
    "OStream -> << Expression",
    "OStreams -> ε",
    "OStreams -> OStream OStreams",

    // 输出流
    "Statement -> istream :: identifier IStreams ;",
    "IStream -> >> identifier",
    "IStreams -> ε",
    "IStreams -> IStream IStreams",

    // 输入流
    "Statement -> if ( Condition ) { Statements } ElseStatement",
    "ElseStatement -> ε",
    "ElseStatement -> else { Statements } ",

    // 定义if语句
    "Statement -> while ( Condition ) do { Statements }",
    "Statement -> break ;",
    "Statement -> continue ;",
    // while do
    "Statement -> switch ( Expression ) { CaseStatements }",
    "CaseStatement -> case Expression : Statements",
    "CaseStatement -> default : Statements",
    "CaseStatements -> ε",
    "CaseStatements -> CaseStatement CaseStatements",

    // switch case
    "Statement -> for ( ForStatement ) { Statements }",
    "ForStatement -> ε",
    "ForStatement -> Declear ; Condition ; Expression",
    "ForStatement -> auto identifier : identifier",

    // for语
    "Expressions -> ε",
    "Expressions -> Expression ExpressionsPrime",
    "ExpressionsPrime -> ε",
    "ExpressionsPrime -> , Expression ExpressionsPrime",

    "Expression -> Term ExpressionPrime",
    "ExpressionPrime -> ε",
    "ExpressionPrime -> + Term ExpressionPrime",
    "ExpressionPrime -> - Term ExpressionPrime",

    "Term -> Factor TermPrime",
    "TermPrime -> ε",
    "TermPrime -> * Factor TermPrime",
    "TermPrime -> / Factor TermPrime",

    "Factor -> null",
    "Factor -> intNumber ",
    "Factor -> floatNumber ",
    "Factor -> string ",
    "Factor -> ! Expression",
    "Factor -> ( Expressions )",
    "Factor -> identifier FactorPrime",
    "FactorPrime -> ε",
    "FactorPrime -> . identifier FactorPrime",
    "FactorPrime -> -> identifier FactorPrime",
    "FactorPrime -> [ Expression ] FactorPrime",
    "FactorPrime -> ( Expressions ) FactorPrime",

    // 定义表达式Condition
    "Condition -> ConditionTermPrime ExpressionsPrime",
    "ExpressionsPrime -> && ConditionTermPrime ExpressionsPrime",
    "ExpressionsPrime -> || ConditionTermPrime ExpressionsPrime",

    "ConditionTermPrime -> Expression ConditionTerm",
    "ConditionTerm -> ε",
    "ConditionTerm -> < Expression",
    "ConditionTerm -> <= Expression",
    "ConditionTerm -> > Expression",
    "ConditionTerm -> >= Expression",
    "ConditionTerm -> == Expression",
    "ConditionTerm -> != Expression",

    // 定义条件语句
    // "Statement -> class identifier { Statements } ;",
    // "Expression -> class identifier { Statements } ;",
    // // 定义类 结构体不需要了
    // identifier 和 number 由词法分析器提供
};

#endif   // FORMULA_H