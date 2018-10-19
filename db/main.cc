#include "ron/ron.hpp"
#include "rdt/rdt.hpp"
#include "rocksdb/db.h"
#include "db/replica.hpp"

using namespace ron;
using namespace std;

typedef Replica<TextFrame> TextReplica;

// ## HUMANE
// create database test with spaces sha3,vv, compression gz, format binary
// show object 1ABCk+gritzko with version 1BCDj, format text
// show vv 1BCDj+gritzko

// ## INHUMANE
// db "*vv @1BCDj+gritzko ?"
// db ":db ! 'zip' 'gz', 'spaces' >sha3>vv>json"
// 1AbC2+gritzko
// db ":tag >test >1AbC2+gritzko"

int main (int argn, char** args) {
    TextReplica replica{};

    Status ok = replica.Create(".swarmdb");

//    TextFrame script{args[1]};
//    TextFrame::Builder response;
//
//    Status ok = replica.Run(response, {script});

    return ok ? 0 : -1;
}
