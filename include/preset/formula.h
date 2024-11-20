#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include <vector>

static const std::vector<std::string> formula = {"S->if C then S else S",
                                                 "S->if C then S",
                                                 "S->while C do S",
                                                 "S->ID := E",
                                                 "S->break",
                                                 "S->switch E case NUM : S",
                                                 "S->return E",
                                                 "S->ID ( )",
                                                 "S->ID ( E )",
                                                 "S->S ; S",
                                                 "C->E < E",
                                                 "C->E <= E",
                                                 "C->E > E",
                                                 "C->E >= E",
                                                 "C->E == E",
                                                 "C->E <> E",
                                                 "E->E + T",
                                                 "E->E - T",
                                                 "E->T",
                                                 "T->T * F",
                                                 "T->T / F",
                                                 "T->F",
                                                 "F->( E )",
                                                 "F->ID",
                                                 "F->NUM"};

#endif   // FORMULA_H