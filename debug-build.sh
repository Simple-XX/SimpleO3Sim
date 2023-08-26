gcc -DDEBUG -DREG_DEBUG -DDIFFTEST main.c CMT.c EX.c ID.c IF.c IS.c ram.c RN.c uart.c fakeCMT.c EX_impl.c csr.c diff.c -o o3sim -fsanitize=address -g

# FIXME: with asan and debug info, runs well
# with asan and no debug info, runs well
# without asan and debug info, runs bad
# without asan but with debug info, runs bad

# asan changed the address of some variables, so the diff is not the same?