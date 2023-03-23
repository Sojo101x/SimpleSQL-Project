/*execute.h*/

//
// Project: Execution of queries for SimpleSQL
//
// Soji Oduneye
// Northwestern University
// CS 211, Winter 2023
//

#pragma once
#include "database.h"
#include "ast.h"
//
// #include header files needed to compile function declarations
//

//
// function declarations:
void execute_query(struct Database* db, struct QUERY* query);

