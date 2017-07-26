#include "fsshelper.h"

int main(int argc, char** argv) {
  LLT inodeN;
  pathToInode(argv[1], &inodeN);
  return 0;
}
